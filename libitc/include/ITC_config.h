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

#ifndef ITC_CONFIG_USE_64BIT_EVENT_COUNTERS
/** Whether to use 32 or 64 bit event counters
 * - 32 bit counters allow for `2^32 - 1` events to be witnessed by each node
 *   in the event tree. If an event happens every second, this counter will
 *   saturate in about 136 years.
 * - 64 bit counters allow for `2^64 - 1` events to be witnessed. If an event
 *   happens every nanosecond, this counter will saturate in about 584.5 years.
*/
#define ITC_CONFIG_USE_64BIT_EVENT_COUNTERS                                  (0)
#endif

#ifndef ITC_CONFIG_ENABLE_EXTENDED_API
/** Enabling this setting gives access to a few additional operations,
 * which are not part of the original ITC specification. These are:
 * - For IDs - makes the following functions part of the public API:
 *   * `ITC_Id_newSeed`
 *   * `ITC_Id_newNull`
 *   * `ITC_Id_destroy`
 *   * `ITC_Id_clone`,
 *   * `ITC_Id_validate`
 *   * `ITC_Id_split`
 *   * `ITC_Id_sum`
 * - For Events - makes the following functions part of the public API:
 *   * `ITC_Event_new`
 *   * `ITC_Event_destroy`
 *   * `ITC_Event_clone`
 *   * `ITC_Event_validate`
 * - For Stamps: Adds 2 additional operations:
 *   * `ITC_Stamp_explode`
 *   * `ITC_Stamp_rebuild`
*/
#define ITC_CONFIG_ENABLE_EXTENDED_API                                      (1)
#endif

#endif /* ITC_CONFIG_H_ */
