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

/******************************************************************************
 * Check the deserialised library version is supported
 ******************************************************************************/

ITC_Status_t ITC_SerDes_Util_validateDesLibVersion(
    const uint8_t u8_LibVersion
)
{
    static uint8_t ru8_SupportedDesLibVersions[] = {
        /* The current `ITC_VERSION_MAJOR`. Hardcoded on purpose, so that
         * this check fails if the library's major version is bumped without
         * manually updating this function first. */
        1,
        0 /* Also compatible with 0.x.x lib versions */
    };

    for (uint32_t u32_I = 0;
         u32_I < sizeof(ru8_SupportedDesLibVersions);
         u32_I++)
    {
        if (ru8_SupportedDesLibVersions[u32_I] == u8_LibVersion)
        {
            return ITC_STATUS_SUCCESS;
        }
    }

    return ITC_STATUS_SERDES_INCOMPATIBLE_LIB_VERSION;
}
