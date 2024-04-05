/**
 * @file ITC_Event.c
 * @author Stan
 * @brief Implementation of the Interval Tree Clock's Event mechanism
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ITC_Event.h"
#include "ITC_Event_private.h"
#include "ITC_Port.h"

/******************************************************************************
 * Private functions
 ******************************************************************************/

/**
 * @brief Validate an existing ITC Event
 *
 * Should be used to validate all incoming Events before any processing is done.
 *
 * @param pt_Event The Event to validate
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t validateEvent(
    const ITC_Event_t *const pt_Event
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    const ITC_Event_t *pt_CurrentEvent = pt_Event;
    const ITC_Event_t *pt_ParentCurrentEvent = NULL;
    const ITC_Event_t *pt_ParentRootEvent = NULL;

    if(!pt_CurrentEvent)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        /* Remember the root parent Event as this might be a subtree */
        pt_ParentRootEvent = pt_CurrentEvent->pt_Parent;

        pt_ParentCurrentEvent = pt_ParentRootEvent;
    }

    /* Perform a pre-order traversal */
    while (t_Status == ITC_STATUS_SUCCESS && pt_CurrentEvent)
    {
        /* Checks:
         *  - The parent pointer must match pt_ParentCurrentEvent.
         *  - If not a leaf node:
         *    - `pt_CurrentEvent->pt_Left != pt_CurrentEvent->pt_Right != NULL`
         */
        if (pt_ParentCurrentEvent != pt_CurrentEvent->pt_Parent ||
            (!ITC_EVENT_IS_LEAF_EVENT(pt_CurrentEvent) &&
             !ITC_EVENT_IS_VALID_PARENT(pt_CurrentEvent)))
        {
            t_Status = ITC_STATUS_CORRUPT_EVENT;
        }
        else
        {
            /* Descend into left tree */
            if (pt_CurrentEvent->pt_Left)
            {
                /* Remember the parent address */
                pt_ParentCurrentEvent = pt_CurrentEvent;

                pt_CurrentEvent = pt_CurrentEvent->pt_Left;
            }
            /* Descend into right tree */
            else if (pt_CurrentEvent->pt_Right)
            {
                /* Remember the parent address */
                pt_ParentCurrentEvent = pt_CurrentEvent;

                pt_CurrentEvent = pt_CurrentEvent->pt_Right;
            }
            else
            {
                /* Trust the parent pointers.
                 * They were validated on the way down */
                pt_ParentCurrentEvent = pt_CurrentEvent->pt_Parent;

                /* Loop until the current element is no longer reachable
                 * through he parent's right child */
                while (pt_ParentCurrentEvent != pt_ParentRootEvent &&
                    pt_ParentCurrentEvent->pt_Right == pt_CurrentEvent)
                {
                    pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
                    pt_ParentCurrentEvent = pt_ParentCurrentEvent->pt_Parent;
                }

                /* There is a right subtree that has not been explored yet */
                if (pt_ParentCurrentEvent != pt_ParentRootEvent)
                {
                    pt_CurrentEvent = pt_ParentCurrentEvent->pt_Right;
                }
                else
                {
                    pt_CurrentEvent = NULL;
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
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t newEvent(
    ITC_Event_t **ppt_Event,
    ITC_Event_t *const pt_Parent,
    const uint32_t u32_Count
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Event_t *pt_Alloc = NULL;

    if (!ppt_Event)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        t_Status = ITC_Port_malloc((void **)&pt_Alloc, sizeof(ITC_Event_t));
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Initialise members */
        pt_Alloc->u32_Count = u32_Count;
        pt_Alloc->pt_Parent = pt_Parent;
        pt_Alloc->pt_Left = NULL;
        pt_Alloc->pt_Right = NULL;

        /* Return the pointer to the allocated memory */
        *ppt_Event = pt_Alloc;
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
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t cloneEvent(
    const ITC_Event_t *const pt_Event,
    ITC_Event_t **ppt_ClonedEvent,
    ITC_Event_t *const pt_ParentEvent
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    const ITC_Event_t *pt_CurrentEvent = pt_Event; /* The current root */
    const ITC_Event_t *pt_CurrentEventParent; /* pt_CurrentEvent's parent */
    ITC_Event_t *pt_ClonedEventClone; /* The current cloned root */

    if (pt_CurrentEvent == NULL)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        /* Remember the parent of the root as this might be a subree */
        pt_CurrentEventParent = pt_CurrentEvent->pt_Parent;

        /* Allocate the root */
        t_Status = newEvent(
            ppt_ClonedEvent, pt_ParentEvent, pt_CurrentEvent->u32_Count);

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            /* Initialise the cloned root pointer */
            pt_ClonedEventClone = *ppt_ClonedEvent;
        }

        while(t_Status == ITC_STATUS_SUCCESS &&
              pt_CurrentEvent != pt_CurrentEventParent)
        {
            if (pt_CurrentEvent->pt_Left && !pt_ClonedEventClone->pt_Left)
            {
                /* Allocate left subtree */
                t_Status = newEvent(
                    &pt_ClonedEventClone->pt_Left,
                    pt_ClonedEventClone,
                    pt_CurrentEvent->pt_Left->u32_Count);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    /* Descend into the left child */
                    pt_CurrentEvent = pt_CurrentEvent->pt_Left;
                    pt_ClonedEventClone = pt_ClonedEventClone->pt_Left;
                }
            }
            else if (
                pt_CurrentEvent->pt_Right && !pt_ClonedEventClone->pt_Right)
            {
                /* Allocate right subtree */
                t_Status = newEvent(
                    &pt_ClonedEventClone->pt_Right,
                    pt_ClonedEventClone,
                    pt_CurrentEvent->pt_Right->u32_Count);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    /* Descend into the right child */
                    pt_CurrentEvent = pt_CurrentEvent->pt_Right;
                    pt_ClonedEventClone = pt_ClonedEventClone->pt_Right;
                }
            }
            else
            {
                /* Go up the tree */
                pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
                pt_ClonedEventClone = pt_ClonedEventClone->pt_Parent;
            }
        }
    }

    /* If something goes wrong during the cloning - the Event is invalid and
     * must not be used. */
    if (t_Status != ITC_STATUS_SUCCESS && t_Status != ITC_STATUS_INVALID_PARAM)
    {
        /* There is nothing else to do if the cloning fails. Also it is more
         * important to convey the cloning failed, rather than the destroy */
        (void)ITC_Event_destroy(ppt_ClonedEvent);
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
    ITC_Event_t **ppt_Event
)
{
    return newEvent(ppt_Event, NULL, 0);
}

/******************************************************************************
 * Free an allocated ITC Event
 ******************************************************************************/

ITC_Status_t ITC_Event_destroy(
    ITC_Event_t **ppt_Event
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Status_t t_FreeStatus = ITC_STATUS_SUCCESS; /* The last free status */
    ITC_Event_t *pt_CurrentEvent = NULL; /* The current element */
    ITC_Event_t *pt_ParentCurrentEvent = NULL;
    ITC_Event_t *pt_RootParent = NULL;

    if (ppt_Event == NULL)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        pt_CurrentEvent = *ppt_Event;
        pt_RootParent = pt_CurrentEvent->pt_Parent;

        /* Keep trying to free elements even if some frees fail */
        while(pt_CurrentEvent && pt_CurrentEvent != pt_RootParent)
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
                pt_ParentCurrentEvent = pt_CurrentEvent->pt_Parent;

                if(pt_ParentCurrentEvent)
                {
                    /* Remove the current element address from the parent */
                    if(pt_ParentCurrentEvent->pt_Left == pt_CurrentEvent)
                    {
                        pt_ParentCurrentEvent->pt_Left = NULL;
                    }
                    else
                    {
                        pt_ParentCurrentEvent->pt_Right = NULL;
                    }
                }

                /* Free the current element */
                t_FreeStatus = ITC_Port_free(pt_CurrentEvent);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    t_Status = t_FreeStatus;
                }

                /* Go up the tree */
                pt_CurrentEvent = pt_ParentCurrentEvent;
            }
        }
    }

    /* Sanitize the freed pointer */
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        *ppt_Event = NULL;
    }

    return t_Status;
}

/******************************************************************************
 * Clone an existing ITC Event
 ******************************************************************************/

ITC_Status_t ITC_Event_clone(
    const ITC_Event_t *const pt_Event,
    ITC_Event_t **ppt_ClonedEvent
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    t_Status = validateEvent(pt_Event);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = cloneEvent(pt_Event, ppt_ClonedEvent, NULL);
    }

    return t_Status;
}