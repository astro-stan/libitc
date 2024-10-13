/**
 * @file ITC_Port.c
 * @brief Port specific implementation
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#include "ITC_config.h"

#if ITC_CONFIG_MEMORY_ALLOCATION_TYPE != ITC_CONFIG_MEMORY_ALLOCATION_TYPE_CUSTOM
#include "ITC_Port.h"

#if ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_CONFIG_MEMORY_ALLOCATION_TYPE_MALLOC
#include <stdlib.h>
#endif /* ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_CONFIG_MEMORY_ALLOCATION_TYPE_MALLOC */

#if ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_CONFIG_MEMORY_ALLOCATION_TYPE_STATIC
#include "ITC_Port_private.h"

#include <string.h>

/******************************************************************************
 * Private functions
 ******************************************************************************/

static ITC_Status_t getStaticMemory(
    ITC_Port_AllocType_t t_AllocType,
    uint8_t **ppu8_Array,
    uint32_t *pu32_ArrayLength,
    uint32_t *pu32_AllocationSize
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    switch (t_AllocType)
    {
        case ITC_PORT_ALLOCTYPE_ITC_ID_T:
        {
            *ppu8_Array = (uint8_t *)gpt_ItcIdNodeAllocationArray;
            *pu32_ArrayLength = gu32_ItcIdNodeAllocationArrayLength;
            *pu32_AllocationSize = sizeof(ITC_Id_t);
            break;
        }
        case ITC_PORT_ALLOCTYPE_ITC_EVENT_T:
        {
            *ppu8_Array = (uint8_t *)gpt_ItcEventNodeAllocationArray;
            *pu32_ArrayLength = gu32_ItcEventNodeAllocationArrayLength;
            *pu32_AllocationSize = sizeof(ITC_Event_t);
            break;
        }
        case ITC_PORT_ALLOCTYPE_ITC_STAMP_T:
        {
            *ppu8_Array = (uint8_t *)gpt_ItcStampNodeAllocationArray;
            *pu32_ArrayLength = gu32_ItcStampNodeAllocationArrayLength;
            *pu32_AllocationSize = sizeof(ITC_Stamp_t);
            break;
        }
        default:
        {
            t_Status = ITC_STATUS_INVALID_PARAM;
            break;
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS &&
        (*pu32_ArrayLength < 1 || *pu32_AllocationSize < 1))
    {
        t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
    }

    return t_Status;
}

static void *staticMalloc(
    ITC_Port_AllocType_t t_AllocType
)
{
    void *pv_Ptr = NULL;
    ITC_Status_t t_Status;
    uint8_t *pu8_Array = NULL;
    uint32_t u32_ArrayLength;
    uint32_t u32_AllocSize;

    t_Status = getStaticMemory(
        t_AllocType, &pu8_Array, &u32_ArrayLength, &u32_AllocSize);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Find an empty spot and return it */
        for (uint32_t u32_I = 0;
             u32_I < u32_ArrayLength * u32_AllocSize;
             u32_I += u32_AllocSize)
        {
            /* Check if the element in the array is free */
            if (pu8_Array[u32_I] == ITC_PORT_FREE_SLOT_PATTERN &&
                memcmp((const void *)&pu8_Array[u32_I],
                       (const void *)&pu8_Array[u32_I + 1],
                       u32_AllocSize - 1) == 0)
            {
                pv_Ptr = (void *)&pu8_Array[u32_I];
                break;
            }
        }
    }

    return pv_Ptr;
}

static ITC_Status_t staticFree(
    void *pv_Ptr,
    ITC_Port_AllocType_t t_AllocType
)
{
    ITC_Status_t t_Status;
    uint8_t *pu8_Array = NULL;
    uint32_t u32_ArrayLength;
    uint32_t u32_AllocSize;

    if (pv_Ptr)
    {
        t_Status = getStaticMemory(
            t_AllocType, &pu8_Array, &u32_ArrayLength, &u32_AllocSize);
    }
    else
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* clang-format off */
        /* Make sure the pointer to be deallocated is:
         * - located inside the corresponding array
         * - is a multiple of the allocation size (i.e. not between elements) */
        if (((uintptr_t)pv_Ptr >= (uintptr_t)&pu8_Array[0]) &&
            ((uintptr_t)pv_Ptr <= (uintptr_t)&pu8_Array[(u32_ArrayLength - 1) * u32_AllocSize]) &&
            (((uintptr_t)pv_Ptr - (uintptr_t)&pu8_Array[0]) % u32_AllocSize == 0))
        {
            /* Free the memory */
            memset(pv_Ptr, ITC_PORT_FREE_SLOT_PATTERN, u32_AllocSize);
        }
        /* clang-format on */
    }

    return t_Status;
}

