/**
 * @file ITC_Id_private.h
 * @brief Private definitions for the Interval Tree Clock's ID mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
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
#define ITC_ID_IS_LEAF_ID(pt_Id)                                               \
    ((pt_Id) && !(pt_Id)->pt_Left && !(pt_Id)->pt_Right)

/** Checks whether the given `ITC_Id_t` is a parent node */
#define ITC_ID_IS_PARENT_ID(pt_Id)                                             \
    ((pt_Id) && (pt_Id)->pt_Left && (pt_Id)->pt_Right)

/** Checks whether the given `ITC_Id_t` is a valid parent node
 * The ID must:
 *  - Have 2 child node addresses != NULL
 *  - Have 2 unique child node addresses
 *  - NOT own its interval (pt_Id->b_IsOwner == false)
 */
#define ITC_ID_IS_VALID_PARENT(pt_Id)                                          \
    (ITC_ID_IS_PARENT_ID(pt_Id) &&                                             \
     ((pt_Id)->pt_Left != (pt_Id)->pt_Right) &&                                \
     (!(pt_Id)->b_IsOwner))                                                    \

/** Checks whether the given `ITC_Id_t` is a null ID */
#define ITC_ID_IS_NULL_ID(pt_Id)                                               \
    (ITC_ID_IS_LEAF_ID(pt_Id) && !(pt_Id)->b_IsOwner)

/** Checks whether the given `ITC_Id_t` is a seed ID */
#define ITC_ID_IS_SEED_ID(pt_Id)                                               \
    (ITC_ID_IS_LEAF_ID(pt_Id) && (pt_Id)->b_IsOwner)

/** Checks whether the given `ITC_Id_t` is a (0, 0) ID.
 * @note This macro assumes the ID is a valid parent node.
 * I.e. `ITC_ID_IS_VALID_PARENT(pt_Id) == true` */
#define ITC_ID_IS_NULL_NULL_ID(pt_Id)                                          \
    (ITC_ID_IS_PARENT_ID(pt_Id) &&                                             \
     ITC_ID_IS_NULL_ID((pt_Id)->pt_Left) &&                                    \
     ITC_ID_IS_NULL_ID((pt_Id)->pt_Right))

/** Checks whether the given `ITC_Id_t` is a (1, 1) ID.
 * @note This macro assumes the ID is a valid parent node.
 * I.e. `ITC_ID_IS_VALID_PARENT(pt_Id) == true` */
#define ITC_ID_IS_SEED_SEED_ID(pt_Id)                                          \
    (ITC_ID_IS_PARENT_ID(pt_Id) &&                                             \
     ITC_ID_IS_SEED_ID((pt_Id)->pt_Left) &&                                    \
     ITC_ID_IS_SEED_ID((pt_Id)->pt_Right))

/** Checks whether the given `ITC_Id_t` is a normalised ID node
 * A normalised ID tree is:
 * - A leaf node
 * - A **valid** parent node with its subtrees having one of the following
 *   combinations of interval ownerships:
 *   - If both subtrees are parent nodes: (0, 0)
 *   - If both subtrees are leaf nodes: (0, 1) or (1, 0)
 *   - Otherwise: (0, 0), (0, 1) or (1, 0), where any parent node ownership
 *     must be set to 0 */
#define ITC_ID_IS_NORMALISED_ID(pt_Id)                                         \
  (ITC_ID_IS_LEAF_ID(pt_Id) ||                                                 \
   (ITC_ID_IS_VALID_PARENT(pt_Id) &&                                           \
    !(((pt_Id)->pt_Left->b_IsOwner && (pt_Id)->pt_Right->b_IsOwner) ||         \
     (ITC_ID_IS_PARENT_ID((pt_Id)->pt_Left) && (pt_Id)->pt_Left->b_IsOwner) || \
     (ITC_ID_IS_PARENT_ID((pt_Id)->pt_Right) && (pt_Id)->pt_Right->b_IsOwner)||\
     (ITC_ID_IS_LEAF_ID((pt_Id)->pt_Left) &&                                   \
      ITC_ID_IS_LEAF_ID((pt_Id)->pt_Right) &&                                  \
      (pt_Id)->pt_Left->b_IsOwner == (pt_Id)->pt_Right->b_IsOwner))))

#endif /* ITC_ID_PRIVATE_H_ */
