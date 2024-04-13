/**
 * @file ITC_Test_package.h
 * @author Stan
 * @brief Unit testing package definitions
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_TEST_PACKAGE_H_
#define ITC_TEST_PACKAGE_H_

#include "unity.h"
#include "ITC_Status.h"
#include "ITC_Id_Test_package.h"
#include "ITC_Event_Test_package.h"

/******************************************************************************
 * Defines
 ******************************************************************************/

/** Returns the size of an array.
 * https://stackoverflow.com/a/4415646/11121557
 */
#define ARRAY_COUNT(x)                                                        \
    ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

/** Test a given function returns ITC_STATUS_SUCCESS */
#define TEST_SUCCESS(x)          TEST_ASSERT_EQUAL_UINT32(ITC_STATUS_SUCCESS, x)

/** Test a given function fails with status t_Status */
#define TEST_FAILURE(x, t_Status)          TEST_ASSERT_EQUAL_UINT32(t_Status, x)

#endif /* ITC_TEST_PACKAGE_H_ */
