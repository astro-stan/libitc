/**
 * @file ITC_Event.c
 * @brief Implementation of the Interval Tree Clock's Event mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#include "ITC_Event.h"
#include "ITC_Event_package.h"
#include "ITC_Event_private.h"
#include "ITC_Config.h"

#include "ITC_SerDes_private.h"
#include "ITC_SerDes_Util_package.h"

#if ITC_CONFIG_ENABLE_EXTENDED_API
#include "ITC_SerDes.h"
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */

#if !(ITC_CONFIG_ENABLE_SERIALISE_TO_STRING_API && ITC_CONFIG_ENABLE_EXTENDED_API)
#include "ITC_SerDes_package.h"
#endif /* !(ITC_CONFIG_ENABLE_SERIALISE_TO_STRING_API && ITC_CONFIG_ENABLE_EXTENDED_API) */

#include "ITC_Id_package.h"
#include "ITC_Id_private.h"
#include "ITC_Port.h"

#include <stdbool.h>
#include <stddef.h>

/******************************************************************************
 * Private functions
 ******************************************************************************/

/**
 * @brief Validate an existing ITC Event
 *
 * Should be used to validate all incoming Events before any processing is done.
 *
 * @param pt_Event The Event to validate
 * @param b_CheckIsNormalised Whether to check if the Event is normalised
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t validateEvent(
    const ITC_Event_t *pt_Event,
    const bool b_CheckIsNormalised
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    /* The current Event parent */
    const ITC_Event_t *pt_CurrentEventParent = NULL;

    if(!pt_Event)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    /* Subtrees are considered invalid when coming through the public API */
    else if (pt_Event->pt_Parent)
    {
        t_Status = ITC_STATUS_CORRUPT_EVENT;
    }

    /* Perform a pre-order traversal */
    while (t_Status == ITC_STATUS_SUCCESS && pt_Event)
    {
        /* Checks:
         *  - The parent pointer must match pt_CurrentEventParent.
         *  - Must be a leaf or a valid parent node
         *  - Must be a normalised Event node (if the check is enabled)
         */
        if (pt_CurrentEventParent != pt_Event->pt_Parent ||
            (!ITC_EVENT_IS_LEAF_EVENT(pt_Event) &&
             !ITC_EVENT_IS_VALID_PARENT(pt_Event)) ||
            (b_CheckIsNormalised &&
             !ITC_EVENT_IS_NORMALISED_EVENT(pt_Event)))
        {
            t_Status = ITC_STATUS_CORRUPT_EVENT;
        }
        else
        {
            /* Descend into left tree */
            if (pt_Event->pt_Left)
            {
                /* Remember the parent address */
                pt_CurrentEventParent = pt_Event;

                pt_Event = pt_Event->pt_Left;
            }
            /* ITC trees must always have both left and right subtrees or
             * be leafs. If this is reached, then a node is missing its
             * left subtree, which makes the tree invalid. Usually this will
             * be caught in the `if` at the beginning of the loop, but do check
             * again just in case */
            else if (pt_Event->pt_Right)
            {
                t_Status = ITC_STATUS_CORRUPT_EVENT;
            }
            else
            {
                /* Loop until the current element is no longer reachable
                 * through the parent's right child */
                while (pt_CurrentEventParent &&
                       pt_CurrentEventParent->pt_Right == pt_Event)
                {
                    pt_Event = pt_Event->pt_Parent;
                    pt_CurrentEventParent = pt_CurrentEventParent->pt_Parent;
                }

                /* There is a right subtree that has not been explored yet */
                if (pt_CurrentEventParent)
                {
                    pt_Event = pt_CurrentEventParent->pt_Right;
                }
                else
                {
                    pt_Event = NULL;
                }
            }
        }
    }

    return t_Status;
}

/**
 * @brief Allocate a new ITC Event
 *
 * @param ppt_Event (out) The pointer to the new Event
 * @param ppt_Parent The pointer to the parent Event in the tree.
 * Otherwise NULL.
 * @param t_Count The number of events witnessed by the Event
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t newEvent(
    ITC_Event_t **const ppt_Event,
    ITC_Event_t *const pt_Parent,
    const ITC_Event_Counter_t t_Count
)
{
    ITC_Status_t t_Status; /* The current status */
    ITC_Event_t *pt_Alloc;

    t_Status = ITC_Port_malloc(
        (void **)&pt_Alloc, ITC_PORT_ALLOCTYPE_ITC_EVENT_T);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Initialise members */
        pt_Alloc->t_Count = t_Count;
        pt_Alloc->pt_Parent = pt_Parent;
        pt_Alloc->pt_Left = NULL;
        pt_Alloc->pt_Right = NULL;

        /* Return the pointer to the allocated memory */
        *ppt_Event = pt_Alloc;
    }
    else
    {
        /* Sanitise pointer */
        *ppt_Event = NULL;
    }

    return t_Status;
}

/**
 * @brief Clone an existing ITC Event
 *
 * @note Memory for the new ITC Event will be dynamically allocated.
 * On error, the cloned Event is automatically deallocated.
 * @param pt_Event The existing Event
 * @param ppt_ClonedEvent The pointer to the cloned Event
 * @param pt_ParentEvent The pointer to parent Event. Otherwise NULL
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t cloneEvent(
    const ITC_Event_t *pt_Event,
    ITC_Event_t **const ppt_ClonedEvent,
    ITC_Event_t *const pt_ParentEvent
)
{
    ITC_Status_t t_Status; /* The current status */
    const ITC_Event_t *pt_RootEventParent; /* The parent of the root */
    ITC_Event_t *pt_CurrentEventClone; /* The current event clone */

    /* Init clone pointer */
    *ppt_ClonedEvent = NULL;
    /* Remember the parent of the root as this might be a subree */
    pt_RootEventParent = pt_Event->pt_Parent;

    /* Allocate the root */
    t_Status = newEvent(
        ppt_ClonedEvent, pt_ParentEvent, pt_Event->t_Count);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Initialise the cloned root pointer */
        pt_CurrentEventClone = *ppt_ClonedEvent;
    }

    while(t_Status == ITC_STATUS_SUCCESS &&
            pt_Event != pt_RootEventParent)
    {
        if (pt_Event->pt_Left && !pt_CurrentEventClone->pt_Left)
        {
            /* Allocate left subtree */
            t_Status = newEvent(
                &pt_CurrentEventClone->pt_Left,
                pt_CurrentEventClone,
                pt_Event->pt_Left->t_Count);

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                /* Descend into the left child */
                pt_Event = pt_Event->pt_Left;
                pt_CurrentEventClone = pt_CurrentEventClone->pt_Left;
            }
        }
        else if (
            pt_Event->pt_Right && !pt_CurrentEventClone->pt_Right)
        {
            /* Allocate right subtree */
            t_Status = newEvent(
                &pt_CurrentEventClone->pt_Right,
                pt_CurrentEventClone,
                pt_Event->pt_Right->t_Count);

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                /* Descend into the right child */
                pt_Event = pt_Event->pt_Right;
                pt_CurrentEventClone = pt_CurrentEventClone->pt_Right;
            }
        }
        else
        {
            /* Go up the tree */
            pt_Event = pt_Event->pt_Parent;
            pt_CurrentEventClone = pt_CurrentEventClone->pt_Parent;
        }
    }

    /* If something goes wrong during the cloning - the Event is invalid and
     * must not be used. */
    if (t_Status != ITC_STATUS_SUCCESS)
    {
        /* There is nothing else to do if the cloning fails. Also it is more
         * important to convey the cloning failed, rather than the destroy */
        (void)ITC_Event_destroy(ppt_ClonedEvent);
    }

    return t_Status;
}

/**
 * @brief Increment an `ITC_EventCounter_t` and detect overflows
 *
 * Essentially this performs a lift operation on a given Event,
 * fulfilling `lift(e)`.
 *
 * The rules for a `lift` operation are:
 *  - lift(n, m) = (n + m)
 *  - lift((n, e1, e2), m) = (n + m, e1, e2)
 *
 * @note If an overflow is detected, the counter will be returned unmodified.
 *
 * @param pt_Counter The counter to increment
 * @param t_IncCount The amount to increment with
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t incEventCounter(
    ITC_Event_Counter_t *const pt_Counter,
    const ITC_Event_Counter_t t_IncCount
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    /* Detect overflow */
    if (t_IncCount > (((ITC_Event_Counter_t)~0) - *pt_Counter))
    {
        t_Status = ITC_STATUS_EVENT_COUNTER_OVERFLOW;
    }
    else
    {
        *pt_Counter += t_IncCount;
    }

    return t_Status;
}

/**
 * @brief Decrement an `ITC_EventCounter_t` and detect underflows
 *
 * Essentially this performs a sink operation on a given Event,
 * fulfilling `sink(e)`.
 *
 * The rules for a `sink` operation are:
 *  - sink(n, m) = (n - m)
 *  - sink((n, e1, e2), m) = (n - m, e1, e2)
 *
 * @note If an underflow is detected, the counter will be returned unmodified.
 *
 * @param pt_Counter The counter to decrement
 * @param t_DecCount The amount to decrement with
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t decEventCounter(
    ITC_Event_Counter_t *const pt_Counter,
    const ITC_Event_Counter_t t_DecCount
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    /* Detect underflow */
    if (t_DecCount > *pt_Counter)
    {
        t_Status = ITC_STATUS_EVENT_COUNTER_UNDERFLOW;
    }
    else
    {
        *pt_Counter -= t_DecCount;
    }

    return t_Status;
}

