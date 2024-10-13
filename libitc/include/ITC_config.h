/**
 * @file ITC_config.h
 * @brief Configuration options for the ITC implementation
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#ifndef ITC_CONFIG_H_
#define ITC_CONFIG_H_

/******************************************************************************
 * Defines
 ******************************************************************************/

/** Use `malloc`/`free` for allocation and deallocation of ITC nodes */
#define ITC_CONFIG_MEMORY_ALLOCATION_TYPE_MALLOC                                    (0)

/** Use static arrays for allocation and deallocation of ITC nodes.
 * Choosing this option requires the following global variables to be defined:
 * - `gpt_ItcIdNodeAllocationArray`
 * - `gpt_ItcIdNodeAllocationArrayLength`
 * - `gpt_ItcEventNodeAllocationArray`
 * - `gpt_ItcEventNodeAllocationArrayLength`
 * - `gpt_ItcStampNodeAllocationArray`
 * - `gpt_ItcStampNodeAllocationArrayLength`
 *
 * @note This mode also requires `ITC_Port_init` to be called before starting to
 * work with `libitc`'s public API.
 *
 * @warning Static allocation uses global static arrays, which is inherently **NOT**
 * thread-safe. It is the responsibility of the users of this library
 * to ensure tread safety when working with the library's public API in static node
 * allocation mode.
 *
 * See `ITC_Port.h` for more information.
 */
#define ITC_CONFIG_MEMORY_ALLOCATION_TYPE_STATIC                                    (1)

/** Use custom implementation for allocation and deallocation of ITC nodes.
 * Choosing this option requires the implementation of the `ITC_Port_init`,
 * `ITC_Port_fini`, `ITC_Port_malloc` and `ITC_Port_free` functions.
 *
 * @note It is the responsibility of the users of the library to call
 * `ITC_Port_init` (before starting to work with `libitc`'s public API) and
 * `ITC_Port_fini` (after finishing work with `libitc`'s public API). These
 * functions exist for convenience only.
 *
 * See `ITC_Port.h` for more information.
 */
#define ITC_CONFIG_MEMORY_ALLOCATION_TYPE_CUSTOM                                    (2)

#ifndef ITC_CONFIG_USE_64BIT_EVENT_COUNTERS
/** Whether to use 32 or 64 bit event counters
 * - 32 bit counters allow for `2^32 - 1` events to be witnessed by each node
 *   in the event tree. If an event happens every second, this counter will
 *   saturate in about 136 years.
 * - 64 bit counters allow for `2^64 - 1` events to be witnessed. If an event
 *   happens every nanosecond, this counter will saturate in about 584.5 years.
*/
#define ITC_CONFIG_USE_64BIT_EVENT_COUNTERS                                  (0)
#endif /* ITC_CONFIG_USE_64BIT_EVENT_COUNTERS */

#ifndef ITC_CONFIG_ENABLE_EXTENDED_API
/** Enabling this setting gives access to a few additional API calls,
 * which are not part of the original ITC specification. These are:
 * - For IDs - makes the following functions part of the public API:
 *   * `ITC_Id_newSeed`
 *   * `ITC_Id_newNull`
 *   * `ITC_Id_destroy`
 *   * `ITC_Id_clone`
 *   * `ITC_Id_validate`
 *   * `ITC_Id_split`
 *   * `ITC_Id_sum`
 *   * `ITC_SerDes_serialiseId`
 *   * `ITC_SerDes_deserialiseId`
 * - For Events - makes the following functions part of the public API:
 *   * `ITC_Event_new`
 *   * `ITC_Event_destroy`
 *   * `ITC_Event_clone`
 *   * `ITC_Event_validate`
 *   * `ITC_Event_join`
 *   * `ITC_SerDes_serialiseEvent`
 *   * `ITC_SerDes_deserialiseEvent`
 * - For Stamps - adds the following functions to the public API:
 *   * `ITC_Stamp_newFromId`
 *   * `ITC_Stamp_newFromIdAndEvent`
 *   * `ITC_Stamp_newPeekFromEvent`
 *   * `ITC_Stamp_getId`
 *   * `ITC_Stamp_getEvent`
 *   * `ITC_Stamp_setId`
 *   * `ITC_Stamp_setEvent`
 */
#define ITC_CONFIG_ENABLE_EXTENDED_API                                       (0)
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */

#ifndef ITC_CONFIG_ENABLE_SERIALISE_TO_STRING_API
/** Enabling this setting gives access to a few additional API calls,
 * which can be used to get an ASCII encoded representation of an ID, Event
 * or Stamp. When enabled, the following functions become available as part of
 * the public API:
 * - `ITC_SerDes_serialiseStampToString`
 * - `ITC_SerDes_serialiseIdToString` (requires
 *        `ITC_CONFIG_ENABLE_EXTENDED_API` to also be enabled)
 * - `ITC_SerDes_serialiseEventToString` (requires
 *        `ITC_CONFIG_ENABLE_EXTENDED_API` to also be enabled)
 */
#define ITC_CONFIG_ENABLE_SERIALISE_TO_STRING_API                            (0)
#endif /* ITC_CONFIG_ENABLE_SERIALISE_TO_STRING_API */

#ifndef ITC_CONFIG_MEMORY_ALLOCATION_TYPE
/** The memory allocation type to use.
 * Possible values:
 * - ITC_CONFIG_MEMORY_ALLOCATION_TYPE_MALLOC
 * - ITC_CONFIG_MEMORY_ALLOCATION_TYPE_STATIC
 * - ITC_CONFIG_MEMORY_ALLOCATION_TYPE_CUSTOM
 */
#define ITC_CONFIG_MEMORY_ALLOCATION_TYPE                                      \
    (ITC_CONFIG_MEMORY_ALLOCATION_TYPE_MALLOC)
#endif /* ITC_CONFIG_MEMORY_ALLOCATION_TYPE */

#endif /* ITC_CONFIG_H_ */
