/**
 * @file ITC_TestUtil.h
 * @brief Testing utilities
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#include "unity.h"

#include "ITC_Test_package.h"
#include "ITC_SerDes_Test_package.h"

#include "ITC_Id_package.h"
#include "ITC_Event_package.h"
#include "ITC_TestUtil.h"

/******************************************************************************
 *  Private functions
 ******************************************************************************/

/**
 * @brief Create a new invalid ID with root parent owner
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidIdWithRootParentOwner(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newSeedId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Left, *ppt_Id));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&(*ppt_Id)->pt_Right, *ppt_Id));
}

/**
 * @brief Create a new invalid ID with nested parent owner
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidIdWithNestedParentOwner(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Left, *ppt_Id));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&(*ppt_Id)->pt_Right, *ppt_Id));
    TEST_SUCCESS(
        ITC_TestUtil_newNullId(
            &(*ppt_Id)->pt_Right->pt_Left,
            (*ppt_Id)->pt_Right));
    TEST_SUCCESS(
        ITC_TestUtil_newSeedId(
            &(*ppt_Id)->pt_Right->pt_Right,
            (*ppt_Id)->pt_Right));
}

/**
 * @brief Create a new invalid ID with asymmetric root parent with only left child
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidIdWithAsymmetricRootParentLeft(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&(*ppt_Id)->pt_Left, *ppt_Id));
}

/**
 * @brief Create a new invalid ID with asymmetric root parent with only right child
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidIdWithAsymmetricRootParentRight(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Right, *ppt_Id));
}

/**
 * @brief Create a new invalid ID with asymmetric nested parent with only left child
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidIdWithAsymmetricNestedParentLeft(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Left, *ppt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Right, *ppt_Id));
    TEST_SUCCESS(
        ITC_TestUtil_newSeedId(
            &(*ppt_Id)->pt_Right->pt_Left,
            (*ppt_Id)->pt_Right));
}

/**
 * @brief Create a new invalid ID with asymmetric nested parent with only right child
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidIdWithAsymmetricNestedParentRight(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Left, *ppt_Id));
    TEST_SUCCESS(
        ITC_TestUtil_newNullId(
            &(*ppt_Id)->pt_Left->pt_Right,
            (*ppt_Id)->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Right, *ppt_Id));
}

/**
 * @brief Create a new invalid not normalised ID
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidNotNormalisedId(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Left, *ppt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Right, *ppt_Id));
}

/**
 * @brief Create a new invalid not normalised nested ID
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidNotNormalisedNestedId(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Left, *ppt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Right, *ppt_Id));
    TEST_SUCCESS(
        ITC_TestUtil_newSeedId(
            &(*ppt_Id)->pt_Right->pt_Left,
            (*ppt_Id)->pt_Right));
    TEST_SUCCESS(
        ITC_TestUtil_newSeedId(
            &(*ppt_Id)->pt_Right->pt_Right,
            (*ppt_Id)->pt_Right));
}

/**
 * @brief Create a new invalid ID with NULL parent pointer
 *
 * Use `destroyInvalidIdWithNullParentPointer` to deallocate the ID
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidIdWithNullParentPointer(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Left, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&(*ppt_Id)->pt_Right, *ppt_Id));
}

/**
 * @brief Deallocate an invalid ID created with
 * `newInvalidIdWithNullParentPointer`
 *
 *
 * @param pt_Id (in) The pointer to the root if the ID.
 */
static void destroyInvalidIdWithNullParentPointer(
    ITC_Id_t **ppt_Id
)
{
    /* Fix the damage so the ID can be properly deallocated */
    (*ppt_Id)->pt_Left->pt_Parent = *ppt_Id;
    TEST_SUCCESS(ITC_Id_destroy(ppt_Id));
}

/**
 * @brief Create a new invalid ID with invalid parent pointer
 *
 * Use `destroyInvalidIdWithInvalidParentPointer` to deallocate the ID
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidIdWithInvalidParentPointer(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Left, *ppt_Id));
    TEST_SUCCESS(
        ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Right, (*ppt_Id)->pt_Left));
}

/**
 * @brief Deallocate an invalid ID created with
 * `newInvalidIdWithInvalidParentPointer`
 *
 * @param pt_Id (in) The pointer to the root if the ID.
 */
