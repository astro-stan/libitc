/**
 * @file ITC_Event_prototypes.h
 * @brief Prototypes for the Interval Tree Clock's Event mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#ifndef ITC_EVENT_PROTOTYPES_H_
#define ITC_EVENT_PROTOTYPES_H_

#include "ITC_config.h"

#if ITC_CONFIG_ENABLE_EXTENDED_API

#include "ITC_Event.h"
#include "ITC_Status.h"

/******************************************************************************
 * Functions
 ******************************************************************************/

/**
 * @brief Allocate a new ITC Event and initialise it
 *
 * @param ppt_Event (out) The pointer to the Event
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Event_new(
    ITC_Event_t **ppt_Event
);

/**
 * @brief Free an allocated ITC Event
 *
 * @warning Even if the function call fails, the Event might be partically freed
 * and not safe to use. For this reason, the returned `*ppt_Event` will always
 * be set to `NULL`.
 *
 * @param ppt_Event (in) The pointer to the Event to deallocate. (out) NULL
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Event_destroy(
    ITC_Event_t **ppt_Event
);

/**
 * @brief Clone an existing ITC Event
 *
 * @param pt_Event The existing Event
 * @param ppt_ClonedEvent (out) The pointer to the cloned Event
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Event_clone(
    const ITC_Event_t *const pt_Event,
    ITC_Event_t **ppt_ClonedEvent
);

/**
 * @brief Validate an Event
 *
 * @param pt_Event The Event to validate
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Event_validate(
    const ITC_Event_t *const pt_Event
);

/**
 * @brief Join two existing Events into a single Event
 *
 * @note On success, `ppt_OtherEvent` will be automatically deallocated to
 * prevent it from being used again accidentally (as well as to reduce developer
 * cleanup burden)
 * @param ppt_Event (in) The first existing Event. (out) The joined Event
 * @param ppt_OtherEvent (in) The second existing Event. (out) NULL
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Event_join(
    ITC_Event_t **ppt_Event,
    ITC_Event_t **ppt_OtherEvent
);

#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */

#endif /* ITC_EVENT_PROTOTYPES_H_ */
