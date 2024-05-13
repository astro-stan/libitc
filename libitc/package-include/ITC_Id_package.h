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

/******************************************************************************
 * Functions
 ******************************************************************************/

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

#endif /* ITC_ID_PACKAGE_H_ */
