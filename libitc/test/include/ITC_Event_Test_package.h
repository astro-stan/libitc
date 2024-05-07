/**
 * @file ITC_Event_Test_package.h
 * @brief Package testing definitions for Interval Tree Clock's Event mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#ifndef ITC_EVENT_TEST_PACKAGE_H_
#define ITC_EVENT_TEST_PACKAGE_H_

#include "unity.h"
#include "ITC_Status.h"

/******************************************************************************
 * Defines
 ******************************************************************************/

/** Checks whether the given `ITC_Event_t` is a leaf Event */
#define ITC_EVENT_IS_LEAF_EVENT(pt_Event)                                      \
    ((pt_Event) && !(pt_Event)->pt_Left && !(pt_Event)->pt_Right)

/** Checks whether the given `ITC_Event_t` is a valid parent node
 * The ID must:
 *  - Have 2 child node addresses != NULL
 *  - Have 2 unique child node addresses
 */
#define ITC_EVENT_IS_VALID_PARENT(pt_Event)                                    \
    ((pt_Event) &&                                                             \
     ((pt_Event)->pt_Left && (pt_Event)->pt_Right) &&                          \
     ((pt_Event)->pt_Left != (pt_Event)->pt_Right))                            \

/** Test the Event is a leaf node and has a specific event count */
#define TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Event, t_Count_)                     \
    do                                                                         \
    {                                                                          \
        TEST_ASSERT_TRUE_MESSAGE(                                              \
            ITC_EVENT_IS_LEAF_EVENT(pt_Event), "Not a leaf Event node");       \
        TEST_ASSERT_EQUAL((t_Count_), (pt_Event)->t_Count);                    \
    }                                                                          \
    while(0)                                                                   \

/** Test the Event is a parent node and has a specific event count */
#define TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_Event, t_Count_)                   \
    do                                                                         \
    {                                                                          \
        TEST_ASSERT_TRUE_MESSAGE(                                              \
            ITC_EVENT_IS_VALID_PARENT(pt_Event),                               \
            "Not a valid parent Event node");                                  \
        TEST_ASSERT_EQUAL((t_Count_), (pt_Event)->t_Count);                    \
    }                                                                          \
    while(0)

#endif /* ITC_EVENT_TEST_PACKAGE_H_ */
