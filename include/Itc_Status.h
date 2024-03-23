/**
 * @file Itc_Status.h
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
#define ITC_STATUS_SUCCESS                                                    0


/******************************************************************************
 * Types
 ******************************************************************************/

/** The resulting status of an operation */
typedef uint32_t Itc_Status_t;

#endif /* ITC_STATUS_H_ */