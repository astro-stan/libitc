/**
 * @file ITC_Memory.h
 * @brief Memory allocation options for the ITC implementation
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#ifndef ITC_MEMORY_H_
#define ITC_MEMORY_H_

/******************************************************************************
 * Defines
 ******************************************************************************/

/** Use `malloc`/`free` for allocation and deallocation of ITC nodes */
#define ITC_MEMORY_ALLOCATION_TYPE_MALLOC                                    (0)

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
 * @warning Static allocation uses global static arrays, which is inherently
 * **NOT** thread-safe. It is the responsibility of the users of this library
 * to ensure tread safety when working with the library's public API in static
 * node allocation mode.
 *
 * See `ITC_Port.h` for more information.
 */
#define ITC_MEMORY_ALLOCATION_TYPE_STATIC                                    (1)

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
#define ITC_MEMORY_ALLOCATION_TYPE_CUSTOM                                    (2)

#endif /* ITC_MEMORY_H_ */
