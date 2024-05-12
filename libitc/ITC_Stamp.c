/**
 * @file ITC_Stamp.c
 * @brief Implementation of the Interval Tree Clock's Stamp mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#include "ITC_Stamp.h"
#include "ITC_SerDes.h"
#include "ITC_SerDes_private.h"
#include "ITC_SerDes_Util_package.h"

#include "ITC_Event_package.h"
#include "ITC_Id_package.h"
#include "ITC_Port.h"

#include <stdbool.h>

#include <string.h>

/******************************************************************************
 * Private functions
 ******************************************************************************/

/**
 * @brief Validate an existing ITC Stamp
 *
 * Should be used to validate all incoming Stamps before any processing is done.
 *
 * @param pt_Stamp The Stamp to validate
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t validateStamp(
    const ITC_Stamp_t *const pt_Stamp
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    if (!pt_Stamp)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else if (!pt_Stamp->pt_Id || !pt_Stamp->pt_Event)
    {
        t_Status = ITC_STATUS_CORRUPT_STAMP;
    }

    /* Validation of the ID and Event trees is done by their respective
     * validation functions */

    return t_Status;
}

/**
 * @brief Allocate a new ITC Stamp
 *
 * @note If `pt_Id != NULL && b_CloneId == false` the `pt_Id` will not be
 * deallocated in case of failure. Similarly if
 * `pt_Event != NULL &&  b_CloneEvent == false` the `pt_Event` will not be
 * deallocated in case of failure.
 * @param ppt_Stamp (out) The pointer to the new Stamp
 * @param pt_Id The pointer to an existing ID tree to be cloned. Otherwise NULL.
 * @param pt_Event The pointer to an existing Event tree to be cloned.
 * Otherwise NULL.
 * @param b_CreateNullId Allocate a NULL ID instead of a Seed ID.
 * Ignored if pt_Id != NULL
 * @param b_CloneId Whether to clone or simply assign the passed ID to the
 * Stamp. Ignored if pt_Id == NULL
 * @param b_CloneEvent Whether to clone or simply assign the passed Event to the
 * Stamp. Ignored if pt_Event == NULL
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t newStamp(
    ITC_Stamp_t **ppt_Stamp,
    ITC_Id_t *pt_Id,
    ITC_Event_t *pt_Event,
    bool b_CreateNullId,
    bool b_CloneId,
    bool b_CloneEvent
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Stamp_t *pt_Alloc = NULL;

    if (!ppt_Stamp)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        t_Status = ITC_Port_malloc((void **)&pt_Alloc, sizeof(ITC_Stamp_t));
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        if (pt_Id)
        {
            if (b_CloneId)
            {
                t_Status = ITC_Id_clone(pt_Id, &pt_Alloc->pt_Id);
            }
            else
            {
                pt_Alloc->pt_Id = pt_Id;
            }
        }
        else if (b_CreateNullId)
        {
            t_Status = ITC_Id_newNull(&pt_Alloc->pt_Id);
        }
        else
        {
            t_Status = ITC_Id_newSeed(&pt_Alloc->pt_Id);
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        if (pt_Event)
        {
            if (b_CloneEvent)
            {
                t_Status = ITC_Event_clone(pt_Event, &pt_Alloc->pt_Event);
            }
            else
            {
                pt_Alloc->pt_Event = pt_Event;
            }
        }
        else
        {
            t_Status = ITC_Event_new(&pt_Alloc->pt_Event);
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Return the pointer to the allocated memory */
        *ppt_Stamp = pt_Alloc;
    }
    else if(pt_Alloc)
    {
        /* If the ID wasn't cloned do not deallocate it on failure */
        if (pt_Id && !b_CloneId)
        {
            pt_Alloc->pt_Id = NULL;
        }

        /* If the Event wasn't cloned do not deallocate it on failure */
        if (pt_Event && !b_CloneEvent)
        {
            pt_Alloc->pt_Event = NULL;
        }

        /* Destroy anything that might have been allocated.
         * Ignore return status, there is nothing else to do if destroy fails */
        (void)ITC_Stamp_destroy(&pt_Alloc);
        *ppt_Stamp = NULL;
    }

    return t_Status;
}

