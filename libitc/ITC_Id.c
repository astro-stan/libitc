/**
 * @file ITC_Id.c
 * @author Stan
 * @brief Implementation of the Interval Tree Clock's ID mechanism
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ITC_Id.h"
#include "ITC_Port.h"
#include "ITC_Id_private.h"

/******************************************************************************
 * Private functions
 ******************************************************************************/

/**
 * @brief Validate an existing ITC ID
 *
 * Should be used to validate all incoming IDs before any processing is done.
 *
 * @param pt_Id The ID to validate
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t validateId(
    const ITC_Id_t *const pt_Id
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    const ITC_Id_t *pt_CurrentId = pt_Id;
    const ITC_Id_t *pt_ParentCurrentId = NULL;
    const ITC_Id_t *pt_ParentRootId = NULL;

    if(!pt_CurrentId)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        /* Remember the root parent ID as this might be a subtree */
        pt_ParentRootId = pt_CurrentId->pt_Parent;

        pt_ParentCurrentId = pt_ParentRootId;
    }

    /* Perform a pre-order traversal */
    while (t_Status == ITC_STATUS_SUCCESS && pt_CurrentId)
    {
        /* Checks:
         *  - The parent pointer must match pt_ParentCurrentId.
         *  - If not a leaf node:
         *    - `pt_CurrentId->b_IsOwner == false`
         *    - `pt_CurrentId->pt_Left != pt_CurrentId->pt_Right != NULL`
         */
        if (pt_ParentCurrentId != pt_CurrentId->pt_Parent ||
            (!ITC_ID_IS_LEAF_ID(pt_CurrentId) &&
             !ITC_ID_IS_VALID_PARENT(pt_CurrentId)))
        {
            t_Status = ITC_STATUS_CORRUPT_ID;
        }
        else
        {
            /* Descend into left tree */
            if (pt_CurrentId->pt_Left)
            {
                /* Remember the parent address */
                pt_ParentCurrentId = pt_CurrentId;

                pt_CurrentId = pt_CurrentId->pt_Left;
            }
            /* Descend into right tree */
            else if (pt_CurrentId->pt_Right)
            {
                /* Remember the parent address */
                pt_ParentCurrentId = pt_CurrentId;

                pt_CurrentId = pt_CurrentId->pt_Right;
            }
            else
            {
                /* Trust the parent pointers.
                 * They were validated on the way down */
                pt_ParentCurrentId = pt_CurrentId->pt_Parent;

                /* Loop until the current element is no longer reachable
                 * through he parent's right child */
                while (pt_ParentCurrentId != pt_ParentRootId &&
                    pt_ParentCurrentId->pt_Right == pt_CurrentId)
                {
                    pt_CurrentId = pt_CurrentId->pt_Parent;
                    pt_ParentCurrentId = pt_ParentCurrentId->pt_Parent;
                }

                /* There is a right subtree that has not been explored yet */
                if (pt_ParentCurrentId != pt_ParentRootId)
                {
                    pt_CurrentId = pt_ParentCurrentId->pt_Right;
                }
                else
                {
                    pt_CurrentId = NULL;
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
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t newId(
    ITC_Id_t **ppt_Id,
    ITC_Id_t *const pt_Parent,
    const bool b_IsOwner
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Id_t *pt_Alloc = NULL;

    if (!ppt_Id)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        t_Status = ITC_Port_malloc((void **)&pt_Alloc, sizeof(ITC_Id_t));
    }

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
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t cloneId(
    const ITC_Id_t *const pt_Id,
    ITC_Id_t **ppt_ClonedId,
    ITC_Id_t *const pt_ParentId
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    const ITC_Id_t *pt_CurrentId = pt_Id; /* The current root */
    const ITC_Id_t *pt_CurrentIdParent; /* pt_CurrentId's parent */
    ITC_Id_t *pt_ClonedIdClone; /* The current cloned root */

    if (pt_CurrentId == NULL)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        /* Remember the parent of the root as this might be a subree */
        pt_CurrentIdParent = pt_CurrentId->pt_Parent;

        /* Allocate the root */
        t_Status = newId(
            ppt_ClonedId, pt_ParentId, pt_CurrentId->b_IsOwner);

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            /* Initialise the cloned root pointer */
            pt_ClonedIdClone = *ppt_ClonedId;
        }

        while(t_Status == ITC_STATUS_SUCCESS &&
              pt_CurrentId != pt_CurrentIdParent)
        {
            if (pt_CurrentId->pt_Left && !pt_ClonedIdClone->pt_Left)
            {
                /* Allocate left subtree */
                t_Status = newId(
                    &pt_ClonedIdClone->pt_Left,
                    pt_ClonedIdClone,
                    pt_CurrentId->pt_Left->b_IsOwner);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    /* Descend into the left child */
                    pt_CurrentId = pt_CurrentId->pt_Left;
                    pt_ClonedIdClone = pt_ClonedIdClone->pt_Left;
                }
            }
            else if (pt_CurrentId->pt_Right && !pt_ClonedIdClone->pt_Right)
            {
                /* Allocate right subtree */
                t_Status = newId(
                    &pt_ClonedIdClone->pt_Right,
                    pt_ClonedIdClone,
                    pt_CurrentId->pt_Right->b_IsOwner);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    /* Descend into the right child */
                    pt_CurrentId = pt_CurrentId->pt_Right;
                    pt_ClonedIdClone = pt_ClonedIdClone->pt_Right;
                }
            }
            else
            {
                /* Go up the tree */
                pt_CurrentId = pt_CurrentId->pt_Parent;
                pt_ClonedIdClone = pt_ClonedIdClone->pt_Parent;
            }
        }
    }

    /* If something goes wrong during the cloning - the ID is invalid and must
     * not be used. */
    if (t_Status != ITC_STATUS_SUCCESS && t_Status != ITC_STATUS_INVALID_PARAM)
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
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

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
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

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
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t splitIdI(
    const ITC_Id_t *const pt_Id,
    ITC_Id_t **ppt_Id1,
    ITC_Id_t **ppt_Id2
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    const ITC_Id_t *pt_CurrentId = pt_Id;
    const ITC_Id_t *pt_CurrentIdParent = NULL;
    ITC_Id_t **ppt_CurrentId1 = ppt_Id1;
    ITC_Id_t *pt_ParentCurrentId1 = NULL;
    ITC_Id_t **ppt_CurrentId2 = ppt_Id2;
    ITC_Id_t *pt_ParentCurrentId2 = NULL;

    if (!pt_CurrentId || !ppt_CurrentId1 || !ppt_CurrentId2)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        /* Remember the parent of the root as this might be a subtree */
        pt_CurrentIdParent = pt_CurrentId->pt_Parent;

        /* Init the new IDs */
        *ppt_CurrentId1 = NULL;
        *ppt_CurrentId2 = NULL;
    }

    while (t_Status == ITC_STATUS_SUCCESS && pt_CurrentId != pt_CurrentIdParent)
    {
        /* split(0) = (0, 0)
         * *ppt_CurrentId1 && *ppt_CurrentId2 should always be NULL here
         * but ensure this is the case before overwriting the pointer as
         * this could lead to a memory leak.
         */
        if (ITC_ID_IS_NULL_ID(pt_CurrentId)
            && !(*ppt_CurrentId1)
            && !(*ppt_CurrentId2))
        {
            t_Status = splitId0(
                ppt_CurrentId1,
                pt_ParentCurrentId1,
                ppt_CurrentId2,
                pt_ParentCurrentId2);

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                pt_CurrentId = pt_CurrentId->pt_Parent;
            }
        }
        /* split(1) = ((1, 0), (0, 1))
         * *ppt_CurrentId1 && *ppt_CurrentId2 should always be NULL here
         * but ensure this is the case before overwriting the pointer as
         * this could lead to a memory leak. */
        else if (ITC_ID_IS_SEED_ID(pt_CurrentId)
                 && !(*ppt_CurrentId1)
                 && !(*ppt_CurrentId2))
        {
            t_Status = splitId1(
                ppt_CurrentId1,
                pt_ParentCurrentId1,
                ppt_CurrentId2,
                pt_ParentCurrentId2);

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                pt_CurrentId = pt_CurrentId->pt_Parent;
            }
        }
        /* split(0, i), split(i, 0), split(i1, i2) */
        else if(!ITC_ID_IS_LEAF_ID(pt_CurrentId))
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
                if (ITC_ID_IS_NULL_ID(pt_CurrentId->pt_Left))
                {
                    /* Create the chilren of the children.
                     * This happens the first time pt_CurentId is reached */
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
                            /* - Descend into pt_CurrentId->pt_Right
                             * - Set the current iteration children as the next
                             *   iteration parents
                             * - Set the right child of the current iteration's
                             *   children as the next iteration's children
                             */
                            pt_CurrentId = pt_CurrentId->pt_Right;
                            pt_ParentCurrentId1 = *ppt_CurrentId1;
                            ppt_CurrentId1 = &(*ppt_CurrentId1)->pt_Right;
                            pt_ParentCurrentId2 = *ppt_CurrentId2;
                            ppt_CurrentId2 = &(*ppt_CurrentId2)->pt_Right;
                        }
                    }
                    /* Children already exist and pt_CurrentId has a parent
                     * This happens on the next iteration after the children
                     * have been created.
                     */
                    else if (pt_CurrentId)
                    {
                        pt_CurrentId = pt_CurrentId->pt_Parent;
                    }
                    else
                    {
                        t_Status = ITC_STATUS_CORRUPT_ID;
                    }
                }
                /* split((i, 0)) = ((i1, 0), (i2, 0)), where (i1, i2) = split(i)
                 */
                else if (ITC_ID_IS_NULL_ID(pt_CurrentId->pt_Right))
                {
                    /* Create the chilren of the children.
                     * This happens the first time pt_CurentId is reached */
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
                            /* - Descend into pt_CurrentId->pt_Left
                             * - Set the current iteration children as the next
                             *   iteration parents
                             * - Set the left child of the current iteration's
                             *   children as the next iteration's children
                             */
                            pt_CurrentId = pt_CurrentId->pt_Left;
                            pt_ParentCurrentId1 = *ppt_CurrentId1;
                            ppt_CurrentId1 = &(*ppt_CurrentId1)->pt_Left;
                            pt_ParentCurrentId2 = *ppt_CurrentId2;
                            ppt_CurrentId2 = &(*ppt_CurrentId2)->pt_Left;
                        }
                    }
                    /* Children already exist and pt_CurrentId has a parent
                     * This happens on the next iteration after the children
                     * have been created.
                     */
                    else if (pt_CurrentId)
                    {
                        pt_CurrentId = pt_CurrentId->pt_Parent;
                    }
                    else
                    {
                        t_Status = ITC_STATUS_CORRUPT_ID;
                    }
                }
                /* split((i1, i2)) = ((i1, 0), (0, i2)) */
                else if (ITC_ID_IS_LEAF_ID(*ppt_CurrentId1)
                         && ITC_ID_IS_LEAF_ID(*ppt_CurrentId2))
                {
                    t_Status = newId(
                        &(*ppt_CurrentId1)->pt_Right,
                        *ppt_CurrentId1,
                        false);

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        t_Status = cloneId(
                            (const ITC_Id_t *const)pt_CurrentId->pt_Left,
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
                            (const ITC_Id_t *const)pt_CurrentId->pt_Right,
                            &(*ppt_CurrentId2)->pt_Right,
                            *ppt_CurrentId2);
                    }

                    if (t_Status == ITC_STATUS_SUCCESS && pt_CurrentId)
                    {
                        pt_CurrentId = pt_CurrentId->pt_Parent;
                    }
                }
                else
                {
                    t_Status = ITC_STATUS_CORRUPT_ID;
                }
            }
        }
        else
        {
            t_Status = ITC_STATUS_CORRUPT_ID;
        }
    }

    /* If something goes wrong during the splitting - the IDs are invalid and
     * must not be used. */
    if (t_Status != ITC_STATUS_SUCCESS && t_Status != ITC_STATUS_INVALID_PARAM)
    {
        /* There is nothing else to do if the destroy fails. Also it is more
         * important to convey the split failed, rather than the destroy */
        (void)ITC_Id_destroy(ppt_Id1);
        (void)ITC_Id_destroy(ppt_Id2);
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
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t normIdI(
    ITC_Id_t *pt_Id
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Status_t t_OpStatus = ITC_STATUS_SUCCESS; /* The current op status */
    ITC_Id_t *pt_CurrentId = pt_Id;
    const ITC_Id_t *pt_CurrentIdParent = NULL;

    if(!pt_CurrentId)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Remember the parent of the root as this might be a subtree */
        pt_CurrentIdParent = pt_CurrentId->pt_Parent;

        /* Start from the left most child*/
        while (pt_CurrentId->pt_Left)
        {
            pt_CurrentId = pt_CurrentId->pt_Left;
        }
    }

    /* Perform a post-order traversal */
    while(t_Status == ITC_STATUS_SUCCESS && pt_CurrentId)
    {
        /* If pt_CurrentId is the root node (no parent) - break */
        if(pt_CurrentId->pt_Parent == pt_CurrentIdParent)
        {
            pt_CurrentId = NULL;
        }
        /* If pt_CurrentId is the right child of its parent */
        else if (pt_CurrentId->pt_Parent->pt_Right == pt_CurrentId)
        {
            pt_CurrentId = pt_CurrentId->pt_Parent;
        }
        /* pt_CurentId is the left child of its parent */
        else
        {
            /* Go to the right child of the pt_CurrentId parent */
            pt_CurrentId = pt_CurrentId->pt_Parent->pt_Right;

            /* Find the left most parent */
            while (pt_CurrentId && !ITC_ID_IS_LEAF_ID(pt_CurrentId))
            {
                /* Go to the left child if there is one, otherwise go to the
                 * right child */
                pt_CurrentId = (pt_CurrentId->pt_Left)
                                    ? pt_CurrentId->pt_Left
                                    : pt_CurrentId->pt_Right;
            }
        }

        /* norm(1, 1) = 1 or norm(0, 0) = 0 */
        if (pt_CurrentId &&
            (ITC_ID_IS_SEED_SEED_ID(pt_CurrentId) ||
             ITC_ID_IS_NULL_NULL_ID(pt_CurrentId)))
        {
            /* Set the interval ownership */
            pt_CurrentId->b_IsOwner = pt_CurrentId->pt_Left->b_IsOwner;

            /* Destroy the children */
            t_Status = ITC_Id_destroy(&pt_CurrentId->pt_Left);
            t_OpStatus = ITC_Id_destroy(&pt_CurrentId->pt_Right);

            if(t_Status == ITC_STATUS_SUCCESS)
            {
                t_Status = t_OpStatus;
            }
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
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t sumIdI(
    const ITC_Id_t *const pt_Id1,
    const ITC_Id_t *const pt_Id2,
    ITC_Id_t **ppt_Id
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    ITC_Id_t **ppt_CurrentId = ppt_Id;
    ITC_Id_t *pt_ParentCurrentId = NULL;
    const ITC_Id_t *pt_CurrentId1 = pt_Id1;
    const ITC_Id_t *pt_CurrentId2 = pt_Id2;

    if(!ppt_CurrentId || !pt_CurrentId1 || !pt_CurrentId2)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        *ppt_CurrentId = NULL;
    }

    while(t_Status == ITC_STATUS_SUCCESS &&
          pt_CurrentId1 != pt_Id1->pt_Parent &&
          pt_CurrentId2 != pt_Id2->pt_Parent)
    {
        /* sum((l1, r1), (l2, r2)) = norm(sum(l1, l2), sum(r1, r2)) */
        if(!ITC_ID_IS_LEAF_ID(pt_CurrentId1) &&
           !ITC_ID_IS_LEAF_ID(pt_CurrentId2))
        {
            /* Create the parent node.
             * This might exist from a previous iteration. This is fine. */
            if(!(*ppt_CurrentId))
            {
                t_Status = newId(ppt_CurrentId, pt_ParentCurrentId, false);
            }

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                /* Descend into left child  */
                if(!(*ppt_CurrentId)->pt_Left)
                {
                    /* Save the parent pointer on the stack */
                    pt_ParentCurrentId = *ppt_CurrentId;

                    ppt_CurrentId = &(*ppt_CurrentId)->pt_Left;
                    pt_CurrentId1 = pt_CurrentId1->pt_Left;
                    pt_CurrentId2 = pt_CurrentId2->pt_Left;
                }
                /* Descend into right child  */
                else if(!(*ppt_CurrentId)->pt_Right)
                {
                    /* Save the parent pointer on the stack */
                    pt_ParentCurrentId = *ppt_CurrentId;

                    ppt_CurrentId = &(*ppt_CurrentId)->pt_Right;
                    pt_CurrentId1 = pt_CurrentId1->pt_Right;
                    pt_CurrentId2 = pt_CurrentId2->pt_Right;
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
                        pt_ParentCurrentId = (*ppt_CurrentId)->pt_Parent;

                        /* Climb back to the parent node */
                        ppt_CurrentId = &pt_ParentCurrentId;
                        pt_CurrentId1 = pt_CurrentId1->pt_Parent;
                        pt_CurrentId2 = pt_CurrentId2->pt_Parent;
                    }
                }
            }
        }
        /* sum(0, i) = i */
        else if (ITC_ID_IS_NULL_ID(pt_CurrentId1))
        {
            t_Status = cloneId(
                pt_CurrentId2, ppt_CurrentId, pt_ParentCurrentId);

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                /* Climb back to the parent node
                * Use the parent pointer saved on the stack instead of
                * `(*ppt_CurrentId)->pt_Parent` as that will be the child
                * element on the next iteration and may get destroyed by
                * `normIdI`
                */
                ppt_CurrentId = &pt_ParentCurrentId;
                pt_CurrentId1 = pt_CurrentId1->pt_Parent;
                pt_CurrentId2 = pt_CurrentId2->pt_Parent;
            }
        }
        /* sum(i, 0) = i */
        else if (ITC_ID_IS_NULL_ID(pt_CurrentId2))
        {

            t_Status = cloneId(
                pt_CurrentId1, ppt_CurrentId, pt_ParentCurrentId);

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                /* Climb back to the parent node
                * Use the parent pointer saved on the stack instead of
                * `(*ppt_CurrentId)->pt_Parent` as that will be the child
                * element on the next iteration and may get destroyed by
                * `normIdI`
                */
                ppt_CurrentId = &pt_ParentCurrentId;
                pt_CurrentId1 = pt_CurrentId1->pt_Parent;
                pt_CurrentId2 = pt_CurrentId2->pt_Parent;
            }
        }
        else
        {
            t_Status = ITC_STATUS_CORRUPT_ID;
        }
    }

    /* If something goes wrong during the summing process - the ID is invalid
     * and must not be used. */
    if (t_Status != ITC_STATUS_SUCCESS && t_Status != ITC_STATUS_INVALID_PARAM)
    {
        /* There is nothing else to do if the destroy fails. Also it is more
         * important to convey the split failed, rather than the destroy */
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
    ITC_Id_t **ppt_Id
)
{
    return newId(ppt_Id, NULL, 1);
}

