/**
 * @file ITC_Id.c
 * @brief Implementation of the Interval Tree Clock's ID mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#include "ITC_Id.h"
#include "ITC_Id_package.h"
#include "ITC_Id_private.h"
#include "ITC_Config.h"

#include "ITC_SerDes_private.h"
#include "ITC_SerDes_Util_package.h"

#if ITC_CONFIG_ENABLE_EXTENDED_API
#include "ITC_SerDes.h"
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */

#if !(ITC_CONFIG_ENABLE_SERIALISE_TO_STRING_API && ITC_CONFIG_ENABLE_EXTENDED_API)
#include "ITC_SerDes_package.h"
#endif /* !(ITC_CONFIG_ENABLE_SERIALISE_TO_STRING_API && ITC_CONFIG_ENABLE_EXTENDED_API) */

#include "ITC_Port.h"

#include <stdbool.h>
#include <stddef.h>

/******************************************************************************
 * Private functions
 ******************************************************************************/

/**
 * @brief Validate an existing ITC ID
 *
 * Should be used to validate all incoming IDs before any processing is done.
 *
 * @param pt_Id The ID to validate
 * @param b_CheckIsNormalised Whether to check if the ID is normalised
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t validateId(
    const ITC_Id_t *pt_Id,
    const bool b_CheckIsNormalised
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    const ITC_Id_t *pt_CurrentIdParent = NULL; /* The current ID node */

    if(!pt_Id)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    /* Subtrees are considered invalid when coming through the public API */
    else if (pt_Id->pt_Parent)
    {
        t_Status = ITC_STATUS_CORRUPT_ID;
    }

    /* Perform a pre-order traversal */
    while (t_Status == ITC_STATUS_SUCCESS && pt_Id)
    {
        /* Checks:
         *  - The parent pointer must match pt_CurrentIdParent.
         *  - Must be a leaf or a valid parent node
         *  - Must be a normalised ID node (if the check is enabled)
         */
        if (pt_CurrentIdParent != pt_Id->pt_Parent ||
            (!ITC_ID_IS_LEAF_ID(pt_Id) &&
             !ITC_ID_IS_VALID_PARENT(pt_Id)) ||
            (b_CheckIsNormalised && !ITC_ID_IS_NORMALISED_ID(pt_Id)))
        {
            t_Status = ITC_STATUS_CORRUPT_ID;
        }
        else
        {
            /* Descend into left tree */
            if (pt_Id->pt_Left)
            {
                /* Remember the parent address */
                pt_CurrentIdParent = pt_Id;

                pt_Id = pt_Id->pt_Left;
            }
            /* ITC trees must always have both left and right subtrees or
             * be leafs. If this is reached, then a node is missing its
             * left subtree, which makes the tree invalid. Usually this will
             * be caught in the `if` at the beginning of the loop, but do check
             * again just in case */
            else if (pt_Id->pt_Right)
            {
                t_Status = ITC_STATUS_CORRUPT_ID;
            }
            else
            {
                /* Loop until the current element is no longer reachable
                 * through the parent's right child */
                while (pt_CurrentIdParent &&
                       pt_CurrentIdParent->pt_Right == pt_Id)
                {
                    pt_Id = pt_Id->pt_Parent;
                    pt_CurrentIdParent = pt_CurrentIdParent->pt_Parent;
                }

                /* There is a right subtree that has not been explored yet */
                if (pt_CurrentIdParent)
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

    return t_Status;
}

/**
 * @brief Allocate a new ITC ID
 *
 * @param ppt_Id (out) The pointer to the new ID
 * @param ppt_Parent The pointer to the parent ID in the tree. Otherwise NULL.
 * @param b_IsOwner Whether the ID owns its interval or not.
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t newId(
    ITC_Id_t **const ppt_Id,
    ITC_Id_t *const pt_Parent,
    const bool b_IsOwner
)
{
    ITC_Status_t t_Status; /* The current status */
    ITC_Id_t *pt_Alloc;

    t_Status = ITC_Port_malloc((void **)&pt_Alloc, ITC_PORT_ALLOCTYPE_ITC_ID_T);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Initialise members */
        pt_Alloc->b_IsOwner = b_IsOwner;
        pt_Alloc->pt_Parent = pt_Parent;
        pt_Alloc->pt_Left = NULL;
        pt_Alloc->pt_Right = NULL;

        /* Return the pointer to the allocated memory */
        *ppt_Id = pt_Alloc;
    }
    else
    {
        /* Sanitise pointer */
        *ppt_Id = NULL;
    }

    return t_Status;
}

/**
 * @brief Clone an existing ITC ID
 *
 * @note Memory for the new ITC ID will be dynamically allocated.
 * On error, the cloned ID is automatically deallocated.
 * @param pt_Id The existing ID
 * @param ppt_ClonedId The pointer to the cloned ID
 * @param pt_ParentId The pointer to parent ID. Otherwise NULL
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t cloneId(
    const ITC_Id_t *pt_Id,
    ITC_Id_t **ppt_ClonedId,
    ITC_Id_t *const pt_ParentId
)
{
    ITC_Status_t t_Status; /* The current status */
    const ITC_Id_t *pt_RootIdParent; /* The parent of the root */
    ITC_Id_t *pt_CurrentIdClone; /* The current ID clone */

    /* Init clone pointer */
    *ppt_ClonedId = NULL;
    /* Remember the parent of the root as this might be a subree */
    pt_RootIdParent = pt_Id->pt_Parent;

    /* Allocate the root */
    t_Status = newId(
        ppt_ClonedId, pt_ParentId, pt_Id->b_IsOwner);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Initialise the cloned root pointer */
        pt_CurrentIdClone = *ppt_ClonedId;
    }

    while(t_Status == ITC_STATUS_SUCCESS &&
            pt_Id != pt_RootIdParent)
    {
        if (pt_Id->pt_Left && !pt_CurrentIdClone->pt_Left)
        {
            /* Allocate left subtree */
            t_Status = newId(
                &pt_CurrentIdClone->pt_Left,
                pt_CurrentIdClone,
                pt_Id->pt_Left->b_IsOwner);

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                /* Descend into the left child */
                pt_Id = pt_Id->pt_Left;
                pt_CurrentIdClone = pt_CurrentIdClone->pt_Left;
            }
        }
        else if (pt_Id->pt_Right && !pt_CurrentIdClone->pt_Right)
        {
            /* Allocate right subtree */
            t_Status = newId(
                &pt_CurrentIdClone->pt_Right,
                pt_CurrentIdClone,
                pt_Id->pt_Right->b_IsOwner);

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                /* Descend into the right child */
                pt_Id = pt_Id->pt_Right;
                pt_CurrentIdClone = pt_CurrentIdClone->pt_Right;
            }
        }
        else
        {
            /* Go up the tree */
            pt_Id = pt_Id->pt_Parent;
            pt_CurrentIdClone = pt_CurrentIdClone->pt_Parent;
        }
    }

    /* If something goes wrong during the cloning - the ID is invalid and must
     * not be used. */
    if (t_Status != ITC_STATUS_SUCCESS)
    {
        /* There is nothing else to do if the cloning fails. Also it is more
         * important to convey the cloning failed, rather than the destroy */
        (void)ITC_Id_destroy(ppt_ClonedId);
    }

    return t_Status;
}