/**
 * @brief Compare two existing Stamps
 *
 * - If `*pt_Stamp1 < *pt_Stamp2`:
 *      `*pt_Result == ITC_STAMP_COMPARISON_LESS_THAN`
 * - If `*pt_Stamp1 > *pt_Stamp2`:
 *      `*pt_Result == ITC_STAMP_COMPARISON_GREATER_THAN`
 * - If `*pt_Stamp1 == *pt_Stamp2`:
 *      `*pt_Result == ITC_STAMP_COMPARISON_EQUAL`
 * - If `*pt_Stamp1 <> *pt_Stamp2`:
 *      `*pt_Result == ITC_STAMP_COMPARISON_CONCURRENT`
 *
 * @param pt_Stamp1 The first Stamp
 * @param pt_Stamp2 The second Stamp
 * @param pt_Result The result of the comparison
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t compareStamps(
    const ITC_Stamp_t *const pt_Stamp1,
    const ITC_Stamp_t *const pt_Stamp2,
    ITC_Stamp_Comparison_t *pt_Result

)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    bool b_IsLeq12; /* `pt_Stamp1->pt_Event <= pt_Stamp2->pt_Event` */
    bool b_IsLeq21; /* `pt_Stamp2->pt_Event <= pt_Stamp1->pt_Event` */

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Check if `pt_Stamp1->pt_Event <= pt_Stamp2->pt_Event` */
        t_Status = ITC_Event_leq(
            pt_Stamp1->pt_Event, pt_Stamp2->pt_Event, &b_IsLeq12);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Check if `pt_Stamp2->pt_Event <= pt_Stamp1->pt_Event` */
        t_Status = ITC_Event_leq(
            pt_Stamp2->pt_Event, pt_Stamp1->pt_Event, &b_IsLeq21);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        if (b_IsLeq12)
        {
            /* (*pt_Stamp1->pt_Event <= *pt_Stamp2->pt_Event) &&
             * (*pt_Stamp2->pt_Event <= *pt_Stamp1->pt_Event) */
            if (b_IsLeq21)
            {
                *pt_Result = ITC_STAMP_COMPARISON_EQUAL;
            }
            /* (*pt_Stamp1->pt_Event <= *pt_Stamp2->pt_Event) &&
             * (*pt_Stamp2->pt_Event >= *pt_Stamp1->pt_Event) */
            else
            {
                *pt_Result = ITC_STAMP_COMPARISON_LESS_THAN;
            }
        }
        else
        {
            /* (*pt_Stamp1->pt_Event >= *pt_Stamp2->pt_Event) &&
             * (*pt_Stamp2->pt_Event <= *pt_Stamp1->pt_Event) */
            if (b_IsLeq21)
            {
                *pt_Result = ITC_STAMP_COMPARISON_GREATER_THAN;
            }
            /* (*pt_Stamp1->pt_Event >= *pt_Stamp2->pt_Event) &&
             * (*pt_Stamp2->pt_Event >= *pt_Stamp1->pt_Event) */
            else
            {
                *pt_Result = ITC_STAMP_COMPARISON_CONCURRENT;
            }
        }
    }

    return t_Status;
}

/**
 * @brief Serialise an `uint32_t` in network-endian
 *
 * @param u32_Value The value to serialise
 * @param pu8_Buffer The buffer to hold the serialised data
 * @param pu32_BufferSize (in) The size of the buffer in bytes. (out) The size
 * of the data inside the buffer in bytes.
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 * @retval `ITC_STATUS_INSUFFICIENT_RESOURCES` if the buffer is not big enough
 */
