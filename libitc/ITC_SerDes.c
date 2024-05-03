/**
 * @file ITC_SerDes.c
 * @brief Implementation of the Interval Tree Clock's Stamp mechanism
 *
 * @copyright Copyright (c) 2024 libITC project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#include "ITC_SerDes.h"

#include "ITC_Event.h"
#include "ITC_Id.h"
#include "ITC_Stamp.h"

/******************************************************************************
 * Private functions
 ******************************************************************************/

/******************************************************************************
 * Public functions
 ******************************************************************************/

/******************************************************************************
 * Serialise an existing ITC Stamp
 ******************************************************************************/

ITC_Status_t ITC_SerDes_serialiseStamp(
    const ITC_Stamp_t *pt_Stamp,
    uint8_t *pu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    return ITC_STATUS_FAILURE;
}

/******************************************************************************
 * Deserialise an ITC Stamp
 ******************************************************************************/

ITC_Status_t ITC_SerDes_deserialiseStamp(
    const uint8_t *pu8_Buffer,
    const uint32_t u32_BufferSize,
    ITC_Stamp_t **ppt_Stamp
)
{
    return ITC_STATUS_FAILURE;
}
