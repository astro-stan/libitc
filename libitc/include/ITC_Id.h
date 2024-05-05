/**
 * @file ITC_Id.h
 * @brief Definitions for the Interval Tree Clock's ID mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#ifndef ITC_ID_H_
#define ITC_ID_H_

#include <stdbool.h>

/* The ITC ID */
typedef struct ITC_Id_t
{
    /** A pointer to the left ID subtree */
    struct ITC_Id_t *pt_Left;
    /** A pointer to the right ID subtree */
    struct ITC_Id_t *pt_Right;
    /** A pointer to the parent ID subtree. NULL if root */
    struct ITC_Id_t *pt_Parent;
    /** Determines whether the interval (or subinterval) represented by this
     * ID is owned by it (i.e. it can be used to inflate events) or not.
     * Parent (i.e. not leaf IDs) should always have this set to `false` */
    bool b_IsOwner;
} ITC_Id_t;

#endif /* ITC_ID_H_ */