static void destroyInvalidIdWithInvalidParentPointer(
    ITC_Id_t **ppt_Id
)
{
    /* Fix the damage so the ID can be properly deallocated */
    (*ppt_Id)->pt_Right->pt_Parent = *ppt_Id;
    TEST_SUCCESS(ITC_Id_destroy(ppt_Id));
}

/**
 * @brief Create a new invalid serialised ID with a parent node without children
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedParentIdWithNoChildren(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_PARENT_ID_HEADER
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised ID with a nested parent node without
 * children
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedNestedParentIdWithNoChildren(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised ID with a asymmetric parent node
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedAsymmetricParentId(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised ID with a asymmetric nested parent
 * node
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedAsymmetricNestedParentId(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised ID with too many child nodes
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedIdWithTooManyChildNodes(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_NULL_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised parent ID with too many nested child
 * nodes
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedParentIdWithTooManyNestedChildNodes(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised ID with multiparent root
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedParentIdWithMultiParentRoot(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised ID with invalid header
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedParentIdWithInvalidHeader(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        123,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised ID with invalid nested header
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedParentIdWithInvalidNestedHeader(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
        123,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised not normalised ID
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedNotNormalisedId(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid Event with asymmetric root parent
 * with only left child
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidEventWithAsymmetricRootParentLeft(
    ITC_Event_t **ppt_Event
)
{
    TEST_SUCCESS(ITC_TestUtil_newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Left, *ppt_Event, 1));
}

/**
 * @brief Create a new invalid Event with asymmetric root parent
 * with only right child
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidEventWithAsymmetricRootParentRight(
    ITC_Event_t **ppt_Event
)
{
    TEST_SUCCESS(ITC_TestUtil_newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Right, *ppt_Event, 1));
}

/**
 * @brief Create a new invalid Event with asymmetric nested parent
 * with only left child
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidEventWithAsymmetricNestedParentLeft(
    ITC_Event_t **ppt_Event
)
{
    TEST_SUCCESS(ITC_TestUtil_newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Left, *ppt_Event, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Right, *ppt_Event, 2));
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &(*ppt_Event)->pt_Right->pt_Left,
            (*ppt_Event)->pt_Right,
            3));
}

/**
 * @brief Create a new invalid Event with asymmetric nested parent
 * with only right child
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidEventWithAsymmetricNestedParentRight(
    ITC_Event_t **ppt_Event
)
{
    TEST_SUCCESS(ITC_TestUtil_newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Left, *ppt_Event, 0));
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &(*ppt_Event)->pt_Left->pt_Right,
            (*ppt_Event)->pt_Left,
            3));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Right, *ppt_Event, 2));
}

/**
 * @brief Create a new invalid not normalised Event
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidNotNormalisedEvent(
    ITC_Event_t **ppt_Event
)
{
    TEST_SUCCESS(ITC_TestUtil_newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Left, *ppt_Event, 1));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Right, *ppt_Event, 2));
}

/**
 * @brief Create a new invalid not normalised nested Event
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidNotNormalisedNestedEvent(
    ITC_Event_t **ppt_Event
)
{
    TEST_SUCCESS(ITC_TestUtil_newEvent(ppt_Event, NULL, 1));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Left, *ppt_Event, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Right, *ppt_Event, 2));
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &(*ppt_Event)->pt_Right->pt_Left,
            (*ppt_Event)->pt_Right,
            2));
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &(*ppt_Event)->pt_Right->pt_Right,
            (*ppt_Event)->pt_Right,
            2));
}

/**
 * @brief Create a new invalid Event with NULL parent pointer
 *
 * Use `destroyInvalidEventWithNullParentPointer` to deallocate
 * the Event
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidEventWithNullParentPointer(
    ITC_Event_t **ppt_Event
)
{
    TEST_SUCCESS(ITC_TestUtil_newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Left, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Right, *ppt_Event, 2));
}

/**
 * @brief Deallocate an invalid Event created with
 * `newInvalidEventWithNullParentPointer`
 *
 *
 * @param pt_Event (in) The pointer to the root if the Event.
 */
static void destroyInvalidEventWithNullParentPointer(
    ITC_Event_t **ppt_Event
)
{
    /* Fix the damage so the Event can be properly deallocated */
    (*ppt_Event)->pt_Left->pt_Parent = *ppt_Event;
    TEST_SUCCESS(ITC_Event_destroy(ppt_Event));
}