/**
 * @brief Splits a NULL ID into 2 new IDs fulfilling `split(0)`
 * Rules:
 *  - split(0) = (0, 0)`
 *
 * @param ppt_Id1 (out) The first ID
 * @param pt_ParentId1 The parent of ID1. Otherwise NULL
 * @param ppt_Id2 (out) The second ID
 * @param pt_ParentId2 The parent of ID2. Otherwise NULL
 * @return ITC_Status_t
 */
static ITC_Status_t splitId0(
    ITC_Id_t **ppt_Id1,
    ITC_Id_t *const pt_ParentId1,
    ITC_Id_t **ppt_Id2,
    ITC_Id_t *const pt_ParentId2
)
{
    ITC_Status_t t_Status; /* The current status */

    /* Init IDs */
    *ppt_Id1 = NULL;
    *ppt_Id2 = NULL;

    t_Status = newId(ppt_Id1, pt_ParentId1, false);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = newId(ppt_Id2, pt_ParentId2, false);
    }

    return t_Status;
}

/**
 * @brief Splits a seed ID into 2 new IDs fulfilling `split(1)`
 * Rules
 *  - split(1) = ((1, 0), (0, 1))`
 *
 * @param ppt_Id1 (out) The first ID
 * @param pt_ParentId1 The parent of ID1. Otherwise NULL
 * @param ppt_Id2 (out) The second ID
 * @param pt_ParentId2 The parent of ID2. Otherwise NULL
 * @return ITC_Status_t
 */
static ITC_Status_t splitId1(
    ITC_Id_t **ppt_Id1,
    ITC_Id_t *const pt_ParentId1,
    ITC_Id_t **ppt_Id2,
    ITC_Id_t *const pt_ParentId2
)
{
    ITC_Status_t t_Status; /* The current status */

    /* Init IDs */
    *ppt_Id1 = NULL;
    *ppt_Id2 = NULL;

    /* Allocate the first root */
    t_Status = newId(ppt_Id1, pt_ParentId1, false);

    /* Allocate the children for the first root: (1, 0) */
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = newId(&(*ppt_Id1)->pt_Left, *ppt_Id1, true);
    }
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = newId(&(*ppt_Id1)->pt_Right, *ppt_Id1, false);
    }

    /* Allocate the second root */
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = newId(ppt_Id2, pt_ParentId2, false);
    }

    /* Allocate the children for the second root: (0, 1) */
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = newId(&(*ppt_Id2)->pt_Left, *ppt_Id2, false);
    }
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = newId(&(*ppt_Id2)->pt_Right, *ppt_Id2, true);
    }

    return t_Status;
}