/******************************************************************************
 * Allocate a new ITC ID and initialise it as a null ID (0)
 ******************************************************************************/

ITC_Status_t ITC_Id_newNull(
    ITC_Id_t **ppt_Id
)
{
    return newId(ppt_Id, NULL, 0);
}

/******************************************************************************
 * Free an allocated ITC ID
 ******************************************************************************/

ITC_Status_t ITC_Id_destroy(
    ITC_Id_t **ppt_Id
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Status_t t_FreeStatus = ITC_STATUS_SUCCESS; /* The last free status */
    ITC_Id_t *pt_CurrentId = NULL; /* The current element */
    ITC_Id_t *pt_ParentCurrentId = NULL;
    ITC_Id_t *pt_RootParent = NULL;

    if (!ppt_Id || !(*ppt_Id))
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        pt_CurrentId = *ppt_Id;
        pt_RootParent = pt_CurrentId->pt_Parent;

        /* Keep trying to free elements even if some frees fail */
        while(pt_CurrentId && pt_CurrentId != pt_RootParent)
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
                pt_ParentCurrentId = pt_CurrentId->pt_Parent;

                if(pt_ParentCurrentId)
                {
                    /* Remove the current element address from the parent */
                    if(pt_ParentCurrentId->pt_Left == pt_CurrentId)
                    {
                        pt_ParentCurrentId->pt_Left = NULL;
                    }
                    else
                    {
                        pt_ParentCurrentId->pt_Right = NULL;
                    }
                }

                /* Free the current element */
                t_FreeStatus = ITC_Port_free(pt_CurrentId);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    t_Status = t_FreeStatus;
                }

                /* Go up the tree */
                pt_CurrentId = pt_ParentCurrentId;
            }
        }
    }

    /* Sanitize the freed pointer */
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        *ppt_Id = NULL;
    }

    return t_Status;
}

