#define TAG "FLASH"

#include <fusexconfig.h>
#include <fusexutil.h>
#include <trame.h>
#include <BGC_Pinout.h>

#include <SPIMemory.h>

#include "init.h"
#include "debug.h"
#include "flash.h"

#define FLASH_MAGIC                "BGCF0"
#define FLASH_MAGIC_LEN            strlen(FLASH_MAGIC)

#define FIRST_PAGE                 0
#define LAST_PAGE                  (flash.getMaxPage() - 1)

#define CURRENT_SLICE_COUNTER_ADDR FIRST_PAGE
#define NEXT_SLICE_COUNTER_ADDR    LAST_PAGE
#define FRONT_MAGIC_ADDR           (FIRST_PAGE+sizeof(slice_current))
#define BACK_MAGIC_ADDR            (LAST_PAGE+sizeof(slice_current))

#define MAX_SLICES                 (16ul - 1)
#define SLICE_SIZE                 (flash.getMaxPage()/(MAX_SLICES+1))
#define FIRST_SLICE_START_ADDR     (FIRST_PAGE + SPI_PAGESIZE)

#define MAX_SLICE_LAST_ADDR        (FIRST_SLICE_START_ADDR + (MAX_SLICES*SLICE_SIZE))
#define SLICE_START_ADDR(x)        (FIRST_SLICE_START_ADDR + (x*SLICE_SIZE))

static SPIFlash flash(BGC_SPIFLASH_CS);

static uint64_t flash_unique_id = 0;
static uint32_t flash_jedec_id = 0;
static uint32_t flash_capacity = 0;
static uint32_t flash_address = 0;
static uint32_t slice_current = 0;
static bool     flash_initialized = false;
static bool     flash_slice_initialized = false;

static bool flash_getInfos ()
{
    flash_jedec_id = flash.getJEDECID();

    if (!flash_jedec_id) {
        BOOTTRACE("Cannot communicate with the flash chip");
        return false;
    } else {
        flash_unique_id = flash.getUniqueID();
        flash_capacity = flash.getCapacity();
    }

    return true;
}

static bool flashSlice_init ()
{
    char buffer[SPI_PAGESIZE];

    /* 1. sanity check */
    memset(buffer, 0, FLASH_MAGIC_LEN);
    flash.readByteArray(FRONT_MAGIC_ADDR, buffer, FLASH_MAGIC_LEN);
    if (strncmp(buffer, FLASH_MAGIC, FLASH_MAGIC_LEN)) {
        return false;
    }

    memset(buffer, 0, FLASH_MAGIC_LEN);
    flash.readByteArray(BACK_MAGIC_ADDR, buffer, FLASH_MAGIC_LEN);
    if (strncmp(buffer, FLASH_MAGIC, FLASH_MAGIC_LEN)) {
        return false;
    }

    /* 2. read first page to get current slice */
    slice_current = flash.readULong(CURRENT_SLICE_COUNTER_ADDR);

    uint32_t slice_next = flash.readULong(NEXT_SLICE_COUNTER_ADDR);
    if (slice_current != (slice_next - 1)) {
        return false;
    }

    /* 3. set flash_address */
    flash_address = SLICE_START_ADDR(slice_current);

    /* 4. read first page of the slice */
    flash.readByteArray(flash_address, buffer, SPI_PAGESIZE);

    /* 5. check if it content is equal to 0 */
    for (int i=0; i<SPI_PAGESIZE; i++) {
        if (buffer[i] != 0xff) {
            return false;
        }
    }

    /* 6. update slice counter */
    slice_current++;
    flash.writeULong(CURRENT_SLICE_COUNTER_ADDR, slice_current);
    flash.writeULong(NEXT_SLICE_COUNTER_ADDR, slice_current+1);

    return true;
}

void dumpFlash (bool isConsole)
{
    MYTRACE("Flash init:          %s\r\n", flash_initialized?"true":"false");
    MYTRACE("Flash Slice init:    %s\r\n", flash_slice_initialized?"true":"false");
    MYTRACE("Flash JEDEC ID:      %lx\r\n", flash_jedec_id);
    MYTRACE("Flash UNIQUE ID:     %lx %lx\r\n", (uint32_t)(flash_unique_id>>32),
                                                (uint32_t)flash_unique_id);
    MYTRACE("Flash Current Slice: %lu/%lu\r\n", slice_current, MAX_SLICES);
    MYTRACE("Flash Slice size:    %u\r\n", SLICE_SIZE);
    MYTRACE("Flash Capacity:      %lu\r\n", flash_capacity);
    MYTRACE("Flash txdata size    %u\r\n", fxtm_gettxdatasize());
    MYTRACE("Flash Address:       0x%08lx\r\n", flash_address);
    MYTRACE("Flash MAX Address:   0x%08lx\r\n", MAX_SLICE_LAST_ADDR);
}

void readFlash (bool isConsole, uint32_t address)
{
    uint8_t data_buffer[SPI_PAGESIZE];

    flash.readByteArray(address, &data_buffer[0], SPI_PAGESIZE);
    hexdump(isConsole, address, data_buffer, SPI_PAGESIZE);
}

void setupFlashSlice ()
{
#if 0
    flash.eraseChip();
#endif
    flash.writeByteArray(FRONT_MAGIC_ADDR, FLASH_MAGIC, FLASH_MAGIC_LEN);
    flash.writeByteArray(BACK_MAGIC_ADDR, FLASH_MAGIC, FLASH_MAGIC_LEN);

    slice_current = 0;
    flash.writeULong(CURRENT_SLICE_COUNTER_ADDR, slice_current);
    flash.writeULong(NEXT_SLICE_COUNTER_ADDR, slice_current+1);
}

void eraseFlash (uint32_t address)
{
     /* Erase 64KB block */
     flash.eraseBlock64K(address);
}

void setupFlash ()
{
    bool setupSuccess = true;

    module_add(TAG);
    if (flash.error()) {
        setupSuccess = false;
    }

    if (setupSuccess) {
        flash.begin();
        if (flash_getInfos() == false) {
            setupSuccess = false;
        }
    }

    if (setupSuccess) {
        if (flashSlice_init() == true) {
            flash_slice_initialized = true;
        }
    }

    if (setupSuccess) {
        flash_initialized = true;
        module_setup(TAG, FXTM_SUCCESS);
    } else {
        BOOTTRACE("%s\n\r", flash.error(VERBOSE));
        module_setup(TAG, FXTM_FAILURE);
        Init_SetSemiFatal();
    }
}

void loopFlash ()
{
    if (flash_initialized == false || flash_slice_initialized == false) {
        fxtm_seterror(FXTM_ERROR_FLASH);
        return;
    }

    if (flash.writeByteArray(flash_address, (uint8_t *)fxtm_getblock(), fxtm_gettxdatasize())) {
        fxtm_seterror(FXTM_ERROR_FLASH);
        return;
    }

    flash_address += fxtm_gettxdatasize();
    if (flash_address >= SLICE_START_ADDR(slice_current+1)) {
        slice_current++;
        flash.writeULong(CURRENT_SLICE_COUNTER_ADDR, slice_current);
        flash.writeULong(NEXT_SLICE_COUNTER_ADDR, slice_current+1);
    }

    if (flash_address >= MAX_SLICE_LAST_ADDR) {
        flash_slice_initialized = false;
    }
}
