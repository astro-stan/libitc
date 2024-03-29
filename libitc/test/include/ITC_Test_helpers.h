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

/** Test a given function returns ITC_STATUS_SUCCESS */
#define TEST_SUCCESS(x) (TEST_ASSERT_EQUAL_UINT32(ITC_STATUS_SUCCESS, x))

/** Test a given function fails with status t_Status */
#define TEST_FAILURE(x, t_Status) (TEST_ASSERT_EQUAL_UINT32(t_Status, x))

#endif /* ITC_TEST_HELPERS_H_ */