/**
 * @brief Splits an ID into 2 new IDs fulfilling `split(i)`
 * Rules:
 *  - split(0) = (0, 0)
 *  - split(1) = ((1, 0), (0, 1))
 *  - split((0, i)) = ((0, i1), (0, i2)), where (i1, i2) = split(i)
 *  - split((i, 0)) = ((i1, 0), (i2, 0)), where (i1, i2) = split(i)
 *  - split((i1, i2)) = ((i1, 0), (0, i2))
 *
 * @param pt_Id The existing ID
 * @param ppt_Id1 The first ID
 * @param ppt_Id2 The second ID
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t splitIdI(
    const ITC_Id_t *pt_Id,
    ITC_Id_t **ppt_Id1,
    ITC_Id_t **ppt_Id2
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    ITC_Id_t **ppt_CurrentId1 = ppt_Id1;
    ITC_Id_t *pt_ParentCurrentId1 = NULL;
    ITC_Id_t **ppt_CurrentId2 = ppt_Id2;
    ITC_Id_t *pt_ParentCurrentId2 = NULL;

    const ITC_Id_t *pt_CurrentIdParent;

    /* Remember the parent of the root as this might be a subtree */
    pt_CurrentIdParent = pt_Id->pt_Parent;

    /* Init the new IDs */
    *ppt_CurrentId1 = NULL;
    *ppt_CurrentId2 = NULL;

    while (t_Status == ITC_STATUS_SUCCESS && pt_Id != pt_CurrentIdParent)
    {
        /* split(0) = (0, 0) */
        if (ITC_ID_IS_NULL_ID(pt_Id))
        {
            t_Status = splitId0(
                ppt_CurrentId1,
                pt_ParentCurrentId1,
                ppt_CurrentId2,
                pt_ParentCurrentId2);

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                pt_Id = pt_Id->pt_Parent;
            }
        }
        /* split(1) = ((1, 0), (0, 1)) */
        else if (ITC_ID_IS_SEED_ID(pt_Id))
        {
            t_Status = splitId1(
                ppt_CurrentId1,
                pt_ParentCurrentId1,
                ppt_CurrentId2,
                pt_ParentCurrentId2);

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                pt_Id = pt_Id->pt_Parent;
            }
        }
        /* split(0, i), split(i, 0), split(i1, i2) */
        else
        {
            /* Create left child container.
             * This might exist from a previous iteration. This is fine. */
            if(!(*ppt_CurrentId1))
            {
                t_Status = newId(
                    ppt_CurrentId1, pt_ParentCurrentId1, false);
            }

            /* Create right child container.
             * This might exist from a previous iteration. This is fine. */
            if(t_Status == ITC_STATUS_SUCCESS && !(*ppt_CurrentId2))
            {
                t_Status = newId(
                    ppt_CurrentId2, pt_ParentCurrentId2, false);
            }

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                /* split((0, i)) = ((0, i1), (0, i2)), where (i1, i2) = split(i)
                 */
                if (ITC_ID_IS_NULL_ID(pt_Id->pt_Left))
                {
                    /* Create the chilren of the children.
                     * This happens the first time the current pt_Id is reached
                     */
                    if (ITC_ID_IS_LEAF_ID(*ppt_CurrentId1) &&
                        ITC_ID_IS_LEAF_ID(*ppt_CurrentId2))
                    {
                        t_Status = newId(
                            &(*ppt_CurrentId1)->pt_Left,
                            *ppt_CurrentId1,
                            false);

                        if(t_Status == ITC_STATUS_SUCCESS)
                        {
                            t_Status = newId(
                                &(*ppt_CurrentId2)->pt_Left,
                                *ppt_CurrentId2,
                                false);
                        }

                        if (t_Status == ITC_STATUS_SUCCESS)
                        {
                            /* - Descend into pt_Id->pt_Right
                             * - Set the current iteration children as the next
                             *   iteration parents
                             * - Set the right child of the current iteration's
                             *   children as the next iteration's children
                             */
                            pt_Id = pt_Id->pt_Right;
                            pt_ParentCurrentId1 = *ppt_CurrentId1;
                            ppt_CurrentId1 = &(*ppt_CurrentId1)->pt_Right;
                            pt_ParentCurrentId2 = *ppt_CurrentId2;
                            ppt_CurrentId2 = &(*ppt_CurrentId2)->pt_Right;
                        }
                    }
                    /* Children already exist and the current pt_Id has a parent
                     * This happens on the next iteration after the children
                     * have been created.
                     */
                    else
                    {
                        pt_Id = pt_Id->pt_Parent;
                    }
                }
                /* split((i, 0)) = ((i1, 0), (i2, 0)), where (i1, i2) = split(i)
                 */
                else if (ITC_ID_IS_NULL_ID(pt_Id->pt_Right))
                {
                    /* Create the chilren of the children.
                     * This happens the first time the current pt_Id is reached
                     */
                    if (ITC_ID_IS_LEAF_ID(*ppt_CurrentId1) &&
                        ITC_ID_IS_LEAF_ID(*ppt_CurrentId2))
                    {
                        t_Status = newId(
                            &(*ppt_CurrentId1)->pt_Right,
                            *ppt_CurrentId1,
                            false);

                        if(t_Status == ITC_STATUS_SUCCESS)
                        {
                            t_Status = newId(
                                &(*ppt_CurrentId2)->pt_Right,
                                *ppt_CurrentId2,
                                false);
                        }

                        if (t_Status == ITC_STATUS_SUCCESS)
                        {
                            /* - Descend into pt_Id->pt_Left
                             * - Set the current iteration children as the next
                             *   iteration parents
                             * - Set the left child of the current iteration's
                             *   children as the next iteration's children
                             */
                            pt_Id = pt_Id->pt_Left;
                            pt_ParentCurrentId1 = *ppt_CurrentId1;
                            ppt_CurrentId1 = &(*ppt_CurrentId1)->pt_Left;
                            pt_ParentCurrentId2 = *ppt_CurrentId2;
                            ppt_CurrentId2 = &(*ppt_CurrentId2)->pt_Left;
                        }
                    }
                    /* Children already exist and the current pt_Id has a parent
                     * This happens on the next iteration after the children
                     * have been created.
                     */
                    else
                    {
                        pt_Id = pt_Id->pt_Parent;
                    }
                }
                /* split((i1, i2)) = ((i1, 0), (0, i2)) */
                else
                {
                    t_Status = newId(
                        &(*ppt_CurrentId1)->pt_Right,
                        *ppt_CurrentId1,
                        false);

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        t_Status = cloneId(
                            (const ITC_Id_t *const)pt_Id->pt_Left,
                            &(*ppt_CurrentId1)->pt_Left,
                            *ppt_CurrentId1);
                    }

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        t_Status = newId(
                            &(*ppt_CurrentId2)->pt_Left,
                            *ppt_CurrentId2,
                            false);
                    }

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        t_Status = cloneId(
                            (const ITC_Id_t *const)pt_Id->pt_Right,
                            &(*ppt_CurrentId2)->pt_Right,
                            *ppt_CurrentId2);
                    }

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        pt_Id = pt_Id->pt_Parent;
                    }
                }
            }
        }
    }

    /* If something goes wrong during the splitting - the IDs are invalid and
     * must not be used. */
    if (t_Status != ITC_STATUS_SUCCESS)
    {
        /* There is nothing else to do if the destroy fails. Also it is more
         * important to convey the split failed, rather than the destroy */
        (void)ITC_Id_destroy(ppt_Id1);
        (void)ITC_Id_destroy(ppt_Id2);
    }

    return t_Status;
}

/**
 * @brief Normalise a (1, 1) or (0, 0) ID
 *
 * @note It is assumed the child ID nodes are leafs, have not bee freed and
 * and have the same ownership - i.e (1, 1) or (0, 0)
 *
 * @param pt_Id The ID on which to perform the operation
 * children
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t normId11Or00(
    ITC_Id_t *pt_Id
)
{
    ITC_Status_t t_Status; /* The current status */

    /* Update the ownership */
    pt_Id->b_IsOwner = pt_Id->pt_Left->b_IsOwner;

    /* Destroy the left leaf child */
    t_Status = ITC_Id_destroy(&pt_Id->pt_Left);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Destroy the right leaf child */
        t_Status = ITC_Id_destroy(&pt_Id->pt_Right);
    }

    return t_Status;
}


