/**
 * @file ITC_Stamp.h
 * @brief Definitions for the Interval Tree Clock's Stamp mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#ifndef ITC_STAMP_H_
#define ITC_STAMP_H_

#include "ITC_Id.h"
#include "ITC_Event.h"

/* The Stamp comparison enum */
typedef enum
{
    /* The Stamp smaller than the other Stamp */
    ITC_STAMP_COMPARISON_LESS_THAN = (1 << 0),
    /* The Stamp bigger than the other Stamp */
    ITC_STAMP_COMPARISON_GREATER_THAN = (1 << 1),
    /* The Stamp is equal to the other Stamp */
    ITC_STAMP_COMPARISON_EQUAL = (1 << 2),
    /* The Stamp is concurrent to the other Stamp */
    ITC_STAMP_COMPARISON_CONCURRENT = (1 << 3),
} ITC_Stamp_Comparison_t;

/* The ITC Stamp */
typedef struct
{
    /* The ITC ID */
    ITC_Id_t *pt_Id;
    /* The ITC Event */
    ITC_Event_t *pt_Event;
} ITC_Stamp_t;

/* Late include. We need to define the types first */
#include "ITC_Stamp_prototypes.h"

#endif /* ITC_STAMP_H_ */