static ITC_Status_t u32ToNetwork(
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

/**
 * @brief Deserialise an `uint32_t` from network-endian
 *
 * @param pu8_Buffer The buffer holding the serialised data
 * @param u32_BufferSize The size of the buffer in bytes
 * @param pu32_Value The pointer to the value
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 * @retval `ITC_STATUS_INVALID_PARAM` if `u32_BufferSize > sizeof(uint32_t)`
 */
static ITC_Status_t u32FromNetwork(
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

/**
 * @brief Serialise an existing ITC Stamp
 *
 * Data format:
 *  - Byte 0: The major component of the version of the `libitc` library used to
 *      serialise the data
 *  - Byte 1: The Stamp header.
 *      Contains 2 fields:
 *      - Bits 0 - 2: The length of the `ID component length` field (see below)
 *      - Bits 3 - 5: The length of the `Event component length` field
 *        (see below)
 *      - Bits 6 - 7: Reserved, always 0
 *  - Bytes 2 - 5: ID component length. Can be 1 - 4 bytes long. The length of
 *    this field is encoded in the Stamp header. Serialised in network-endian.
 *  - Bytes (3 - 6) - <ID_END> (see above): The ID tree, **without** a version
 *    field. See ::serialiseId()
 *  - Bytes <ID_END + 1> - <ID_END + 4>: Event component length. Can be 1 - 4
 *    bytes long. The length of this field is encoded in the Stamp header.
 *    Serialised in network-endian.
 *  - Bytes (<ID_END + 2> - <ID_END + 5>) - <EVENT_END> (see above): The Event
 *    tree, **without** a version field. See ::serialiseEvent()
 *
 * @param ppt_Stamp The pointer to the Stamp
 * @param pu8_Buffer The buffer to hold the serialised data
 * @param pu32_BufferSize (in) The size of the buffer in bytes. (out) The size
 * of the data inside the buffer in bytes.
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 * @retval `ITC_STATUS_INSUFFICIENT_RESOURCES` if the buffer is not big enough
 */
static ITC_Status_t serialiseStamp(
    const ITC_Stamp_t *const pt_Stamp,
    uint8_t *const pu8_Buffer,
    uint32_t *const pu32_BufferSize
)
{
    ITC_Status_t t_Status; /* The current status */
    ITC_SerDes_Header_t t_StampHeader = 0;
    uint32_t u32_Offset = 0; /* The current offset into the buffer */
    uint32_t u32_ComponentLength; /* The current serialised component size*/
    /* The serialised `u32_ComponentLength` */
    uint8_t ru8_ComponentLength[sizeof(uint32_t)] = { 0 };
    /* The length of `ru8_ComponentLength` */
    uint32_t u32_ComponentLengthLength;

    /* Add the lib version (provided by build system c args) */
    pu8_Buffer[u32_Offset] = ITC_VERSION_MAJOR;

    /* Increment offset */
    u32_Offset += ITC_VERSION_MAJOR_LEN;

    /* Leave space for the header */
    u32_Offset += sizeof(ITC_SerDes_Header_t);

    /* Calculate the size of the buffer */
    u32_ComponentLength = *pu32_BufferSize - u32_Offset;

    /* Serialise ID component */
    t_Status = ITC_SerDes_Util_serialiseId(
        pt_Stamp->pt_Id,
        &pu8_Buffer[u32_Offset],
        &u32_ComponentLength,
        false);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Set the size of the buffer */
        u32_ComponentLengthLength = sizeof(ru8_ComponentLength);

        /* Serialise the ID component length */
        t_Status = u32ToNetwork(
            u32_ComponentLength,
            &ru8_ComponentLength[0],
            &u32_ComponentLengthLength);
    }

    /* Check there is enough space left in the buffer before continuing */
    if (t_Status == ITC_STATUS_SUCCESS &&
        (*pu32_BufferSize <
         (u32_Offset + u32_ComponentLengthLength + u32_ComponentLength)))
    {
        t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Move contents to free space for the ID component length */
        memmove(
            &pu8_Buffer[u32_Offset + u32_ComponentLengthLength],
            &pu8_Buffer[u32_Offset],
            u32_ComponentLength);

        /* Copy the serialised ID component length */
        memcpy(
            &pu8_Buffer[u32_Offset],
            &ru8_ComponentLength[0],
            u32_ComponentLengthLength);

        /* Set the `ID component length` length in the header */
        t_StampHeader = ITC_SERDES_STAMP_SET_ID_COMPONENT_LEN_LEN(
            t_StampHeader, u32_ComponentLengthLength);

        /* Increment the offset */
        u32_Offset += u32_ComponentLength + u32_ComponentLengthLength;

        /* Set the size of the buffer */
        u32_ComponentLength = *pu32_BufferSize - u32_Offset;

        /* Serialise Event component */
        t_Status = ITC_SerDes_Util_serialiseEvent(
            pt_Stamp->pt_Event,
            &pu8_Buffer[u32_Offset],
            &u32_ComponentLength,
            false);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Set the size of the buffer */
        u32_ComponentLengthLength = sizeof(ru8_ComponentLength);

        /* Serialise the Event component length */
        t_Status = u32ToNetwork(
            u32_ComponentLength,
            &ru8_ComponentLength[0],
            &u32_ComponentLengthLength);
    }

    /* Check there is enough space left in the buffer before continuing */
    if (t_Status == ITC_STATUS_SUCCESS &&
        (*pu32_BufferSize <
         (u32_Offset + u32_ComponentLengthLength + u32_ComponentLength)))
    {
        t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Move contents to free space for the Event component length */
        memmove(
            &pu8_Buffer[u32_Offset + u32_ComponentLengthLength],
            &pu8_Buffer[u32_Offset],
            u32_ComponentLength);

        /* Copy the serialised Event component length */
        memcpy(
            &pu8_Buffer[u32_Offset],
            &ru8_ComponentLength[0],
            u32_ComponentLengthLength);

        /* Set the `Event component length` length in the header */
        t_StampHeader = ITC_SERDES_STAMP_SET_EVENT_COMPONENT_LEN_LEN(
            t_StampHeader, u32_ComponentLengthLength);

        /* Increment the offset */
        u32_Offset += u32_ComponentLength + u32_ComponentLengthLength;

        /* Add the Stamp header */
        pu8_Buffer[ITC_VERSION_MAJOR_LEN] = t_StampHeader;

        /* Return the size of the buffer */
        *pu32_BufferSize = u32_Offset;
    }

    return t_Status;
}

/**
 * @brief Deserialise an ITC Stamp
 *
 * For the expected data format see ::serialiseStamp()
 *
 * @param pu8_Buffer The buffer holding the serialised Stamp data
 * @param u32_BufferSize The size of the buffer in bytes
 * @param ppt_Stamp The pointer to the deserialised Stamp
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t deserialiseStamp(
    const uint8_t *const pu8_Buffer,
    const uint32_t u32_BufferSize,
    ITC_Stamp_t **ppt_Stamp
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_SerDes_Header_t t_StampHeader;
    uint32_t u32_Offset = 0; /* The current offset into the buffer */
    uint32_t u32_ComponentLength; /* The current serialised component size */
    /* The length of the `serialised component length` length */
    uint32_t u32_ComponentLengthLength;

    ITC_Id_t *pt_Id = NULL;
    ITC_Event_t *pt_Event = NULL;

    /* Init stamp */
    *ppt_Stamp = NULL;

    /* Check the lib version (provided by build system c args) */
    if (pu8_Buffer[u32_Offset] != ITC_VERSION_MAJOR)
    {
        t_Status = ITC_STATUS_SERDES_INCOMPATIBLE_LIB_VERSION;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Increment the offset */
        u32_Offset += ITC_VERSION_MAJOR_LEN;

        /* Get the stamp header */
        t_StampHeader = pu8_Buffer[u32_Offset];

        /* This is an invalid header */
        if (t_StampHeader & ~ITC_SERDES_STAMP_HEADER_MASK)
        {
            t_Status = ITC_STATUS_CORRUPT_STAMP;
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Increment the offset */
        u32_Offset += sizeof(ITC_SerDes_Header_t);

        /* Set the size of the buffer */
        u32_ComponentLengthLength =
            ITC_SERDES_STAMP_GET_ID_COMPONENT_LEN_LEN(t_StampHeader);

        if ((u32_ComponentLengthLength < 1) ||
            (u32_ComponentLengthLength > (u32_BufferSize - u32_Offset)))
        {
            t_Status = ITC_STATUS_CORRUPT_STAMP;
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Deserialise the ID component length */
        t_Status = u32FromNetwork(
            &pu8_Buffer[u32_Offset],
            u32_ComponentLengthLength,
            &u32_ComponentLength);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Increment offset */
        u32_Offset += u32_ComponentLengthLength;

        if ((u32_ComponentLength < 1) ||
            (u32_ComponentLength > (u32_BufferSize - u32_Offset)))
        {
            t_Status = ITC_STATUS_CORRUPT_STAMP;
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Deserialise the ID component */
        t_Status = ITC_SerDes_Util_deserialiseId(
            &pu8_Buffer[u32_Offset],
            u32_ComponentLength,
            false,
            &pt_Id);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Increment the offset */
        u32_Offset += u32_ComponentLength;

        /* Set the size of the buffer */
        u32_ComponentLengthLength =
            ITC_SERDES_STAMP_GET_EVENT_COMPONENT_LEN_LEN(t_StampHeader);

        if ((u32_ComponentLengthLength < 1) ||
            (u32_ComponentLengthLength > (u32_BufferSize - u32_Offset)))
        {
            t_Status = ITC_STATUS_CORRUPT_STAMP;
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Deserialise the Event component length */
        t_Status = u32FromNetwork(
            &pu8_Buffer[u32_Offset],
            u32_ComponentLengthLength,
            &u32_ComponentLength);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Increment offset */
        u32_Offset += u32_ComponentLengthLength;

        if ((u32_ComponentLength < 1) ||
            (u32_ComponentLength > (u32_BufferSize - u32_Offset)))
        {
            t_Status = ITC_STATUS_CORRUPT_STAMP;
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Deserialise the Event component */
        t_Status = ITC_SerDes_Util_deserialiseEvent(
            &pu8_Buffer[u32_Offset],
            u32_ComponentLength,
            false,
            &pt_Event);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Increment offset */
        u32_Offset += u32_ComponentLength;

        /* Something has gone wrong, there is still data in the buffer */
        if (u32_Offset < u32_BufferSize)
        {
            t_Status = ITC_STATUS_CORRUPT_STAMP;
        }
    }

    /* Create the Stamp */
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = newStamp(ppt_Stamp, pt_Id, pt_Event, false, false, false);
    }

    if (t_Status != ITC_STATUS_SUCCESS)
    {
        /* There is nothing else to do if a destroy call fails. Also it is more
         * important to convey the deserialisation failed, rather than the
         * destroy, so ignore return statuses */

        if (*ppt_Stamp)
        {
            (void)ITC_Stamp_destroy(ppt_Stamp);
        }
        else
        {
            if (pt_Id)
            {
                (void)ITC_Id_destroy(&pt_Id);
            }
            if (pt_Event)
            {
                (void)ITC_Event_destroy(&pt_Event);
            }
        }
    }

    return t_Status;
}

/******************************************************************************
 * Public functions
 ******************************************************************************/

/******************************************************************************
 * Allocate a new ITC seed Stamp and initialise it
 ******************************************************************************/

ITC_Status_t ITC_Stamp_newSeed(
    ITC_Stamp_t **ppt_Stamp
)
{
    return newStamp(ppt_Stamp, NULL, NULL, false, false, false);
}

/******************************************************************************
 * Allocate a new ITC peek Stamp based on an existing Stamp
 ******************************************************************************/

ITC_Status_t ITC_Stamp_newPeek(
    const ITC_Stamp_t *const pt_Stamp,
    ITC_Stamp_t **ppt_PeekStamp
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    t_Status = validateStamp(pt_Stamp);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = newStamp(
            ppt_PeekStamp, NULL, pt_Stamp->pt_Event, true, false, true);
    }

    return t_Status;
}

/******************************************************************************
 * Free an allocated ITC Stamp
 ******************************************************************************/

ITC_Status_t ITC_Stamp_destroy(
    ITC_Stamp_t **ppt_Stamp
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Status_t t_FreeStatus = ITC_STATUS_SUCCESS; /* The last free status */

    if (!ppt_Stamp || !(*ppt_Stamp))
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        if ((*ppt_Stamp)->pt_Event)
        {
            /* Destroy the Event tree */
            t_FreeStatus = ITC_Event_destroy(&(*ppt_Stamp)->pt_Event);

            if (t_FreeStatus != ITC_STATUS_SUCCESS)
            {
                t_Status = t_FreeStatus;
            }
        }

        if ((*ppt_Stamp)->pt_Id)
        {
            /* Destroy the ID tree */
            t_FreeStatus = ITC_Id_destroy(&(*ppt_Stamp)->pt_Id);

            if (t_FreeStatus != ITC_STATUS_SUCCESS)
            {
                t_Status = t_FreeStatus;
            }
        }

        t_FreeStatus = ITC_Port_free(*ppt_Stamp);

        if (t_FreeStatus != ITC_STATUS_SUCCESS)
        {
            t_Status = t_FreeStatus;
        }
    }

    if (t_Status != ITC_STATUS_INVALID_PARAM)
    {
        /* Sanitize the freed pointer regardless of the exit status */
        *ppt_Stamp = NULL;
    }

    return t_Status;
}

/******************************************************************************
 * Clone an existing ITC Stamp
 ******************************************************************************/

ITC_Status_t ITC_Stamp_clone(
    const ITC_Stamp_t *const pt_Stamp,
    ITC_Stamp_t **ppt_ClonedStamp
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    t_Status = validateStamp(pt_Stamp);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = newStamp(
            ppt_ClonedStamp,
            pt_Stamp->pt_Id,
            pt_Stamp->pt_Event,
            false,
            true,
            true);
    }

    return t_Status;
}

/******************************************************************************
 * Fork an existing Stamp
 ******************************************************************************/

ITC_Status_t ITC_Stamp_fork(
    const ITC_Stamp_t *const pt_Stamp,
    ITC_Stamp_t **ppt_Stamp1,
    ITC_Stamp_t **ppt_Stamp2
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Id_t *pt_SplitId1;
    ITC_Id_t *pt_SplitId2;

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateStamp(pt_Stamp);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Split the ID */
        t_Status = ITC_Id_split(pt_Stamp->pt_Id, &pt_SplitId1, &pt_SplitId2);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Create the first stamp */
        t_Status = newStamp(
            ppt_Stamp1, pt_SplitId1, pt_Stamp->pt_Event, false, false, true);

        if (t_Status != ITC_STATUS_SUCCESS)
        {
            /* Destroy the IDs.
             * Ignore the return status. There is nothing else to do if
             * destroy fails and its more important to return the reason
             * for the failure */
            (void)ITC_Id_destroy(&pt_SplitId1);
            (void)ITC_Id_destroy(&pt_SplitId2);
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Create the second stamp */
        t_Status = newStamp(
            ppt_Stamp2, pt_SplitId2, pt_Stamp->pt_Event, false, false, true);

        if (t_Status != ITC_STATUS_SUCCESS)
        {
            /* Destroy the other Stamp and the ID.
             * Ignore the return status. There is nothing else to do if
             * destroy fails and its more important to return the reason
             * for the failure */
            (void)ITC_Stamp_destroy(ppt_Stamp1);
            (void)ITC_Id_destroy(&pt_SplitId2);
        }
    }

    return t_Status;
}

/******************************************************************************
 * Add a new Event to the Stamp
 ******************************************************************************/

ITC_Status_t ITC_Stamp_event(
    const ITC_Stamp_t *const pt_Stamp
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    bool b_WasFilled = false;

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateStamp(pt_Stamp);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = ITC_Event_fill(
            pt_Stamp->pt_Event, pt_Stamp->pt_Id, &b_WasFilled);
    }

    if (t_Status == ITC_STATUS_SUCCESS && !b_WasFilled)
    {
        t_Status = ITC_Event_grow(pt_Stamp->pt_Event, pt_Stamp->pt_Id);
    }

    return t_Status;
}