/**
 * @brief Normalise an ID fulfilling `norm(i)`
 * Rules:
 *  - norm(0, 0) = 0
 *  - norm(1, 1) = 1
 *  - norm(i) = i
 *
 * @param pt_Id The ID to normalise
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t normIdI(
    ITC_Id_t *pt_Id
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    /* Remember the parent as this might be a subtree */
    const ITC_Id_t *pt_RootIdParent = pt_Id->pt_Parent;

    /* Start from the left most child */
    while (pt_Id->pt_Left)
    {
        pt_Id = pt_Id->pt_Left;
    }

    /* Perform a post-order traversal */
    while(t_Status == ITC_STATUS_SUCCESS && pt_Id)
    {
        /* If pt_Id is the root node (no parent) - break */
        if(pt_Id->pt_Parent == pt_RootIdParent)
        {
            pt_Id = NULL;
        }
        /* If pt_Id is the right child of its parent */
        else if (pt_Id->pt_Parent->pt_Right == pt_Id)
        {
            pt_Id = pt_Id->pt_Parent;
        }
        /* Current pt_Id is the left child of its parent */
        else
        {
            /* Go to the right child of the pt_Id parent */
            pt_Id = pt_Id->pt_Parent->pt_Right;

            /* Find the left most node */
            while (pt_Id && ITC_ID_IS_PARENT_ID(pt_Id))
            {
                /* Go to the left child if there is one, otherwise go to the
                 * right child */
                pt_Id = (pt_Id->pt_Left) ? pt_Id->pt_Left : pt_Id->pt_Right;
            }
        }

        /* norm(1, 1) = 1 or norm(0, 0) = 0 */
        if (ITC_ID_IS_SEED_SEED_ID(pt_Id) || ITC_ID_IS_NULL_NULL_ID(pt_Id))
        {
            t_Status = normId11Or00(pt_Id);
        }
    }

    return t_Status;
}

/**
 * @brief Sum two IDs into a new ID fulfilling `sum(i1, i2)`
 * Rules:
 *  - sum(0, i) = i
 *  - sum(i, 0) = i
 *  - sum((l1, r1), (l2, r2)) = norm(sum(l1, l2), sum(r1, r2))
 *
 * @param pt_Id1 The first ID
 * @param pt_Id2 The second ID
 * @param ppt_Id The new ID
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t sumIdI(
    const ITC_Id_t *pt_Id1,
    const ITC_Id_t *pt_Id2,
    ITC_Id_t **ppt_Id
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    ITC_Id_t **ppt_CurrentId = ppt_Id;
    ITC_Id_t *pt_CurrentIdParent = NULL;
    const ITC_Id_t *pt_RootId1Parent;
    const ITC_Id_t *pt_RootId2Parent;

    /* Init the ID */
    *ppt_CurrentId = NULL;

    /* Remember the root parent as this might be a subtree */
    pt_RootId1Parent = pt_Id1->pt_Parent;
    pt_RootId2Parent = pt_Id2->pt_Parent;

    while(t_Status == ITC_STATUS_SUCCESS &&
          pt_Id1 != pt_RootId1Parent &&
          pt_Id2 != pt_RootId2Parent)
    {
        /* sum((l1, r1), (l2, r2)) = norm(sum(l1, l2), sum(r1, r2)) */
        if(ITC_ID_IS_PARENT_ID(pt_Id1) &&
           ITC_ID_IS_PARENT_ID(pt_Id2))
        {
            /* Create the parent node.
             * This might exist from a previous iteration. This is fine. */
            if(!(*ppt_CurrentId))
            {
                t_Status = newId(ppt_CurrentId, pt_CurrentIdParent, false);
            }

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                /* Descend into left child  */
                if(!(*ppt_CurrentId)->pt_Left)
                {
                    /* Save the parent pointer on the stack */
                    pt_CurrentIdParent = *ppt_CurrentId;

                    ppt_CurrentId = &(*ppt_CurrentId)->pt_Left;
                    pt_Id1 = pt_Id1->pt_Left;
                    pt_Id2 = pt_Id2->pt_Left;
                }
                /* Descend into right child  */
                else if(!(*ppt_CurrentId)->pt_Right)
                {
                    /* Save the parent pointer on the stack */
                    pt_CurrentIdParent = *ppt_CurrentId;

                    ppt_CurrentId = &(*ppt_CurrentId)->pt_Right;
                    pt_Id1 = pt_Id1->pt_Right;
                    pt_Id2 = pt_Id2->pt_Right;
                }
                /* Normalise ID and climb back to parent */
                else
                {
                    /* Normalise ID.
                    * This may destroy all child nodes stored under
                    * *ppt_CurrentId
                    */
                    t_Status = normIdI(*ppt_CurrentId);

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        /* Save the parent pointer on the stack */
                        pt_CurrentIdParent = (*ppt_CurrentId)->pt_Parent;

                        /* Climb back to the parent node */
                        ppt_CurrentId = &pt_CurrentIdParent;
                        pt_Id1 = pt_Id1->pt_Parent;
                        pt_Id2 = pt_Id2->pt_Parent;
                    }
                }
            }
        }
        /* sum(0, i) = i */
        else if (ITC_ID_IS_NULL_ID(pt_Id1))
        {
            t_Status = cloneId(
                pt_Id2, ppt_CurrentId, pt_CurrentIdParent);

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                /* Climb back to the parent node
                * Use the parent pointer saved on the stack instead of
                * `(*ppt_CurrentId)->pt_Parent` as that will be the child
                * element on the next iteration and may get destroyed by
                * `normIdI`
                */
                ppt_CurrentId = &pt_CurrentIdParent;
                pt_Id1 = pt_Id1->pt_Parent;
                pt_Id2 = pt_Id2->pt_Parent;
            }
        }
        /* sum(i, 0) = i */
        else if (ITC_ID_IS_NULL_ID(pt_Id2))
        {

            t_Status = cloneId(
                pt_Id1, ppt_CurrentId, pt_CurrentIdParent);

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                /* Climb back to the parent node
                * Use the parent pointer saved on the stack instead of
                * `(*ppt_CurrentId)->pt_Parent` as that will be the child
                * element on the next iteration and may get destroyed by
                * `normIdI`
                */
                ppt_CurrentId = &pt_CurrentIdParent;
                pt_Id1 = pt_Id1->pt_Parent;
                pt_Id2 = pt_Id2->pt_Parent;
            }
        }
        else
        {
            t_Status = ITC_STATUS_OVERLAPPING_ID_INTERVAL;
        }
    }

    /* If something goes wrong during the summing process - the ID is invalid
     * and must not be used. */
    if (t_Status != ITC_STATUS_SUCCESS)
    {
        /* There is nothing else to do if the destroy fails. Also it is more
         * important to convey the split failed, rather than the destroy */
        (void)ITC_Id_destroy(ppt_Id);
    }

    return t_Status;
}

