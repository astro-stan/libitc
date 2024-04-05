/**
 * @file ITC_port.h
 * @author Stan
 * @brief Port specific definitions
 * @brief Configuration options for the ITC implementation
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_PORT_H_
#define ITC_PORT_H_

#include "ITC_Status.h"

#include <stddef.h>

/**
 * @brief Allocate memory
 *
 * @param ppv_Ptr (out) Pointer to the allocated memory
 * @param n_Size The size to allocate
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Port_malloc(
    void **ppv_Ptr,
    size_t n_Size
);

/**
 * @brief Deallocate memory
 *
 * @param pv_Ptr Pointer to the memory to be freed
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Port_free(
    void *pv_Ptr
);

#endif /* ITC_PORT_H_ */