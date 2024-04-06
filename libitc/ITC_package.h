/**
 * @file ITC_package.h
 * @author Stan
 * @brief Package definitions for the Interval Tree Clock's library
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_PACKAGE_H_
#define ITC_PACKAGE_H_

/******************************************************************************
 * Defines
 ******************************************************************************/

/** Returns the minimum of two values
 * @note Non-idempotent expressions, such as `myVar++` or `myVar--` will
 * execute multiple times and can cause undefined behaviour
*/
#define MIN(a, b)                                      (((a) < (b)) ? (a) : (b))

#endif /* ITC_PACKAGE_H_ */