/**
 * @brief Serialise an existing ITC Id
 *
 * Data format:
 *  - Byte 0: The major component of the version of the `libitc` library used to
 *      serialise the data. Optional, can be ommitted.
 *  - Bytes (0 - 1) - N (see above): The ID tree.
 *    Each node of the ID tree is serialised in pre-order. I.e the root is
 *    serialised first, followed by the left child, then the right child. Each
 *    node consists of a 1-byte header.
 *    See:
 *    - define ITC_SERDES_PARENT_ID_HEADER
 *    - define ITC_SERDES_SEED_ID_HEADER
 *    - define ITC_SERDES_NULL_ID_HEADER
 *    Any other header value is invalid.
 *
 * @param ppt_Id The pointer to the Id
 * @param pu8_Buffer The buffer to hold the serialised data
 * @param pu32_BufferSize (in) The size of the buffer in bytes. (out) The size
 * of the data inside the buffer in bytes.
 * @param b_AddVersion Whether to prepend the value of `ITC_VERSION_MAJOR` to
 * the output.
 * of the data inside the buffer in bytes.
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t serialiseId(
    const ITC_Id_t *pt_Id,
    uint8_t *const pu8_Buffer,
    uint32_t *const pu32_BufferSize,
    const bool b_AddVersion
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    const ITC_Id_t *pt_CurrentIdParent = NULL; /* The parent of the current ID*/
    const ITC_Id_t *pt_RootIdParent = NULL; /* The parent of the root node */
    uint32_t u32_Offset = 0; /* The current offset */

    /* Remember the root parent as this might be a subtree */
    pt_RootIdParent = pt_Id->pt_Parent;

    if (b_AddVersion)
    {
        /* Prepend the lib version (provided by build system c args) */
        pu8_Buffer[u32_Offset] = ITC_VERSION_MAJOR;

        /* Increment offset */
        u32_Offset += ITC_VERSION_MAJOR_LEN;
    }

    /* Perform a pre-order traversal */
    while (pt_Id && t_Status == ITC_STATUS_SUCCESS)
    {
        if ((u32_Offset + sizeof(ITC_SerDes_Header_t)) > *pu32_BufferSize)
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

#if ITC_CONFIG_ENABLE_SERIALISE_TO_STRING_API

/**
 * @brief Serialise an existing ITC Id to ASCII string
 *
 * @param ppt_Id The pointer to the Id
 * @param pc_Buffer The buffer to hold the serialised data
 * @param pu32_BufferSize (in) The size of the buffer in bytes. (out) The size
 * of the data inside the buffer in bytes.
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 * @retval `ITC_STATUS_INSUFFICIENT_RESOURCES` if the buffer is not big enough
 */
static ITC_Status_t serialiseIdToString(
    const ITC_Id_t *pt_Id,
    char *const pc_Buffer,
    uint32_t *const pu32_BufferSize
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    const ITC_Id_t *pt_CurrentIdParent = NULL; /* The current ID node */
    const ITC_Id_t *pt_RootIdParent = pt_Id->pt_Parent; /* The root parent */
    uint32_t u32_Offset = 0;

    /* Ensure there is at least space for the NULL termination */
    if (*pu32_BufferSize < 1)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    /* Perform a pre-order traversal */
    while (t_Status == ITC_STATUS_SUCCESS && pt_Id)
    {
        /* Check there is space left in the buffer, taking into account the
         * NULL termination byte and opening bracket/ownership indicator */
        if (u32_Offset >= (*pu32_BufferSize - 1))
        {
            t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
        }
        else
        {
            if (ITC_ID_IS_PARENT_ID(pt_Id))
            {
                /* Open a bracket to signify a parent node */
                pc_Buffer[u32_Offset] = '(';
            }
            else if (pt_Id->b_IsOwner)
            {
                pc_Buffer[u32_Offset] = '1';
            }
            else
            {
                pc_Buffer[u32_Offset] = '0';
            }

            /* Increment offset */
            u32_Offset++;

            /* Descend into left tree */
            if (pt_Id->pt_Left)
            {
                /* Remember the parent address */
                pt_CurrentIdParent = pt_Id;

                pt_Id = pt_Id->pt_Left;
            }
            /* Valid parent ITC Id trees always have both left and right
             * nodes. Thus, there is no need to check if the current node
             * doesn't have a left child but has a right one.
             *
             * Instead directly start backtracking up the tree */
            else
            {
                /* Loop until the current element is no longer reachable
                 * through the parent's right child */
                while (t_Status == ITC_STATUS_SUCCESS &&
                       pt_CurrentIdParent != pt_RootIdParent &&
                       pt_CurrentIdParent->pt_Right == pt_Id)
                {
                    pt_Id = pt_Id->pt_Parent;
                    pt_CurrentIdParent = pt_CurrentIdParent->pt_Parent;

                    /* Check there is space left in the buffer, taking into
                     * account the NULL termination byte and bracket */
                    if (u32_Offset >= (*pu32_BufferSize - 1))
                    {
                        t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
                    }
                    else
                    {
                        /* Close the current parent node bracket */
                        pc_Buffer[u32_Offset] = ')';
                        u32_Offset++;
                    }
                }

                /* There is a right subtree that has not been explored yet */
                if (t_Status == ITC_STATUS_SUCCESS &&
                    pt_CurrentIdParent != pt_RootIdParent)
                {
                    pt_Id = pt_CurrentIdParent->pt_Right;

                    /* Check there is space left in the buffer, taking into
                     * account the NULL termination byte, comma and space */
                    if (u32_Offset >= (*pu32_BufferSize - 2))
                    {
                        t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
                    }
                    else
                    {
                        /* Add a comma to signify the start of a new node */
                        pc_Buffer[u32_Offset] = ',';
                        u32_Offset++;

                        /* Add a space between nodes */
                        pc_Buffer[u32_Offset] = ' ';
                        u32_Offset++;
                    }
                }
                else
                {
                    pt_Id = NULL;
                }
            }
        }
    }

    if (t_Status != ITC_STATUS_INVALID_PARAM)
    {
        /* Ensure the string is always NULL-termiated */
        pc_Buffer[u32_Offset] = '\0';
        u32_Offset++;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Return the size of the data in the buffer */
        *pu32_BufferSize = u32_Offset;
    }

    return t_Status;
}

#endif /* ITC_CONFIG_ENABLE_SERIALISE_TO_STRING_API */

/**
 * @brief Deserialise an ITC Id
 *
 * For the expected data format see ::serialiseId()
 *
 * @param pu8_Buffer The buffer holding the serialised Id data
 * @param u32_BufferSize The size of the buffer in bytes
 * @param b_HasVersion Whether the `ITC_VERSION_MAJOR` field is present in the
 * serialised input
 * @param ppt_Id The pointer to the deserialised Id
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t deserialiseId(
    const uint8_t *const pu8_Buffer,
    const uint32_t u32_BufferSize,
    const bool b_HasVersion,
    ITC_Id_t **ppt_Id
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Id_t **ppt_CurrentId = NULL; /* The current ID */
    ITC_Id_t *pt_CurrentIdParent = NULL;
    uint32_t u32_Offset = 0; /* The current offset */

    *ppt_Id = NULL;
    ppt_CurrentId = ppt_Id;

    /* If input contains a version check it matches the current lib version
     * (provided by build system c args) */
    if (b_HasVersion)
    {
        if (pu8_Buffer[u32_Offset] != ITC_VERSION_MAJOR)
        {
            t_Status = ITC_STATUS_SERDES_INCOMPATIBLE_LIB_VERSION;
        }

        u32_Offset += ITC_VERSION_MAJOR_LEN;
    }

    /* The last serialised node cannot be a parent */
    if (t_Status == ITC_STATUS_SUCCESS &&
        pu8_Buffer[u32_BufferSize - 1] == ITC_SERDES_PARENT_ID_HEADER)
    {
        t_Status = ITC_STATUS_CORRUPT_ID;
    }

    while (u32_Offset < u32_BufferSize && t_Status == ITC_STATUS_SUCCESS)
    {
        /* Deserialise a parent ID node */
        if (pu8_Buffer[u32_Offset] == ITC_SERDES_PARENT_ID_HEADER)
        {
            t_Status = newId(ppt_CurrentId, pt_CurrentIdParent, false);
        }
        /* Deserialise a leaf ID node */
        else if (pu8_Buffer[u32_Offset] == ITC_SERDES_NULL_ID_HEADER ||
                 pu8_Buffer[u32_Offset] == ITC_SERDES_SEED_ID_HEADER)
        {
            t_Status = newId(
                ppt_CurrentId,
                pt_CurrentIdParent,
                (pu8_Buffer[u32_Offset] == ITC_SERDES_NULL_ID_HEADER) ? false
                                                                      : true);
        }
        /* Unknown node header value */
        else
        {
            t_Status = ITC_STATUS_CORRUPT_ID;
        }

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            /* Get ready for the next header:
             *
             * - If the current header was a parent - descend into left child
             * - If the current header was a leaf - find the first unallocated
             *   right child node
             */
            if (pu8_Buffer[u32_Offset] == ITC_SERDES_PARENT_ID_HEADER)
            {
                pt_CurrentIdParent = *ppt_CurrentId;
                ppt_CurrentId = &(*ppt_CurrentId)->pt_Left;
            }
            else
            {
                /* Backtrack the tree until an unallocated right child is found
                 * or there are no more parent nodes */
                while (pt_CurrentIdParent && pt_CurrentIdParent->pt_Right)
                {
                    ppt_CurrentId = &pt_CurrentIdParent;
                    pt_CurrentIdParent = (*ppt_CurrentId)->pt_Parent;
                }

                /* Descend into the unallocated right child of the parent */
                if (pt_CurrentIdParent)
                {
                    ppt_CurrentId = &pt_CurrentIdParent->pt_Right;
                }
                /* There aren't any unallocated right child nodes in the tree.
                 *
                 * Usually this would signal the end of the loop as the only
                 * time this should happen is when the full input buffer has
                 * been deserialised (i.e.`u32_NextOffset == u32_BufferSize`),
                 * since the tree is serialised in a pre-order traversal fasion.
                 *
                 * However, if the input buffer is malformed in some way it is
                 * possible that there are elements in it that havent't been
                 * deserialised yet (i.e. `u32_NextOffset < u32_BufferSize`).
                 *
                 * This should be treated as error as otherwise the user might
                 * not get the expected (or full) ID tree. */
                else if ((u32_Offset + sizeof(ITC_SerDes_Header_t)) <
                         u32_BufferSize)
                {
                    t_Status = ITC_STATUS_CORRUPT_ID;
                }
                else
                {
                    /* Nothing to do */
                }
            }

            /* Get the next header */
            u32_Offset += sizeof(ITC_SerDes_Header_t);
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Check the deserialised ID is valid */
        t_Status = validateId(*ppt_Id, true);
    }

    if (t_Status != ITC_STATUS_SUCCESS)
    {
        /* There is nothing else to do if the destroy fails. Also it is more
         * important to convey the deserialisation failed, rather than the
         * destroy */
        (void)ITC_Id_destroy(ppt_Id);
    }

    return t_Status;
}

