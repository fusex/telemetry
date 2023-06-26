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

#define FIRST_PAGE                 0ul
#define LAST_PAGE                  (flash.getMaxPage() - 1)

#define FRONT_SLICE_INFO_ADDR      (FIRST_PAGE*SPI_PAGESIZE) 
#define BACK_SLICE_INFO_ADDR       (LAST_PAGE*SPI_PAGESIZE) 

#define SLICE_SIZE                 (64ul)
#define MAX_SLICES                 ((LAST_PAGE - 1)*SPI_PAGESIZE/SLICE_SIZE)

#define FIRST_SLICE_START_ADDR     (FRONT_SLICE_INFO_ADDR + SPI_PAGESIZE)
#define MAX_SLICE_LAST_ADDR        BACK_SLICE_INFO_ADDR 

#define SLICE_START_ADDR(x)        (FIRST_SLICE_START_ADDR + (x*SLICE_SIZE))

#define FRONT_MAGIC_ADDR           FRONT_SLICE_INFO_ADDR
#define BACK_MAGIC_ADDR            BACK_SLICE_INFO_ADDR

static SPIFlash flash(BGC_SPIFLASH_CS);

static uint64_t flash_unique_id = 0;
static uint32_t flash_jedec_id = 0;
static uint32_t flash_capacity = 0;
static uint32_t flash_address = FIRST_SLICE_START_ADDR;
static uint32_t slice_current = 0;
static bool     flash_initialized = false;
static bool     flash_slice_initialized = false;

static bool flash_getInfos ()
{
    flash_jedec_id = flash.getJEDECID();

    if (!flash_jedec_id) {
        BOOTTRACE("FLASH: Cannot communicate with the flash chip!\r\n");
        return false;
    } else {
        flash_unique_id = flash.getUniqueID();
        flash_capacity = flash.getCapacity();
    }

    return true;
}

static bool flashSlice_init ()
{
    for (slice_current=0; slice_current<MAX_SLICES; slice_current++) {
        bool    empty = true;
        uint8_t buffer[SPI_PAGESIZE];

        flash_address = SLICE_START_ADDR(slice_current);
        memset(buffer, 0, SPI_PAGESIZE);
        flash.readByteArray(flash_address, buffer, SPI_PAGESIZE);

        for (int i=0; i<SPI_PAGESIZE; i++) {
            if (buffer[i] != 0xff) {
                empty = false;
                break;
            }
        }
        if (empty == true) {
            BOOTTRACE("FLASH: Slice Current id: %lu\n\r", slice_current);
            return true;
        }
    }

    return false;
}

void dumpFlash (bool isConsole)
{
    MYTRACE("Flash init:           %s\r\n", flash_initialized?"true":"false");
    MYTRACE("Flash JEDEC ID:       %lx\r\n", flash_jedec_id);
    MYTRACE("Flash UNIQUE ID:      %lx %lx\r\n", (uint32_t)(flash_unique_id>>32),
                                                (uint32_t)flash_unique_id);
    MYTRACE("Flash Capacity:       %lu\r\n", flash_capacity);
    MYTRACE("Flash txdata size     %u\r\n", fxtm_gettxdatasize());
    MYTRACE("Flash CurrentAddress: 0x%08lx\r\n", flash_address);
    MYTRACE("Flash Slice init:     %s\r\n", flash_slice_initialized?"true":"false");
    MYTRACE("Flash Current Slice:  %lu/%lu\r\n", slice_current, MAX_SLICES);
    MYTRACE("Flash Slice size:     %lu\r\n", SLICE_SIZE);

    MYTRACE("Flash FSlice Address: 0x%08lx\r\n", FRONT_SLICE_INFO_ADDR);
    MYTRACE("Flash BSlice Address: 0x%08lx\r\n", BACK_SLICE_INFO_ADDR);

}

void readFlash (bool isConsole, uint32_t address)
{
    uint8_t buffer[SPI_PAGESIZE];

    memset(buffer, 0, SPI_PAGESIZE);

    flash.readByteArray(address, &buffer[0], SPI_PAGESIZE);
    hexdump(isConsole, address, buffer, SPI_PAGESIZE);
}

void writeFlash (uint32_t addr, uint32_t value)
{
    flash.writeULong(addr, value);
}

void setupFlashSlice()
{
    flash.eraseChip();
    flash.writeByteArray(FRONT_MAGIC_ADDR, FLASH_MAGIC, FLASH_MAGIC_LEN);
    flash.writeByteArray(BACK_MAGIC_ADDR, FLASH_MAGIC, FLASH_MAGIC_LEN);
}

void eraseFlash (uint32_t address)
{
#if 1
    flash.eraseChip();
#elif 0
     /* Erase 4KB block */
     flash.eraseSector(address);
#elif 0
     /* Erase Page */
     flash.eraseSection(address, SPI_PAGESIZE);
     /*FIXME: It erase more than SPI_PAGESIZE */
#elif 0
     /* Erase 32KB block */
     flash.eraseBlock32K(address);
#else
     /* Erase 64KB block */
     flash.eraseBlock64K(address);
#endif
}

void setupFlash ()
{
    bool setupSuccess = true;

    module_add(TAG);
    if (flash.error()) {
        setupSuccess = false;
    }

    if (setupSuccess) {
        flash.setClock(8000000);
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
        BOOTTRACE("FLASH: %s\n\r", flash.error(VERBOSE));
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
    slice_current++;

    if (flash_address >= MAX_SLICE_LAST_ADDR) {
        flash_slice_initialized = false;
    }
}
