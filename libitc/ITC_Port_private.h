/**
 * @file ITC_Port_private.h
 * @brief Private port-specific definitions
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#ifndef ITC_PORT_PRIVATE_H_
#define ITC_PORT_PRIVATE_H_

#include "ITC_config.h"

#if ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_CONFIG_MEMORY_ALLOCATION_TYPE_STATIC

/******************************************************************************
 * Defines
 ******************************************************************************/

/* Pattern used to detect free slots in the static allocation arrays */
#define ITC_PORT_FREE_SLOT_PATTERN                                        (0x55)

#endif /* ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_CONFIG_MEMORY_ALLOCATION_TYPE_STATIC */

#endif /* ITC_PORT_PRIVATE_H_ */
