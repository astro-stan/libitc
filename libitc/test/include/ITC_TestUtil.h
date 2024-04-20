/**
 * @file ITC_TestUtil.h
 * @author Stan
 * @brief Testing utilities
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_TESTUTIL_H_
#define ITC_TESTUTIL_H_

#include "ITC_Id.h"
#include "ITC_Event.h"
#include "ITC_Status.h"

/**
 * @brief Same as ITC_Id_newNull but enforces setting the parent
 *
 * @param ppt_Id (out) The pointer to the NULL ID
 * @param pt_Parent The pointer to the parent ID. Otherwise NULL
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_TestUtil_newNullId(
    ITC_Id_t **ppt_Id,
    ITC_Id_t *pt_Parent
);

/**
 * @brief Same as ITC_Id_newSeed but enforces setting the parent
 *
 * @param ppt_Id (out) The pointer to the seed ID
 * @param pt_Parent The pointer to the parent ID. Otherwise NULL
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_TestUtil_newSeedId(
    ITC_Id_t **ppt_Id,
    ITC_Id_t *pt_Parent
);

/**
 * @brief Same as ITC_Event_new but enforces setting the parent and an
 * event count
 *
 * @param ppt_Event (out) The pointer to the Event
 * @param pt_Parent The pointer to the parent Event. Otherwise NULL
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_TestUtil_newEvent(
    ITC_Event_t **ppt_Event,
    ITC_Event_t *pt_Parent,
    ITC_Event_Counter_t t_Count
);

#endif /* ITC_TESTUTIL_H_ */
