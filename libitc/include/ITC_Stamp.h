/**
 * @file ITC_Stamp.h
 * @author Stan
 * @brief Definitions for the Interval Tree Clock's Stamp mechanism
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_STAMP_H_
#define ITC_STAMP_H_

#include "ITC_Id.h"
#include "ITC_Event.h"

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
