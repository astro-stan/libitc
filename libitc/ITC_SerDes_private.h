/**
 * @file ITC_SerDes_private.h
 * @brief Private definitions for the Interval Tree Clock's serialisation and
 * deserialisation mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#ifndef ITC_SERDES_PRIVATE_H_
#define ITC_SERDES_PRIVATE_H_

#include <stdint.h>

/******************************************************************************
 * Defines
 ******************************************************************************/

/* Get a field from a serialised ITC node header */
#define ITC_SERDES_HEADER_GET(t_Header, t_Mask, t_Offset)                      \
    (((t_Header) & (t_Mask)) >> (t_Offset))

/* Set a field in a serialised ITC node header */
#define ITC_SERDES_HEADER_SET(t_Header, t_Field, t_Mask, t_Offset)             \
  (((t_Header) & ~(t_Mask)) |                                                  \
   ((((ITC_SerDes_Header_t)(t_Field)) << (t_Offset)) & (t_Mask)))

/* The header of a serialised leaf null ITC ID */
#define ITC_SERDES_NULL_ID_HEADER                                        (0x00U)
/* The header of a serialised leaf seed ITC ID */
#define ITC_SERDES_SEED_ID_HEADER                                        (0x02U)
/* The header of a serialised parent ITC ID */
#define ITC_SERDES_PARENT_ID_HEADER                                      (0x01U)

/* The offset of the `IS_PARENT` flag in a serialised ITC Event header */
#define ITC_SERDES_EVENT_IS_PARENT_OFFSET                                   (0U)
/* The mask of the `IS_PARENT` flag in a serialised ITC Event header */
#define ITC_SERDES_EVENT_IS_PARENT_MASK                                  (0x01U)

/* The offset of the counter length field in a serialised ITC Event header */
#define ITC_SERDES_EVENT_COUNTER_LEN_OFFSET                                 (1U)
/* The mask of the counter length field in a serialised ITC Event header.
 * This allows for a maximum of a 4-bit counter, which allows encoding the size
 * of up to a 15-byte Event counter (i.e. sizeof(ITC_Event_Counter_t) <= 15) */
#define ITC_SERDES_EVENT_COUNTER_LEN_MASK                                (0x1EU)

/* Get the `IS_PARENT` flag of a serialised Event node */
#define ITC_SERDES_EVENT_GET_IS_PARENT(t_Header)                               \
    ITC_SERDES_HEADER_GET(                                                     \
        t_Header,                                                              \
        ITC_SERDES_EVENT_IS_PARENT_MASK,                                       \
        ITC_SERDES_EVENT_IS_PARENT_OFFSET)                                     \

/* Set the `IS_PARENT` flag of a serialised Event node */
#define ITC_SERDES_EVENT_SET_IS_PARENT(t_Header, b_IsParent)                   \
    ITC_SERDES_HEADER_SET(                                                     \
        t_Header,                                                              \
        b_IsParent,                                                            \
        ITC_SERDES_EVENT_IS_PARENT_MASK,                                       \
        ITC_SERDES_EVENT_IS_PARENT_OFFSET)

/* Get the length of the event counter field of serialised a Event node */
#define ITC_SERDES_EVENT_GET_COUNTER_LEN(t_Header)                             \
    ITC_SERDES_HEADER_GET(                                                     \
        t_Header,                                                              \
        ITC_SERDES_EVENT_COUNTER_LEN_MASK,                                     \
        ITC_SERDES_EVENT_COUNTER_LEN_OFFSET)

/** Set the length of the event counter field of serialised a Event node.
 * @warning The `u8_CounterLen` must be `<= 15` */
#define ITC_SERDES_EVENT_SET_COUNTER_LEN(t_Header, u8_CounterLen)              \
    ITC_SERDES_HEADER_SET(                                                     \
        t_Header,                                                              \
        u8_CounterLen,                                                         \
        ITC_SERDES_EVENT_COUNTER_LEN_MASK,                                     \
        ITC_SERDES_EVENT_COUNTER_LEN_OFFSET)

/** Create the header for a serialised ITC Event
 * @warning The `u8_CounterLen` must be `<= 15` */
#define ITC_SERDES_CREATE_EVENT_HEADER(b_IsParent, u8_CounterLen)              \
  ((ITC_SerDes_Header_t)ITC_SERDES_EVENT_SET_IS_PARENT(                        \
       (ITC_SerDes_Header_t)0U, b_IsParent) |                                  \
   ITC_SERDES_EVENT_SET_COUNTER_LEN(                                           \
        (ITC_SerDes_Header_t)0U, u8_CounterLen))

/******************************************************************************
 * Types
 ******************************************************************************/

/* The header of a single serialised node */
typedef uint8_t ITC_SerDes_Header_t;

#endif /* ITC_SERDES_PRIVATE_H_ */
