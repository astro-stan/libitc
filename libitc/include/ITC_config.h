/**
 * @file ITC_config.h
 * @author Stan
 * @brief Configuration options for the ITC implementation
 *
 * @copyright Copyright (c) 2024
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

#endif /* ITC_CONFIG_H_ */