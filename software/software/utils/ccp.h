/*
 * ccp.h
 *
 * Created: 20.05.2025 08:39:30
 *  Author: Gus
 */ 


#ifndef CPU_CCP_H
#define CPU_CCP_H

#include "compiler.h"
#include <avr/io.h>
#include "protected_io.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Write to a CCP-protected 8-bit I/O register
 *
 * \param addr Address of the I/O register
 * \param value Value to be written
 *
 * \note Using IAR Embedded workbench, the choice of memory model has an impact
 *       on calling convention. The memory model is not visible to the
 *       preprocessor, so it must be defined in the Assembler preprocessor directives.
 */
static inline void ccp_write_io(void *addr, uint8_t value)
{
	protected_write_io(addr, CCP_IOREG_gc, value);
}

/** @} */

/**
 * \brief Write to CCP-protected 8-bit SPM register
 *
 * \param addr Address of the SPM register
 * \param value Value to be written
 *
 * \note Using IAR Embedded workbench, the choice of memory model has an impact
 *       on calling convention. The memory model is not visible to the
 *       preprocessor, so it must be defined in the Assembler preprocessor directives.
 */
static inline void ccp_write_spm(void *addr, uint8_t value)
{
	protected_write_io(addr, CCP_SPM_gc, value);
}

	/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CCP_H */
