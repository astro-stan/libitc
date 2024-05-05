/**
 * @file ITC_Event.h
 * @brief Definitions for the Interval Tree Clock's Event mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#ifndef ITC_EVENT_H_
#define ITC_EVENT_H_

#include "ITC_config.h"

#include <stdint.h>

#if ITC_CONFIG_USE_64BIT_EVENT_COUNTERS
/* The ITC Event counter */
typedef uint64_t ITC_Event_Counter_t;
#else
/* The ITC Event counter */
typedef uint32_t ITC_Event_Counter_t;
#endif

/* The ITC Event */
typedef struct ITC_Event_t
{
    /** A pointer to the left Event subtree */
    struct ITC_Event_t *pt_Left;
    /** A pointer to the right Event subtree */
    struct ITC_Event_t *pt_Right;
    /** A pointer to the parent Event subtree. NULL if root */
    struct ITC_Event_t *pt_Parent;
    /** Counts the number of events witnessed by this node in the event tree */
    ITC_Event_Counter_t t_Count;
} ITC_Event_t;

#endif /* ITC_EVENT_H_ */
