/*
 * interrupt_avr8.h
 *
 * Created: 20.05.2025 08:55:42
 *  Author: Gus
 */ 


#ifndef UTILS_INTERRUPT_AVR8_H
#define UTILS_INTERRUPT_AVR8_H

/**
 * \weakgroup interrupt_group
 *
 * @{
 */

#ifdef ISR_CUSTOM_H
#include ISR_CUSTOM_H
#else

/**
 * \def ISR
 * \brief Define service routine for specified interrupt vector
 *
 * Usage:
 * \code
    ISR(FOO_vect)
    {
        ...
    }
\endcode
 *
 * \param vect Interrupt vector name as found in the device header files.
 */
#if defined(__DOXYGEN__)
#define ISR(vect)
#elif defined(__GNUC__)
#include <avr/interrupt.h>
#elif defined(__ICCAVR__)
#define __ISR(x) _Pragma(#x)
#define ISR(vect) __ISR(vector = vect) __interrupt void handler_##vect(void)
#endif
#endif // ISR_CUSTOM_H

#ifdef __GNUC__
#define cpu_irq_enable() sei()
#define cpu_irq_disable() cli()
#else
#define cpu_irq_enable() __enable_interrupt()
#define cpu_irq_disable() __disable_interrupt()
#endif

//! @}

/**
 * \weakgroup interrupt_deprecated_group
 * @{
 */
// Deprecated definitions.
#define Enable_global_interrupt() cpu_irq_enable()
#define Disable_global_interrupt() cpu_irq_disable()
#define Is_global_interrupt_enabled() cpu_irq_is_enabled()
//! @}

#endif /* UTILS_INTERRUPT_AVR8_H */
