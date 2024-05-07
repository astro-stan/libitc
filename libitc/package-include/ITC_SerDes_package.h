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

#include "ITC_Id.h"
#include "ITC_Event.h"
#include "ITC_Status.h"

/******************************************************************************
 * Functions
 ******************************************************************************/

/**
 * @brief Validate serialization/deserialization buffer
 *
 * @param pu8_Buffer The buffer
 * @param pu32_BufferSize The buffer size
 * @param u32_MinSize The minimum buffer size
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 * @retval `ITC_STATUS_INSUFFICIENT_RESOURCES` if *pu32_BufferSize < u32_MinSize
 */
ITC_Status_t ITC_SerDes_Util_validateBuffer(
    const uint8_t *pu8_Buffer,
    const uint32_t *pu32_BufferSize,
    const uint32_t u32_MinSize
);

/**
 * @brief Serialise an Event counter in network-endian
 *
 * @param t_Counter The counter to serialise
 * @param pu8_Buffer The buffer to hold the serialised data
 * @param pu32_BufferSize (in) The size of the buffer in bytes. (out) The size
 * of the data inside the buffer in bytes.
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 * @retval `ITC_STATUS_INSUFFICIENT_RESOURCES` if the buffer is not big enough
 */
ITC_Status_t ITC_SerDes_Util_eventCounterToNetwork(
    ITC_Event_Counter_t t_Counter,
    uint8_t *pu8_Buffer,
    uint32_t *pu32_BufferSize
);

/**
 * @brief Deserialise an Event counter from network-endian
 *
 * @param pu8_Buffer The buffer holding the serialised data
 * @param u32_BufferSize The size of the buffer in bytes
 * @param pt_Counter The pointer to the counter
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 * @retval `ITC_STATUS_EVENT_UNSUPPORTED_COUNTER_SIZE` if
 * `u32_BufferSize > sizeof(ITC_Event_Counter_t)`
 */
ITC_Status_t ITC_SerDes_Util_eventCounterFromNetwork(
    const uint8_t *pu8_Buffer,
    const uint32_t u32_BufferSize,
    ITC_Event_Counter_t *pt_Counter
);

/**
 * @brief Serialise an existing ITC Id
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
    const ITC_Id_t *pt_Id,
    uint8_t *pu8_Buffer,
    uint32_t *pu32_BufferSize
);

/**
 * @brief Deserialise an ITC Id
 *
 * @param pu8_Buffer The buffer holding the serialised Id data
 * @param u32_BufferSize The size of the buffer in bytes
 * @param ppt_Id The pointer to the deserialised Id
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_SerDes_deserialiseId(
    const uint8_t *pu8_Buffer,
    const uint32_t u32_BufferSize,
    ITC_Id_t **ppt_Id
);

/**
 * @brief Serialise an existing ITC Event
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
    const ITC_Event_t *pt_Event,
    uint8_t *pu8_Buffer,
    uint32_t *pu32_BufferSize
);

/**
 * @brief Deserialise an ITC Event
 *
 * @param pu8_Buffer The buffer holding the serialised Event data
 * @param u32_BufferSize The size of the buffer in bytes
 * @param ppt_Event The pointer to the deserialised Event
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_SerDes_deserialiseEvent(
    const uint8_t *pu8_Buffer,
    const uint32_t u32_BufferSize,
    ITC_Event_t **ppt_Event
);

#endif /* ITC_SERDES_PACKAGE_H_ */
