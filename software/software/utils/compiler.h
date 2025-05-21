/*
 * compiler.h
 *
 * Created: 20.05.2025 08:54:47
 *  Author: Gus
 */ 


#ifndef UTILS_COMPILER_H
#define UTILS_COMPILER_H

/**
 * \defgroup doc_driver_utils_compiler Compiler abstraction
 * \ingroup doc_driver_utils
 *
 * Compiler abstraction layer and code utilities for 8-bit AVR.
 * This module provides various abstraction layers and utilities
 * to make code compatible between different compilers.
 *
 * \{
 */

#if defined(__GNUC__)
#include <avr/io.h>
#include <avr/builtins.h>
#elif defined(__ICCAVR__)
#define ENABLE_BIT_DEFINITIONS 1
#include <ioavr.h>
#include <intrinsics.h>

#ifndef CCP_IOREG_gc
#define CCP_IOREG_gc 0xD8 /* CPU_CCP_IOREG_gc */
#endif
#ifndef CCP_SPM_gc
#define CCP_SPM_gc 0x9D /* CPU_CCP_SPM_gc */
#endif

#else
#error Unsupported compiler.
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include "interrupt_avr8.h"

/**
 * \def UNUSED
 * \brief Marking \a v as a unused parameter or value.
 */
#define UNUSED(v) (void)(v)

#endif /* UTILS_COMPILER_H */