/******************************************************************************
 * Public functions
 ******************************************************************************/

/******************************************************************************
 * Allocate a new ITC ID and initialise it as a seed ID (1)
 ******************************************************************************/

ITC_Status_t ITC_Id_newSeed(
    ITC_Id_t **const ppt_Id
)
{
    if (!ppt_Id)
    {
        return ITC_STATUS_INVALID_PARAM;
    }

    return newId(ppt_Id, NULL, true);
}

/******************************************************************************
 * Allocate a new ITC ID and initialise it as a null ID (0)
 ******************************************************************************/

ITC_Status_t ITC_Id_newNull(
    ITC_Id_t **const ppt_Id
)
{
    if (!ppt_Id)
    {
        return ITC_STATUS_INVALID_PARAM;
    }

    return newId(ppt_Id, NULL, false);
}

/******************************************************************************
 * Free an allocated ITC ID
 ******************************************************************************/

ITC_Status_t ITC_Id_destroy(
    ITC_Id_t **const ppt_Id
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Status_t t_FreeStatus = ITC_STATUS_SUCCESS; /* The last free status */
    ITC_Id_t *pt_CurrentId = NULL; /* The current element */
    ITC_Id_t *pt_CurrentIdParent = NULL;
    ITC_Id_t *pt_RootIdParent = NULL;

    if (!ppt_Id)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else if (*ppt_Id)
    {
        pt_CurrentId = *ppt_Id;
        /* Remember the parent as this might be a subtree */
        pt_RootIdParent = pt_CurrentId->pt_Parent;

        /* Keep trying to free elements even if some frees fail */
        while(pt_CurrentId && pt_CurrentId != pt_RootIdParent)
        {
            /* Advance into left subtree */
            if(pt_CurrentId->pt_Left)
            {
                pt_CurrentId = pt_CurrentId->pt_Left;
            }
            /* Advance into right subtree */
            else if(pt_CurrentId->pt_Right)
            {
                pt_CurrentId = pt_CurrentId->pt_Right;
            }
            else
            {
                /* Remember the parent element */
                pt_CurrentIdParent = pt_CurrentId->pt_Parent;

                if(pt_CurrentIdParent)
                {
                    /* Remove the current element address from the parent */
                    if(pt_CurrentIdParent->pt_Left == pt_CurrentId)
                    {
                        pt_CurrentIdParent->pt_Left = NULL;
                    }
                    else
                    {
                        pt_CurrentIdParent->pt_Right = NULL;
                    }
                }

                /* Free the current element */
                t_FreeStatus = ITC_Port_free(
                    pt_CurrentId, ITC_PORT_ALLOCTYPE_ITC_ID_T);

                /* Return last error */
                if (t_FreeStatus != ITC_STATUS_SUCCESS)
                {
                    t_Status = t_FreeStatus;
                }

                /* Go up the tree */
                pt_CurrentId = pt_CurrentIdParent;
            }
        }
    }
    else
    {
        /* Nothing to do */
    }

    if (t_Status != ITC_STATUS_INVALID_PARAM)
    {
        /* Sanitize the freed pointer regardless of the exit status */
        *ppt_Id = NULL;
    }

    return t_Status;
}