/******************************************************************************
 * Join two existing Stamps
 ******************************************************************************/

ITC_Status_t ITC_Stamp_join(
    const ITC_Stamp_t *const pt_Stamp1,
    const ITC_Stamp_t *const pt_Stamp2,
    ITC_Stamp_t **ppt_Stamp
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Id_t *pt_SummedId;
    ITC_Event_t *pt_JoinedEvent;

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateStamp(pt_Stamp1);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateStamp(pt_Stamp2);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = ITC_Id_sum(pt_Stamp1->pt_Id, pt_Stamp2->pt_Id, &pt_SummedId);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = ITC_Event_join(
            pt_Stamp1->pt_Event, pt_Stamp2->pt_Event, &pt_JoinedEvent);

        if (t_Status != ITC_STATUS_SUCCESS)
        {
            /* Destroy the summed ID.
             * Ignore the return status. There is nothing else to do if
             * destroy fails and its more important to return the reason
             * for the failure */
            (void)ITC_Id_destroy(&pt_SummedId);
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = newStamp(
            ppt_Stamp, pt_SummedId, pt_JoinedEvent, false, false, false);

        if (t_Status != ITC_STATUS_SUCCESS)
        {
            /* Destroy the summed ID and the joined Event.
             * Ignore the return status. There is nothing else to do if
             * destroy fails and its more important to return the reason
             * for the failure */
            (void)ITC_Id_destroy(&pt_SummedId);
            (void)ITC_Event_destroy(&pt_JoinedEvent);
        }
    }

    return t_Status;
}

