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
#include "ITC_SerDes_package.h"

/******************************************************************************
 * Public functions
 ******************************************************************************/

/******************************************************************************
 * Validate serialization/deserialization buffer
 ******************************************************************************/

ITC_Status_t ITC_SerDes_Util_validateBuffer(
    const uint8_t *pu8_Buffer,
    const uint32_t *pu32_BufferSize,
    const uint32_t u32_MinSize
)
{
    if (!pu8_Buffer || !pu32_BufferSize || !(*pu32_BufferSize))
    {
        return ITC_STATUS_INVALID_PARAM;
    }
    else if (*pu32_BufferSize < u32_MinSize)
    {
        return ITC_STATUS_INSUFFICIENT_RESOURCES;
    }

    return ITC_STATUS_SUCCESS;
}

/******************************************************************************
 * Serialise an `uint32_t` in network-endian
 ******************************************************************************/

ITC_Status_t ITC_SerDes_Util_u32ToNetwork(
    uint32_t u32_Value,
    uint8_t *pu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    uint32_t u32_ValueCopy = u32_Value;
    /* The number of bytes needed to serialise the value */
    uint32_t u32_BytesNeeded = 0;

    /* Determine the bytes needed to serialise the value */
    do
    {
        u32_ValueCopy >>= 8U;
        u32_BytesNeeded++;
    } while (u32_ValueCopy != 0);

    if (u32_BytesNeeded > *pu32_BufferSize)
    {
        t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
    }
    else
    {
        /* Serialise in network-endian */
        for (uint32_t u32_I = u32_BytesNeeded; u32_I > 0; u32_I--)
        {
            pu8_Buffer[u32_I - 1] = (uint8_t)(u32_Value & 0xFFU);
            u32_Value >>= 8U;
        }

        /* Return the size of the data in the buffer */
        *pu32_BufferSize = u32_BytesNeeded;
    }

    return t_Status;
}

/******************************************************************************
 * Deserialise an `uint32_t` from network-endian
 ******************************************************************************/

ITC_Status_t ITC_SerDes_Util_u32FromNetwork(
    const uint8_t *pu8_Buffer,
    const uint32_t u32_BufferSize,
    uint32_t *pu32_Value
)
{
    if (u32_BufferSize > sizeof(uint32_t))
    {
        /* The buffer is to big to fit into an uint32_t.
         * This is probably an input error */
        return ITC_STATUS_INVALID_PARAM;
    }

    /* Init the value */
    *pu32_Value = 0;

    /* Deserialise from network-endian */
    for (uint32_t u32_I = 0; u32_I < u32_BufferSize; u32_I++)
    {
        *pu32_Value <<= 8U;
        *pu32_Value |= pu8_Buffer[u32_I];
    }

    return ITC_STATUS_SUCCESS;
}

/******************************************************************************
 * Serialise an Event counter in network-endian
 ******************************************************************************/

ITC_Status_t ITC_SerDes_Util_eventCounterToNetwork(
    ITC_Event_Counter_t t_Counter,
    uint8_t *pu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Event_Counter_t t_CounterCopy = t_Counter;
    /* The number of bytes needed to serialise the counter */
    uint32_t u32_BytesNeeded = 0;

    /* Determine the bytes needed to serialise the counter */
    do
    {
        t_CounterCopy >>= 8U;
        u32_BytesNeeded++;
    } while (t_CounterCopy != 0);

    if (u32_BytesNeeded > *pu32_BufferSize)
    {
        t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
    }
    else
    {
        /* Serialise in network-endian */
        for (uint32_t u32_I = u32_BytesNeeded; u32_I > 0; u32_I--)
        {
            pu8_Buffer[u32_I - 1] = (uint8_t)(t_Counter & 0xFFU);
            t_Counter >>= 8U;
        }

        /* Return the size of the data in the buffer */
        *pu32_BufferSize = u32_BytesNeeded;
    }

    return t_Status;
}

/******************************************************************************
 * Deserialise an Event counter from network-endian
 ******************************************************************************/

ITC_Status_t ITC_SerDes_Util_eventCounterFromNetwork(
    const uint8_t *pu8_Buffer,
    const uint32_t u32_BufferSize,
    ITC_Event_Counter_t *pt_Counter
)
{
    if (u32_BufferSize > sizeof(ITC_Event_Counter_t))
    {
        /* The counter size is not supported on this platform */
        return ITC_STATUS_EVENT_UNSUPPORTED_COUNTER_SIZE;
    }

    /* Init the counter */
    *pt_Counter = 0;

    /* Deserialise from network-endian */
    for (uint32_t u32_I = 0; u32_I < u32_BufferSize; u32_I++)
    {
        *pt_Counter <<= 8U;
        *pt_Counter |= pu8_Buffer[u32_I];
    }

    return ITC_STATUS_SUCCESS;
}
