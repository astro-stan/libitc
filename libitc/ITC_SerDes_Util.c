/**
 * @file ITC_SerDes_Util.c
 * @brief Utility functions for Interval Tree Clock's serialisation and
 * deserialisation mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#include "ITC_SerDes_Util_package.h"

/******************************************************************************
 * Public functions
 ******************************************************************************/

/******************************************************************************
 * Validate serialization/deserialization buffer
 ******************************************************************************/

ITC_Status_t ITC_SerDes_Util_validateBuffer(
    const uint8_t *const pu8_Buffer,
    const uint32_t *const pu32_BufferSize,
    const uint32_t u32_MinSize,
    const bool b_SerialiseOp
)
{
    if (!pu8_Buffer || !pu32_BufferSize || !(*pu32_BufferSize))
    {
        return ITC_STATUS_INVALID_PARAM;
    }
    else if (*pu32_BufferSize < u32_MinSize)
    {
        /* Return an appropriate exception depending on the use case */
        return (b_SerialiseOp) ? ITC_STATUS_INSUFFICIENT_RESOURCES
                               : ITC_STATUS_INVALID_PARAM;
    }

    return ITC_STATUS_SUCCESS;
}