/**
 * @brief Create a new invalid Event with invalid parent pointer
 *
 * Use `destroyInvalidEventWithInvalidParentPointer` to deallocate
 * the Event
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidEventWithInvalidParentPointer(
    ITC_Event_t **ppt_Event
)
{
    TEST_SUCCESS(ITC_TestUtil_newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Left, *ppt_Event, 1));
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &(*ppt_Event)->pt_Right,
            (*ppt_Event)->pt_Left,
            2));
}

/**
 * @brief Deallocate an invalid Event created with
 * `newInvalidEventWithInvalidParentPointer`
 *
 * @param pt_Event (in) The pointer to the root if the Event.
 */
static void destroyInvalidEventWithInvalidParentPointer(
    ITC_Event_t **ppt_Event
)
{
    /* Fix the damage so the Event can be properly deallocated */
    (*ppt_Event)->pt_Right->pt_Parent = *ppt_Event;
    TEST_SUCCESS(ITC_Event_destroy(ppt_Event));
}

/**
 * @brief Create a new invalid serialised Event with a parent node without
 * children
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedParentEventWithNoChildren(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised Event with a nested parent node
 * without children
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedNestedParentEventWithNoChildren(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_EVENT_HEADER(true, 1),
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised Event with a asymmetric parent node
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedAsymmetricParentEvent(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        1,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised Event with a asymmetric nested parent
 * node
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedAsymmetricNestedParentEvent(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_EVENT_HEADER(true, 1),
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(true, 1),
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        1,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised Event with too many child nodes
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedEventWithTooManyChildNodes(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        1,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised parent Event with too many nested
 * child nodes
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedParentEventWithTooManyNestedChildNodes(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        1,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised Event with multiparent root
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedParentEventWithMultiParentRoot(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_EVENT_HEADER(true, 1),
        123,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        1,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised Event with invalid header
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedParentEventWithInvalidHeader(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_EVENT_HEADER_MASK << 1,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised Event with invalid nested header
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedParentEventWithInvalidNestedHeader(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
        ITC_SERDES_EVENT_HEADER_MASK << 2,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised Event with no header
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedEventWithNoHeader(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        1, // Just an event count, no header
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised nested Event with no header
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedNestedEventWithNoHeader(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        2, // Just an event count, no header
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised not normalised Event
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedNotNormalisedEvent(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        2,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised Event with smaller Event counter
 * length
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedEventWithSmallerEventCounterLength(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised Event with higher Event counter
 * length
 *
 * @param ppu8_Buffer (out) The pointer to the buffer
 * @param pu32_BufferSize (out) The size of the buffer
 */
static void newInvalidSerialisedEventWithHigherEventCounterLength(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 2),
        1
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid Stamp with no ID component
 *
 * @param pt_Stamp (out) The pointer to the Stamp
 */
static void newInvalidStampWithNoID(ITC_Stamp_t **ppt_Stamp)
{
    TEST_SUCCESS(ITC_Stamp_newSeed(ppt_Stamp));
    TEST_SUCCESS(ITC_Id_destroy(&(*ppt_Stamp)->pt_Id));
}

/**
 * @brief Create a new invalid Stamp with no Event component
 *
 * @param pt_Stamp (out) The pointer to the Stamp
 */
static void newInvalidStampWithNoEvent(ITC_Stamp_t **ppt_Stamp)
{
    TEST_SUCCESS(ITC_Stamp_newSeed(ppt_Stamp));
    TEST_SUCCESS(ITC_Event_destroy(&(*ppt_Stamp)->pt_Event));
}

/**
 * @brief Create a new invalid serialised Stamp with no ID component
 *
 * @param pt_Stamp (out) The pointer to the Stamp
 */
static void newInvalidSerialisedStampWithNoID(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_STAMP_HEADER(0, 1),
        5,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 4),
        1,
        2,
        3,
        4,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised Stamp with no Event component
 *
 * @param pt_Stamp (out) The pointer to the Stamp
 */
