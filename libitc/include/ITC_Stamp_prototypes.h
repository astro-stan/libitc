/**
 * @file ITC_Stamp_prototypes.h
 * @author Stan
 * @brief Prototypes for the Interval Tree Clock's Stamp mechanism
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_STAMP_PROTOTYPES_H_
#define ITC_STAMP_PROTOTYPES_H_

#include "ITC_Stamp.h"
#include "ITC_Status.h"

/******************************************************************************
 * Functions
 ******************************************************************************/

/**
 * @brief Allocate a new ITC seed Stamp and initialise it
 *
 * @param ppt_Stamp (out) The pointer to the Stamp
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Stamp_newSeed(
    ITC_Stamp_t **ppt_Stamp
);

/**
 * @brief Allocate a new ITC peek Stamp based on an existing Stamp
 *
 * @param pt_Stamp The existing Stamp
 * @param ppt_PeekStamp The pointer to the peek Stamp
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Stamp_newPeek(
    const ITC_Stamp_t *const pt_Stamp,
    ITC_Stamp_t **ppt_PeekStamp
);

/**
 * @brief Free an allocated ITC Stamp
 *
 * @param ppt_Stamp (in) The pointer to the Stamp to deallocate. (out) NULL
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Stamp_destroy(
    ITC_Stamp_t **ppt_Stamp
);

/**
 * @brief Clone an existing ITC Stamp
 *
 * @note Memory for the new ITC Stamp will be dynamically allocated.
 * On error, the cloned Stamp is automatically deallocated.
 * @param pt_Stamp The existing Stamp
 * @param ppt_ClonedStamp The pointer to the cloned Stamp
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Stamp_clone(
    const ITC_Stamp_t *const pt_Stamp,
    ITC_Stamp_t **ppt_ClonedStamp
);

/**
 * @brief Fork an existing Stamp
 * Creates 2 stamps with distinct (non-overlaping) IDs and the same
 * event history.
 *
 * @param pt_Stamp The existing Stamp
 * @param ppt_Stamp1 The first Stamp
 * @param ppt_Stamp2 The second Stamp
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Stamp_fork(
    const ITC_Stamp_t *const pt_Stamp,
    ITC_Stamp_t **ppt_Stamp1,
    ITC_Stamp_t **ppt_Stamp2
);

/**
 * @brief Add a new Event to the Stamp
 *
 * @param pt_Stamp The existing Stamp
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Stamp_event(
    const ITC_Stamp_t *const pt_Stamp
);

/**
 * @brief Join two existing Stamps
 * Joins 2 stamps into a single Stamp, combining their IDs and event histories.
 *
 * @param pt_Stamp1 The first Stamp
 * @param pt_Stamp2 The second Stamp
 * @param ppt_Stamp The new Stamp
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Stamp_join(
    const ITC_Stamp_t *const pt_Stamp1,
    const ITC_Stamp_t *const pt_Stamp2,
    ITC_Stamp_t **ppt_Stamp
);

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
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Stamp_compare(
    const ITC_Stamp_t *const pt_Stamp1,
    const ITC_Stamp_t *const pt_Stamp2,
    ITC_Stamp_Comparison_t *pt_Result
);

#endif /* ITC_STAMP_PROTOTYPES_H_ */
