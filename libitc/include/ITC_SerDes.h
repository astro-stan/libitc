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

/******************************************************************************
 * Functions
 ******************************************************************************/

/**
 * @brief Serialise an existing ITC Stamp
 *
 * @warning This library performs a few basic checks to ensure data correctness
 * during deserialisation. However, it is strongly recommended to further
 * protect the data integrity with a checksum or some other external mechanism
 * when transmitting over the wire. Otherwise, deserialisation _might_ still
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
    const ITC_Stamp_t *pt_Stamp,
    uint8_t *pu8_Buffer,
    uint32_t *pu32_BufferSize
);

/**
 * @brief Deserialise an ITC Stamp
 *
 * @warning This library performs a few basic checks to ensure data correctness
 * during deserialisation. However, it is strongly recommended to further
 * protect the data integrity with a checksum or some other external mechanism
 * when transmitting over the wire. Otherwise, deserialisation _might_ still
 * succeed but result in an unexpected behaviour.
 *
 * @param pu8_Buffer The buffer holding the serialised Stamp data
 * @param u32_BufferSize The size of the buffer in bytes
 * @param ppt_Stamp The pointer to the deserialised Stamp
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_SerDes_deserialiseStamp(
    const uint8_t *pu8_Buffer,
    const uint32_t u32_BufferSize,
    ITC_Stamp_t **ppt_Stamp
);

#endif /* ITC_SERDES_H_ */
