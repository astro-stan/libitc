/**
 * @file ITC_Id_package.h
 * @brief Prototypes for the Interval Tree Clock's ID mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#ifndef ITC_ID_PACKAGE_H_
#define ITC_ID_PACKAGE_H_

#include "ITC_Id.h"
#include "ITC_Status.h"
#include "ITC_Config.h"

/******************************************************************************
 * Functions
 ******************************************************************************/

#if !ITC_CONFIG_ENABLE_EXTENDED_API

/**
 * @brief Allocate a new ITC ID and initialise it as a seed ID (1)
 *
 * @param ppt_Id (out) The pointer to the seed ID
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Id_newSeed(
    ITC_Id_t **ppt_Id
);

/**
 * @brief Allocate a new ITC ID and initialise it as a null ID (0)
 *
 * @param ppt_Id (out) The pointer to the null ID
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Id_newNull(
    ITC_Id_t **ppt_Id
);

/**
 * @brief Free an allocated ITC ID
 *
 * @warning Even if the function call fails, the ID might be partically freed
 * and not safe to use. For this reason, the returned `*ppt_ID` will always be
 * set to `NULL`.
 *
 * @param ppt_Id (in) The pointer to the ID to deallocate. (out) NULL
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
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
 * @param ppt_ClonedId (out) The pointer to the cloned ID
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Id_clone(
    const ITC_Id_t *const pt_Id,
    ITC_Id_t **ppt_ClonedId
);

/**
 * @brief Validate an ID
 *
 * @param pt_Id The ID to validate
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Id_validate(
    const ITC_Id_t *const pt_Id
);

/**
 * @brief Split an existing ITC ID into two distinct (non-overlaping) ITC IDs
 *
 * @param ppt_Id (in) The existing ID. (out) The first split ID
 * @param ppt_OtherId (out) The second split ID
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Id_split(
    ITC_Id_t **ppt_Id,
    ITC_Id_t **ppt_OtherId
);

/**
 * @brief Sum two existing IDs into a single ID
 *
 * @note On success, `ppt_OtherId` will be automatically deallocated to prevent
 * it from being used again accidentally (as well as to reduce developer
 * cleanup burden)
 * @param ppt_Id (in) The first existing ID. (out) The summed ID
 * @param ppt_OtherId (in) The second existing ID. (out) NULL
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Id_sum(
    ITC_Id_t **ppt_Id,
    ITC_Id_t **ppt_OtherId
);

#endif /* !ITC_CONFIG_ENABLE_EXTENDED_API */

/**
 * @brief Split an ID similar to ::ITC_Id_split() but do not modify the source ID
 *
 * @param pt_Id The existing ID
 * @param ppt_Id1 The first half of the split ID
 * @param ppt_Id2 The second half of the split ID
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Id_splitConst(
    const ITC_Id_t *const pt_Id,
    ITC_Id_t **ppt_Id1,
    ITC_Id_t **ppt_Id2
);

/**
 * @brief Sum two IDs similar to ::ITC_Id_sum() but do not modify the source IDs
 *
 * @param ppt_Id1 The first existing ID
 * @param ppt_Id2 The second existing ID
 * @param ppt_Id The summed ID
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Id_sumConst(
    const ITC_Id_t *const pt_Id1,
    const ITC_Id_t *const pt_Id2,
    ITC_Id_t **ppt_Id
);

#if IS_UNIT_TEST_BUILD

/**
 * @brief Normalise an ID
 *
 * @param pt_Id The ID to normalise
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Id_normalise(
    ITC_Id_t *pt_Id
);

#endif /* IS_UNIT_TEST_BUILD */

#endif /* ITC_ID_PACKAGE_H_ */
