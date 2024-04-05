/**
 * @file ITC_Port.c
 * @author Stan
 * @brief Port specific implementation
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ITC_Port.h"

#include <stdlib.h>

/******************************************************************************
 * Public functions
 ******************************************************************************/

/******************************************************************************
 * Allocate memory
 ******************************************************************************/

ITC_Status_t ITC_Port_malloc(
    void **ppv_Ptr,
    size_t n_Size
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS;

    if (ppv_Ptr)
    {
        *ppv_Ptr = malloc(n_Size);

        if (!ppv_Ptr)
        {
            t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
        }
    }
    else
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }


    return t_Status;
}

/******************************************************************************
 * Deallocate memory
 ******************************************************************************/

ITC_Status_t ITC_Port_free(
    void *pv_Ptr
)
{
    free(pv_Ptr);

    /* Always suceeds */
    return ITC_STATUS_SUCCESS;
}
