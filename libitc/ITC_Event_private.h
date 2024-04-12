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
    (((pt_Event)->pt_Left->t_Count == 0) ||                                   \
    ((pt_Event)->pt_Right->t_Count == 0))) ||                                 \
   (ITC_EVENT_IS_LEAF_EVENT(pt_Event)))

/**
 * @brief Lift an Event fulfilling `lift(e)`
 * Rules:
 *  - lift(n, m) = (n + m)
 *  - lift((n, e1, e2), m) = (n + m, e1, e2)
 *
 * Effective signature:
 *      ITC_Status_t LIFT_EVENT_E(pt_Event, t_Count)
 *
 * @param pt_Event The event to be lifted
 * @param t_Count_ The number of events to be lifted with
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
#define LIFT_EVENT_E(pt_Event, t_Count_)                                      \
    incEventCounter(&(pt_Event)->t_Count, (t_Count_))

/**
 * @brief Sink an Event fulfilling `sink(e)`
 * Rules:
 *  - sink(n, m) = (n - m)
 *  - sink((n, e1, e2), m) = (n - m, e1, e2)
 *
 * Effective signature:
 *      ITC_Status_t SINK_EVENT_E(pt_Event, t_Count)
 *
 * @param pt_Event The Event to be sinked
 * @param t_Count The number of events to be sinked with
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
#define SINK_EVENT_E(pt_Event, t_Count_)                                      \
    decEventCounter(&(pt_Event)->t_Count, (t_Count_))

#endif /* ITC_EVENT_PRIVATE_H_ */