#endif /* ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_CONFIG_MEMORY_ALLOCATION_TYPE_STATIC */

/******************************************************************************
 * Public functions
 ******************************************************************************/

/******************************************************************************
 * Init port
 ******************************************************************************/

ITC_Status_t ITC_Port_init(void)
{
#if ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_CONFIG_MEMORY_ALLOCATION_TYPE_STATIC
    memset(
        (void *)&gpt_ItcIdNodeAllocationArray[0],
        ITC_PORT_FREE_SLOT_PATTERN,
        gu32_ItcIdNodeAllocationArrayLength * sizeof(ITC_Id_t)
    );
    memset(
        (void *)&gpt_ItcEventNodeAllocationArray[0],
        ITC_PORT_FREE_SLOT_PATTERN,
        gu32_ItcEventNodeAllocationArrayLength * sizeof(ITC_Event_t)
    );
    memset(
        (void *)&gpt_ItcStampNodeAllocationArray[0],
        ITC_PORT_FREE_SLOT_PATTERN,
        gu32_ItcStampNodeAllocationArrayLength * sizeof(ITC_Stamp_t)
    );
#endif /* ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_CONFIG_MEMORY_ALLOCATION_TYPE_STATIC */

    /* Always succeeds */
    return ITC_STATUS_SUCCESS;
}

/******************************************************************************
 * Fini port
 ******************************************************************************/

ITC_Status_t ITC_Port_fini(void)
{
    /* Always succeeds */
    return ITC_STATUS_SUCCESS;
}

/******************************************************************************
 * Allocate memory
 ******************************************************************************/

ITC_Status_t ITC_Port_malloc(
    void **ppv_Ptr,
    ITC_Port_AllocType_t t_AllocType
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS;

    if (ppv_Ptr)
    {
#if ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_CONFIG_MEMORY_ALLOCATION_TYPE_STATIC
        *ppv_Ptr = staticMalloc(t_AllocType);
#else
        switch (t_AllocType)
        {
            case ITC_PORT_ALLOCTYPE_ITC_ID_T:
                {
                    *ppv_Ptr = malloc(sizeof(ITC_Id_t));
                    break;
                }
            case ITC_PORT_ALLOCTYPE_ITC_EVENT_T:
                {
                    *ppv_Ptr = malloc(sizeof(ITC_Event_t));
                    break;
                }
            case ITC_PORT_ALLOCTYPE_ITC_STAMP_T:
                {
                    *ppv_Ptr = malloc(sizeof(ITC_Stamp_t));
                    break;
                }
            default:
                {
                    *ppv_Ptr = NULL;
                    t_Status = ITC_STATUS_INVALID_PARAM;
                    break;
                }
        }
#endif /* ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_CONFIG_MEMORY_ALLOCATION_TYPE_STATIC */

        if (!(*ppv_Ptr) && t_Status == ITC_STATUS_SUCCESS)
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
    void *pv_Ptr,
    ITC_Port_AllocType_t t_AllocType
)
{
#if ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_CONFIG_MEMORY_ALLOCATION_TYPE_STATIC
    return staticFree(pv_Ptr, t_AllocType);
#else
    free(pv_Ptr);
    /* Always suceeds */
    return ITC_STATUS_SUCCESS;
#endif /* ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_CONFIG_MEMORY_ALLOCATION_TYPE_STATIC */
}
#endif /* ITC_CONFIG_MEMORY_ALLOCATION_TYPE != ITC_CONFIG_MEMORY_ALLOCATION_TYPE_CUSTOM */
