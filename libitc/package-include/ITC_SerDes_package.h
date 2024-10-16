/**
 * @file ITC_SerDes_package.h
 * @brief Package definitions for the Interval Tree Clock's serialisation and
 * deserialisation mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#ifndef ITC_SERDES_PACKAGE_H_
#define ITC_SERDES_PACKAGE_H_

#include "ITC_Config.h"

#if !(ITC_CONFIG_ENABLE_SERIALISE_TO_STRING_API && ITC_CONFIG_ENABLE_EXTENDED_API)
#include "ITC_Status.h"
#include "ITC_Id.h"
#include "ITC_Event.h"


/******************************************************************************
 * Functions
 ******************************************************************************/

/**
 * @brief Serialise an existing ITC Id to ASCII string
 *
 * @note The output buffer is always NULL-terminated
 * @param ppt_Id The pointer to the Id
 * @param pc_Buffer The buffer to hold the serialised data
 * @param pu32_BufferSize (in) The size of the buffer in bytes. (out) The size
 * of the data inside the buffer in bytes (including the NULL termination byte).
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 * @retval `ITC_STATUS_INSUFFICIENT_RESOURCES` if the buffer is not big enough
 */
ITC_Status_t ITC_SerDes_serialiseIdToString(
    const ITC_Id_t *const pt_Id,
    char *const pc_Buffer,
    uint32_t *const pu32_BufferSize
);

/**
 * @brief Serialise an existing ITC Event to ASCII string
 *
 * @note The output buffer is always NULL-terminated
 * @param ppt_Event The pointer to the Event
 * @param pc_Buffer The buffer to hold the serialised data
 * @param pu32_BufferSize (in) The size of the buffer in bytes. (out) The size
 * of the data inside the buffer in bytes (including the NULL termination byte).
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 * @retval `ITC_STATUS_INSUFFICIENT_RESOURCES` if the buffer is not big enough
 */
ITC_Status_t ITC_SerDes_serialiseEventToString(
    const ITC_Event_t *const pt_Event,
    char *const pc_Buffer,
    uint32_t *const pu32_BufferSize
);
#endif /* !(ITC_CONFIG_ENABLE_SERIALISE_TO_STRING_API && ITC_CONFIG_ENABLE_EXTENDED_API) */

#endif /* ITC_SERDES_PACKAGE_H_ */
