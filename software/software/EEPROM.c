/*
 * EEPROM.c
 *
 * Created: 19.05.2025 23:38:33
 *  Author: SteinerF
 */ 


#include <string.h>
#include "utils/ccp.h"
#include "EEPROM.h"

/**
 * \brief Initialize nvmctrl interface
 * \return Return value 0 if success
 */
int8_t FLASH_0_init()
{

	//NVMCTRL.CTRLB = 1 << NVMCTRL_APCWP_bp       /* Application code write protect: enabled */
	//                | 1 << NVMCTRL_BOOTLOCK_bp; /* Boot Lock: enabled */

	// NVMCTRL.INTCTRL = 0 << NVMCTRL_EEREADY_bp; /* EEPROM Ready: disabled */

	return 0;
}

/**
 * \brief Read a byte from eeprom
 *
 * \param[in] eeprom_adr The byte-address in eeprom to read from
 *
 * \return The read byte
 */
uint8_t FLASH_0_read_eeprom_byte(eeprom_adr_t eeprom_adr)
{

	// Read operation will be stalled by hardware if any write is in progress

	return *(uint8_t *)(EEPROM_START + eeprom_adr);
}

/**
 * \brief Write a byte to eeprom
 *
 * \param[in] eeprom_adr The byte-address in eeprom to write to
 * \param[in] data The byte to write
 *
 * \return Status of write operation
 */
nvmctrl_status_t FLASH_0_write_eeprom_byte(eeprom_adr_t eeprom_adr, uint8_t data)
{

	/* Wait for completion of previous write */
	while (NVMCTRL.STATUS & NVMCTRL_EEBUSY_bm)
		;

	/* Clear page buffer */
	ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEBUFCLR_gc);

	/* Write byte to page buffer */
	*(uint8_t *)(EEPROM_START + eeprom_adr) = data;

	/* Erase byte and program it with desired value */
	ccp_write_spm((void *)&NVMCTRL.CTRLA, NVMCTRL_CMD_PAGEERASEWRITE_gc);

	return NVM_OK;
}