/******************************************************************************
 * Clone an existing ITC ID
 ******************************************************************************/

ITC_Status_t ITC_Id_clone(
    const ITC_Id_t *const pt_Id,
    ITC_Id_t **const ppt_ClonedId
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    if (!ppt_ClonedId)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateId(pt_Id, true);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = cloneId(pt_Id, ppt_ClonedId, NULL);
    }

    return t_Status;
}

/******************************************************************************
 * Validate an ID
 ******************************************************************************/

ITC_Status_t ITC_Id_validate(
    const ITC_Id_t *const pt_Id
)
{
    return validateId(pt_Id, true);
}

#if ITC_CONFIG_ENABLE_EXTENDED_API

/******************************************************************************
 * Split an existing ITC ID into two distinct (non-overlaping) ITC IDs
 ******************************************************************************/

ITC_Status_t ITC_Id_split(
    ITC_Id_t **const ppt_Id,
    ITC_Id_t **const ppt_OtherId
)
{
    ITC_Status_t t_Status; /* The current status */
    ITC_Id_t *pt_NewId = NULL;

    if (!ppt_Id || !ppt_OtherId)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        t_Status = ITC_Id_splitConst(*ppt_Id, &pt_NewId, ppt_OtherId);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* XXX: Save the parent pointer in case the original ID was a subtree.
         * This makes no sense from a functional point of view, especially
         * since `validateId` should fail if a subtree is passed in. However,
         * if that somehow fails and a subtree is split, the new Id will lose
         * the reference to the original Id subtree's parent nodes, which
         * would lead to a memory leak. */
        pt_NewId->pt_Parent = (*ppt_Id)->pt_Parent;

        /* Destroy the old ID.
         * Ignore return statuses. There is nothing else to do if the destroy
         * fails. Also it is more important to convey that the overall sum
         * operation was successful */
        (void)ITC_Id_destroy(ppt_Id);

        /* Return the first half of the split ID */
        *ppt_Id = pt_NewId;
    }

    return t_Status;
}

/******************************************************************************
 * Sum two existing IDs into a single ID
 ******************************************************************************/

