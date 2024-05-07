/**
 * @file ITC_TestUtil.h
 * @brief Testing utilities
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#ifndef ITC_TESTUTIL_H_
#define ITC_TESTUTIL_H_

#include "ITC_Id.h"
#include "ITC_Event.h"
#include "ITC_Status.h"

#include <stdint.h>

/******************************************************************************
 *  Defines
 ******************************************************************************/

/** The index of the first normalisation related invalid ID test inside
 * the `gpv_InvalidIdConstructorTable` and `gpv_InvalidIdDestructorTable`
 * tables.
 */
#define FIRST_NORMALISATION_RELATED_INVALID_ID_INDEX                         (8)

/** The index of the first normalisation related invalid Event test inside
 * the `gpv_InvalidEventConstructorTable` and `gpv_InvalidEventDestructorTable`
 * tables.
 */
#define FIRST_NORMALISATION_RELATED_INVALID_EVENT_INDEX                      (6)

/******************************************************************************
 *  Global variables
 ******************************************************************************/

/**
 * @brief Table of constructors for various types of invalid IDs
 *  Each constructor must return an invalid ITC_Id_t**.
 *
 *  It is expected that a a destructor for the invalid ID exists at the
 *  corresponding index in `gpv_InvalidIdDestructorTable`
 */
extern void (*const gpv_InvalidIdConstructorTable[])(ITC_Id_t **);

/**
 * @brief Table of destructors for various types of invalid IDs
 *  Each destructor must fully deallocate the invalid ID.
 *
 *  It is expected that a a constructor for the invalid ID exists at the
 *  corresponding index in `gpv_InvalidIdConstructorTable`
 */
extern void (*const gpv_InvalidIdDestructorTable[])(ITC_Id_t **);

/**
 * @brief The size of the `gpv_InvalidIdDestructorTable` and
 *  `gpv_InvalidIdConstructorTable` arrays.
 */
extern const uint32_t gu32_InvalidIdTablesSize;

/**
 * @brief Table of constructors for various types of invalid serialised IDs
 *  Each constructor must return an invalid serialised ID buffer and it size.
 *
 *  It is expected that the buffers are statically allocated and no desctuctor
 *  is necessary.
 */
extern void (*const gpv_InvalidSerialisedIdConstructorTable[])(
    const uint8_t **ppu8_Buffer, uint32_t *pu32_BufferSize);

/**
 * @brief The size of the `gpv_InvalidSerialisedIdConstructorTable` array.
 */
extern const uint32_t gu32_InvalidSerialisedIdTableSize;

/**
 * @brief Table of constructors for various types of invalid Events
 *  Each constructor must return an invalid ITC_Event_t**.
 *
 *  It is expected that a a destructor for the invalid Event exists at the
 *  corresponding index in `gpv_InvalidEventDestructorTable`
 */
extern void (*const gpv_InvalidEventConstructorTable[])(ITC_Event_t **);

/**
 * @brief Table of destructors for various types of invalid Events
 *  Each destructor must fully deallocate the invalid Event.
 *
 *  It is expected that a a constructor for the invalid Event exists at the
 *  corresponding index in `gpv_InvalidEventConstructorTable`
 */
extern void (*const gpv_InvalidEventDestructorTable[])(ITC_Event_t **);

/**
 * @brief The size of the `gpv_InvalidEventConstructorTable` and
 *  `gpv_InvalidEventDestructorTable` arrays.
 */
extern const uint32_t gu32_InvalidEventTablesSize;

/**
 * @brief Table of constructors for various types of invalid serialised Events
 *  Each constructor must return an invalid serialised Event buffer and it size.
 *
 *  It is expected that the buffers are statically allocated and no desctuctor
 *  is necessary.
 */
extern void (*const gpv_InvalidSerialisedEventConstructorTable[])(
    const uint8_t **ppu8_Buffer, uint32_t *pu32_BufferSize);

/**
 * @brief The size of the `gpv_InvalidSerialisedEventConstructorTable` array.
 */
extern const uint32_t gu32_InvalidSerialisedEventTableSize;

/******************************************************************************
 *  Public functions
 ******************************************************************************/

/**
 * @brief Same as ITC_Id_newNull but enforces setting the parent
 *
 * @param ppt_Id (out) The pointer to the NULL ID
 * @param pt_Parent The pointer to the parent ID. Otherwise NULL
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_TestUtil_newNullId(
    ITC_Id_t **ppt_Id,
    ITC_Id_t *pt_Parent
);

/**
 * @brief Same as ITC_Id_newSeed but enforces setting the parent
 *
 * @param ppt_Id (out) The pointer to the seed ID
 * @param pt_Parent The pointer to the parent ID. Otherwise NULL
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_TestUtil_newSeedId(
    ITC_Id_t **ppt_Id,
    ITC_Id_t *pt_Parent
);

/**
 * @brief Same as ITC_Event_new but enforces setting the parent and an
 * event count
 *
 * @param ppt_Event (out) The pointer to the Event
 * @param pt_Parent The pointer to the parent Event. Otherwise NULL
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_TestUtil_newEvent(
    ITC_Event_t **ppt_Event,
    ITC_Event_t *pt_Parent,
    ITC_Event_Counter_t t_Count
);

#endif /* ITC_TESTUTIL_H_ */
