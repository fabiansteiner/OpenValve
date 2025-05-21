/*
 * EEPROM.h
 *
 * Created: 02.11.2022 16:19:40
 *  Author: SteinerF
 *
 * For storing data permanently (configuration, open/close state, etc...)
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

/*
void safeSoilLevel(uint8_t soilLevel);
void safeMultiplicator(uint8_t multiplicator);

uint8_t restoreSoilLevel();
uint8_t restoreMultiplicator();
*/

#include "utils/compiler.h"

#define BOOTLOADER_SECTION __attribute__((section(".bootloader")))

#ifdef __cplusplus
extern "C" {
	#endif

	/** Datatype for flash address */
	typedef uint16_t flash_adr_t;

	/** Datatype for EEPROM address */
	typedef uint16_t eeprom_adr_t;

	/** Datatype for return status of NVMCTRL operations */
	typedef enum {
		NVM_OK    = 0, ///< NVMCTRL free, no write ongoing.
		NVM_ERROR = 1, ///< NVMCTRL operation retsulted in error
		NVM_BUSY  = 2, ///< NVMCTRL busy, write ongoing.
	} nvmctrl_status_t;

	#if defined(__GNUC__)
	#define _FLASH_RELOCATE_BL_SECTION BOOTLOADER_SECTION
	#elif defined(__ICCAVR__)
	#define _FLASH_RELOCATE_BL_SECTION _Pragma("location = \"BOOTLOADER_SEGMENT\"")
	#else
	#error Unsupported compiler.
	#endif

	int8_t FLASH_0_init();

	uint8_t FLASH_0_read_eeprom_byte(eeprom_adr_t eeprom_adr);

	nvmctrl_status_t FLASH_0_write_eeprom_byte(eeprom_adr_t eeprom_adr, uint8_t data);

	

	#ifdef __cplusplus
}
#endif


#endif /* EEPROM_H_ */