/**
 * @brief Given an event `(n, e1, e2)` performs:
 *     `(n, e1, e2) = (lift(n, m), sink(e1, m), sink(e2, m))`,
 *     where `m = min(e1, e2)`.
 *
 * @note It is assumed `e1` and `e2` are normalised Events, such that
 * `min((n, e1, e2)) == n`, i.e one of the subtrees has an event counter
 * equal to 0
 *
 * @param pt_Event The Event on which to perform the operation
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t liftSinkSinkEvent(
    ITC_Event_t *const pt_Event
)
{
    ITC_Status_t t_Status;
    ITC_Event_Counter_t t_Count;

    /* Find the min count */
    t_Count = MIN(pt_Event->pt_Left->t_Count, pt_Event->pt_Right->t_Count);

    /* Lift the event counter of the root node */
    t_Status = incEventCounter(&pt_Event->t_Count, t_Count);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Sink the left child */
        t_Status = decEventCounter(&pt_Event->pt_Left->t_Count, t_Count);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Sink the right child */
        t_Status = decEventCounter(&pt_Event->pt_Right->t_Count, t_Count);
    }

    return t_Status;
}

/**
 * @brief Given an event `(n, e1, e2)` performs:
 *     `(n, e1, e2) = lift(n, m)`,
 *     where `m = max(e1, e2)`.
 *
 * @note It is assumed `e1` and `e2` are leaf Events, such that
 * `max((n, e1, e2)) == n + max(e1, e2)`
 *
 * @param pt_Event The Event on which to perform the operation
 * children
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t liftDestroyDestroyEvent(
    ITC_Event_t *const pt_Event
)
{
    ITC_Status_t t_Status;

    /* Lift the event counter of the root node */
    t_Status = incEventCounter(
        &pt_Event->t_Count,
        MAX(pt_Event->pt_Left->t_Count, pt_Event->pt_Right->t_Count));

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Destroy the left leaf child */
        t_Status = ITC_Event_destroy(&pt_Event->pt_Left);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Destroy the right leaf child */
        t_Status = ITC_Event_destroy(&pt_Event->pt_Right);
    }

    return t_Status;
}

/**
 * @brief Turn a leaf Event into a parent by allocating 2 child nodes for it
 *
 * @note It is assumed the Event passed in is a leaf event. If not, a memory
 * leak will occur as the original children will not be deallocated before
 * allocating the new child nodes
 *
 * @param pt_Event The Event on which to perform the operation
 * children
 * @param t_LeftCount The event counter to assign to the left child node
 * @param t_RightCount The event counter to assign to the right child node
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t createChildEventNodes(
    ITC_Event_t *const pt_Event,
    const ITC_Event_Counter_t t_LeftCount,
    const ITC_Event_Counter_t t_RightCount
)
{
    ITC_Status_t t_Status;

    /* Allocate the left child */
    t_Status = newEvent(&pt_Event->pt_Left, pt_Event, t_LeftCount);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Allocate the right child */
        t_Status = newEvent(&pt_Event->pt_Right, pt_Event, t_RightCount);
    }

    return t_Status;
}

/**
 * @brief Normalise an Event fulfilling `norm(e)`
 * Rules:
 *  - norm(n) = n
 *  - norm(n, m, m) = lift(n, m)
 *  - norm((n, e1, e2)) = (lift(n, m), sink(e1, m), sink(e2, m)), where:
 *    - m = min(min(e1), min(e2))
 *    - For normalised event trees (one subtree having an event counter == 0):
 *      - min(n) = n
 *      - min((n, e1, e2)) = n
 *
 * @param pt_Event The Event to normalise
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t normEventE(
    ITC_Event_t *pt_Event
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    /* Remember the parent as this might be a subtree */
    ITC_Event_t *pt_RootEventParent = pt_Event->pt_Parent;

    while (t_Status == ITC_STATUS_SUCCESS &&
           pt_Event != pt_RootEventParent)
    {
        /* norm((n, e1, e2)) */
        if (ITC_EVENT_IS_PARENT_EVENT(pt_Event))
        {
            /* Normalise e1 */
            if (!ITC_EVENT_IS_NORMALISED_EVENT(pt_Event->pt_Left))
            {
                pt_Event = pt_Event->pt_Left;
            }
            /* Normalise e2 */
            else if (!ITC_EVENT_IS_NORMALISED_EVENT(pt_Event->pt_Right))
            {
                pt_Event = pt_Event->pt_Right;
            }
            /* norm((n, m, m)) = lift(n, m) */
            else if (ITC_EVENT_IS_LEAF_EVENT(pt_Event->pt_Left) &&
                     ITC_EVENT_IS_LEAF_EVENT(pt_Event->pt_Right) &&
                     (pt_Event->pt_Left->t_Count ==
                          pt_Event->pt_Right->t_Count))
            {
                /* Lift the root, destroy the children */
                t_Status = liftDestroyDestroyEvent(pt_Event);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    pt_Event = pt_Event->pt_Parent;
                }
            }
            /*
             * norm((n, e1, e2)) = (lift(n, m), sink(e1, m), sink(e2, m)),
             *
             * Where:
             *    - e1 and e2 are normalised event trees or leafs
             *    - min(n) = n
             *    - min((n, e1, e2)) = n
             */
            else if (!ITC_EVENT_IS_NORMALISED_EVENT(pt_Event))
            {
                /* Lift the root, sink the children */
                t_Status = liftSinkSinkEvent(pt_Event);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    pt_Event = pt_Event->pt_Parent;
                }
            }
            /* pt_Event event is normalised. Nothing to do */
            else
            {
                pt_Event = pt_Event->pt_Parent;
            }
        }
        /* norm(n) = n */
        else
        {
            pt_Event = pt_Event->pt_Parent;
        }
    }

    return t_Status;
}

