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

#include "ITC_Event_package.h"
#include "ITC_Id_package.h"
#include "ITC_Port.h"

#include <stdbool.h>

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
