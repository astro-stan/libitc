/**
 * @file Itc_Id_prototypes.h
 * @author Stan
 * @brief Prototypes for the Interval Tree Clock's ID mechanism
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_ID_PROTOTYPES_H_
#define ITC_ID_PROTOTYPES_H_

#include "Itc_Id.h"
#include "Itc_Status.h"

/**
 * @brief Allocate a new ITC ID
 *
 * @param ppt_Id (out) The pointer to the new ID
 * @param ppt_Id The pointer to the parent ID in the tree. Otherwise NULL.
 * @return Itc_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
Itc_Status_t ITC_Id_alloc(
    ITC_Id_t **ppt_Id,
    ITC_Id_t *pt_Parent
);

/**
 * @brief Free an allocated ITC ID
 *
 * @param ppt_Id (in) The pointer to the ID to deallocate. (out) NULL
 * @return Itc_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
Itc_Status_t ITC_Id_free(
    ITC_Id_t **ppt_Id
);

/**
 * @brief Clone an existing ITC ID
 *
 * @note Memory for the new ITC ID will be dynamically allocated.
 * On error, the cloned ID is automatically deallocated.
 * @param pt_Id The existing ID
 * @param ppt_ClonedId The pointer to the cloned ID
 * @return Itc_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
Itc_Status_t ITC_Id_clone(
    const ITC_Id_t const *pt_Id,
    ITC_Id_t **ppt_ClonedId
);

/**
 * @brief Initialise an ITC ID as a seed ID
 *
 * @note All contained subtrees (if any) will be deallocated
 * @param pt_Id The ID to initialise
 * @return Itc_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
Itc_Status_t ITC_Id_init(
    ITC_Id_t *pt_Id
);

/**
 * @brief Split an existing ITC ID into two distinct (non-overlaping) ITC IDs
 *
 * @param pt_Id The existing ID
 * @param pt_Id1 The first ID
 * @param pt_Id2 The second ID
 * @return Itc_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
Itc_Status_t ITC_Id_split(
    ITC_Id_t *pt_Id,
    ITC_Id_t *pt_Id1,
    ITC_Id_t *pt_Id2
);

/**
 * @brief Sum two existing IDs into a single ID
 *
 * @param pt_Id1 The first ID
 * @param pt_Id2 The second ID
 * @param pt_Id The new ID
 * @return Itc_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
Itc_Status_t ITC_Id_sum(
    ITC_Id_t *pt_Id1,
    ITC_Id_t *pt_Id2,
    ITC_Id_t *pt_Id
);

#endif /* ITC_ID_PROTOTYPES_H_ */