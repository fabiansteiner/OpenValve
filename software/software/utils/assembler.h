/*
 * assembler.h
 *
 * Created: 20.05.2025 09:07:47
 *  Author: Gus
 */ 


#ifndef ASSEMBLER_H_INCLUDED
#define ASSEMBLER_H_INCLUDED

#if !defined(__ASSEMBLER__) && !defined(__IAR_SYSTEMS_ASM__) && !defined(__DOXYGEN__)
#error This file may only be included from assembly files
#endif

#if defined(__ASSEMBLER__)
#include "assembler/gas.h"
#include <avr/io.h>
#elif defined(__IAR_SYSTEMS_ASM__)
#include "assembler/iar.h"
#include <ioavr.h>
#endif

#endif /* ASSEMBLER_H_INCLUDED */
