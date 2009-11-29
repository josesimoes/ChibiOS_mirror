/*
    ChibiOS/RT - Copyright (C) 2006-2007 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file AT91SAM7/pal_lld.h
 * @brief AT91SAM7 PIO low level driver header
 * @addtogroup AT91SAM7_PAL
 * @{
 */

#ifndef _PAL_LLD_H_
#define _PAL_LLD_H_

#include "board.h"

/*===========================================================================*/
/* Unsupported modes and specific modes                                      */
/*===========================================================================*/

#undef PAL_MODE_INPUT_PULLDOWN

/*===========================================================================*/
/* I/O Ports Types and constants.                                            */
/*===========================================================================*/

/**
 * @brief PIO port setup info.
 */
typedef struct {
  /** Initial value for ODSR register (data).*/
  uint32_t      odsr;
  /** Initial value for OSR register (direction).*/
  uint32_t      osr;
  /** Initial value for PUSR register (Pull-ups).*/
  uint32_t      pusr;
} at91sam7_pio_setup_t;

/**
 * @brief AT91SAM7 PIO static initializer.
 * @details An instance of this structure must be passed to @p palInit() at
 *          system startup time in order to initialized the digital I/O
 *          subsystem. This represents only the initial setup, specific pads
 *          or whole ports can be reprogrammed at later time.
 */
typedef struct {
  /** @brief Port 0 setup data.*/
  at91sam7_pio_setup_t P0Data;
#if (SAM7_PLATFORM == SAM7X128) || (SAM7_PLATFORM == SAM7X256) || \
    (SAM7_PLATFORM == SAM7X256) || defined(__DOXYGEN__)
  /** @brief Port 1 setup data.*/
  at91sam7_pio_setup_t P1Data;
#endif
} AT91SAM7PIOConfig;

/**
 * @brief Width, in bits, of an I/O port.
 */
#define PAL_IOPORTS_WIDTH 32

/**
 * @brief Digital I/O port sized unsigned type.
 */
typedef uint32_t ioportmask_t;

/**
 * @brief Port Identifier.
 * @details This type can be a scalar or some kind of pointer, do not make
 *          any assumption about it, use the provided macros when populating
 *          variables of this type.
 */
typedef AT91PS_PIO ioportid_t;

/*===========================================================================*/
/* I/O Ports Identifiers.                                                    */
/*===========================================================================*/

/**
 * @brief PIO port A identifier.
 */
#define IOPORT1         AT91C_BASE_PIOA

/**
 * @brief PIO port B identifier.
 */
#if (SAM7_PLATFORM == SAM7X128) || (SAM7_PLATFORM == SAM7X256) || \
    (SAM7_PLATFORM == SAM7X256) || defined(__DOXYGEN__)
#define IOPORT2         AT91C_BASE_PIOB
#endif

/*===========================================================================*/
/* Implementation, some of the following macros could be implemented as      */
/* functions, if so please put them in a file named pal_lld.c.               */
/*===========================================================================*/

/**
 * @brief Low level PAL subsystem initialization.
 */
#define pal_lld_init(config) _pal_lld_init(config)

/**
 * @brief Reads the physical I/O port states.
 * @details This function is implemented by reading the PIO_PDSR register, the
 *          implementation has no side effects.
 *
 * @param[in] port the port identifier
 * @return The port bits.
 *
 * @note This function is not meant to be invoked directly by the application
 *       code.
 */
#define pal_lld_readport(port) ((port)->PIO_PDSR)

/**
 * @brief Reads the output latch.
 * @details This function is implemented by reading the PIO_ODSR register, the
 *          implementation has no side effects.
 *
 * @param[in] port the port identifier
 * @return The latched logical states.
 *
 * @note This function is not meant to be invoked directly by the application
 *       code.
 */
#define pal_lld_readlatch(port) ((port)->PIO_ODSR)

/**
 * @brief Writes a bits mask on a I/O port.
 * @details This function is implemented by writing the PIO_ODSR register, the
 *          implementation has no side effects.
 *
 * @param[in] port the port identifier
 * @param[in] bits the bits to be written on the specified port
 *
 * @note This function is not meant to be invoked directly by the application
 *       code.
 */
#define pal_lld_writeport(port, bits) {                                 \
  (port)->PIO_ODSR = (bits);                                            \
}

/**
 * @brief Sets a bits mask on a I/O port.
 * @details This function is implemented by writing the PIO_SODR register, the
 *          implementation has no side effects.
 *
 * @param[in] port the port identifier
 * @param[in] bits the bits to be ORed on the specified port
 *
 * @note This function is not meant to be invoked directly by the application
 *       code.
 */
#define pal_lld_setport(port, bits) {                                   \
  (port)->PIO_SODR = (bits);                                            \
}


/**
 * @brief Clears a bits mask on a I/O port.
 * @details This function is implemented by writing the PIO_CODR register, the
 *          implementation has no side effects.
 *
 * @param[in] port the port identifier
 * @param[in] bits the bits to be cleared on the specified port
 *
 * @note This function is not meant to be invoked directly by the application
 *       code.
 */
#define pal_lld_clearport(port, bits) {                                 \
  (port)->PIO_CODR = (bits);                                            \
}

/**
 * @brief Writes a group of bits.
 * @details This function is implemented by writing the PIO_OWER, PIO_ODSR and
 *          PIO_OWDR registers, the implementation is not atomic because the
 *          multiple accesses.
 *
 * @param[in] port the port identifier
 * @param[in] mask the group mask
 * @param[in] offset the group bit offset within the port
 * @param[in] bits the bits to be written. Values exceeding the group width
 *            are masked.
 *
 * @note This function is not meant to be invoked directly by the application
 *       code.
 */
#define pal_lld_writegroup(port, mask, offset, bits) {                  \
  (port)->PIO_OWER = (mask) << (offset);                                \
  (port)->PIO_ODSR = (bits) << (offset);                                \
  (port)->PIO_OWDR = (mask) << (offset);                                \
}

/**
 * @brief Pads group mode setup.
 * @details This function programs a pads group belonging to the same port
 *          with the specified mode.
 *
 * @param[in] port the port identifier
 * @param[in] mask the group mask
 * @param[in] mode the mode
 *
 * @note This function is not meant to be invoked directly by the application
 *       code.
 * @note @p PAL_MODE_UNCONNECTED is implemented as push pull output with high
 *       state.
 */
#define pal_lld_setgroupmode(port, mask, mode) \
  _pal_lld_setgroupmode(port, mask, mode)

/**
 * @brief Writes a logical state on an output pad.
 *
 * @param[in] port the port identifier
 * @param[in] pad the pad number within the port
 * @param[out] bit the logical value, the value must be @p 0 or @p 1
 *
 * @note This function is not meant to be invoked directly by the application
 *       code.
 */
#define pal_lld_writepad(port, pad, bit) pal_lld_writegroup(port, 1, pad, bit)

#ifdef __cplusplus
extern "C" {
#endif
  void _pal_lld_init(const AT91SAM7PIOConfig *config);
  void _pal_lld_setgroupmode(ioportid_t port,
                             ioportmask_t mask,
                             uint_fast8_t mode);
#ifdef __cplusplus
}
#endif

#endif /* _PAL_LLD_H_ */

/** @} */
