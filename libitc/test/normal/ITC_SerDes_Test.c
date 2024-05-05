/**
 * @file ITC_SerDes_Test.h
 * @brief Unit tests for the Interval Tree Clock's serialisation and
 * deserialisation mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#include "ITC_SerDes.h"
#include "ITC_SerDes_package.h"
#include "ITC_SerDes_Test.h"

#include "ITC_SerDes_Test_package.h"
#include "ITC_Test_package.h"
#include "ITC_TestUtil.h"

#include "ITC_Id_package.h"
#include "ITC_Event_package.h"

#include "ITC_Stamp.h"

#include <stdint.h>

/******************************************************************************
 *  Private functions
 ******************************************************************************/

/******************************************************************************
 *  Global variables
 ******************************************************************************/

/******************************************************************************
 *  Public functions
 ******************************************************************************/

/* Init test */
void setUp(void) {}

/* Fini test */
void tearDown(void) {}

/* Test serialising a Id fails with invalid param */
void ITC_SerDes_Test_serialiseIdFailInvalidParam(void)
{
    ITC_Id_t *pt_Dummy = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    TEST_FAILURE(
        ITC_SerDes_serialiseId(
            pt_Dummy,
            &ru8_Buffer[0],
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_serialiseId(
            NULL,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_serialiseId(
            pt_Dummy,
            NULL,
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);
}

/* Test serialising an ID fails with corrupt ID */
void ITC_SerDes_Test_serialiseIdFailWithCorruptId(void)
{
    ITC_Id_t *pt_Id;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_SerDes_serialiseId(
                pt_Id,
                &ru8_Buffer[0],
                &u32_BufferSize),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Id);
    }
}

/* Test serialising a leaf ID succeeds */
void ITC_SerDes_Test_serialiseIdLeafSuccessful(void)
{
    ITC_Id_t *pt_Id = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    uint8_t ru8_ExpectedSeedIdSerialisedData[] = {
        ITC_SERDES_SEED_ID_HEADER
    };
    uint8_t ru8_ExpectedNullIdSerialisedData[] = {
        ITC_SERDES_NULL_ID_HEADER
    };

    /* Create a new seed ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id, NULL));

    /* Serialise the ID */
    TEST_SUCCESS(
        ITC_SerDes_serialiseId(pt_Id, &ru8_Buffer[0], &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedSeedIdSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedSeedIdSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedSeedIdSerialisedData));

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* Create a new null ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));

    /* Reset the buffer size */
    u32_BufferSize = sizeof(ru8_Buffer);

    /* Serialise the ID */
    TEST_SUCCESS(
        ITC_SerDes_serialiseId(pt_Id, &ru8_Buffer[0], &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedNullIdSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedNullIdSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedNullIdSerialisedData));

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test serialising a leaf ID fails with insufficent resources */
void ITC_SerDes_Test_serialiseIdFailWithInsufficentResources(void)
{
    ITC_Id_t *pt_Id = NULL;
    uint8_t ru8_Buffer[2] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Create a new ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));

    /* Serialise the ID */
    TEST_FAILURE(
        ITC_SerDes_serialiseId(
            pt_Id,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INSUFFICIENT_RESOURCES);

    u32_BufferSize = 0;

    /* Serialise the ID */
    TEST_FAILURE(
        ITC_SerDes_serialiseId(
            pt_Id->pt_Left,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test serialising a leaf ID subtree succeeds */
void ITC_SerDes_Test_serialiseIdLeafSubtreeSuccessful(void)
{
    ITC_Id_t *pt_Id = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    uint8_t ru8_ExpectedSeedIdSerialisedData[] = {
        ITC_SERDES_SEED_ID_HEADER
    };
    uint8_t ru8_ExpectedNullIdSerialisedData[] = {
        ITC_SERDES_NULL_ID_HEADER
    };

    /* Create a new ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));

    /* Serialise the ID */
    TEST_SUCCESS(
        ITC_SerDes_serialiseId(
            pt_Id->pt_Left,
            &ru8_Buffer[0],
            &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedSeedIdSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedSeedIdSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedSeedIdSerialisedData));

    /* Reset the buffer size */
    u32_BufferSize = sizeof(ru8_Buffer);

    /* Serialise the ID */
    TEST_SUCCESS(
        ITC_SerDes_serialiseId(
            pt_Id->pt_Right,
            &ru8_Buffer[0],
            &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedNullIdSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedNullIdSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedNullIdSerialisedData));

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test serialising a parent ID succeeds */
void ITC_SerDes_Test_serialiseIdParentSuccessful(void)
{
    ITC_Id_t *pt_Id = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Serialised (0, ((1, 0), 1)) ID */
    uint8_t ru8_ExpectedIdSerialisedData[] = {
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
    };

    /* clang-format off */
    /* Create a new (0, ((1, 0), 1)) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right->pt_Left, pt_Id->pt_Right));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Right->pt_Left->pt_Left, pt_Id->pt_Right->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right->pt_Left->pt_Right, pt_Id->pt_Right->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Right->pt_Right, pt_Id->pt_Right));
    /* clang-format on */

    /* Serialise the ID */
    TEST_SUCCESS(
        ITC_SerDes_serialiseId(
            pt_Id,
            &ru8_Buffer[0],
            &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedIdSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedIdSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedIdSerialisedData));

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test deserialising a ID fails with invalid param */
void ITC_SerDes_Test_deserialiseIdFailInvalidParam(void)
{
    ITC_Id_t *pt_Dummy = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    TEST_FAILURE(
        ITC_SerDes_deserialiseId(
            &ru8_Buffer[0],
            0,
            &pt_Dummy),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_deserialiseId(
            NULL,
            u32_BufferSize,
            &pt_Dummy),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_deserialiseId(
            &ru8_Buffer[0],
            u32_BufferSize,
            NULL),
        ITC_STATUS_INVALID_PARAM);
}

/* Test deserialising an ID fails with corrupt ID */
void ITC_SerDes_Test_deserialiseIdFailWithCorruptId(void)
{
    ITC_Id_t *pt_Id;
    const uint8_t *pu8_Buffer = NULL;
    uint32_t u32_BufferSize = 0;

    /* Test different invalid serialised IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidSerialisedIdTableSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidSerialisedIdConstructorTable[u32_I](
            &pu8_Buffer, &u32_BufferSize);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_SerDes_deserialiseId(
                pu8_Buffer,
                u32_BufferSize,
                &pt_Id),
            ITC_STATUS_CORRUPT_ID);
    }
}

/* Test deserialising a leaf ID suceeds */
void ITC_SerDes_Test_deserialiseIdSuccessful(void)
{
    ITC_Id_t *pt_Id;
    uint8_t ru8_Buffer[] = { ITC_SERDES_SEED_ID_HEADER };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Test deserialising a seed ID */
    TEST_SUCCESS(
        ITC_SerDes_deserialiseId(&ru8_Buffer[0], u32_BufferSize, &pt_Id));

    /* Test this is a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* Test with a null ID */
    ru8_Buffer[0] = ITC_SERDES_NULL_ID_HEADER;

    /* Test deserialising a seed ID */
    TEST_SUCCESS(
        ITC_SerDes_deserialiseId(&ru8_Buffer[0], u32_BufferSize, &pt_Id));

    /* Test this is a seed ID */
    TEST_ITC_ID_IS_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test deserialising a parent ID suceeds */
void ITC_SerDes_Test_deserialiseParentIdSuccessful(void)
{
    ITC_Id_t *pt_Id;
    /* Serialised (0, ((1, 0), 1)) ID */
    uint8_t ru8_Buffer[] = {
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
    };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Test deserialising a seed ID */
    TEST_SUCCESS(
        ITC_SerDes_deserialiseId(&ru8_Buffer[0], u32_BufferSize, &pt_Id));

    /* Test this is a (0, ((1, 0), 1)) ID */
    TEST_ITC_ID_IS_NULL_ID(pt_Id->pt_Left);
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_Id->pt_Right->pt_Left);
    TEST_ITC_ID_IS_SEED_ID(pt_Id->pt_Right->pt_Right);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test serialising an Event fails with invalid param */
void ITC_SerDes_Test_serialiseEventFailInvalidParam(void)
{
    ITC_Event_t *pt_Dummy = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    TEST_FAILURE(
        ITC_SerDes_serialiseEvent(
            pt_Dummy,
            &ru8_Buffer[0],
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_serialiseEvent(
            NULL,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_serialiseEvent(
            pt_Dummy,
            NULL,
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);
}

/* Test serialising an Event fails with corrupt Event */
void ITC_SerDes_Test_serialiseEventFailWithCorruptEvent(void)
{
    ITC_Event_t *pt_Event;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_SerDes_serialiseEvent(
                pt_Event,
                &ru8_Buffer[0],
                &u32_BufferSize),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Event);
    }
}

/* Test serialising a leaf Event succeeds */
void ITC_SerDes_Test_serialiseEventLeafSuccessful(void)
{
    ITC_Event_t *pt_Event = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    uint8_t ru8_ExpectedEventSerialisedData[] = {
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        123
    };

    /* Create a new Event */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event, NULL, 123));

    /* Serialise the Event */
    TEST_SUCCESS(
        ITC_SerDes_serialiseEvent(pt_Event, &ru8_Buffer[0], &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedEventSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedEventSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedEventSerialisedData));

    /* Destroy the Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
}

/* Test serialising a leaf Event fails with insufficent resources */
void ITC_SerDes_Test_serialiseEventFailWithInsufficentResources(void)
{
    ITC_Event_t *pt_Event = NULL;
    uint8_t ru8_Buffer[5] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Create a new Event */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Left, pt_Event, 1));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Right, pt_Event, 0));

    /* Serialise the Event */
    TEST_FAILURE(
        ITC_SerDes_serialiseEvent(
            pt_Event,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INSUFFICIENT_RESOURCES);

    u32_BufferSize = 0;

    /* Serialise the Event */
    TEST_FAILURE(
        ITC_SerDes_serialiseEvent(
            pt_Event->pt_Left,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);

    /* Destroy the Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
}

/* Test serialising a leaf Event subtree succeeds */
void ITC_SerDes_Test_serialiseEventLeafSubtreeSuccessful(void)
{
    ITC_Event_t *pt_Event = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    uint8_t ru8_ExpectedEventSerialisedData[] = {
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        0
    };

    /* Create a new Event */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Left, pt_Event, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Right, pt_Event, 1));

    /* Serialise the Event */
    TEST_SUCCESS(
        ITC_SerDes_serialiseEvent(
            pt_Event->pt_Left,
            &ru8_Buffer[0],
            &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedEventSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedEventSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedEventSerialisedData));

    /* Destroy the Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
}

/* Test serialising a parent Event succeeds */
void ITC_SerDes_Test_serialiseEventParentSuccessful(void)
{
    ITC_Event_t *pt_Event = NULL;
    uint8_t ru8_Buffer[18] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Serialised (0, 1, (0, (4242, 0, 123123123), 0)) Event */
    uint8_t ru8_ExpectedEventSerialisedData[] = {
        ITC_SERDES_CREATE_EVENT_HEADER(true, 1),
        0,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(true, 1),
        0,
        ITC_SERDES_CREATE_EVENT_HEADER(true, 2),
        (4242U >> 8U) & 0xFFU,
        4242U & 0xFFU,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        0,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 4),
        (123123123U >> 24U) & 0xFFU,
        (123123123U >> 16U) & 0xFFU,
        (123123123U >> 8U) & 0xFFU,
        123123123U & 0xFFU,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        0,
    };

    /* clang-format off */
    /* Create a new (0, 1, (0, (4242, 0, 123123123), 0)) Event */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Left, pt_Event, 1));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Right, pt_Event, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Right->pt_Left, pt_Event->pt_Right, 4242));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Right->pt_Left->pt_Left, pt_Event->pt_Right->pt_Left, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Right->pt_Left->pt_Right, pt_Event->pt_Right->pt_Left, 123123123));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Right->pt_Right, pt_Event->pt_Right, 0));
    /* clang-format on */

    /* Serialise the Event */
    TEST_SUCCESS(
        ITC_SerDes_serialiseEvent(
            pt_Event,
            &ru8_Buffer[0],
            &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedEventSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedEventSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedEventSerialisedData));

    /* Destroy the Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
}

/* Test serialising a Stamp fails with invalid param */
void ITC_SerDes_Test_serialiseStampFailInvalidParam(void)
{
    ITC_Stamp_t *pt_Dummy = NULL;
    uint8_t ru8_Buffer[10];
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    TEST_FAILURE(
        ITC_SerDes_serialiseStamp(
            pt_Dummy,
            &ru8_Buffer[0],
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_serialiseStamp(
            NULL,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_serialiseStamp(
            pt_Dummy,
            NULL,
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);
}

/* Test deserialising a Stamp fails with invalid param */
void ITC_SerDes_Test_deserialiseStampFailInvalidParam(void)
{
    ITC_Stamp_t *pt_Dummy = NULL;
    uint8_t ru8_Buffer[10] = { 0 };

    TEST_FAILURE(
        ITC_SerDes_deserialiseStamp(
            &ru8_Buffer[0],
            sizeof(ru8_Buffer),
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_deserialiseStamp(
            NULL,
            sizeof(ru8_Buffer),
            &pt_Dummy),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_deserialiseStamp(
            &ru8_Buffer[0],
            0,
            &pt_Dummy),
        ITC_STATUS_INVALID_PARAM);
}
