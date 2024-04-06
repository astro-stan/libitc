/**
 * @file ITC_Id_Test_private.h
 * @author Stan
 * @brief Private testing definitions for Interval Tree Clock's ID mechanism
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_ID_TEST_PRIVATE_H_
#define ITC_ID_TEST_PRIVATE_H_

#include "unity.h"
#include "ITC_Status.h"

/******************************************************************************
 * Defines
 ******************************************************************************/

/** Checks whether the given `ITC_Id_t` is a leaf ID */
#define ITC_ID_IS_LEAF_ID(pt_Id)                                              \
    ((pt_Id) && !(pt_Id)->pt_Left && !(pt_Id)->pt_Right)

/** Checks whether the given `ITC_Id_t` is a valid parent node
 * The ID must:
 *  - Have 2 child node addresses != NULL
 *  - Have 2 unique child node addresses
 *  - NOT own its interval (pt_Id->b_IsOwner == false)
 */
#define ITC_ID_IS_VALID_PARENT(pt_Id)                                         \
    ((pt_Id) &&                                                               \
     ((pt_Id)->pt_Left && (pt_Id)->pt_Right) &&                               \
     ((pt_Id)->pt_Left != (pt_Id)->pt_Right) &&                               \
     (!(pt_Id)->b_IsOwner))                                                   \

/** Checks whether the given `ITC_Id_t` is a NULL ID */
#define ITC_ID_IS_NULL_ID(pt_Id)                                              \
    (ITC_ID_IS_LEAF_ID(pt_Id) && !(pt_Id)->b_IsOwner)

/** Checks whether the given `ITC_Id_t` is a seed ID */
#define ITC_ID_IS_SEED_ID(pt_Id)                                              \
    (ITC_ID_IS_LEAF_ID(pt_Id) && (pt_Id)->b_IsOwner)

/** Checks whether the given `ITC_Id_t` is a (0, 1) ID */
#define ITC_ID_IS_NULL_SEED_ID(pt_Id)                                         \
    (ITC_ID_IS_VALID_PARENT(pt_Id) &&                                         \
     ITC_ID_IS_NULL_ID((pt_Id)->pt_Left) &&                                   \
     ITC_ID_IS_SEED_ID((pt_Id)->pt_Right))

/** Checks whether the given `ITC_Id_t` is a (1, 0) ID */
#define ITC_ID_IS_SEED_NULL_ID(pt_Id)                                         \
    (ITC_ID_IS_VALID_PARENT(pt_Id) &&                                         \
     ITC_ID_IS_SEED_ID((pt_Id)->pt_Left) &&                                   \
     ITC_ID_IS_NULL_ID((pt_Id)->pt_Right))

/** Checks whether the given `ITC_Id_t` is a (0, 0) ID */
#define ITC_ID_IS_NULL_NULL_ID(pt_Id)                                         \
    (ITC_ID_IS_VALID_PARENT(pt_Id) &&                                         \
     ITC_ID_IS_NULL_ID((pt_Id)->pt_Left) &&                                   \
     ITC_ID_IS_NULL_ID((pt_Id)->pt_Right))

/** Checks whether the given `ITC_Id_t` is a (1, 1) ID */
#define ITC_ID_IS_SEED_SEED_ID(pt_Id)                                         \
    (ITC_ID_IS_VALID_PARENT(pt_Id) &&                                         \
     ITC_ID_IS_SEED_ID((pt_Id)->pt_Left) &&                                   \
     ITC_ID_IS_SEED_ID((pt_Id)->pt_Right))

/** Test an ID is a NULL ID */
#define TEST_ID_IS_NULL_ID(pt_Id)                                             \
    TEST_ASSERT_TRUE(ITC_ID_IS_NULL_ID(pt_Id))

/** Test an ID is a seed ID */
#define TEST_ID_IS_SEED_ID(pt_Id)                                             \
    TEST_ASSERT_TRUE(ITC_ID_IS_SEED_ID(pt_Id))

/** Test an ID is not a leaf ID */
#define TEST_ID_IS_NOT_LEAF_ID(pt_Id)                                         \
    TEST_ASSERT_TRUE(ITC_ID_IS_VALID_PARENT(pt_Id))

/** Test an ID represents (1, 0) */
#define TEST_ID_IS_SEED_NULL_ID(pt_Id)                                        \
    TEST_ASSERT_TRUE(ITC_ID_IS_SEED_NULL_ID(pt_Id))

/** Test an ID represents (0, 1) */
#define TEST_ID_IS_NULL_SEED_ID(pt_Id)                                        \
    TEST_ASSERT_TRUE(ITC_ID_IS_NULL_SEED_ID(pt_Id))

/** Test an ID represents (0, 0) */
#define TEST_ID_IS_NULL_NULL_ID(pt_Id)                                        \
    TEST_ASSERT_TRUE(ITC_ID_IS_NULL_NULL_ID(pt_Id))

/** Test an ID represents (1, 1) */
#define TEST_ID_IS_SEED_SEED_ID(pt_Id)                                        \
    TEST_ASSERT_TRUE(ITC_ID_IS_SEED_SEED_ID(pt_Id))

#endif /* ITC_ID_TEST_PRIVATE_H_ */