/******************************************************************************
 * Compare two existing Stamps
 ******************************************************************************/

ITC_Status_t ITC_Stamp_compare(
    const ITC_Stamp_t *const pt_Stamp1,
    const ITC_Stamp_t *const pt_Stamp2,
    ITC_Stamp_Comparison_t *pt_Result

)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    if (!pt_Result)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateStamp(pt_Stamp1);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateStamp(pt_Stamp2);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Compare *pt_Stamp1 to *pt_Stamp2 */
        t_Status = compareStamps(pt_Stamp1, pt_Stamp2, pt_Result);
    }

    return t_Status;
}

/******************************************************************************
 * Serialise an existing ITC Stamp
 ******************************************************************************/

ITC_Status_t ITC_SerDes_serialiseStamp(
    const ITC_Stamp_t *const pt_Stamp,
    uint8_t *const pu8_Buffer,
    uint32_t *const pu32_BufferSize
)
{
    ITC_Status_t t_Status; /* The current status */

    t_Status = ITC_SerDes_Util_validateBuffer(
        pu8_Buffer,
        pu32_BufferSize,
        ITC_SERDES_STAMP_MIN_BUFFER_LEN + ITC_VERSION_MAJOR_LEN,
        true);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateStamp(pt_Stamp);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = serialiseStamp(pt_Stamp, pu8_Buffer, pu32_BufferSize);
    }

    return t_Status;
}

