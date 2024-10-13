/**
 * @file ITC_port.h
 * @brief Port-specific definitions
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#ifndef ITC_PORT_H_
#define ITC_PORT_H_

#include "ITC_Status.h"
#include "ITC_config.h"
#include "ITC_Event.h"
#include "ITC_Id.h"
#include "ITC_Stamp.h"


/******************************************************************************
 * Type definitions
 ******************************************************************************/

/**
 * Enum used to specify what type of data is being allocated/deallocated.
 */
typedef enum {
    /** Corresponds to an `ITC_Id_t` */
    ITC_PORT_ALLOCTYPE_ITC_ID_T,
    /** Corresponds to an `ITC_Event_t` */
    ITC_PORT_ALLOCTYPE_ITC_EVENT_T,
    /** Corresponds to an `ITC_Stamp_t` */
    ITC_PORT_ALLOCTYPE_ITC_STAMP_T,
} ITC_Port_AllocType_t;

#if ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_CONFIG_MEMORY_ALLOCATION_TYPE_STATIC

/******************************************************************************
 * Global variables
 ******************************************************************************/


/* The array storing all allocated ITC Id nodes */
extern ITC_Id_t *gpt_ItcIdNodeAllocationArray;

/* The length of the `gpt_ItcIdNodeAllocationArray` array */
extern uint32_t gu32_ItcIdNodeAllocationArrayLength;

/* The array storing all allocated ITC Event nodes */
extern ITC_Event_t *gpt_ItcEventNodeAllocationArray;

/* The length of the `gpt_ItcEventNodeAllocationArray` array */
extern uint32_t gu32_ItcEventNodeAllocationArrayLength;

/* The array storing all allocated ITC Stamp nodes */
extern ITC_Stamp_t *gpt_ItcStampNodeAllocationArray;

/* The length of the `gpt_ItcStampNodeAllocationArray` array */
extern uint32_t gu32_ItcStampNodeAllocationArrayLength;

#endif /* ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_CONFIG_MEMORY_ALLOCATION_TYPE_STATIC */

/******************************************************************************
 * Functions
 ******************************************************************************/

/**
 * @brief Init port
 *
 * Initialises the port logic for allocating/deallocating ITC nodes.
 *
 * @note This function will not be called automatically. It is the responsibility
 * of the user to call it when needed/appropriate. See `ITC_config.h` for more
 * information.
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Port_init(void);

/**
 * @brief Fini port
 *
 * Finalises the port logic for allocating/deallocating ITC nodes.
 *
 * @note This function will not be called automatically. It is the responsibility
 * of the user to call it when needed/appropriate. See `ITC_config.h` for more
 * infosrmation.
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Port_fini(void);

/**
 * @brief Allocate memory
 *
 * @param ppv_Ptr (out) Pointer to the allocated memory
 * @param t_AllocType The type of data being allocated
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Port_malloc(
    void **ppv_Ptr,
    ITC_Port_AllocType_t t_AllocType
);

/**
 * @brief Deallocate memory
 *
 * @param pv_Ptr Pointer to the memory to be freed
 * @param t_AllocType The type of data being deallocated
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Port_free(
    void *pv_Ptr,
    ITC_Port_AllocType_t t_AllocType
);

#endif /* ITC_PORT_H_ */