/**
 * @brief Join two Events into a new Event fulfilling `join(e1, e2)`
 * Rules:
 *  - join(n1, n2) = max(n1, n2)
 *  - join(n1, (n2, l2, r2)) = join((n1, 0, 0), (n2, l2, r2))
 *  - join((n1, l1, r1), n2) = join((n1, l1, r1), (n2, 0, 0))
 *  - join((n1, l1, r1), (n2, l2, r2)):
 *    - If n1 > n2:
 *         join((n2, l2, r2), (n1, l1, r1))
 *    - If n1 <= n2:
 *         norm((n1, join(l1, lift(l2, n2 - n1)), join(r1, lift(r2, n2 - n1))))
 *
 * @param pt_Event1 The first Event
 * @param pt_Event2 The second Event
 * @param ppt_Event The new Event
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t joinEventE(
    const ITC_Event_t *const pt_Event1,
    const ITC_Event_t *const pt_Event2,
    ITC_Event_t **const ppt_Event
)
{
    ITC_Status_t t_Status; /* The current status */

    ITC_Event_t *pt_CurrentEvent1 = NULL;
    ITC_Event_t *pt_RootEvent1 = NULL;
    ITC_Event_t *pt_CurrentEvent2 = NULL;
    ITC_Event_t *pt_RootEvent2 = NULL;

    ITC_Event_t *pt_SwapEvent = NULL;

    ITC_Event_t **ppt_CurrentEvent = ppt_Event;
    ITC_Event_t *pt_CurrentEventParent = NULL;

    /* Init Event */
    *ppt_CurrentEvent = NULL;

    /* Clone the input events, as they will get modified during the
     * joining process */
    t_Status = cloneEvent(pt_Event1, &pt_CurrentEvent1, NULL);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Save the root so it can be easily deallocated */
        pt_RootEvent1 = pt_CurrentEvent1;

        t_Status = cloneEvent(pt_Event2, &pt_CurrentEvent2, NULL);

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            pt_RootEvent2 = pt_CurrentEvent2;
        }
    }

    while (t_Status == ITC_STATUS_SUCCESS &&
           pt_CurrentEvent1 != pt_RootEvent1->pt_Parent &&
           pt_CurrentEvent2 != pt_RootEvent2->pt_Parent)
    {
        /* join(n1, n2) = max(n1, n2) */
        if (ITC_EVENT_IS_LEAF_EVENT(pt_CurrentEvent1) &&
            ITC_EVENT_IS_LEAF_EVENT(pt_CurrentEvent2))
        {
            t_Status = newEvent(
                ppt_CurrentEvent,
                pt_CurrentEventParent,
                MAX(pt_CurrentEvent1->t_Count, pt_CurrentEvent2->t_Count));

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                /* Climb back to the parent node
                * Use the parent pointer saved on the stack instead of
                * `(*ppt_CurrentEvent)->pt_Parent` as that will be the child
                * element on the next iteration and may get destroyed by
                * `normEventI`
                */
                ppt_CurrentEvent = &pt_CurrentEventParent;
                pt_CurrentEvent1 = pt_CurrentEvent1->pt_Parent;
                pt_CurrentEvent2 = pt_CurrentEvent2->pt_Parent;
            }
        }
        /* join((n1, l1, r1), (n2, l2, r2)):
         * - If n1 > n2:
         *      join((n2, l2, r2), (n1, l1, r1))
         * - If n1 <= n2:
         *      norm((n1, join(l1, lift(l2, n2 - n1)), join(r1, lift(r2, n2 - n1))))
         */
        else if(ITC_EVENT_IS_PARENT_EVENT(pt_CurrentEvent1) &&
                ITC_EVENT_IS_PARENT_EVENT(pt_CurrentEvent2))
        {
            /* Create the parent node.
             * This might exist from a previous iteration. This is fine. */
            if (!*ppt_CurrentEvent)
            {
                t_Status = newEvent(ppt_CurrentEvent, pt_CurrentEventParent, 0);
            }

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                if (!(*ppt_CurrentEvent)->pt_Left ||
                    !(*ppt_CurrentEvent)->pt_Right)
                {
                    /* Save the parent pointer on the stack */
                    pt_CurrentEventParent = *ppt_CurrentEvent;

                    /* If n1 > n2: flip them around */
                    if (pt_CurrentEvent1->t_Count > pt_CurrentEvent2->t_Count)
                    {
                        pt_SwapEvent = pt_CurrentEvent1;
                        pt_CurrentEvent1 = pt_CurrentEvent2;
                        pt_CurrentEvent2 = pt_SwapEvent;
                    }
                }

                /* Descend into left child */
                if (!(*ppt_CurrentEvent)->pt_Left)
                {
                    ppt_CurrentEvent = &(*ppt_CurrentEvent)->pt_Left;
                    pt_CurrentEvent1 = pt_CurrentEvent1->pt_Left;
                    pt_CurrentEvent2 = pt_CurrentEvent2->pt_Left;

                    t_Status = incEventCounter(
                        &pt_CurrentEvent2->t_Count,
                        pt_CurrentEvent2->pt_Parent->t_Count -
                            pt_CurrentEvent1->pt_Parent->t_Count);
                }
                /* Descend into right child */
                else if (!(*ppt_CurrentEvent)->pt_Right)
                {
                    ppt_CurrentEvent = &(*ppt_CurrentEvent)->pt_Right;
                    pt_CurrentEvent1 = pt_CurrentEvent1->pt_Right;
                    pt_CurrentEvent2 = pt_CurrentEvent2->pt_Right;

                    t_Status = incEventCounter(
                        &pt_CurrentEvent2->t_Count,
                        pt_CurrentEvent2->pt_Parent->t_Count -
                            pt_CurrentEvent1->pt_Parent->t_Count);
                }
                else
                {
                    /* If the events were swapped during the descend (i.e
                     * n1 was bigger than n2), swap them them back before
                     * continuing */
                    if (pt_CurrentEvent1->t_Count < pt_CurrentEvent2->t_Count)
                    {
                        pt_SwapEvent = pt_CurrentEvent1;
                        pt_CurrentEvent1 = pt_CurrentEvent2;
                        pt_CurrentEvent2 = pt_SwapEvent;
                    }

                    /* Copy the root value (n1) - n1 is always the smaller of
                     * the two. Take it from `pt_CurrentEvent2` as that is
                     * always the smaller one due to the swap condition above.*/
                    (*ppt_CurrentEvent)->t_Count = pt_CurrentEvent2->t_Count;

                    /* Normalise Event.
                     * This may destroy all child nodes stored under
                     * *ppt_CurrentEvent
                     */
                    t_Status = normEventE(*ppt_CurrentEvent);

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        /* Save the parent pointer on the stack */
                        pt_CurrentEventParent = (*ppt_CurrentEvent)->pt_Parent;

                        /* Climb back to the parent node */
                        ppt_CurrentEvent = &pt_CurrentEventParent;
                        pt_CurrentEvent2 = pt_CurrentEvent2->pt_Parent;
                        pt_CurrentEvent1 = pt_CurrentEvent1->pt_Parent;
                    }
                }
            }
        }
        /* join(n1, (n2, l2, r2)) = join((n1, 0, 0), (n2, l2, r2)) */
        else if (ITC_EVENT_IS_LEAF_EVENT(pt_CurrentEvent1))
        {
            t_Status = createChildEventNodes(pt_CurrentEvent1, 0, 0);
        }
        /* join((n1, l1, r1), n2) = join((n1, l1, r1), (n2, 0, 0)) */
        else
        {
            t_Status = createChildEventNodes(pt_CurrentEvent2, 0, 0);
        }
    }

    /* Destroy the copied input events */
    if (pt_RootEvent1)
    {
        /* There is nothing else to do if the destroy fails. */
        (void)ITC_Event_destroy(&pt_RootEvent1);
    }

    if (pt_RootEvent2)
    {
        /* There is nothing else to do if the destroy fails. */
        (void)ITC_Event_destroy(&pt_RootEvent2);
    }

    /* If something goes wrong during the joining process - the Event is invalid
     * and must not be used. */
    if (t_Status != ITC_STATUS_SUCCESS)
    {
        /* There is nothing else to do if the destroy fails. Also it is more
         * important to convey the join failed, rather than the destroy */
        (void)ITC_Event_destroy(ppt_Event);
    }

    return t_Status;
}

/**
 * @brief Check if one Event is `<=` to another, fulfilling `leq(e1, e2)`
 * Rules:
 *  - leq(n1, n2) = n1 <= n2
 *  - leq(n1, (n2, l2, r2)) = n1 <= n2
 *  - leq((n1, l1, r1), n2):
 *       n1 <= n2 && leq(lift(l1, n1), n2) && leq(lift(r1, n1), n2)
 *  - leq((n1, l1, r1), (n2, l2, r2)):
 *       n1 <= n2 && leq(lift(l1, n1), lift(l2, n2)) && leq(lift(r1, n1), lift(r2, n2))
 *
 * @param pt_Event1 The first Event
 * @param pt_Event2 The second Event
 * @param pb_IsLeq (out) `true` if `*pt_Event1 <= *pt_Event2`. Otherwise `false`
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t leqEventE(
    const ITC_Event_t *pt_Event1,
    const ITC_Event_t *pt_Event2,
    bool *const pb_IsLeq
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    const ITC_Event_t *pt_CurrentEvent1Parent;
    const ITC_Event_t *pt_RootEvent1Parent;

    /* Holds the event count from the root to the current parent node */
    ITC_Event_Counter_t t_ParentsCountEvent1 = 0;
    ITC_Event_Counter_t t_ParentsCountEvent2 = 0;

    /* Holds the total current event count
     * (pt_EventX->t_Count + t_ParentsCountEventX) */
    ITC_Event_Counter_t t_CurrentCountEvent1 = 0;
    ITC_Event_Counter_t t_CurrentCountEvent2 = 0;

    /* Keeps track of how many descends have been skipped by pt_Event2 due
     * to its tree branch being shallower than the one in pt_Event1 */
    uint32_t u32_CurrentEvent2DescendSkips = 0;

    /* Init flag */
    *pb_IsLeq = true;

    /* Remember the root parent Event as this might be a subtree */
    pt_RootEvent1Parent = pt_Event1->pt_Parent;

    pt_CurrentEvent1Parent = pt_RootEvent1Parent;

    /* Perform a pre-order traversal.
     *
     * For `pt_Event1 <= pt_Event2` all `<=` checks must pass.
     * If a check fails - exit early */
    while (t_Status == ITC_STATUS_SUCCESS && *pb_IsLeq && pt_Event1)
    {
        /* Calculate the total current event count for both Event trees
         *
         * Essentially this is a `lift([lr]X, nX)` operation but
         * doesn't modify the original Event trees */
        t_CurrentCountEvent1 = pt_Event1->t_Count;
        t_Status = incEventCounter(&t_CurrentCountEvent1, t_ParentsCountEvent1);

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            t_CurrentCountEvent2 = pt_Event2->t_Count;
            t_Status = incEventCounter(
                &t_CurrentCountEvent2, t_ParentsCountEvent2);
        }

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            /* n1 <= n2 */
            *pb_IsLeq = t_CurrentCountEvent1 <= t_CurrentCountEvent2;

            if (*pb_IsLeq)
            {
                /* Descend into left tree */
                if (pt_Event1->pt_Left)
                {
                    /* Increment the parent height */
                    t_Status = incEventCounter(
                        &t_ParentsCountEvent1, pt_Event1->t_Count);

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        pt_Event1 = pt_Event1->pt_Left;

                        /* If pt_Event2 has a left node - descend down */
                        if (pt_Event2->pt_Left)
                        {
                            /* Increment the parent height */
                            t_Status = incEventCounter(
                                &t_ParentsCountEvent2,
                                pt_Event2->t_Count);

                            if (t_Status == ITC_STATUS_SUCCESS)
                            {
                                pt_Event2 = pt_Event2->pt_Left;
                            }
                        }
                        /* Otherwise, keep track of how many times the descend
                        * was skipped due to a shallow tree */
                        else
                        {
                            u32_CurrentEvent2DescendSkips++;
                        }
                    }
                }
                /* Valid parent ITC Event trees always have both left and right
                * nodes. Thus, there is no need to check if the current node
                * doesn't have a left child but has a right one.
                *
                * Instead directly start backtracking up the tree */
                else
                {
                    /* Get the current parent node */
                    pt_CurrentEvent1Parent = pt_Event1->pt_Parent;

                    /* Loop until the current node is no longer its
                     * parent's right child node */
                    while (t_Status == ITC_STATUS_SUCCESS &&
                           pt_CurrentEvent1Parent != pt_RootEvent1Parent &&
                           pt_CurrentEvent1Parent->pt_Right == pt_Event1)
                    {
                        pt_Event1 = pt_Event1->pt_Parent;
                        pt_CurrentEvent1Parent =
                            pt_CurrentEvent1Parent->pt_Parent;

                        /* Decrement the parent height */
                        t_Status = decEventCounter(
                            &t_ParentsCountEvent1, pt_Event1->t_Count);

                        if (t_Status == ITC_STATUS_SUCCESS)
                        {
                            /* Decrement the amount of skipped descends */
                            if (u32_CurrentEvent2DescendSkips)
                            {
                                u32_CurrentEvent2DescendSkips--;
                            }
                            /* If no descends were skipped - start backtracking
                            * up the pt_Event2 tree.
                            * There is no need to check if the current pt_Event2
                            * is it's parent's right child, because its descend
                            * is tied to the descends of pt_Event1 */
                            else
                            {
                                pt_Event2 = pt_Event2->pt_Parent;

                                /* Decrement the parent height */
                                t_Status = decEventCounter(
                                    &t_ParentsCountEvent2,
                                    pt_Event2->t_Count);
                            }
                        }
                    }

                    /* There is a right subtree that has not been explored
                    * yet */
                    if (pt_CurrentEvent1Parent != pt_RootEvent1Parent)
                    {
                        /* Jump from the left node of the current parent to
                         * right one */
                        pt_Event1 = pt_CurrentEvent1Parent->pt_Right;

                        /* Do the same for pt_Event2 if it hasn't skipped
                         * any descends due to its tree being shallow */
                        if(!u32_CurrentEvent2DescendSkips)
                        {
                            pt_Event2 = pt_Event2->pt_Parent->pt_Right;
                        }
                    }
                    /* The tree has been fully explored. Exit loop */
                    else
                    {
                        pt_Event1 = NULL;
                    }
                }
            }
        }
    }

    return t_Status;
}