/******************************************************************************
 * Deserialise an ITC Stamp
 ******************************************************************************/

ITC_Status_t ITC_SerDes_deserialiseStamp(
    const uint8_t *const pu8_Buffer,
    const uint32_t u32_BufferSize,
    ITC_Stamp_t **ppt_Stamp
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS;

    if (!ppt_Stamp)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = ITC_SerDes_Util_validateBuffer(
            pu8_Buffer,
            &u32_BufferSize,
            ITC_SERDES_STAMP_MIN_BUFFER_LEN + ITC_VERSION_MAJOR_LEN,
            false);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = deserialiseStamp(pu8_Buffer, u32_BufferSize, ppt_Stamp);
    }

    return t_Status;
}

/******************************************************************************
 * Explode an existing Stamp into its core components - an ID and Event
 ******************************************************************************/

ITC_Status_t ITC_Stamp_explode(
    const ITC_Stamp_t *const pt_Stamp,
    ITC_Id_t **ppt_Id,
    ITC_Event_t **ppt_Event
)
{
    ITC_Status_t t_Status; /* The current status */

    t_Status = validateStamp(pt_Stamp);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = ITC_Id_clone(pt_Stamp->pt_Id, ppt_Id);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = ITC_Event_clone(pt_Stamp->pt_Event, ppt_Event);
    }

    return t_Status;
}

/******************************************************************************
 * Rebuild a Stamp from its core components - an ID and Event
 ******************************************************************************/

ITC_Status_t ITC_Stamp_rebuild(
    ITC_Id_t *const pt_Id,
    ITC_Event_t *const pt_Event,
    ITC_Stamp_t **ppt_Stamp
)
{
    ITC_Status_t t_Status; /* The current status */

    t_Status = ITC_Id_validate(pt_Id);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = ITC_Event_validate(pt_Event);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = newStamp(ppt_Stamp, pt_Id, pt_Event, false, true, true);
    }

    return t_Status;
}
