/**
 * @file ITC_Id_prototypes.h
 * @author Stan
 * @brief Prototypes for the Interval Tree Clock's ID mechanism
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_ID_PROTOTYPES_H_
#define ITC_ID_PROTOTYPES_H_

#include "ITC_Id.h"
#include "ITC_Status.h"

/******************************************************************************
 * Functions
 ******************************************************************************/

/**
 * @brief Free an allocated ITC ID
 *
 * @param ppt_Id (in) The pointer to the ID to deallocate. (out) NULL
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Id_destroy(
    ITC_Id_t **ppt_Id
);

/**
 * @brief Clone an existing ITC ID
 *
 * @note Memory for the new ITC ID will be dynamically allocated.
 * On error, the cloned ID is automatically deallocated.
 * @param pt_Id The existing ID
 * @param ppt_ClonedId The pointer to the cloned ID
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Id_clone(
    const ITC_Id_t *const pt_Id,
    ITC_Id_t **ppt_ClonedId
);

/**
 * @brief Allocate a new ITC ID and initialise it as a seed ID (1)
 *
 * @param ppt_Id (out) The pointer to the seed ID
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Id_newSeed(
    ITC_Id_t **ppt_Id
);

/**
 * @brief Allocate a new ITC ID and initialise it as a null ID (0)
 *
 * @param ppt_Id (out) The pointer to the null ID
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Id_newNull(
    ITC_Id_t **ppt_Id
);

/**
 * @brief Split an existing ITC ID into two distinct (non-overlaping) ITC IDs
 *
 * @param pt_Id The existing ID
 * @param ppt_Id1 The first ID
 * @param ppt_Id2 The second ID
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Id_split(
    const ITC_Id_t *const pt_Id,
    ITC_Id_t **ppt_Id1,
    ITC_Id_t **ppt_Id2
);

/**
 * @brief Normalise an ID
 *
 * @param pt_Id The ID to normalise
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Id_normalise(
    ITC_Id_t *pt_Id
);

/**
 * @brief Sum two existing IDs into a single ID
 *
 * @param pt_Id1 The first ID
 * @param pt_Id2 The second ID
 * @param pt_Id The new ID
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Id_sum(
    ITC_Id_t *pt_Id1,
    ITC_Id_t *pt_Id2,
    ITC_Id_t *pt_Id
);

#endif /* ITC_ID_PROTOTYPES_H_ */