/**
 * @brief Maximise an Event fulfilling `max(e)`
 * Rules:
 *  - max(n) = n
 *  - max(n, e1, e2) = n + max(max(e1), max(e2))
 *
 * The resulting Event is always a leaf Event
 *
 * @param pt_Event The Event to maximise
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t maxEventE(
    ITC_Event_t *pt_Event
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS;
    ITC_Event_t *pt_RootEventParent;

    /* Remember the parent as this might be a subtree */
    pt_RootEventParent = pt_Event->pt_Parent;

    while (t_Status == ITC_STATUS_SUCCESS && pt_Event != pt_RootEventParent)
    {
        if (ITC_EVENT_IS_LEAF_EVENT(pt_Event))
        {
            /* The Event is maximised. Nothing to do. */
            pt_Event = pt_Event->pt_Parent;
        }
        else if (ITC_EVENT_IS_PARENT_EVENT(pt_Event->pt_Left))
        {
            /* Explore left subtree */
            pt_Event = pt_Event->pt_Left;
        }
        else if (ITC_EVENT_IS_PARENT_EVENT(pt_Event->pt_Right))
        {
            /* Explore right subtree */
            pt_Event = pt_Event->pt_Right;
        }
        /* Both Event subtrees are leafs */
        else
        {
            /* Maximise the Event count */
            t_Status = liftDestroyDestroyEvent(pt_Event);
        }
    }

    return t_Status;
}

/**
 * @brief Fill an Event, fulfilling `fill(i, e)`
 * Rules:
 *  - fill(0, e) = e
 *  - fill(1, e) = max(e)
 *  - fill(i, n) = n
 *  - fill((1, ir), (n, el, er)):
 *        norm((n, max(max(el), min(er')), er')), where er' = fill(ir, er)
 *  - fill((il, 1), (n, el, er)):
 *        norm((n, el', max(max(er), min(el')))), where el' = fill(il, el)
 *  - fill((il, ir), (n, el, er)):
 *        norm((n, fill(il, el), fill(ir, er)))
 *
 * @param ppt_Event The Event to fill
 * @param pt_Id The ID showing the ownership information for the interval
 * @param pb_WasFilled (out) Whether the event was filled or not. In some cases
 * filling an Event (simplifying + inflating) is not possible
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t fillEventE(
    ITC_Event_t **const ppt_Event,
    const ITC_Id_t *pt_Id,
    bool *const pb_WasFilled
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    /* Clone of `*ppt_Event` - modifications will be done on the cloned event */
    ITC_Event_t *pt_ClonedEvent = NULL;
    ITC_Event_t *pt_CurrentEvent = *ppt_Event;
    /* Remember the root parents as these might be Event or ID subtrees */
    ITC_Event_t *pt_RootEventParent = (*ppt_Event)->pt_Parent;
    ITC_Id_t *pt_RootIdParent = pt_Id->pt_Parent;

    /* The previously iterated ID subtree.
     * Used to keep track of which nodes have been explored */
    const ITC_Id_t *pt_PrevId = NULL;

    /* Init the flag */
    *pb_WasFilled = false;

    /* Clone the event */
    t_Status = cloneEvent(*ppt_Event, &pt_ClonedEvent, pt_RootEventParent);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        while (t_Status == ITC_STATUS_SUCCESS &&
               pt_CurrentEvent != pt_RootEventParent &&
               pt_Id != pt_RootIdParent)
        {
            /* fill(0, e) = e or fill(i, n) = n */
            if(ITC_ID_IS_NULL_ID(pt_Id) ||
               ITC_EVENT_IS_LEAF_EVENT(pt_CurrentEvent))
            {
                pt_PrevId = pt_Id;

                /* Nothing to inflate. Go up the tree */
                pt_Id = pt_Id->pt_Parent;
                pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
            }
            /* fill(1, e) = max(e) */
            else if (ITC_ID_IS_SEED_ID(pt_Id))
            {
                pt_PrevId = pt_Id;

                /* Maximise Event (turn it into a leaf) */
                t_Status = maxEventE(pt_CurrentEvent);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    *pb_WasFilled = true;

                    pt_Id = pt_Id->pt_Parent;
                    pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
                }
            }
            /* fill((1, ir), (n, el, er)):
             *     norm((n, max(max(el), min(er')), er')), where:
             *         er' = fill(ir, er)
             */
            else if (ITC_ID_IS_SEED_ID(pt_Id->pt_Left))
            {
                /* er' = fill(ir, er) */
                if (pt_PrevId != pt_Id->pt_Right)
                {
                    pt_PrevId = pt_Id;

                    pt_Id = pt_Id->pt_Right;
                    pt_CurrentEvent = pt_CurrentEvent->pt_Right;
                }
                /* norm((n, max(max(el), min(er')), er')) */
                else
                {
                    /* el = max(el) */
                    if (ITC_EVENT_IS_PARENT_EVENT(pt_CurrentEvent->pt_Left))
                    {
                        /* Turn el into a leaf Event */
                        t_Status = maxEventE(pt_CurrentEvent->pt_Left);

                        if (t_Status == ITC_STATUS_SUCCESS)
                        {
                            *pb_WasFilled = true;
                        }
                    }

                    /* el = max(el, min(er')) */
                    if (t_Status == ITC_STATUS_SUCCESS &&
                        (pt_CurrentEvent->pt_Left->t_Count <
                            pt_CurrentEvent->pt_Right->t_Count))
                    {
                        /* For a normalised Event: min((n, el, er)) = n */
                        pt_CurrentEvent->pt_Left->t_Count =
                            pt_CurrentEvent->pt_Right->t_Count;

                        *pb_WasFilled = true;
                    }

                    /* norm((n, el, er')) */
                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        t_Status = normEventE(pt_CurrentEvent);

                        if (t_Status == ITC_STATUS_SUCCESS)
                        {
                            pt_PrevId = pt_Id;

                            pt_Id = pt_Id->pt_Parent;
                            pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
                        }
                    }
                }
            }
            /* fill((il, 1), (n, el, er)):
            *     norm((n, el', max(max(er), min(el')))), where:
            *         el' = fill(il, el)
            */
            else if (ITC_ID_IS_SEED_ID(pt_Id->pt_Right))
            {
                /* el' = fill(il, el) */
                if (pt_PrevId != pt_Id->pt_Left)
                {
                    pt_PrevId = pt_Id;

                    pt_Id = pt_Id->pt_Left;
                    pt_CurrentEvent = pt_CurrentEvent->pt_Left;
                }
                /* norm((n, el', max(max(er), min(el')))) */
                else
                {
                    /* er = max(er) */
                    if (ITC_EVENT_IS_PARENT_EVENT(pt_CurrentEvent->pt_Right))
                    {
                        /* Turn er into a leaf Event */
                        t_Status = maxEventE(pt_CurrentEvent->pt_Right);

                        if (t_Status == ITC_STATUS_SUCCESS)
                        {
                            *pb_WasFilled = true;
                        }
                    }

                    /* er = max(er, min(el')) */
                    if (t_Status == ITC_STATUS_SUCCESS &&
                        (pt_CurrentEvent->pt_Right->t_Count <
                            pt_CurrentEvent->pt_Left->t_Count))
                    {
                        /* For a normalised Event: min((n, el, er)) = n */
                        pt_CurrentEvent->pt_Right->t_Count =
                            pt_CurrentEvent->pt_Left->t_Count;

                        *pb_WasFilled = true;
                    }

                    /* norm((n, el', er)) */
                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        t_Status = normEventE(pt_CurrentEvent);

                        if (t_Status == ITC_STATUS_SUCCESS)
                        {
                            pt_PrevId = pt_Id;

                            pt_Id = pt_Id->pt_Parent;
                            pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
                        }
                    }
                }
            }
            /* fill((il, ir), (n, el, er)):
            *     norm((n, fill(il, el), fill(ir, er)))
            */
            else
            {
                /* fill(il, el) */
                if (pt_PrevId != pt_Id->pt_Left && pt_PrevId != pt_Id->pt_Right)
                {
                    pt_PrevId = pt_Id;

                    pt_Id = pt_Id->pt_Left;
                    pt_CurrentEvent = pt_CurrentEvent->pt_Left;
                }
                /* fill(ir, er) */
                else if (pt_PrevId != pt_Id->pt_Right)
                {
                    pt_PrevId = pt_Id;

                    pt_Id = pt_Id->pt_Right;
                    pt_CurrentEvent = pt_CurrentEvent->pt_Right;
                }
                /* norm((n, el, er) */
                else
                {
                    t_Status = normEventE(pt_CurrentEvent);

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        pt_PrevId = pt_Id;

                        pt_Id = pt_Id->pt_Parent;
                        pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
                    }
                }
            }
        }

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            /* Destroy the cloned Event
             * Ignore return statuses. There is nothing else to do if the
             * destroy fails. Also it is more important to convey that the
             * overall fill operation succeeded */
            (void)ITC_Event_destroy(&pt_ClonedEvent);
        }
        else
        {
            /* Destroy the original Event
            * Ignore return statuses. There is nothing else to do if the destroy
            * fails. Also it is more important to convey why the fill operation
            * failed */
            (void)ITC_Event_destroy(ppt_Event);

            /* Replace the original event with the clone */
            *ppt_Event = pt_ClonedEvent;

            /* Even though this value is not supposed to be used (since the
             * status indicates a failure), ensure it is false */
            *pb_WasFilled = false;
        }
    }

    return t_Status;
}

