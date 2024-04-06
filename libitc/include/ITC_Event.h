/**
 * @file ITC_Event.h
 * @author Stan
 * @brief Definitions for the Interval Tree Clock's Event mechanism
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_EVENT_H_
#define ITC_EVENT_H_

#include <stdint.h>

/* The ITC Event counter */
typedef uint32_t ITC_Event_Counter_t;

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

/* Late include. We need to define the types first */
#include "ITC_Event_prototypes.h"

#endif /* ITC_EVENT_H_ */