/**
 * @file ITC_Event_prototypes.h
 * @author Stan
 * @brief Prototypes for the Interval Tree Clock's Event mechanism
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_EVENT_PROTOTYPES_H_
#define ITC_EVENT_PROTOTYPES_H_

#include "ITC_Event.h"
#include "ITC_Status.h"

/******************************************************************************
 * Functions
 ******************************************************************************/

/**
 * @brief Allocate a new ITC Event and initialise it
 *
 * @param ppt_Event (out) The pointer to the Event
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Event_new(
    ITC_Event_t **ppt_Event
);

/**
 * @brief Free an allocated ITC Event
 *
 * @param ppt_Event (in) The pointer to the Event to deallocate. (out) NULL
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Event_destroy(
    ITC_Event_t **ppt_Event
);

/**
 * @brief Clone an existing ITC Event
 *
 * @note Memory for the new ITC Event will be dynamically allocated.
 * On error, the cloned Event is automatically deallocated.
 * @param pt_Event The existing Event
 * @param ppt_ClonedEvent The pointer to the cloned Event
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Event_clone(
    const ITC_Event_t *const pt_Event,
    ITC_Event_t **ppt_ClonedEvent
);

/**
 * @brief Normalise an Event
 *
 * @param pt_Event The Event to normalise
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Event_normalise(
    ITC_Event_t *pt_Event
);

/**
 * @brief Join two existing Events into a single Event
 *
 * @param pt_Event1 The first Event
 * @param pt_Event2 The second Event
 * @param ppt_Event The new Event
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Event_join(
    const ITC_Event_t *const pt_Event1,
    const ITC_Event_t *const pt_Event2,
    ITC_Event_t **ppt_Event
);

/**
 * @brief Compare two existing Events
 *
 * - If `*pt_Event1 < *pt_Event2`:
 *      `*pt_Result == ITC_EVENT_COMPARISON_LESS_THAN`
 * - If `*pt_Event1 > *pt_Event2`:
 *      `*pt_Result == ITC_EVENT_COMPARISON_GREATER_THAN`
 * - If `*pt_Event1 == *pt_Event2`:
 *      `*pt_Result == ITC_EVENT_COMPARISON_EQUAL`
 * - If `*pt_Event1 <> *pt_Event2`:
 *      `*pt_Result == ITC_EVENT_COMPARISON_CONCURRENT`
 *
 * @param pt_Event1 The first Event
 * @param pt_Event2 The second Event
 * @param pt_Result The result of the comparison
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Event_compare(
    const ITC_Event_t *const pt_Event1,
    const ITC_Event_t *const pt_Event2,
    ITC_Event_Comparison_t *pt_Result
);

#endif /* ITC_EVENT_PROTOTYPES_H_ */