/**
 * @brief Grow an Event, fulfilling `grow(i, e)`
 *
 * Grow performs a dynamic programming based optimization to choose the
 * inflation that can be performed, given the available ID tree, so as to
 * minimize the cost (`c`) of the event tree growth. The cost of an Event tree
 * growth is defined so that:
 *  - incrementing an event counter is preferable over adding a node
 *  - an operation near the root is preferable to one further away
 *
 * Rules:
 *  - grow(1, n) = (n + 1, 0)
 *  - grow(i, n) = (e', c + N), where:
 *    - (e', c) = grow(i, (n, 0, 0))
 *    - N is a constant, greater than the maximum tree depth that arises
 *      Alternatively it could be implemented as a pair under
 *      lexicographic order
 *  - grow((0, ir), (n, el, er)):
 *        ((n, el, er'), cr + 1), where (er', cr) = grow(ir, er)
 *  - grow((il, 0), (n, el, er)):
 *        ((n, el', er), cl + 1), where (el', cl) = grow(il, el)
 *  - grow((il, ir), (n, el, er)):
 *    - If cl < cr:
 *          ((n, el', er), cl + 1), where (el', cl) = grow(il, el)
 *    - If cl >= cr:
 *          ((n, el, er'), cr + 1), where (er', cr) = grow(ir, er)
 *
 * @param ppt_Event The Event to grow
 * @param pt_Id The ID showing the ownership information for the interval
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t growEventE(
    ITC_Event_t **const ppt_Event,
    const ITC_Id_t *pt_Id
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    /* Clone of `*ppt_Event` - modifications will be done on the cloned event */
    ITC_Event_t *pt_ClonedEvent = NULL;
    ITC_Event_t *pt_CurrentEvent = *ppt_Event;
    /* Remember the root parents as these might be Event or ID subtrees */
    ITC_Event_t *pt_RootEventParent = (*ppt_Event)->pt_Parent;
    ITC_Id_t *pt_RootIdParent = pt_Id->pt_Parent;

    /* The previously iterated ID subtree.
     * Used to keep track of which nodes have been explored */
    const ITC_Id_t *pt_PrevId = NULL;

    /* Use 64-bit counters to avoid having to use lexicographic order, which
     * would require dynamically allocating a list of integers.
     *
     * Instead the node expansion cost (when performing `grow(i, n)`) is set to
     * be `UINT32_MAX`. This is effectively the same as having a list of 32-bit
     * integers in lexicographic order as long as the Event tree height is less
     * than `UINT32_MAX`.
     *
     * For all practical purposes this should be orders of magnitde bigger than
     * any encountered Event tree height. */
    uint64_t u64_CostLeft = 0;
    uint64_t u64_CostRight = 0;

    /* Init the cost pointer with the `&u64_CostLeft`. This is because if
     * `cl >= cr`, the right subtree must be expanded. Two cases exist:
     *
     * - If the initial `*ppt_Event` is *not* a leaf:
     *   `u64_CostLeft == u64_CostRight == 0`, thus the right subtree will be
     *   expanded on the first iteration of the loop.
     *
     *  - If the initial `*ppt_Event` is a leaf:
     *    On the first iteration, `pt_CurrentEvent` will expanded into a parent
     *    node(`grow(i, n) = (e', c + N), where (e', c) = grow(i, (n, 0, 0)))`).
     *    Thus, on the second iteration `u64_CostLeft > u64_CostRight` will be
     *    be true, which would again expand the right subtree. */
    uint64_t *pu64_CostPtr = &u64_CostLeft;

    /* Clone the event */
    t_Status = cloneEvent(*ppt_Event, &pt_ClonedEvent, pt_RootEventParent);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        while (t_Status == ITC_STATUS_SUCCESS &&
               pt_CurrentEvent != pt_RootEventParent &&
               pt_Id != pt_RootIdParent)
        {
            /* This is a special case to protect against an infinite loop if a
             * NULL ID is encountered. */
            if (ITC_ID_IS_NULL_ID(pt_Id))
            {
                pt_PrevId = pt_Id;

                pt_Id = pt_Id->pt_Parent;
                pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
            }
            /* grow(1, n) or grow(i, n) */
            else if (ITC_EVENT_IS_LEAF_EVENT(pt_CurrentEvent))
            {
                /* grow(1, n) = (n + 1, 0) */
                if (ITC_ID_IS_SEED_ID(pt_Id))
                {
                    t_Status = incEventCounter(&pt_CurrentEvent->t_Count, 1);

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        pt_PrevId = pt_Id;

                        /* This case has no cost */

                        pt_Id = pt_Id->pt_Parent;
                        pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
                    }
                }
                /* grow(i, n) = (e', c + N) */
                else
                {
                    /* Expand the event tree by adding 2 child nodes */
                    t_Status = createChildEventNodes(pt_CurrentEvent, 0, 0);

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        pt_PrevId = pt_Id;

                        /* This cost is equvalent to a `shift` operation on a
                         * list of 32-bit integers in lexicographic order, as
                         * long as the tree height is less than UINT32_MAX */
                        *pu64_CostPtr += UINT32_MAX;

                        /* Don't go up back the tree. Instead run through the
                         * cases again with e' (a parent node) */
                    }
                }
            }
            /* grow((0, ir), (n, el, er)):
             * ((n, el, er'), cr + 1), where (er', cr) = grow(ir, er) */
            else if (ITC_ID_IS_NULL_ID(pt_Id->pt_Left))
            {
                /* (er', cr) = grow(ir, er) */
                if (pt_PrevId != pt_Id->pt_Right)
                {
                    pt_PrevId = pt_Id;

                    pt_Id = pt_Id->pt_Right;
                    pt_CurrentEvent = pt_CurrentEvent->pt_Right;
                    pu64_CostPtr = &u64_CostRight;
                }
                /* ((n, el, er'), cr + 1) */
                else
                {
                    pt_PrevId = pt_Id;

                    u64_CostRight++;

                    pt_Id = pt_Id->pt_Parent;
                    pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
                }
            }
            /* grow((il, 0), (n, el, er)):
             * ((n, el', er), cl + 1), where (el', cl) = grow(il, el) */
            else if (ITC_ID_IS_NULL_ID(pt_Id->pt_Right))
            {
                /* (el', cl) = grow(il, el) */
                if (pt_PrevId != pt_Id->pt_Left)
                {
                    pt_PrevId = pt_Id;

                    pt_Id = pt_Id->pt_Left;
                    pt_CurrentEvent = pt_CurrentEvent->pt_Left;
                    pu64_CostPtr = &u64_CostLeft;
                }
                /* ((n, el', er), cl + 1) */
                else
                {
                    pt_PrevId = pt_Id;

                    u64_CostLeft++;

                    pt_Id = pt_Id->pt_Parent;
                    pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
                }
            }
            /* grow((il, ir), (n, el, er)):
             *  - If cl < cr:
             *    ((n, el', er), cl + 1), where (el', cl) = grow(il, el)
             *  - If cl >= cr:
             *    ((n, el, er'), cr + 1), where (er', cr) = grow(ir, er) */
            else
            {
                if (pt_PrevId != pt_Id->pt_Left && pt_PrevId != pt_Id->pt_Right)
                {
                    pt_PrevId = pt_Id;

                    /* cl < cr; (el', cl) = grow(il, el) */
                    if (u64_CostLeft < u64_CostRight)
                    {
                        pt_Id = pt_Id->pt_Left;
                        pt_CurrentEvent = pt_CurrentEvent->pt_Left;
                        pu64_CostPtr = &u64_CostLeft;
                    }
                    /* cl >= cr; (er', cr) = grow(ir, er) */
                    else
                    {
                        pt_Id = pt_Id->pt_Right;
                        pt_CurrentEvent = pt_CurrentEvent->pt_Right;
                        pu64_CostPtr = &u64_CostRight;
                    }
                }
                else
                {
                    /* cl < cr; ((n, el', er), cl + 1) */
                    if (pt_PrevId == pt_Id->pt_Left)
                    {
                        u64_CostLeft++;
                    }
                    /* cl >= cr; ((n, el, er'), cr + 1) */
                    else
                    {
                        u64_CostRight++;
                    }

                    pt_PrevId = pt_Id;

                    pt_Id = pt_Id->pt_Parent;
                    pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
                }
            }
        }

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            /* Destroy the cloned Event
             * Ignore return statuses. There is nothing else to do if the
             * destroy fails. Also it is more important to convey that the
             * overall fill operation succeeded */
            (void)ITC_Event_destroy(&pt_ClonedEvent);
        }
        else
        {
            /* Destroy the original Event
            * Ignore return statuses. There is nothing else to do if the destroy
            * fails. Also it is more important to convey why the grow operation
            * failed */
            (void)ITC_Event_destroy(ppt_Event);

            /* Replace the original event with the clone */
            *ppt_Event = pt_ClonedEvent;
        }
    }

    return t_Status;
}

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
static ITC_Status_t eventCounterToNetwork(
    ITC_Event_Counter_t t_Counter,
    uint8_t *const pu8_Buffer,
    uint32_t *const pu32_BufferSize
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
static ITC_Status_t eventCounterFromNetwork(
    const uint8_t *const pu8_Buffer,
    const uint32_t u32_BufferSize,
    ITC_Event_Counter_t *const pt_Counter
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

/**
 * @brief Serialise an existing ITC Event
 *
 * Data format:
 *  - Byte 0: The major component of the version of the `libitc` library used to
 *      serialise the data. Optional, can be ommitted.
 *  - Bytes (0 - 1) - N (see above): The Event tree.
 *    Each node of the Event tree is serialised in pre-order. I.e the root is
 *    serialised first, followed by the left child, then the right child. Each
 *    serialised node consists of:
 *    - Byte 0: The Event node header.
 *      Contains an `IS_PARENT` flag (bit 0), and the length of the encoded
 *      event counter (bits 1 - 4). See define ITC_SERDES_CREATE_EVENT_HEADER.
 *      If the node event counter is `0`, this length is also set to 0 and the
 *      next field (shown below) is ommitted. Bits 5 - 7 are reserved and always
 *      0.
 *    - Bytes 1 - 4: The node event count.
 *      Can be 0 - 4 bytes long. The length of this field is encoded in the
 *      Event header (see above). Serialised in network-endian. Optional,
 *      ommitted if the node event counter is 0.
 *
 * @param ppt_Event The pointer to the Event
 * @param pu8_Buffer The buffer to hold the serialised data
 * @param pu32_BufferSize (in) The size of the buffer in bytes. (out) The size
 * of the data inside the buffer in bytes.
 * @param b_AddVersion Whether to prepend the value of `ITC_VERSION_MAJOR` to
 * the output.
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 * @retval `ITC_STATUS_INSUFFICIENT_RESOURCES` if the buffer is not big enough
 */
static ITC_Status_t serialiseEvent(
    const ITC_Event_t *pt_Event,
    uint8_t *const pu8_Buffer,
    uint32_t *const pu32_BufferSize,
    const bool b_AddVersion
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    /* The parent of the current Event */
    const ITC_Event_t *pt_CurrentEventParent = NULL;
    /* The parent of the root node */
    const ITC_Event_t *pt_RootEventParent = NULL;
    uint32_t u32_Offset = 0; /* The current offset */
    uint32_t u32_CurrentEventCounterSize;

    /* Remember the root parent as this might be a subtree */
    pt_RootEventParent = pt_Event->pt_Parent;

    if (b_AddVersion)
    {
        /* Prepend the lib version (provided by build system c args) */
        pu8_Buffer[u32_Offset] = ITC_VERSION_MAJOR;

        /* Increment offset */
        u32_Offset += ITC_VERSION_MAJOR_LEN;
    }

    /* Perform a pre-order traversal */
    while (pt_Event && t_Status == ITC_STATUS_SUCCESS)
    {
        if ((u32_Offset + sizeof(ITC_SerDes_Header_t)) > *pu32_BufferSize)
        {
            t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
        }
        /* Serialise the Event counter */
        else if (pt_Event->t_Count > 0)
        {
            /* Calculate the remaining space in the buffer, while leaving space
             * for the header */
            u32_CurrentEventCounterSize =
                *pu32_BufferSize - (u32_Offset + sizeof(ITC_SerDes_Header_t));

            /* Serialise the event counter */
            t_Status = eventCounterToNetwork(
                pt_Event->t_Count,
                &pu8_Buffer[u32_Offset + sizeof(ITC_SerDes_Header_t)],
                &u32_CurrentEventCounterSize);
        }
        /* Special case - the Event counter is 0, nothing to serialise */
        else
        {
            u32_CurrentEventCounterSize = 0;
        }

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            /* Create the header */
            pu8_Buffer[u32_Offset] = ITC_SERDES_CREATE_EVENT_HEADER(
                ITC_EVENT_IS_PARENT_EVENT(pt_Event),
                u32_CurrentEventCounterSize);

            /* Increment the offset */
            u32_Offset +=
                sizeof(ITC_SerDes_Header_t) + u32_CurrentEventCounterSize;

            /* Descend into left tree */
            if (pt_Event->pt_Left)
            {
                pt_Event = pt_Event->pt_Left;
            }
            /* Valid parent ITC Event trees always have both left and right
            * nodes. Thus, there is no need to check if the current node
            * doesn't have a left child but has a right one.
            *
            * Instead directly start backtracking up the tree */
            else
            {
                /* Remember the parent */
                pt_CurrentEventParent = pt_Event->pt_Parent;

                /* Loop until the current element is no longer reachable
                * through the parent's right child */
                while (pt_CurrentEventParent != pt_RootEventParent &&
                    pt_CurrentEventParent->pt_Right == pt_Event)
                {
                    pt_Event = pt_Event->pt_Parent;
                    pt_CurrentEventParent =
                        pt_CurrentEventParent->pt_Parent;
                }

                /* There is a right subtree that has not been explored yet*/
                if (pt_CurrentEventParent != pt_RootEventParent)
                {
                    pt_Event = pt_CurrentEventParent->pt_Right;
                }
                else
                {
                    pt_Event = NULL;
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
 * @brief Serialise an Event counter to ASCII string
 *
 * This function intentionally avoids using `sprintf` to improve portability
 * and efficiency.
 *
 * @warning The resulting string is NOT NULL terminated.
 * @param t_Count The event counter
 * @param pc_Buffer The buffer where to write the data
 * @param pu32_BufferSize (in) The size of the buffer in bytes. (out) The size
 * of the data inside the buffer in bytes.
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 * @retval `ITC_STATUS_INSUFFICIENT_RESOURCES` if the buffer is not big enough
 */
static ITC_Status_t eventCounterToString(
    ITC_Event_Counter_t t_Count,
    char *const pc_Buffer,
    uint32_t *const pu32_BufferSize
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS;
    uint32_t u32_Offset = 0;
    char c_Tmp = 0;

    do
    {
        /* Check there is space left in the buffer */
        if (u32_Offset >= *pu32_BufferSize)
        {
            t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
        }
        else
        {
            /* Serialise the next digit */
            pc_Buffer[u32_Offset] = '0' + (char)(t_Count % 10);
            t_Count /= 10;
            u32_Offset++;
        }
    } while (t_Status == ITC_STATUS_SUCCESS && t_Count);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Reverse the string to get the final output */
        for (uint32_t u32_I = u32_Offset - 1;
             u32_I > (u32_Offset - 1) / 2;
             u32_I--)
        {
            c_Tmp = pc_Buffer[u32_I];
            pc_Buffer[u32_I] = pc_Buffer[u32_Offset - 1 - u32_I];
            pc_Buffer[u32_Offset - 1 - u32_I] = c_Tmp;
        }

        /* Return the size of the data in the buffer */
        *pu32_BufferSize = u32_Offset;
    }

    return t_Status;
}


/**
 * @brief Serialise an existing ITC Event to ASCII string
 *
 * @note The output buffer is always NULL-terminated
 * @param ppt_Event The pointer to the Event
 * @param pc_Buffer The buffer to hold the serialised data
 * @param pu32_BufferSize (in) The size of the buffer in bytes. (out) The size
 * of the data inside the buffer in bytes (including the NULL termination byte).
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 * @retval `ITC_STATUS_INSUFFICIENT_RESOURCES` if the buffer is not big enough
 */
static ITC_Status_t serialiseEventToString(
    const ITC_Event_t *pt_Event,
    char *const pc_Buffer,
    uint32_t *const pu32_BufferSize
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    /* The current Event node */
    const ITC_Event_t *pt_CurrentEventParent = NULL;
    /* The root parent */
    const ITC_Event_t *pt_RootEventParent = pt_Event->pt_Parent;
    uint32_t u32_Offset = 0;
    /* The size of the stringified current node event counter */
    uint32_t u32_EventCounterSize = 0;

    /* Ensure there is at least space for the NULL termination */
    if (*pu32_BufferSize < 1)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    /* Perform a pre-order traversal */
    while (t_Status == ITC_STATUS_SUCCESS && pt_Event)
    {
        /* Check there is space left in the buffer, taking into account the
         * NULL termination byte and bracket */
        if (u32_Offset >= (*pu32_BufferSize - 1))
        {
            t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
        }

        if (t_Status == ITC_STATUS_SUCCESS && ITC_EVENT_IS_PARENT_EVENT(pt_Event))
        {
            /* Open a bracket to signify a parent node */
            pc_Buffer[u32_Offset] = '(';
            /* Increment the offset */
            u32_Offset++;

            /* Check there is space left in the buffer, taking into account the
             * NULL termination byte and at least 1 digit event counter */
             if (u32_Offset >= (*pu32_BufferSize - 1))
             {
                 t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
             }
        }

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            /* Calculate the space left in the buffer, taking into account the
             * NULL termination byte */
            u32_EventCounterSize = *pu32_BufferSize - u32_Offset - 1;

            /* Serialise the current node event counter */
            t_Status = eventCounterToString(
                pt_Event->t_Count,
                &pc_Buffer[u32_Offset],
                &u32_EventCounterSize);
        }

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            /* Increment the offset */
            u32_Offset += u32_EventCounterSize;

            /* If this is a parent, check there is space left in the buffer,
             * taking into account the NULL termination byte, and the space for
             * the extra comma and space */
            if (ITC_EVENT_IS_PARENT_EVENT(pt_Event) &&
                u32_Offset >= (*pu32_BufferSize - 2))
            {
                t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
            }
        }

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            if (ITC_EVENT_IS_PARENT_EVENT(pt_Event))
            {
                /* Add a comma to separete the current node counter from
                 * its children */
                pc_Buffer[u32_Offset] = ',';
                /* Increment the offset */
                u32_Offset++;

                /* Add space between the current node counter and child nodes */
                pc_Buffer[u32_Offset] = ' ';
                /* Increment the offset */
                u32_Offset++;
            }

            /* Descend into left tree */
            if (pt_Event->pt_Left)
            {
                /* Remember the parent address */
                pt_CurrentEventParent = pt_Event;

                pt_Event = pt_Event->pt_Left;
            }
            /* Valid parent ITC Event trees always have both left and right
             * nodes. Thus, there is no need to check if the current node
             * doesn't have a left child but has a right one.
             *
             * Instead directly start backtracking up the tree */
            else
            {
                /* Loop until the current element is no longer reachable
                 * through the parent's right child */
                while (t_Status == ITC_STATUS_SUCCESS &&
                       pt_CurrentEventParent != pt_RootEventParent &&
                       pt_CurrentEventParent->pt_Right == pt_Event)
                {
                    pt_Event = pt_Event->pt_Parent;
                    pt_CurrentEventParent = pt_CurrentEventParent->pt_Parent;

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
                        /* Increment the offset */
                        u32_Offset++;
                    }
                }

                /* There is a right subtree that has not been explored yet */
                if (t_Status == ITC_STATUS_SUCCESS &&
                    pt_CurrentEventParent != pt_RootEventParent)
                {
                    pt_Event = pt_CurrentEventParent->pt_Right;

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
                        /* Increment the offset */
                        u32_Offset++;

                        /* Add space between nodes */
                        pc_Buffer[u32_Offset] = ' ';
                        /* Increment the offset */
                        u32_Offset++;
                    }
                }
                else
                {
                    pt_Event = NULL;
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
 * @brief Deserialise an ITC Event
 *
 * For the expected data format see ::serialiseEvent()
 *
 * @param pu8_Buffer The buffer holding the serialised Event data
 * @param u32_BufferSize The size of the buffer in bytes
 * @param b_HasVersion Whether the `ITC_VERSION_MAJOR` field is present in the
 * serialised input
 * @param ppt_Event The pointer to the deserialised Event
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
static ITC_Status_t deserialiseEvent(
    const uint8_t *const pu8_Buffer,
    const uint32_t u32_BufferSize,
    const bool b_HasVersion,
    ITC_Event_t **const ppt_Event
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Event_t **ppt_CurrentEvent = NULL; /* The current Event */
    ITC_Event_t *pt_CurrentEventParent = NULL;
    uint32_t u32_Offset = 0; /* The current offset */
    uint32_t u32_CounterLen; /* The serialised Event counter length */
    uint32_t u32_NextHeaderOffset; /* Used for serialisation data validation */
    bool b_IsParent;

    *ppt_Event = NULL;
    ppt_CurrentEvent = ppt_Event;

    /* If input contains a version check it matches the current lib version
     * (provided by build system c args) */
    if (b_HasVersion)
    {
        t_Status = ITC_SerDes_Util_validateDesLibVersion(
            pu8_Buffer[u32_Offset]);

        u32_Offset += ITC_VERSION_MAJOR_LEN;
    }

    while (u32_Offset < u32_BufferSize && t_Status == ITC_STATUS_SUCCESS)
    {
        /* Unknown node header value */
        if (pu8_Buffer[u32_Offset] & ~ITC_SERDES_EVENT_HEADER_MASK)
        {
            t_Status = ITC_STATUS_CORRUPT_EVENT;
        }
        else
        {
            /* Get the node type */
            b_IsParent = ITC_SERDES_EVENT_GET_IS_PARENT(pu8_Buffer[u32_Offset]);

            /* Get the serialised Event counter length */
            u32_CounterLen =
                ITC_SERDES_EVENT_GET_COUNTER_LEN(pu8_Buffer[u32_Offset]);

            /* Calculate the offset of the next node header */
            u32_NextHeaderOffset =
                u32_Offset + u32_CounterLen + sizeof(ITC_SerDes_Header_t);

            /* Check for serialisation data validity:
             * - Check there is enough data left in the buffer to deserialise
             *   the current node
             * - Check the last serialised node is not a parent
             */
            if ((u32_NextHeaderOffset > u32_BufferSize) ||
                (b_IsParent && u32_NextHeaderOffset == u32_BufferSize))
            {
                t_Status = ITC_STATUS_CORRUPT_EVENT;
            }
        }

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            /* Create a new node */
            t_Status = newEvent(ppt_CurrentEvent, pt_CurrentEventParent, 0);
        }

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            /* Increment the offset */
            u32_Offset += sizeof(ITC_SerDes_Header_t);

            /* Special case - if the counter length is 0, then the
             * serialised Event had an Event counter == 0 */
            if (u32_CounterLen > 0)
            {
                /* Deserialise the event counter */
                t_Status = eventCounterFromNetwork(
                    &pu8_Buffer[u32_Offset],
                    u32_CounterLen,
                    &(*ppt_CurrentEvent)->t_Count);

            }
        }

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            /* Increment the offset */
            u32_Offset += u32_CounterLen;

            /* If the current header was a parent - descend into left child */
            if (b_IsParent)
            {
                pt_CurrentEventParent = *ppt_CurrentEvent;
                ppt_CurrentEvent = &(*ppt_CurrentEvent)->pt_Left;
            }
            /* If the current header was a leaf - find the first unallocated
             * right child node or error out if there isn't one */
            else
            {
                /* Backtrack the tree until an unallocated right child is found
                 * or there are no more parent nodes */
                while (pt_CurrentEventParent && pt_CurrentEventParent->pt_Right)
                {
                    ppt_CurrentEvent = &pt_CurrentEventParent;
                    pt_CurrentEventParent = (*ppt_CurrentEvent)->pt_Parent;
                }

                /* Descend into the unallocated right child of the parent */
                if (pt_CurrentEventParent)
                {
                    ppt_CurrentEvent = &pt_CurrentEventParent->pt_Right;
                }
                /* There aren't any unallocated right child nodes in the tree.
                 *
                 * Usually this would signal the end of the loop as the only
                 * time this should happen is when the full input buffer has
                 * been deserialised (i.e.`u32_Offset == u32_BufferSize`),
                 * since the tree is serialised in a pre-order traversal fasion.
                 *
                 * However, if the input buffer is malformed in some way it is
                 * possible that there are elements in it that havent't been
                 * deserialised yet (i.e. `u32_Offset < u32_BufferSize`).
                 *
                 * This should be treated as error as otherwise the user might
                 * not get the expected (or full) Event tree. */
                else if (u32_Offset < u32_BufferSize)
                {
                    t_Status = ITC_STATUS_CORRUPT_EVENT;
                }
                else
                {
                    /* Nothing to do */
                }
            }
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Check the deserialised Event is valid */
        t_Status = validateEvent(*ppt_Event, true);
    }

    if (t_Status != ITC_STATUS_SUCCESS)
    {
        /* There is nothing else to do if the destroy fails. Also it is more
         * important to convey the deserialisation failed, rather than the
         * destroy */
        (void)ITC_Event_destroy(ppt_Event);
    }

    return t_Status;
}

/******************************************************************************
 * Public functions
 ******************************************************************************/

/******************************************************************************
 * Allocate a new ITC Event and initialise it
 ******************************************************************************/

ITC_Status_t ITC_Event_new(
    ITC_Event_t **const ppt_Event
)
{
    if (!ppt_Event)
    {
        return ITC_STATUS_INVALID_PARAM;
    }

    return newEvent(ppt_Event, NULL, 0);
}

/******************************************************************************
 * Free an allocated ITC Event
 ******************************************************************************/

ITC_Status_t ITC_Event_destroy(
    ITC_Event_t **const ppt_Event
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Status_t t_FreeStatus = ITC_STATUS_SUCCESS; /* The last free status */
    ITC_Event_t *pt_CurrentEvent = NULL; /* The current element */
    ITC_Event_t *pt_CurrentEventParent = NULL;
    ITC_Event_t *pt_RootEventParent = NULL;

    if (!ppt_Event)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else if (*ppt_Event)
    {
        pt_CurrentEvent = *ppt_Event;
        /* Remember the parent as this might be a subtree */
        pt_RootEventParent = pt_CurrentEvent->pt_Parent;

        /* Keep trying to free elements even if some frees fail */
        while(pt_CurrentEvent && pt_CurrentEvent != pt_RootEventParent)
        {
            /* Advance into left subtree */
            if(pt_CurrentEvent->pt_Left)
            {
                pt_CurrentEvent = pt_CurrentEvent->pt_Left;
            }
            /* Advance into right subtree */
            else if(pt_CurrentEvent->pt_Right)
            {
                pt_CurrentEvent = pt_CurrentEvent->pt_Right;
            }
            else
            {
                /* Remember the parent element */
                pt_CurrentEventParent = pt_CurrentEvent->pt_Parent;

                if(pt_CurrentEventParent)
                {
                    /* Remove the current element address from the parent */
                    if(pt_CurrentEventParent->pt_Left == pt_CurrentEvent)
                    {
                        pt_CurrentEventParent->pt_Left = NULL;
                    }
                    else
                    {
                        pt_CurrentEventParent->pt_Right = NULL;
                    }
                }

                /* Free the current element */
                t_FreeStatus = ITC_Port_free(
                    pt_CurrentEvent, ITC_PORT_ALLOCTYPE_ITC_EVENT_T);

                /* Return last error */
                if (t_FreeStatus != ITC_STATUS_SUCCESS)
                {
                    t_Status = t_FreeStatus;
                }

                /* Go up the tree */
                pt_CurrentEvent = pt_CurrentEventParent;
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
        *ppt_Event = NULL;
    }

    return t_Status;
}

/******************************************************************************
 * Clone an existing ITC Event
 ******************************************************************************/

ITC_Status_t ITC_Event_clone(
    const ITC_Event_t *const pt_Event,
    ITC_Event_t **const ppt_ClonedEvent
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    if (!ppt_ClonedEvent)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateEvent(pt_Event, true);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = cloneEvent(pt_Event, ppt_ClonedEvent, NULL);
    }

    return t_Status;
}

/******************************************************************************
 * Validate an Event
 ******************************************************************************/

ITC_Status_t ITC_Event_validate(
    const ITC_Event_t *const pt_Event
)
{
    return validateEvent(pt_Event, true);
}

#if ITC_CONFIG_ENABLE_EXTENDED_API

/******************************************************************************
 * Join two existing Events into a single Event
 ******************************************************************************/

ITC_Status_t ITC_Event_join(
    ITC_Event_t **const ppt_Event,
    ITC_Event_t **const ppt_OtherEvent
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Event_t *pt_JoinedEvent;

    if (!ppt_Event || !ppt_OtherEvent)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        t_Status = ITC_Event_joinConst(
            *ppt_Event, *ppt_OtherEvent, &pt_JoinedEvent);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* XXX: Save the parent pointer in case the original Event was a subtree.
         * This makes no sense from a functional point of view, especially
         * since `validateEvent` should fail if a subtree is passed in.
         * However, if that somehow fails and a subtree is joined, the new Event
         * will lose the reference to the original Event subtree's parent nodes,
         * which would lead to a memory leak. */
        pt_JoinedEvent->pt_Parent = (*ppt_Event)->pt_Parent;

        /* Destroy the old Events
         * Ignore return statuses. There is nothing else to do if the destroy
         * fails. Also it is more important to convey that the overall join
         * operation was successful */
        (void)ITC_Event_destroy(ppt_Event);
        (void)ITC_Event_destroy(ppt_OtherEvent);

        /* Return the joined Event */
        *ppt_Event = pt_JoinedEvent;
    }

    return t_Status;
}

#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */

/******************************************************************************
 * Join two Events similar to ::ITC_Event_join() but do not modify the source Events
 ******************************************************************************/

ITC_Status_t ITC_Event_joinConst(
    const ITC_Event_t *const pt_Event1,
    const ITC_Event_t *const pt_Event2,
    ITC_Event_t **const ppt_Event
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    if (!ppt_Event)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateEvent(pt_Event1, true);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateEvent(pt_Event2, true);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = joinEventE(pt_Event1, pt_Event2, ppt_Event);
    }

    return t_Status;
}

/******************************************************************************
 * Check if an Event is `less than or equal` (`<=`) to another Event
 ******************************************************************************/

ITC_Status_t ITC_Event_leq(
    const ITC_Event_t *const pt_Event1,
    const ITC_Event_t *const pt_Event2,
    bool *const pb_IsLeq

)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    if (!pb_IsLeq)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateEvent(pt_Event1, true);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateEvent(pt_Event2, true);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Check if `pt_Event1 <= pt_Event2` */
        t_Status = leqEventE(pt_Event1, pt_Event2, pb_IsLeq);
    }

    return t_Status;
}