static void newInvalidSerialisedStampWithNoEvent(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_STAMP_HEADER(1, 0),
        5,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised Stamp with no header
 *
 * @param pt_Stamp (out) The pointer to the Stamp
 */
static void newInvalidSerialisedStampWithNoHeader(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        3,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised Stamp with smaller ID component length
 * length
 *
 * @param pt_Stamp (out) The pointer to the Stamp
 */
static void newInvalidSerialisedStampWithSmallerIDComponentLengthlength(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_STAMP_HEADER(1, 1),
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised Stamp with bigger ID component length
 * length
 *
 * @param pt_Stamp (out) The pointer to the Stamp
 */
static void newInvalidSerialisedStampWithBiggerIDComponentLengthlength(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_STAMP_HEADER(1, 1),
        3,
        1,
        0,
        2,
        ITC_SERDES_NULL_ID_HEADER,
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised Stamp with smaller Event component
 * length length
 *
 * @param pt_Stamp (out) The pointer to the Stamp
 */
static void newInvalidSerialisedStampWithSmallerEventComponentLengthlength(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_STAMP_HEADER(1, 1),
        1,
        ITC_SERDES_SEED_ID_HEADER,
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        1,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised Stamp with bigger Event component
 * length length
 *
 * @param pt_Stamp (out) The pointer to the Stamp
 */
static void newInvalidSerialisedStampWithBiggerEventComponentLengthlength(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_STAMP_HEADER(1, 1),
        1,
        ITC_SERDES_SEED_ID_HEADER,
        1,
        0,
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        1,
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/**
 * @brief Create a new invalid serialised Stamp with invalid header
 *
 * @param pt_Stamp (out) The pointer to the Stamp
 */
static void newInvalidSerialisedStampWithInvalidHeader(
    const uint8_t **ppu8_Buffer,
    uint32_t *pu32_BufferSize
)
{
    static const uint8_t ru8_Buffer[] =
    {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_STAMP_HEADER_MASK << 1,
        1,
        ITC_SERDES_SEED_ID_HEADER,
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
    };

    *ppu8_Buffer = &ru8_Buffer[0];
    *pu32_BufferSize = sizeof(ru8_Buffer);
}

/******************************************************************************
 *  Global variables
 ******************************************************************************/

/******************************************************************************
 * Table of constructors for varous types of invalid IDs
 ******************************************************************************/

void (*const gpv_InvalidIdConstructorTable[])(ITC_Id_t **) =
{
    newInvalidIdWithAsymmetricRootParentLeft,
    newInvalidIdWithAsymmetricRootParentRight,
    newInvalidIdWithAsymmetricNestedParentLeft,
    newInvalidIdWithAsymmetricNestedParentRight,
    newInvalidIdWithRootParentOwner,
    newInvalidIdWithNestedParentOwner,
    newInvalidIdWithNullParentPointer,
    newInvalidIdWithInvalidParentPointer,
    /* Normalisation related invalid IDs.
     * If adding more constructors before this point,
     * be sure to update `FIRST_NORMALISATION_RELATED_INVALID_ID_INDEX` */
    newInvalidNotNormalisedId,
    newInvalidNotNormalisedNestedId,
};

/******************************************************************************
 * Table of destructors for varous types of invalid IDs
 ******************************************************************************/

void (*const gpv_InvalidIdDestructorTable[])(ITC_Id_t **) =
{
    /* Cast the funcion pointer to the type of the table
     * This is ugly but beats needlessly having to write a destructor
     * for each invalid ID */
    (void (*)(ITC_Id_t **))ITC_Id_destroy,
    (void (*)(ITC_Id_t **))ITC_Id_destroy,
    (void (*)(ITC_Id_t **))ITC_Id_destroy,
    (void (*)(ITC_Id_t **))ITC_Id_destroy,
    (void (*)(ITC_Id_t **))ITC_Id_destroy,
    (void (*)(ITC_Id_t **))ITC_Id_destroy,
    destroyInvalidIdWithNullParentPointer,
    destroyInvalidIdWithInvalidParentPointer,
    /* Normalisation related invalid IDs.
     * If adding more destructors before this point,
     * be sure to update `FIRST_NORMALISATION_RELATED_INVALID_ID_INDEX` */
    (void (*)(ITC_Id_t **))ITC_Id_destroy,
    (void (*)(ITC_Id_t **))ITC_Id_destroy,
};

/******************************************************************************
 * The size of the `gpv_InvalidIdDestructorTable` and `gpv_InvalidIdConstructorTable` arrays.
 ******************************************************************************/

const uint32_t gu32_InvalidIdTablesSize =
    ARRAY_COUNT(gpv_InvalidIdConstructorTable);

/******************************************************************************
 * Table of constructors for various types of invalid serialised IDs
 ******************************************************************************/

void (*const gpv_InvalidSerialisedIdConstructorTable[])(
    const uint8_t **ppu8_Buffer, uint32_t *pu32_BufferSize) =
{
    newInvalidSerialisedParentIdWithNoChildren,
    newInvalidSerialisedNestedParentIdWithNoChildren,
    newInvalidSerialisedAsymmetricParentId,
    newInvalidSerialisedAsymmetricNestedParentId,
    newInvalidSerialisedIdWithTooManyChildNodes,
    newInvalidSerialisedParentIdWithTooManyNestedChildNodes,
    newInvalidSerialisedParentIdWithMultiParentRoot,
    newInvalidSerialisedParentIdWithInvalidHeader,
    newInvalidSerialisedParentIdWithInvalidNestedHeader,
    newInvalidSerialisedNotNormalisedId,
};

/******************************************************************************
 * The size of the `gpv_InvalidSerialisedIdConstructorTable` array
 ******************************************************************************/

const uint32_t gu32_InvalidSerialisedIdTableSize =
    ARRAY_COUNT(gpv_InvalidSerialisedIdConstructorTable);

/******************************************************************************
 * Table of constructors for varous types of invalid Events
 ******************************************************************************/

void (*const gpv_InvalidEventConstructorTable[])(ITC_Event_t **) =
{
    newInvalidEventWithAsymmetricRootParentLeft,
    newInvalidEventWithAsymmetricRootParentRight,
    newInvalidEventWithAsymmetricNestedParentLeft,
    newInvalidEventWithAsymmetricNestedParentRight,
    newInvalidEventWithNullParentPointer,
    newInvalidEventWithInvalidParentPointer,
    /* Normalisation related invalid Events.
     * If adding more constructors before this point,
     * be sure to update `FIRST_NORMALISATION_RELATED_INVALID_EVENT_INDEX` */
    newInvalidNotNormalisedEvent,
    newInvalidNotNormalisedNestedEvent,
};


/******************************************************************************
 * Table of destructors for varous types of invalid Events
 ******************************************************************************/

void (*const gpv_InvalidEventDestructorTable[])(ITC_Event_t **) =
{
    /* Cast the funcion pointer to the type of the table
     * This is ugly but beats needlessly having to write a destructor
     * for each invalid Event */
    (void (*)(ITC_Event_t **))ITC_Event_destroy,
    (void (*)(ITC_Event_t **))ITC_Event_destroy,
    (void (*)(ITC_Event_t **))ITC_Event_destroy,
    (void (*)(ITC_Event_t **))ITC_Event_destroy,
    destroyInvalidEventWithNullParentPointer,
    destroyInvalidEventWithInvalidParentPointer,
    /* Normalisation related invalid Events.
     * If adding more destructors before this point,
     * be sure to update `FIRST_NORMALISATION_RELATED_INVALID_EVENT_INDEX` */
    (void (*)(ITC_Event_t **))ITC_Event_destroy,
    (void (*)(ITC_Event_t **))ITC_Event_destroy,
};

/******************************************************************************
 * The size of the `gpv_InvalidEventConstructorTable` and `gpv_InvalidEventDestructorTable` arrays.
 ******************************************************************************/

const uint32_t gu32_InvalidEventTablesSize =
    ARRAY_COUNT(gpv_InvalidEventConstructorTable);

/******************************************************************************
 * Table of constructors for various types of invalid serialised Events
 ******************************************************************************/

void (*const gpv_InvalidSerialisedEventConstructorTable[])(
    const uint8_t **ppu8_Buffer, uint32_t *pu32_BufferSize) =
{
    newInvalidSerialisedParentEventWithNoChildren,
    newInvalidSerialisedNestedParentEventWithNoChildren,
    newInvalidSerialisedAsymmetricParentEvent,
    newInvalidSerialisedAsymmetricNestedParentEvent,
    newInvalidSerialisedEventWithTooManyChildNodes,
    newInvalidSerialisedParentEventWithTooManyNestedChildNodes,
    newInvalidSerialisedParentEventWithMultiParentRoot,
    newInvalidSerialisedParentEventWithInvalidHeader,
    newInvalidSerialisedParentEventWithInvalidNestedHeader,
    newInvalidSerialisedEventWithNoHeader,
    newInvalidSerialisedNestedEventWithNoHeader,
    newInvalidSerialisedNotNormalisedEvent,
    newInvalidSerialisedEventWithSmallerEventCounterLength,
    newInvalidSerialisedEventWithHigherEventCounterLength,
};

/******************************************************************************
 * The size of the `gpv_InvalidSerialisedEventConstructorTable` array
 ******************************************************************************/

const uint32_t gu32_InvalidSerialisedEventTableSize =
    ARRAY_COUNT(gpv_InvalidSerialisedEventConstructorTable);

/******************************************************************************
 * Table of constructors for various types of invalid Stamps
 ******************************************************************************/

void (*const gpv_InvalidStampConstructorTable[])(ITC_Stamp_t **) =
{
    newInvalidStampWithNoID,
    newInvalidStampWithNoEvent,
};

/******************************************************************************
 * Table of destructors for various types of invalid Stamps
 ******************************************************************************/

void (*const gpv_InvalidStampDestructorTable[])(ITC_Stamp_t **) =
{
    /* Cast the funcion pointer to the type of the table
     * This is ugly but beats needlessly having to write a destructor
     * for each invalid Stamp */
    (void (*)(ITC_Stamp_t **))ITC_Stamp_destroy,
    (void (*)(ITC_Stamp_t **))ITC_Stamp_destroy,
};

/******************************************************************************
 * The size of the `gpv_InvalidStampConstructorTable` and `gpv_InvalidStampDestructorTable` arrays.
 ******************************************************************************/

const uint32_t gu32_InvalidStampTablesSize =
    ARRAY_COUNT(gpv_InvalidStampConstructorTable);

/******************************************************************************
 * Table of constructors for various types of invalid serialised Stamps
 ******************************************************************************/

void (*const gpv_InvalidSerialisedStampConstructorTable[])(
    const uint8_t **ppu8_Buffer, uint32_t *pu32_BufferSize) =
{
    /* Stamps here are specially crafted to trigger the `CORRUPT_STAMP` exception
     * `CORRUPT_EVENT` and `CORRUPT_ID` exceptions are checked for in their
     * respective unit tests. */
    newInvalidSerialisedStampWithNoID,
    newInvalidSerialisedStampWithNoEvent,
    newInvalidSerialisedStampWithNoHeader,
    newInvalidSerialisedStampWithSmallerIDComponentLengthlength,
    newInvalidSerialisedStampWithBiggerIDComponentLengthlength,
    newInvalidSerialisedStampWithSmallerEventComponentLengthlength,
    newInvalidSerialisedStampWithBiggerEventComponentLengthlength,
    newInvalidSerialisedStampWithInvalidHeader,
};

/******************************************************************************
 * The size of the `gpv_InvalidSerialisedStampConstructorTable` array
 ******************************************************************************/

const uint32_t gu32_InvalidSerialisedStampTableSize =
    ARRAY_COUNT(gpv_InvalidSerialisedStampConstructorTable);

/******************************************************************************
 *  Public functions
 ******************************************************************************/

/******************************************************************************
 * Same as ITC_Id_newNull but enforces setting the parent
 ******************************************************************************/

ITC_Status_t ITC_TestUtil_newNullId(
    ITC_Id_t **ppt_Id,
    ITC_Id_t *pt_Parent
)
{
    ITC_Status_t t_Status;

    t_Status = ITC_Id_newNull(ppt_Id);
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        (*ppt_Id)->pt_Parent = pt_Parent;
    }

    return t_Status;
}

/******************************************************************************
 * Same as ITC_Id_newSeed but enforces setting the parent
 ******************************************************************************/

ITC_Status_t ITC_TestUtil_newSeedId(
    ITC_Id_t **ppt_Id,
    ITC_Id_t *pt_Parent
)
{
    ITC_Status_t t_Status;

    t_Status = ITC_Id_newSeed(ppt_Id);
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        (*ppt_Id)->pt_Parent = pt_Parent;
    }

    return t_Status;
}

/******************************************************************************
 * Same as ITC_Event_new but enforces setting the parent and an
 ******************************************************************************/

ITC_Status_t ITC_TestUtil_newEvent(
    ITC_Event_t **ppt_Event,
    ITC_Event_t *pt_Parent,
    ITC_Event_Counter_t t_Count
)
{
    ITC_Status_t t_Status;

    t_Status = ITC_Event_new(ppt_Event);
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        (*ppt_Event)->pt_Parent = pt_Parent;
        (*ppt_Event)->t_Count = t_Count;
    }

    return t_Status;
}
