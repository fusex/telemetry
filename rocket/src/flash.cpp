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

#define CURRENT_SLICE_COUNTER_ADDR FRONT_SLICE_INFO_ADDR 
#define NEXT_SLICE_COUNTER_ADDR    BACK_SLICE_INFO_ADDR
#define FRONT_MAGIC_ADDR           (FRONT_SLICE_INFO_ADDR+sizeof(slice_current))
#define BACK_MAGIC_ADDR            (BACK_SLICE_INFO_ADDR+sizeof(slice_current))

#define MAX_SLICES                 (16ul)
#define SLICE_SIZE                 (SPI_PAGESIZE*flash.getMaxPage()/(MAX_SLICES))

#define FIRST_SLICE_START_ADDR     (FRONT_SLICE_INFO_ADDR + SPI_PAGESIZE)
#define MAX_SLICE_LAST_ADDR        BACK_SLICE_INFO_ADDR 

#define SLICE_START_ADDR(x)        (FIRST_SLICE_START_ADDR + (x*SLICE_SIZE))

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
# if 0
static bool flashSlice_init ()
{
    uint8_t buffer[SPI_PAGESIZE];

    /* 1. sanity check */
    memset(buffer, 0, FLASH_MAGIC_LEN);
    flash.readByteArray(FRONT_MAGIC_ADDR, buffer, FLASH_MAGIC_LEN);
    if (strncmp(buffer, FLASH_MAGIC, FLASH_MAGIC_LEN)) {
	BOOTTRACE("FLASH: Slice info page (front) not installed ... abort!\r\n");
        return false;
    }

    memset(buffer, 0, FLASH_MAGIC_LEN);
    flash.readByteArray(BACK_MAGIC_ADDR, buffer, FLASH_MAGIC_LEN);
    if (strncmp(buffer, FLASH_MAGIC, FLASH_MAGIC_LEN)) {
	BOOTTRACE("FLASH: Slice info page (back) not installed ... abort!\r\n");
        return false;
    }

    /* 2. read first page to get current slice */
    slice_current = flash.readULong(CURRENT_SLICE_COUNTER_ADDR);
    uint32_t slice_next = flash.readULong(NEXT_SLICE_COUNTER_ADDR);
    if (slice_next != 0) {
	if (slice_current != (slice_next - 1)) {
	    BOOTTRACE("FLASH: Slice info counters mismatch %llu vs %llu ... abort!\r\n", slice_current, slice_next);
	    return false;
	}
    } else if (slice_current == 0 && slice_next == 0) {
    }

    /* 3. set flash_address */
    flash_address = SLICE_START_ADDR(slice_next);

    /* 4. read first page of the slice */
    memset(buffer, 0, SPI_PAGESIZE);
    flash.readByteArray(flash_address, buffer, SPI_PAGESIZE);

    /* 5. check if the content is empty */
    for (int i=0; i<SPI_PAGESIZE; i++) {
        if (buffer[i] != 0xff) {
	    BOOTTRACE("FLASH: Current Slice is not empty at "
		      "buffer[%d]=%x ... abort!\r\n", i, buffer[i]);
	    hexdump(true, flash_address, buffer, SPI_PAGESIZE);
            return false;
        }
    }

    /* 6. update slice counters */
    slice_current++;
    flash.writeULong(CURRENT_SLICE_COUNTER_ADDR, slice_current);
    flash.writeULong(NEXT_SLICE_COUNTER_ADDR, slice_current+1);

    return true;
}
#else
static bool flashSlice_init ()
{
    uint8_t buffer[SPI_PAGESIZE];

    /* 1. sanity check */
    memset(buffer, 0, FLASH_MAGIC_LEN);
    flash.readByteArray(FRONT_MAGIC_ADDR, buffer, FLASH_MAGIC_LEN);
    if (strncmp(buffer, FLASH_MAGIC, FLASH_MAGIC_LEN)) {
	BOOTTRACE("FLASH: Slice info page (front) not installed ... abort!\r\n");
        return false;
    }

    memset(buffer, 0, FLASH_MAGIC_LEN);
    flash.readByteArray(BACK_MAGIC_ADDR, buffer, FLASH_MAGIC_LEN);
    if (strncmp(buffer, FLASH_MAGIC, FLASH_MAGIC_LEN)) {
	BOOTTRACE("FLASH: Slice info page (back) not installed ... abort!\r\n");
        return false;
    }

    /* 2. read first page to get current slice */
    slice_current = flash.readULong(CURRENT_SLICE_COUNTER_ADDR);
    uint32_t slice_next = flash.readULong(NEXT_SLICE_COUNTER_ADDR);
    if (slice_current != slice_next) {
	BOOTTRACE("FLASH: Slice info counters mismatch %lu vs %lu ... abort!\r\n", slice_current, slice_next);
	return false;
    }
    BOOTTRACE("FLASH: Slice Current id: %lu\n\r", slice_current);

    /* 3. set flash_address */
    flash_address = SLICE_START_ADDR(slice_next);

    /* 4. read first page of the slice */
    memset(buffer, 0, SPI_PAGESIZE);
    flash.readByteArray(flash_address, buffer, SPI_PAGESIZE);

    /* 5. check if the content is empty */
    for (int i=0; i<SPI_PAGESIZE; i++) {
        if (buffer[i] != 0xff) {
	    BOOTTRACE("FLASH: Current Slice is not empty at "
		      "buffer[%d]=%x ... abort!\r\n", i, buffer[i]);
	    hexdump(true, flash_address, buffer, SPI_PAGESIZE);
            return false;
        }
    }

    /* 6. update slice counters */
    slice_current++;
    flash.writeULong(CURRENT_SLICE_COUNTER_ADDR, slice_current);
    flash.writeULong(NEXT_SLICE_COUNTER_ADDR, slice_current);

    return true;
}
#endif

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
    MYTRACE("Flash Current Slice:  0x%lx/%lu\r\n", slice_current, MAX_SLICES);
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

void setupFlashSlice ()
{
#if 0
    flash.eraseChip();
#endif
    flash.writeByteArray(FRONT_MAGIC_ADDR, FLASH_MAGIC, FLASH_MAGIC_LEN);
    flash.writeByteArray(BACK_MAGIC_ADDR, FLASH_MAGIC, FLASH_MAGIC_LEN);

    slice_current = 0ul;
    flash.writeULong(CURRENT_SLICE_COUNTER_ADDR, slice_current);
    flash.writeULong(NEXT_SLICE_COUNTER_ADDR, slice_current);
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
    if (flash_address >= SLICE_START_ADDR(slice_current+1)) {
        slice_current++;
        flash.writeULong(CURRENT_SLICE_COUNTER_ADDR, slice_current);
        flash.writeULong(NEXT_SLICE_COUNTER_ADDR, slice_current+1);
    }

    if (flash_address >= MAX_SLICE_LAST_ADDR) {
        flash_slice_initialized = false;
    }
}