/******************************************************************************
 * Fill an Event
 ******************************************************************************/

ITC_Status_t ITC_Event_fill(
    ITC_Event_t **const ppt_Event,
    const ITC_Id_t *const pt_Id,
    bool *const pb_WasFilled
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    if (!pb_WasFilled || !ppt_Event)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateEvent(*ppt_Event, true);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Validate the ID */
        t_Status = ITC_Id_validate(pt_Id);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = fillEventE(ppt_Event, pt_Id, pb_WasFilled);
    }

    return t_Status;
}

/******************************************************************************
 * Grow an Event
 ******************************************************************************/

ITC_Status_t ITC_Event_grow(
    ITC_Event_t **const ppt_Event,
    const ITC_Id_t *const pt_Id
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    if (!ppt_Event)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateEvent(*ppt_Event, true);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Validate the ID */
        t_Status = ITC_Id_validate(pt_Id);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = growEventE(ppt_Event, pt_Id);
    }

    return t_Status;
}

/******************************************************************************
 * Serialise an existing ITC Event
 ******************************************************************************/

ITC_Status_t ITC_SerDes_Util_serialiseEvent(
    const ITC_Event_t *const pt_Event,
    uint8_t *const pu8_Buffer,
    uint32_t *const pu32_BufferSize,
    const bool b_AddVersion
)
{
    ITC_Status_t t_Status; /* The current status */

    t_Status = ITC_SerDes_Util_validateBuffer(
        pu8_Buffer,
        pu32_BufferSize,
        (b_AddVersion) ? ITC_SERDES_EVENT_MIN_BUFFER_LEN + ITC_VERSION_MAJOR_LEN
                       : ITC_SERDES_EVENT_MIN_BUFFER_LEN,
        true);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateEvent(pt_Event, true);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = serialiseEvent(
            pt_Event, pu8_Buffer, pu32_BufferSize, b_AddVersion);
    }

    return t_Status;
}

