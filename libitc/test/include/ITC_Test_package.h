/**
 * @file ITC_Test_helpers.h
 * @author Stan
 * @brief Helpers for unit testing
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_TEST_HELPERS_H_
#define ITC_TEST_HELPERS_H_

#include "unity.h"
#include "ITC_Status.h"

/******************************************************************************
 * Defines
 ******************************************************************************/

/** Checks whether the given `ITC_Id_t` is a leaf ID */
#define ITC_ID_IS_LEAF_ID(pt_Id)                                              \
    ((pt_Id) && !(pt_Id)->pt_Left && !(pt_Id)->pt_Right)

/** Checks whether the given `ITC_Id_t` is a null ID */
#define ITC_ID_IS_NULL_ID(pt_Id)                                              \
    (ITC_ID_IS_LEAF_ID(pt_Id) && !(pt_Id)->b_IsOwner)

/** Checks whether the given `ITC_Id_t` is a seed ID */
#define ITC_ID_IS_SEED_ID(pt_Id)                                              \
    (ITC_ID_IS_LEAF_ID(pt_Id) && (pt_Id)->b_IsOwner)

/** Checks whether the given `ITC_Id_t` is a (0, 1) ID */
#define ITC_ID_IS_NULL_SEED_ID(pt_Id)                                         \
    (!ITC_ID_IS_LEAF_ID(pt_Id) &&                                             \
     ITC_ID_IS_NULL_ID(pt_Id->pt_Left) &&                                     \
     ITC_ID_IS_SEED_ID(pt_Id->pt_Right))

/** Checks whether the given `ITC_Id_t` is a (1, 0) ID */
#define ITC_ID_IS_SEED_NULL_ID(pt_Id)                                         \
    (!ITC_ID_IS_LEAF_ID(pt_Id) &&                                             \
     ITC_ID_IS_SEED_ID(pt_Id->pt_Left) &&                                     \
     ITC_ID_IS_NULL_ID(pt_Id->pt_Right))

/** Checks whether the given `ITC_Id_t` is a (0, 0) ID */
#define ITC_ID_IS_NULL_NULL_ID(pt_Id)                                         \
    (!ITC_ID_IS_LEAF_ID(pt_Id) &&                                             \
     ITC_ID_IS_NULL_ID(pt_Id->pt_Left) &&                                     \
     ITC_ID_IS_NULL_ID(pt_Id->pt_Right))

/** Checks whether the given `ITC_Id_t` is a (1, 1) ID */
#define ITC_ID_IS_SEED_SEED_ID(pt_Id)                                         \
    (!ITC_ID_IS_LEAF_ID(pt_Id) &&                                             \
     ITC_ID_IS_SEED_ID(pt_Id->pt_Left) &&                                     \
     ITC_ID_IS_SEED_ID(pt_Id->pt_Right))

/** Test a given function returns ITC_STATUS_SUCCESS */
#define TEST_SUCCESS(x)          TEST_ASSERT_EQUAL_UINT32(ITC_STATUS_SUCCESS, x)

/** Test a given function fails with status t_Status */
#define TEST_FAILURE(x, t_Status)          TEST_ASSERT_EQUAL_UINT32(t_Status, x)

/** Test an ID is a NULL ID */
#define TEST_IS_NULL_ID(pt_Id)        TEST_ASSERT_TRUE(ITC_ID_IS_NULL_ID(pt_Id))

/** Test an ID is a seed ID */
#define TEST_IS_SEED_ID(pt_Id)        TEST_ASSERT_TRUE(ITC_ID_IS_SEED_ID(pt_Id))

/** Test an ID is not a leaf ID */
#define TEST_IS_NOT_LEAF_ID(pt_Id)   TEST_ASSERT_FALSE(ITC_ID_IS_LEAF_ID(pt_Id))

/** Test an ID represents (1, 0) */
#define TEST_IS_SEED_NULL_ID(pt_Id)                                           \
    TEST_ASSERT_TRUE(ITC_ID_IS_SEED_NULL_ID(pt_Id))

/** Test an ID represents (0, 1) */
#define TEST_IS_NULL_SEED_ID(pt_Id)                                           \
    TEST_ASSERT_TRUE(ITC_ID_IS_NULL_SEED_ID(pt_Id))

/** Test an ID represents (0, 0) */
#define TEST_IS_NULL_NULL_ID(pt_Id)                                           \
    TEST_ASSERT_TRUE(ITC_ID_IS_NULL_NULL_ID(pt_Id))

/** Test an ID represents (1, 1) */
#define TEST_IS_SEED_SEED_ID(pt_Id)                                           \
    TEST_ASSERT_TRUE(ITC_ID_IS_SEED_SEED_ID(pt_Id))

/******************************************************************************
 * Types
 ******************************************************************************/

/* A null-terminated character string */
typedef char* string_t;

/* A constant null-terminated character string */
typedef const char* cstring_t;

#endif /* ITC_TEST_HELPERS_H_ */