ITC_Status_t ITC_Id_sum(
    ITC_Id_t **const ppt_Id,
    ITC_Id_t **const ppt_OtherId
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Id_t *pt_SummedId = NULL;

    if (!ppt_Id || !ppt_OtherId)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        t_Status = ITC_Id_sumConst(*ppt_Id, *ppt_OtherId, &pt_SummedId);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* XXX: Save the parent pointer in case the original ID was a subtree.
         * This makes no sense from a functional point of view, especially
         * since `validateId` should fail if a subtree is passed in. However,
         * if that somehow fails and a subtree is summed, the new Id will lose
         * the reference to the original Id subtree's parent nodes, which
         * would lead to a memory leak. */
        pt_SummedId->pt_Parent = (*ppt_Id)->pt_Parent;

        /* Destroy the old IDs.
         * Ignore return statuses. There is nothing else to do if the destroy
         * fails. Also it is more important to convey that the overall sum
         * operation was successful */
        (void)ITC_Id_destroy(ppt_Id);
        (void)ITC_Id_destroy(ppt_OtherId);

        /* Return the summed ID */
        *ppt_Id = pt_SummedId;
    }

    return t_Status;
}

#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */

/******************************************************************************
 * Split an ID similar to ::ITC_Id_split() but do not modify the source ID
 ******************************************************************************/

ITC_Status_t ITC_Id_splitConst(
    const ITC_Id_t *const pt_Id,
    ITC_Id_t **const ppt_Id1,
    ITC_Id_t **const ppt_Id2
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    if (!ppt_Id1 || !ppt_Id2)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateId(pt_Id, true);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = splitIdI(pt_Id, ppt_Id1, ppt_Id2);
    }

    return t_Status;
}

/******************************************************************************
 * Sum two IDs similar to ::ITC_Id_sum() but do not modify the source IDs
 ******************************************************************************/

ITC_Status_t ITC_Id_sumConst(
    const ITC_Id_t *const pt_Id1,
    const ITC_Id_t *const pt_Id2,
    ITC_Id_t **const ppt_Id
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    if (!ppt_Id)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateId(pt_Id1, true);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateId(pt_Id2, true);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = sumIdI(pt_Id1, pt_Id2, ppt_Id);
    }

    return t_Status;
}

/******************************************************************************
 * Serialise an existing ITC Id
 ******************************************************************************/

ITC_Status_t ITC_SerDes_Util_serialiseId(
    const ITC_Id_t *const pt_Id,
    uint8_t *const pu8_Buffer,
    uint32_t *const pu32_BufferSize,
    const bool b_AddVersion
)
{
    ITC_Status_t t_Status; /* The current status */

    t_Status = ITC_SerDes_Util_validateBuffer(
        &pu8_Buffer[0],
        pu32_BufferSize,
        (b_AddVersion) ? ITC_SERDES_ID_MIN_BUFFER_LEN + ITC_VERSION_MAJOR_LEN
                       : ITC_SERDES_ID_MIN_BUFFER_LEN,
        true);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateId(pt_Id, true);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = serialiseId(
            pt_Id, &pu8_Buffer[0], pu32_BufferSize, b_AddVersion);
    }

    return t_Status;
}

/******************************************************************************
 * Deserialise an ITC Id
 ******************************************************************************/

ITC_Status_t ITC_SerDes_Util_deserialiseId(
    const uint8_t *const pu8_Buffer,
    const uint32_t u32_BufferSize,
    const bool b_HasVersion,
    ITC_Id_t **const ppt_Id
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS;

    if (!ppt_Id)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = ITC_SerDes_Util_validateBuffer(
            &pu8_Buffer[0],
            &u32_BufferSize,
            (b_HasVersion) ? ITC_SERDES_ID_MIN_BUFFER_LEN + ITC_VERSION_MAJOR_LEN
                           : ITC_SERDES_ID_MIN_BUFFER_LEN,
            false);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = deserialiseId(
            &pu8_Buffer[0], u32_BufferSize, b_HasVersion, ppt_Id);
    }

    return t_Status;
}

#if ITC_CONFIG_ENABLE_SERIALISE_TO_STRING_API

/******************************************************************************
 * Serialise an existing ITC Id to string
 ******************************************************************************/

ITC_Status_t ITC_SerDes_serialiseIdToString(
    const ITC_Id_t *const pt_Id,
    char *const pc_Buffer,
    uint32_t *const pu32_BufferSize
)
{
    ITC_Status_t t_Status; /* The current status */

    t_Status = ITC_SerDes_Util_validateBuffer(
        (uint8_t *)&pc_Buffer[0],
        pu32_BufferSize,
        ITC_SER_TO_STR_ID_MIN_BUFFER_LEN,
        true);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateId(pt_Id, true);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = serialiseIdToString(pt_Id, &pc_Buffer[0], pu32_BufferSize);
    }

    return t_Status;
}

#endif /* ITC_CONFIG_ENABLE_SERIALISE_TO_STRING_API */

#if ITC_CONFIG_ENABLE_EXTENDED_API

/******************************************************************************
 * Serialise an existing ITC Id
 ******************************************************************************/

ITC_Status_t ITC_SerDes_serialiseId(
    const ITC_Id_t *const pt_Id,
    uint8_t *const pu8_Buffer,
    uint32_t *const pu32_BufferSize
)
{
    return ITC_SerDes_Util_serialiseId(
        pt_Id,
        pu8_Buffer,
        pu32_BufferSize,
        true);
}

/******************************************************************************
 * Deserialise an ITC Id
 ******************************************************************************/

ITC_Status_t ITC_SerDes_deserialiseId(
    const uint8_t *const pu8_Buffer,
    const uint32_t u32_BufferSize,
    ITC_Id_t **const ppt_Id
)
{
    return ITC_SerDes_Util_deserialiseId(
        pu8_Buffer,
        u32_BufferSize,
        true,
        ppt_Id);
}

#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */

#if IS_UNIT_TEST_BUILD

/******************************************************************************
 * Normalise an ID
 ******************************************************************************/

ITC_Status_t ITC_Id_normalise(
    ITC_Id_t *const pt_Id
)
{
    ITC_Status_t t_Status; /* The current status */

    t_Status = validateId(pt_Id, false);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = normIdI(pt_Id);
    }

    return t_Status;
}

#endif /* IS_UNIT_TEST_BUILD */
