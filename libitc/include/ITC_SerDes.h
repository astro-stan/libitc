/**
 * @file ITC_SerDes.h
 * @brief Definitions for the Interval Tree Clock's serialisation and
 * deserialisation mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#ifndef ITC_SERDES_H_
#define ITC_SERDES_H_

#include "ITC_Stamp.h"
#include "ITC_Status.h"
#include "ITC_Config.h"

#if ITC_CONFIG_ENABLE_EXTENDED_API
#include "ITC_Id.h"
#include "ITC_Event.h"
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */


/******************************************************************************
 * Functions
 ******************************************************************************/

#if ITC_CONFIG_ENABLE_EXTENDED_API

/**
 * @brief Serialise an existing ITC Id
 *
 * @warning A few basic checks are performed on the serialised data during
 * deserialisation to ensure data correctness. However, it is strongly
 * recommended to further protect the serialised data integrity with a checksum
 * or some other external mechanism when transmitting it over the wire.
 * Otherwise, in certain cases, deserialisation of corrupted data _might_ still
 * succeed but result in an unexpected behaviour.
 *
 * @param ppt_Id The pointer to the Id
 * @param pu8_Buffer The buffer to hold the serialised data
 * @param pu32_BufferSize (in) The size of the buffer in bytes. (out) The size
 * of the data inside the buffer in bytes.
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 * @retval `ITC_STATUS_INSUFFICIENT_RESOURCES` if the buffer is not big enough
 */
ITC_Status_t ITC_SerDes_serialiseId(
    const ITC_Id_t *const pt_Id,
    uint8_t *const pu8_Buffer,
    uint32_t *const pu32_BufferSize
);

/**
 * @brief Deserialise an ITC Id
 *
 * @warning A few basic checks are performed on the serialised data during
 * deserialisation to ensure data correctness. However, it is strongly
 * recommended to further protect the serialised data integrity with a checksum
 * or some other external mechanism when transmitting it over the wire.
 * Otherwise, in certain cases, deserialisation of corrupted data _might_ still
 * succeed but result in an unexpected behaviour.
 *
 * @param pu8_Buffer The buffer holding the serialised Id data
 * @param u32_BufferSize The size of the buffer in bytes
 * @param ppt_Id The pointer to the deserialised Id
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_SerDes_deserialiseId(
    const uint8_t *const pu8_Buffer,
    const uint32_t u32_BufferSize,
    ITC_Id_t **const ppt_Id
);

/**
 * @brief Serialise an existing ITC Event
 *
 * @warning A few basic checks are performed on the serialised data during
 * deserialisation to ensure data correctness. However, it is strongly
 * recommended to further protect the serialised data integrity with a checksum
 * or some other external mechanism when transmitting it over the wire.
 * Otherwise, in certain cases, deserialisation of corrupted data _might_ still
 * succeed but result in an unexpected behaviour.
 *
 * @param ppt_Event The pointer to the Event
 * @param pu8_Buffer The buffer to hold the serialised data
 * @param pu32_BufferSize (in) The size of the buffer in bytes. (out) The size
 * of the data inside the buffer in bytes.
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 * @retval `ITC_STATUS_INSUFFICIENT_RESOURCES` if the buffer is not big enough
 */
ITC_Status_t ITC_SerDes_serialiseEvent(
    const ITC_Event_t *const pt_Event,
    uint8_t *const pu8_Buffer,
    uint32_t *const pu32_BufferSize
);

/**
 * @brief Deserialise an ITC Event
 *
 * @warning A few basic checks are performed on the serialised data during
 * deserialisation to ensure data correctness. However, it is strongly
 * recommended to further protect the serialised data integrity with a checksum
 * or some other external mechanism when transmitting it over the wire.
 * Otherwise, in certain cases, deserialisation of corrupted data _might_ still
 * succeed but result in an unexpected behaviour.
 *
 * @param pu8_Buffer The buffer holding the serialised Event data
 * @param u32_BufferSize The size of the buffer in bytes
 * @param ppt_Event The pointer to the deserialised Event
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_SerDes_deserialiseEvent(
    const uint8_t *const pu8_Buffer,
    const uint32_t u32_BufferSize,
    ITC_Event_t **const ppt_Event
);

#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */

/**
 * @brief Serialise an existing ITC Stamp
 *
 * @warning A few basic checks are performed on the serialised data during
 * deserialisation to ensure data correctness. However, it is strongly
 * recommended to further protect the serialised data integrity with a checksum
 * or some other external mechanism when transmitting it over the wire.
 * Otherwise, in certain cases, deserialisation of corrupted data _might_ still
 * succeed but result in an unexpected behaviour.
 *
 * @param ppt_Stamp The pointer to the Stamp
 * @param pu8_Buffer The buffer to hold the serialised data
 * @param pu32_BufferSize (in) The size of the buffer in bytes. (out) The size
 * of the data inside the buffer in bytes.
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 * @retval `ITC_STATUS_INSUFFICIENT_RESOURCES` if the buffer is not big enough
 */
ITC_Status_t ITC_SerDes_serialiseStamp(
    const ITC_Stamp_t *const pt_Stamp,
    uint8_t *const pu8_Buffer,
    uint32_t *const pu32_BufferSize
);

/**
 * @brief Deserialise an ITC Stamp
 *
 * @warning A few basic checks are performed on the serialised data during
 * deserialisation to ensure data correctness. However, it is strongly
 * recommended to further protect the serialised data integrity with a checksum
 * or some other external mechanism when transmitting it over the wire.
 * Otherwise, in certain cases, deserialisation of corrupted data _might_ still
 * succeed but result in an unexpected behaviour.
 *
 * @param pu8_Buffer The buffer holding the serialised Stamp data
 * @param u32_BufferSize The size of the buffer in bytes
 * @param ppt_Stamp The pointer to the deserialised Stamp
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_SerDes_deserialiseStamp(
    const uint8_t *const pu8_Buffer,
    const uint32_t u32_BufferSize,
    ITC_Stamp_t **const ppt_Stamp
);

#if ITC_CONFIG_ENABLE_SERIALISE_TO_STRING_API

#if ITC_CONFIG_ENABLE_EXTENDED_API

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

#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */

/**
 * @brief Serialise an existing ITC Stamp to ASCII string
 *
 * @note The output buffer is always NULL-terminated
 * @param ppt_Stamp The pointer to the Stamp
 * @param pc_Buffer The buffer to hold the serialised data
 * @param pu32_BufferSize (in) The size of the buffer in bytes. (out) The size
 * of the data inside the buffer in bytes (including the NULL termination byte).
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 * @retval `ITC_STATUS_INSUFFICIENT_RESOURCES` if the buffer is not big enough
 */
ITC_Status_t ITC_SerDes_serialiseStampToString(
    const ITC_Stamp_t *const pt_Stamp,
    char *const pc_Buffer,
    uint32_t *const pu32_BufferSize
);

#endif /* ITC_CONFIG_ENABLE_SERIALISE_TO_STRING_API */

#endif /* ITC_SERDES_H_ */