/******************************************************************************
 * Clone an existing ITC ID
 ******************************************************************************/

ITC_Status_t ITC_Id_clone(
    const ITC_Id_t *const pt_Id,
    ITC_Id_t **ppt_ClonedId
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    t_Status = validateId(pt_Id);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = cloneId(pt_Id, ppt_ClonedId, NULL);
    }

    return t_Status;
}

/******************************************************************************
 * Split an existing ITC ID into two distinct (non-overlaping) ITC IDs
 ******************************************************************************/

ITC_Status_t ITC_Id_split(
    const ITC_Id_t *const pt_Id,
    ITC_Id_t **ppt_Id1,
    ITC_Id_t **ppt_Id2
)
{
    ITC_Status_t t_Status; /* The current status */

    t_Status = validateId(pt_Id);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = splitIdI(pt_Id, ppt_Id1, ppt_Id2);
    }

    return t_Status;
}

/******************************************************************************
 * Normalise an ID
 ******************************************************************************/

ITC_Status_t ITC_Id_normalise(
    ITC_Id_t *pt_Id
)
{
    ITC_Status_t t_Status; /* The current status */

    t_Status = validateId(pt_Id);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = normIdI(pt_Id);
    }

    return t_Status;
}

/******************************************************************************
 * Sum two existing IDs into a single ID
 ******************************************************************************/

ITC_Status_t ITC_Id_sum(
    const ITC_Id_t *const pt_Id1,
    const ITC_Id_t *const pt_Id2,
    ITC_Id_t **ppt_Id
)
{
    ITC_Status_t t_Status; /* The current status */

    t_Status = validateId(pt_Id1);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateId(pt_Id2);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = sumIdI(pt_Id1, pt_Id2, ppt_Id);
    }

    return t_Status;
}