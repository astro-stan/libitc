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

/**
 * @brief Allocate a new ITC ID
 *
 * @param ppt_Id (out) The pointer to the new ID
 * @param ppt_Id The pointer to the parent ID in the tree. Otherwise NULL.
 * @return Itc_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
Itc_Status_t ITC_Id_new(
    ITC_Id_t **ppt_Id,
    ITC_Id_t *pt_Parent
);

#endif /* ITC_ID_PACKAGE_H_ */