/**
 * @file Itc_Id.h
 * @author Stan
 * @brief Definitions for the Interval Tree Clock's ID mechanism
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_ID_H_
#define ITC_ID_H_

#include <stdbool.h>

/* The ITC ID */
typedef struct
{
    /** A pointer to the left ID subtree */
    ITC_Id_t *pt_Left;
    /** A pointer to the right ID subtree */
    ITC_Id_t *pt_Right;
    /** Determines whether the interval (or subinterval) represented by this
     * ID is owned by it (i.e. it can be used to inflate events) or not */
    bool b_IsOwner;
} ITC_Id_t;

/* Late include. We need to define the types first */
#include "Itc_Id_prototypes.h"

#endif /* ITC_ID_H_ */