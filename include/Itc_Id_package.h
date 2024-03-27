/**
 * @file Itc_Id_package.h
 * @author Stan
 * @brief Package definitions for the Interval Tree Clock's ID mechanism
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_ID_PACKAGE_H_
#define ITC_ID_PACKAGE_H_

#include "Itc_Id.h"
#include "Itc_Status.h"


/******************************************************************************
 * Defines
 ******************************************************************************/

/** Checks whether the given `ITC_Id_t` is a leaf node */
#define ITC_ID_IS_LEAF_NODE(pt_Id)                                            \
    ((pt_Id) && !(pt_Id)->pt_Left && !(pt_Id)->pt_Right)

/** Checks whether the given `ITC_Id_t` is a null ID */
#define ITC_ID_IS_NULL_ID(pt_Id)                                              \
    (ITC_ID_IS_LEAF_NODE(pt_Id) && !(pt_Id)->b_IsOwner)

/** Checks whether the given `ITC_Id_t` is a seed ID */
#define ITC_ID_IS_SEED_ID(pt_Id)                                              \
    (ITC_ID_IS_LEAF_NODE(pt_Id) && (pt_Id)->b_IsOwner)

/******************************************************************************
 * Functions
 ******************************************************************************/

/**
 * @brief Allocate a new ITC ID
 *
 * @param ppt_Id (out) The pointer to the new ID
 * @param ppt_Parent The pointer to the parent ID in the tree. Otherwise NULL.
 * @param b_IsOwner Whether the ID owns its interval or not.
 * @return Itc_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
Itc_Status_t ITC_Id_new(
    ITC_Id_t **ppt_Id,
    ITC_Id_t *pt_Parent,
    bool b_IsOwner
);

#endif /* ITC_ID_PACKAGE_H_ */