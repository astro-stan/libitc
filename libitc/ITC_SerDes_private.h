/**
 * @file ITC_SerDes_private.h
 * @brief Private definitions for the Interval Tree Clock's serialisation and
 * deserialisation mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#ifndef ITC_SERDES_PRIVATE_H_
#define ITC_SERDES_PRIVATE_H_

#include <stdint.h>

/******************************************************************************
 * Defines
 ******************************************************************************/

/* The header of a serialised leaf null ITC ID */
#define ITC_SERDES_NULL_ID_HEADER                                        (0x00U)
/* The header of a serialised leaf seed ITC ID */
#define ITC_SERDES_SEED_ID_HEADER                                        (0x02U)
/* The header of a serialised parent ITC ID */
#define ITC_SERDES_PARENT_ID_HEADER                                      (0x01U)

/******************************************************************************
 * Types
 ******************************************************************************/

/* The header of a single serialised node */
typedef uint8_t ITC_SerDes_Header_t;

#endif /* ITC_SERDES_PRIVATE_H_ */
