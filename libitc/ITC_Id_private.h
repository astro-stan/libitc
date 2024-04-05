/**
 * @file ITC_Id_private.h
 * @author Stan
 * @brief Package definitions for the Interval Tree Clock's ID mechanism
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_ID_PRIVATE_H_
#define ITC_ID_PRIVATE_H_

#include "ITC_Id.h"
#include "ITC_Status.h"


/******************************************************************************
 * Defines
 ******************************************************************************/

/** Checks whether the given `ITC_Id_t` is a leaf node */
#define ITC_ID_IS_LEAF_ID(pt_Id)                                              \
    ((pt_Id) && !(pt_Id)->pt_Left && !(pt_Id)->pt_Right)

/** Checks whether the given `ITC_Id_t` is a null ID */
#define ITC_ID_IS_NULL_ID(pt_Id)                                              \
    (ITC_ID_IS_LEAF_ID(pt_Id) && !(pt_Id)->b_IsOwner)

/** Checks whether the given `ITC_Id_t` is a seed ID */
#define ITC_ID_IS_SEED_ID(pt_Id)                                              \
    (ITC_ID_IS_LEAF_ID(pt_Id) && (pt_Id)->b_IsOwner)

/** Checks whether the given `ITC_Id_t` is a (0, 0) ID */
#define ITC_ID_IS_NULL_NULL_ID(pt_Id)                                         \
    (!ITC_ID_IS_LEAF_ID(pt_Id) &&                                             \
     ITC_ID_IS_NULL_ID(pt_Id->pt_Left) &&                                     \
     ITC_ID_IS_NULL_ID(pt_Id->pt_Right))

/** Checks whether the given `ITC_Id_t` is a (1, 1) ID */
#define ITC_ID_IS_SEED_SEED_ID(pt_Id)                                         \
    (!ITC_ID_IS_LEAF_ID(pt_Id) &&                                             \
     ITC_ID_IS_SEED_ID(pt_Id->pt_Left) &&                                     \
     ITC_ID_IS_SEED_ID(pt_Id->pt_Right))

/** Checks whether the given `ITC_Id_t` is a valid parent node
 * The ID must:
 *  - Have 2 child node addresses != NULL
 *  - Have 2 unique child node addresses
 *  - NOT own its interval (pt_Id->b_IsOwner == false)
 */
#define ITC_IS_VALID_PARENT_ID(pt_Id)                                         \
    ((pt_Id->pt_Left && pt_Id->pt_Right) &&                                   \
     (pt_Id->pt_Left != pt_Id->pt_Right) &&                                   \
     (!pt_Id->b_IsOwner))                                                     \

#endif /* ITC_ID_PRIVATE_H_ */