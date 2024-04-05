/**
 * @file ITC_Status.h
 * @author Stan
 * @brief Status codes for the ITC implementation
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_STATUS_H_
#define ITC_STATUS_H_

#include <stdint.h>

/******************************************************************************
 * Defines
 ******************************************************************************/

/** Operation succeeded */
#define ITC_STATUS_SUCCESS                                                   (0)

/** Operation failed */
#define ITC_STATUS_FAILURE                                                   (1)

/** Operation failed due to invalid input parameter */
#define ITC_STATUS_INVALID_PARAM                                             (2)

/** Operation failed due to insufficient resources */
#define ITC_STATUS_INSUFFICIENT_RESOURCES                                    (3)

/** Operation failed due to the ITC ID being corrupted */
#define ITC_STATUS_CORRUPT_ID                                                (4)

/** Operation failed due to the ITC Event being corrupted */
#define ITC_STATUS_CORRUPT_EVENT                                             (5)

/******************************************************************************
 * Types
 ******************************************************************************/

/** The resulting status of an operation */
typedef uint32_t ITC_Status_t;

#endif /* ITC_STATUS_H_ */