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
#include "ITC_SerDes_package.h"
#include "ITC_SerDes_private.h"

#include "ITC_Event.h"
#include "ITC_Id.h"
#include "ITC_Stamp.h"

#include "ITC_Id_package.h"
#include "ITC_Id_private.h"

#include <stddef.h>

/******************************************************************************
 * Private functions
 ******************************************************************************/

/**
 * @brief Validate serialization/deserialization buffer
 *
 * @param pu8_Buffer The buffer
 * @param pu32_BufferSize The buffer size
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t validateBuffer(
    const uint8_t *pu8_Buffer,
    const uint32_t *pu32_BufferSize
)
{
    if (!pu8_Buffer || !pu32_BufferSize)
    {
        return ITC_STATUS_INVALID_PARAM;
    }

    return ITC_STATUS_SUCCESS;
}

/**
 * @brief Serialise an existing ITC Id
 *
 * @param ppt_Id The pointer to the Id
 * @param pu8_Buffer The buffer to hold the serialised data
 * @param pu32_BufferSize (in) The size of the buffer in bytes. (out) The size
 * of the data inside the buffer in bytes.
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t serialiseId(
    const ITC_Id_t *pt_Id,
    uint8_t *pu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    const ITC_Id_t *pt_CurrentIdParent = NULL; /* The parent of the current ID*/
    const ITC_Id_t *pt_RootIdParent = NULL; /* The parent of the root node */
    uint32_t u32_Offset = 0; /* The current offset */

    /* Remember the root parent as this might be a subtree */
    pt_RootIdParent = pt_Id->pt_Parent;

    /* Perform a pre-order traversal */
    while (pt_Id && t_Status == ITC_STATUS_SUCCESS)
    {
        if (u32_Offset + sizeof(ITC_SerDes_Header_t) > *pu32_BufferSize)
        {
            t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
        }
        else
        {
            /* Create the header */
            pu8_Buffer[u32_Offset] =
                (ITC_ID_IS_LEAF_ID(pt_Id))
                    ? ((pt_Id->b_IsOwner) ? ITC_SERDES_SEED_ID_HEADER
                                          : ITC_SERDES_NULL_ID_HEADER)
                    : ITC_SERDES_PARENT_ID_HEADER;

            /* Increment the offset */
            u32_Offset += sizeof(ITC_SerDes_Header_t);

            /* Descend into left tree */
            if (pt_Id->pt_Left)
            {
                pt_Id = pt_Id->pt_Left;
            }
            /* Valid parent ITC ID trees always have both left and right
            * nodes. Thus, there is no need to check if the current node
            * doesn't have a left child but has a right one.
            *
            * Instead directly start backtracking up the tree */
            else
            {
                /* Remember the parent */
                pt_CurrentIdParent = pt_Id->pt_Parent;

                /* Loop until the current element is no longer reachable
                * through the parent's right child */
                while (pt_CurrentIdParent != pt_RootIdParent &&
                    pt_CurrentIdParent->pt_Right == pt_Id)
                {
                    pt_Id = pt_Id->pt_Parent;
                    pt_CurrentIdParent = pt_CurrentIdParent->pt_Parent;
                }

                /* There is a right subtree that has not been explored yet */
                if (pt_CurrentIdParent != pt_RootIdParent)
                {
                    pt_Id = pt_CurrentIdParent->pt_Right;
                }
                else
                {
                    pt_Id = NULL;
                }
            }
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Return the size of the data in the buffer */
        *pu32_BufferSize = u32_Offset;
    }

    return t_Status;
}

/******************************************************************************
 * Public functions
 ******************************************************************************/

/******************************************************************************
 * Serialise an existing ITC Id
 ******************************************************************************/

ITC_Status_t ITC_SerDes_serialiseId(
    const ITC_Id_t *pt_Id,
    uint8_t *pu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    ITC_Status_t t_Status; /* The current status */

    t_Status = ITC_Id_validate(pt_Id);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateBuffer(pu8_Buffer, pu32_BufferSize);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = serialiseId(pt_Id, pu8_Buffer, pu32_BufferSize);
    }

    return t_Status;
}

/******************************************************************************
 * Deserialise an ITC Id
 ******************************************************************************/

ITC_Status_t ITC_SerDes_deserialiseId(
    const uint8_t *pu8_Buffer,
    const uint32_t u32_BufferSize,
    ITC_Id_t **ppt_Id
)
{
    return ITC_STATUS_FAILURE;
}

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
