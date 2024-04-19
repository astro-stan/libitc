/**
 * @file ITC_Event_Test_package.h
 * @author Stan
 * @brief Package testing definitions for Interval Tree Clock's Event mechanism
 *
 * @copyright Copyright (c) 2024
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
#define ITC_EVENT_IS_LEAF_EVENT(pt_Event)                                     \
    ((pt_Event) && !(pt_Event)->pt_Left && !(pt_Event)->pt_Right)

/** Checks whether the given `ITC_Event_t` is a valid parent node
 * The ID must:
 *  - Have 2 child node addresses != NULL
 *  - Have 2 unique child node addresses
 */
#define ITC_EVENT_IS_VALID_PARENT(pt_Event)                                   \
    ((pt_Event) &&                                                            \
     ((pt_Event)->pt_Left && (pt_Event)->pt_Right) &&                         \
     ((pt_Event)->pt_Left != (pt_Event)->pt_Right))                           \

/** Test the Event is a leaf node and has a specific event count */
#define TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Event, t_Count_)                    \
    TEST_ASSERT_TRUE(                                                         \
        (ITC_EVENT_IS_LEAF_EVENT(pt_Event) &&                                 \
        (pt_Event)->t_Count == (t_Count_)))

/** Test the Event is a parent node and has a specific event count */
#define TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_Event, t_Count_)                  \
    TEST_ASSERT_TRUE(                                                         \
        (ITC_EVENT_IS_VALID_PARENT(pt_Event) &&                               \
        (pt_Event)->t_Count == (t_Count_)))

/** The index of the first normalisation related invalid Event test inside
 * the `gpv_InvalidEventConstructorTable` and `gpv_InvalidEventDestructorTable`
 * tables.
 */
#define FIRST_NORMALISATION_RELATED_INVALID_EVENT_INDEX                      (4)

#endif /* ITC_EVENT_TEST_PACKAGE_H_ */
