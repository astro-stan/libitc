/**
 * @file ITC_Event_private.h
 * @author Stan
 * @brief Private definitions for the Interval Tree Clock's Event mechanism
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_EVENT_PRIVATE_H_
#define ITC_EVENT_PRIVATE_H_

#include "ITC_Event.h"
#include "ITC_Status.h"


/******************************************************************************
 * Defines
 ******************************************************************************/

/** Checks whether the given `ITC_Event_t` is a leaf node */
#define ITC_EVENT_IS_LEAF_EVENT(pt_Event)                                     \
    ((pt_Event) && !(pt_Event)->pt_Left && !(pt_Event)->pt_Right)

/** Checks whether the given `ITC_Event_t` is a valid parent node
 * The ID must:
 *  - Have 2 child node addresses != NULL
 *  - Have 2 unique child node addresses
 */
#define ITC_EVENT_IS_VALID_PARENT(pt_Event)                                   \
    ((pt_Event) &&                                                            \
     ((pt_Event)->pt_Left && (pt_Event)->pt_Right) &&                         \
     ((pt_Event)->pt_Left != (pt_Event)->pt_Right))

/** Checks whether the given `ITC_Event_t` is a normalised event node
 * A normalised event tree is:
 * - A leaf node
 * - A valid parent node with one of its subtrees having an event counter == 0
*/
#define ITC_EVENT_IS_NORMALISED_EVENT(pt_Event)                               \
  ((ITC_EVENT_IS_VALID_PARENT(pt_Event) &&                                    \
    (((pt_Event)->pt_Left->u32_Count == 0) ||                                 \
    ((pt_Event)->pt_Right->u32_Count == 0))) ||                               \
   (ITC_EVENT_IS_LEAF_EVENT(pt_Event)))

#endif /* ITC_EVENT_PRIVATE_H_ */