/******************************************************************************
 * Deserialise an ITC Event
 ******************************************************************************/

ITC_Status_t ITC_SerDes_Util_deserialiseEvent(
    const uint8_t *const pu8_Buffer,
    const uint32_t u32_BufferSize,
    const bool b_HasVersion,
    ITC_Event_t **const ppt_Event
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS;

    if (!ppt_Event)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = ITC_SerDes_Util_validateBuffer(
            pu8_Buffer,
            &u32_BufferSize,
            (b_HasVersion) ? ITC_SERDES_EVENT_MIN_BUFFER_LEN + ITC_VERSION_MAJOR_LEN
                           : ITC_SERDES_EVENT_MIN_BUFFER_LEN,
            false);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = deserialiseEvent(
            pu8_Buffer, u32_BufferSize, b_HasVersion, ppt_Event);
    }

    return t_Status;
}

#if ITC_CONFIG_ENABLE_SERIALISE_TO_STRING_API

/******************************************************************************
 * Serialise an existing ITC Event to string
 ******************************************************************************/

ITC_Status_t ITC_SerDes_serialiseEventToString(
    const ITC_Event_t *const pt_Event,
    char *const pc_Buffer,
    uint32_t *const pu32_BufferSize
)
{
    ITC_Status_t t_Status; /* The current status */

    t_Status = ITC_SerDes_Util_validateBuffer(
        (uint8_t *)&pc_Buffer[0],
        pu32_BufferSize,
        ITC_SER_TO_STR_EVENT_MIN_BUFFER_LEN,
        true);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateEvent(pt_Event, true);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = serialiseEventToString(
            pt_Event, &pc_Buffer[0], pu32_BufferSize);
    }

    return t_Status;
}

#endif /* ITC_CONFIG_ENABLE_SERIALISE_TO_STRING_API */

#if ITC_CONFIG_ENABLE_EXTENDED_API

/******************************************************************************
 * Serialise an existing ITC Event
 ******************************************************************************/

ITC_Status_t ITC_SerDes_serialiseEvent(
    const ITC_Event_t *const pt_Event,
    uint8_t *const pu8_Buffer,
    uint32_t *const pu32_BufferSize
)
{
    return ITC_SerDes_Util_serialiseEvent(
        pt_Event,
        pu8_Buffer,
        pu32_BufferSize,
        true);
}

/******************************************************************************
 * Deserialise an ITC Event
 ******************************************************************************/

ITC_Status_t ITC_SerDes_deserialiseEvent(
    const uint8_t *const pu8_Buffer,
    const uint32_t u32_BufferSize,
    ITC_Event_t **const ppt_Event
)
{
    return ITC_SerDes_Util_deserialiseEvent(
        pu8_Buffer,
        u32_BufferSize,
        true,
        ppt_Event);
}

#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */

#if IS_UNIT_TEST_BUILD

/******************************************************************************
 * Normalise an Event
 ******************************************************************************/

ITC_Status_t ITC_Event_normalise(
    ITC_Event_t *const pt_Event
)
{
    ITC_Status_t t_Status; /* The current status */

    t_Status = validateEvent(pt_Event, false);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = normEventE(pt_Event);
    }

    return t_Status;
}

/******************************************************************************
 * Maximise an Event
 ******************************************************************************/

ITC_Status_t ITC_Event_maximise(
    ITC_Event_t *const pt_Event
)
{
    ITC_Status_t t_Status; /* The current status */

    t_Status = validateEvent(pt_Event, true);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = maxEventE(pt_Event);
    }

    return t_Status;
}

#endif /* IS_UNIT_TEST_BUILD */
