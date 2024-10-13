/**
 * @file ITC_Stamp_Test.h
 * @brief Unit tests for the Interval Tree Clock's Stamp mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#include "ITC_Stamp.h"
#include "ITC_Stamp_Test.h"

#include "ITC_Event_package.h"
#include "ITC_Id_package.h"
#include "ITC_Port.h"

#include "ITC_Test_package.h"
#include "ITC_TestUtil.h"
#include "ITC_config.h"

#if ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_MEMORY_ALLOCATION_TYPE_STATIC
#include "ITC_Port.h"

#include <string.h>
#endif /* ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_MEMORY_ALLOCATION_TYPE_STATIC */

/******************************************************************************
 *  Public functions
 ******************************************************************************/

/* Init test */
void setUp(void)
{
#if ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_MEMORY_ALLOCATION_TYPE_STATIC
    static bool b_MemoryInit = false;

    if (!b_MemoryInit)
    {
        TEST_SUCCESS(ITC_Port_init());
        b_MemoryInit = true;
    }
#endif /* ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_MEMORY_ALLOCATION_TYPE_STATIC */
}

/* Fini test */
void tearDown(void)
{
#if ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_MEMORY_ALLOCATION_TYPE_STATIC
/* Test all memory is freed at the end of each test */

    TEST_ASSERT_EQUAL_UINT(
        ITC_PORT_FREE_SLOT_PATTERN,
        ((uint8_t *)gpt_ItcIdNodeAllocationArray)[0]);
    TEST_ASSERT_EQUAL_UINT(
        0,
        memcmp((const void *)&((uint8_t *)gpt_ItcIdNodeAllocationArray)[0],
               (const void *)&((uint8_t *)gpt_ItcIdNodeAllocationArray)[1],
               (gu32_ItcIdNodeAllocationArrayLength * sizeof(ITC_Id_t)) -
                   1));

    TEST_ASSERT_EQUAL_UINT(
        ITC_PORT_FREE_SLOT_PATTERN,
        ((uint8_t *)gpt_ItcEventNodeAllocationArray)[0]);
    TEST_ASSERT_EQUAL_UINT(
        0,
        memcmp((const void *)&((uint8_t *)gpt_ItcEventNodeAllocationArray)[0],
               (const void *)&((uint8_t *)gpt_ItcEventNodeAllocationArray)[1],
               (gu32_ItcEventNodeAllocationArrayLength * sizeof(ITC_Event_t)) -
                   1));

    TEST_ASSERT_EQUAL_UINT(
        ITC_PORT_FREE_SLOT_PATTERN,
        ((uint8_t *)gpt_ItcStampNodeAllocationArray)[0]);
    TEST_ASSERT_EQUAL_UINT(
        0,
        memcmp((const void *)&((uint8_t *)gpt_ItcStampNodeAllocationArray)[0],
               (const void *)&((uint8_t *)gpt_ItcStampNodeAllocationArray)[1],
               (gu32_ItcStampNodeAllocationArrayLength * sizeof(ITC_Stamp_t)) -
                   1));
#endif /* ITC_CONFIG_MEMORY_ALLOCATION_TYPE == ITC_MEMORY_ALLOCATION_TYPE_STATIC */
}

/* Test destroying a Stamp fails with invalid param */
void ITC_Stamp_Test_destroyStampFailInvalidParam(void)
{
    TEST_FAILURE(ITC_Stamp_destroy(NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test destroying a Stamp suceeds */
void ITC_Stamp_Test_destroyStampSuccessful(void)
{
    ITC_Stamp_t *pt_Dummy = NULL;

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Dummy));

    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Dummy));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Dummy));
}

/* Test creating a Stamp fails with invalid param */
void ITC_Stamp_Test_createStampFailInvalidParam(void)
{
    TEST_FAILURE(ITC_Stamp_newSeed(NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test creating a Stamp succeeds */
void ITC_Stamp_Test_createStampSuccessful(void)
{
    ITC_Stamp_t *pt_Stamp;

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Test this is a Stamp with Seed ID node with leaf Event with 0 events */
    TEST_ITC_ID_IS_SEED_ID(pt_Stamp->pt_Id);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 0);

    /* Destroy the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}

/* Test creating a peek Stamp fails with invalid param */
void ITC_Stamp_Test_createPeekStampFailInvalidParam(void)
{
    ITC_Stamp_t *pt_DummyStamp = NULL;

    TEST_FAILURE(
        ITC_Stamp_newPeek(NULL, &pt_DummyStamp), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_newPeek(pt_DummyStamp, NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test creating a peek Stamp fails with corrupt stamp */
void ITC_Stamp_Test_createPeekStampFailWithCorruptStamp(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_PeekStamp;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_ASSERT_NOT_EQUAL(
            ITC_Stamp_newPeek(pt_Stamp, &pt_PeekStamp),
            /* Depending on the failure, different exceptions might be
             * returned */
            ITC_STATUS_SUCCESS);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
}

/* Test creating a peek Stamp fails with corrupt Id or Event */
void ITC_Stamp_Test_createPeekStampFailWithCorruptIdOrEvent(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_PeekStamp;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_newPeek(pt_Stamp, &pt_PeekStamp),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_newPeek(pt_Stamp, &pt_PeekStamp),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}

/* Test creating a peek Stamp succeeds */
void ITC_Stamp_Test_createPeekStampSuccessful(void)
{
    ITC_Stamp_t *pt_OriginalStamp;
    ITC_Stamp_t *pt_PeekStamp;

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_OriginalStamp));

    /* Create the peek Stamp */
    TEST_SUCCESS(ITC_Stamp_newPeek(pt_OriginalStamp, &pt_PeekStamp));

    /* Destroy the original stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_OriginalStamp));

    /* Test this is a Stamp with NULL ID node with leaf Event with 0 events */
    TEST_ITC_ID_IS_NULL_ID(pt_PeekStamp->pt_Id);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_PeekStamp->pt_Event, 0);

    /* Destroy the peek Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_PeekStamp));
}

/* Test cloning an Stamp fails with invalid param */
void ITC_Stamp_Test_cloneStampFailInvalidParam(void)
{
  ITC_Stamp_t *pt_DummyStamp = NULL;

  TEST_FAILURE(ITC_Stamp_clone(NULL, &pt_DummyStamp), ITC_STATUS_INVALID_PARAM);
  TEST_FAILURE(ITC_Stamp_clone(pt_DummyStamp, NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test cloning an Stamp fails with corrupt stamp */
void ITC_Stamp_Test_cloneStampFailWithCorruptStamp(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_ClonedStamp;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_ASSERT_NOT_EQUAL(
            ITC_Stamp_clone(pt_Stamp, &pt_ClonedStamp),
            /* Depending on the failure, different exceptions might be
             * returned */
            ITC_STATUS_SUCCESS);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
}

/* Test cloning a Stamp fails with corrupt Id or Event */
void ITC_Stamp_Test_cloneStampFailWithCorruptIdOrEvent(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_ClonedStamp;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_clone(pt_Stamp, &pt_ClonedStamp),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_clone(pt_Stamp, &pt_ClonedStamp),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}

/* Test cloning a Stamp succeeds */
void ITC_Stamp_Test_cloneStampSuccessful(void)
{
    ITC_Stamp_t *pt_OriginalStamp = NULL;
    ITC_Stamp_t *pt_ClonedStamp = NULL;

    /* Test cloning an Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_OriginalStamp));
    TEST_SUCCESS(ITC_Stamp_clone(pt_OriginalStamp, &pt_ClonedStamp));
    TEST_ASSERT_TRUE(pt_OriginalStamp != pt_ClonedStamp);
    TEST_ASSERT_TRUE(pt_OriginalStamp->pt_Id != pt_ClonedStamp->pt_Id);
    TEST_ASSERT_TRUE(pt_OriginalStamp->pt_Event != pt_ClonedStamp->pt_Event);
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_OriginalStamp));

    /* Test the cloned Stamp has a Seed ID node with leaf Event with 0 events */
    TEST_ITC_ID_IS_SEED_ID(pt_ClonedStamp->pt_Id);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_ClonedStamp->pt_Event, 0);
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_ClonedStamp));
}

/* Test validating a Stamp fails with invalid param */
void ITC_Stamp_Test_validateStampFailInvalidParam(void)
{
    TEST_FAILURE(ITC_Stamp_validate(NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test validating a Stamp fails with corrupt Stamp */
void ITC_Stamp_Test_validatingStampFailWithCorruptStamp(void)
{
    ITC_Stamp_t *pt_Stamp;

    /* Test different invalid Stamps are handled properly.
     * Only test invalid Stamps that are not related to normalisation */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_ASSERT_NOT_EQUAL(
            ITC_Stamp_validate(pt_Stamp),
            /* Depending on the failure, different exceptions might be
             * returned */
            ITC_STATUS_SUCCESS);

        /* Destroy the ID */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
}

/* Test validating a Stamp fails with corrupt Id or Event */
void ITC_Stamp_Test_validateStampFailWithCorruptIdOrEvent(void)
{
    ITC_Stamp_t *pt_Stamp;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(ITC_Stamp_validate(pt_Stamp), ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(ITC_Stamp_validate(pt_Stamp), ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}

/* Test validating a Stamp succeeds */
void ITC_Stamp_Test_validateStampSuccessful(void)
{
    ITC_Stamp_t *pt_Stamp;

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));
    /* Validate the Stamp */
    TEST_SUCCESS(ITC_Stamp_validate(pt_Stamp));
    /* Destroy the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}

/* Test forking a Stamp fails with invalid param */
void ITC_Stamp_Test_forkStampFailInvalidParam(void)
{
    ITC_Stamp_t *pt_DummyStamp = NULL;

    TEST_FAILURE(
        ITC_Stamp_fork(&pt_DummyStamp, NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_fork(NULL, &pt_DummyStamp), ITC_STATUS_INVALID_PARAM);
}

/* Test forking a Stamp fails with corrupt stamp */
void ITC_Stamp_Test_forkStampFailWithCorruptStamp(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_OtherStamp;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_ASSERT_NOT_EQUAL(
            ITC_Stamp_fork(&pt_Stamp, &pt_OtherStamp),
            /* Depending on the failure, different exceptions might be
             * returned */
            ITC_STATUS_SUCCESS);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
}

/* Test forking a Stamp fails with corrupt Id or Event */
void ITC_Stamp_Test_forkStampFailWithCorruptIdOrEvent(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_OtherStamp;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_fork(&pt_Stamp, &pt_OtherStamp),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_fork(&pt_Stamp, &pt_OtherStamp),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}

/* Test forking a Stamp succeeds */
void ITC_Stamp_Test_forkStampSuccessful(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_OtherStamp;

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Fork the Stamp */
    TEST_SUCCESS(ITC_Stamp_fork(&pt_Stamp, &pt_OtherStamp));

    /* Test the ID was cloned and split and the Event history was cloned */
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_Stamp->pt_Id);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_OtherStamp->pt_Id);
    TEST_ASSERT_TRUE(pt_Stamp->pt_Event != pt_OtherStamp->pt_Event);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 0);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_OtherStamp->pt_Event, 0);

    /* Destroy the forked Stamps */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_OtherStamp));
}

/* Test joining two Stamps fails with invalid param */
void ITC_Stamp_Test_joinStampsFailInvalidParam(void)
{
    ITC_Stamp_t *pt_DummyStamp = NULL;

    TEST_FAILURE(
        ITC_Stamp_join(&pt_DummyStamp, NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_join(NULL, &pt_DummyStamp), ITC_STATUS_INVALID_PARAM);
}

/* Test joining two Stamps fails with corrupt stamp */
void ITC_Stamp_Test_joinStampsFailWithCorruptStamp(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_OtherStamp;

    /* Construct the other Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_OtherStamp));

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_ASSERT_NOT_EQUAL(
            ITC_Stamp_join(&pt_Stamp, &pt_OtherStamp),
            /* Depending on the failure, different exceptions might be
             * returned */
            ITC_STATUS_SUCCESS);
        /* And the other way around */
        TEST_ASSERT_NOT_EQUAL(
            ITC_Stamp_join(&pt_OtherStamp, &pt_Stamp),
            /* Depending on the failure, different exceptions might be
             * returned */
            ITC_STATUS_SUCCESS);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }

    /* Destroy the other Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_OtherStamp));
}

/* Test joining a Stamp fails with corrupt Id or Event */
void ITC_Stamp_Test_joinStampFailWithCorruptIdOrEvent(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_OtherStamp;

    /* Create new Stamps */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_OtherStamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_join(&pt_Stamp, &pt_OtherStamp),
            ITC_STATUS_CORRUPT_ID);
        /* Test the other way around */
        TEST_FAILURE(
            ITC_Stamp_join(&pt_OtherStamp, &pt_Stamp),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_join(&pt_Stamp, &pt_OtherStamp),
            ITC_STATUS_CORRUPT_EVENT);
        /* Test the other way around */
        TEST_FAILURE(
            ITC_Stamp_join(&pt_OtherStamp, &pt_Stamp),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamps */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_OtherStamp));
}

/* Test joining two Stamps succeeds */
void ITC_Stamp_Test_joinStampsSuccessful(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_OtherStamp;

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Fork the Stamp */
    TEST_SUCCESS(ITC_Stamp_fork(&pt_Stamp, &pt_OtherStamp));

    /* Join the Stamps */
    TEST_SUCCESS(ITC_Stamp_join(&pt_OtherStamp, &pt_Stamp));

    /* Test the ID is a seed ID and the Event history is a leaf with 0 events */
    TEST_ITC_ID_IS_SEED_ID(pt_OtherStamp->pt_Id);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_OtherStamp->pt_Event, 0);

    /* Destroy the original Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_OtherStamp));
}

/* Test inflating the Event of as Stamp fails with invalid param */
void ITC_Stamp_Test_eventStampFailInvalidParam(void)
{
    TEST_FAILURE(ITC_Stamp_event(NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test inflating the Event of a Stamp fails with corrupt stamp */
void ITC_Stamp_Test_eventStampFailWithCorruptStamp(void)
{
    ITC_Stamp_t *pt_Stamp;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_ASSERT_NOT_EQUAL(
            ITC_Stamp_event(pt_Stamp),
            /* Depending on the failure, different exceptions might be
             * returned */
            ITC_STATUS_SUCCESS);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
}

/* Test inflating the Event of a Stamp fails with corrupt Id or Event */
void ITC_Stamp_Test_eventStampFailWithCorruptIdOrEvent(void)
{
    ITC_Stamp_t *pt_Stamp;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_event(pt_Stamp),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_event(pt_Stamp),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}

/* Test inflating the Event of a Stamp succeeds */
void ITC_Stamp_Test_eventStampSuccessful(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_PeekStamp;
    ITC_Stamp_t *pt_OriginalStamp;
    ITC_Stamp_Comparison_t t_Result;

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Retain a copy for comparison */
    TEST_SUCCESS(ITC_Stamp_clone(pt_Stamp, &pt_OriginalStamp));

    /* Inflate the Stamp Event tree by growing it */
    TEST_SUCCESS(ITC_Stamp_event(pt_Stamp));

    /* Test the Event counter has grown */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 1);
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp, pt_OriginalStamp, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);

    /* Create a new peek Stamp */
    TEST_SUCCESS(ITC_Stamp_newPeek(pt_Stamp, &pt_PeekStamp));

    /* Attempt to inflate the peek Stamp */
    TEST_SUCCESS(ITC_Stamp_event(pt_PeekStamp));

    /* Test the Event counter has not changed */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_PeekStamp->pt_Event, 1);
    TEST_SUCCESS(ITC_Stamp_compare(pt_PeekStamp, pt_Stamp, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_EQUAL, t_Result);

    /* Destroy the Stamps */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_PeekStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_OriginalStamp));

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Retain a copy for comparison */
    TEST_SUCCESS(ITC_Stamp_clone(pt_Stamp, &pt_OriginalStamp));

    /* Add children to the Event tree */
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &pt_Stamp->pt_Event->pt_Left, pt_Stamp->pt_Event, 0));
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &pt_Stamp->pt_Event->pt_Right, pt_Stamp->pt_Event, 3));

    /* Inflate the Stamp Event tree this time by filling it */
    TEST_SUCCESS(ITC_Stamp_event(pt_Stamp));

    /* Test the Event counter has been filled */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 3);
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp, pt_OriginalStamp, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);

    /* Destroy the Stamps */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_OriginalStamp));
}

/* Test comparing Stamps fails with invalid param */
void ITC_Stamp_Test_compareStampsFailInvalidParam(void)
{
    ITC_Stamp_t *pt_DummyStamp = NULL;
    ITC_Stamp_Comparison_t t_DummyResult;

    TEST_FAILURE(
        ITC_Stamp_compare(pt_DummyStamp, NULL, NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_compare(NULL, pt_DummyStamp, NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_compare(
            NULL,
            NULL,
            &t_DummyResult),
        ITC_STATUS_INVALID_PARAM);
}

/* Test comparing an Stamp fails with corrupt Stamp */
void ITC_Stamp_Test_compareStampFailWithCorruptStamp(void)
{
    ITC_Stamp_t *pt_Stamp1;
    ITC_Stamp_t *pt_Stamp2;
    ITC_Stamp_Comparison_t t_Result;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp1);

        /* Construct the other Stamp */
        TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp2));

        /* Test for the failure */
        TEST_ASSERT_NOT_EQUAL(
            ITC_Stamp_compare(pt_Stamp1, pt_Stamp2, &t_Result),
            /* Depending on the failure, different exceptions might be
             * returned */
            ITC_STATUS_SUCCESS);
        /* And the other way around */
        TEST_ASSERT_NOT_EQUAL(
            ITC_Stamp_compare(pt_Stamp2, pt_Stamp1, &t_Result),
            /* Depending on the failure, different exceptions might be
             * returned */
            ITC_STATUS_SUCCESS);

        /* Destroy the Stamps */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp1);
        TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp2));
    }
}

/* Test comparing a Stamp fails with corrupt Id or Event */
void ITC_Stamp_Test_compareStampFailWithCorruptIdOrEvent(void)
{
    ITC_Stamp_t *pt_Stamp1;
    ITC_Stamp_t *pt_Stamp2;
    ITC_Stamp_Comparison_t t_Result;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp1));
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp2));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp1->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp1->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_compare(pt_Stamp1, pt_Stamp2, &t_Result),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp1->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp1->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp1->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp1->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_compare(pt_Stamp1, pt_Stamp2, &t_Result),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp1->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp1));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp2));
}

/* Test comparing two Stamps succeeds */
void ITC_Stamp_Test_compareStampsSucceeds(void)
{
    ITC_Stamp_t *pt_Stamp1;
    ITC_Stamp_t *pt_Stamp2;
    ITC_Stamp_Comparison_t t_Result;

    /* Create the Stamps */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp1));
    TEST_SUCCESS(ITC_Stamp_newPeek(pt_Stamp1, &pt_Stamp2));

    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &pt_Stamp1->pt_Event->pt_Left, pt_Stamp1->pt_Event, 0));
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &pt_Stamp1->pt_Event->pt_Right, pt_Stamp1->pt_Event, 3));

    pt_Stamp2->pt_Event->t_Count = 1;
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &pt_Stamp2->pt_Event->pt_Left, pt_Stamp2->pt_Event, 0));
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &pt_Stamp2->pt_Event->pt_Right, pt_Stamp2->pt_Event, 2));

    /* Compare Stamps */
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp1, pt_Stamp2, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_LESS_THAN, t_Result);
    /* Compare the other way around */
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp2, pt_Stamp1, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);

    /* Make the 2 Stamps concurrent */
    pt_Stamp2->pt_Event->pt_Right->t_Count -= 1;

    /* Compare Stamps */
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp1, pt_Stamp2, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_CONCURRENT, t_Result);
    /* Compare the other way around */
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp2, pt_Stamp1, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_CONCURRENT, t_Result);

    /* Check stamps are equal to themselves */
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp1, pt_Stamp1, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_EQUAL, t_Result);
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp2, pt_Stamp2, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_EQUAL, t_Result);

    /* Destroy the Stamps */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp1));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp2));
}

/* Test full Stamp lifecycle */
void ITC_Stamp_Test_fullStampLifecycle(void)
{
    ITC_Stamp_t *pt_Stamp0 = NULL;
    ITC_Stamp_t *pt_Stamp1 = NULL;
    ITC_Stamp_t *pt_Stamp2 = NULL;
    ITC_Stamp_t *pt_Stamp3 = NULL;
    ITC_Stamp_t *pt_Stamp4 = NULL;
    ITC_Stamp_t *pt_Stamp5 = NULL;
    ITC_Stamp_t *pt_Stamp6 = NULL;
    ITC_Stamp_t *pt_Stamp7 = NULL;

    /* clang-format off */
    /* Each pair corresponds to the call arg order of `ITC_Stamp_join`.
     * The join order is arbitrary */
    ITC_Stamp_t **rppt_JoinOrder[] = {
        &pt_Stamp1, &pt_Stamp2,
        &pt_Stamp1, &pt_Stamp5,
        &pt_Stamp6, &pt_Stamp3,
        &pt_Stamp1, &pt_Stamp6,
        &pt_Stamp0, &pt_Stamp1,
        &pt_Stamp4, &pt_Stamp7,
        &pt_Stamp0, &pt_Stamp4,
    };
    /* clang-format on */

    ITC_Stamp_t *pt_TmpStamp1;
    ITC_Stamp_t *pt_TmpStamp2;

    ITC_Stamp_Comparison_t t_Result;

    /* Create the initial stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp0));

    /* Split into Stamps with (1, 0) and (0, 1) IDs */
    TEST_SUCCESS(ITC_Stamp_fork(&pt_Stamp0, &pt_Stamp4));

    /* Add some events */
    TEST_SUCCESS(ITC_Stamp_event(pt_Stamp0));
    TEST_SUCCESS(ITC_Stamp_event(pt_Stamp0));
    TEST_SUCCESS(ITC_Stamp_event(pt_Stamp4));

    /* Test stamp ordering */
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp0, pt_Stamp4, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_CONCURRENT, t_Result);

    /* Split into Stamps with IDs:
     * pt_Stamp0 = ((1, 0), 0)
     * pt_Stamp2 = ((0, 1), 0)
     * pt_Stamp4 = (0, (1, 0))
     * pt_Stamp6 = (0, (0, 1))
     */
    TEST_SUCCESS(ITC_Stamp_fork(&pt_Stamp0, &pt_Stamp2));
    TEST_SUCCESS(ITC_Stamp_fork(&pt_Stamp4, &pt_Stamp6));

    /* Add some events */
    TEST_SUCCESS(ITC_Stamp_event(pt_Stamp2));
    TEST_SUCCESS(ITC_Stamp_event(pt_Stamp6));

    /* Test stamp ordering */
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp2, pt_Stamp6, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_CONCURRENT, t_Result);
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp2, pt_Stamp0, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp4, pt_Stamp6, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_LESS_THAN, t_Result);
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp0, pt_Stamp4, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_CONCURRENT, t_Result);

    /* Split into Stamps with IDs:
     * pt_Stamp0 = (((1, 0), 0), 0)
     * pt_Stamp1 = (((0, 1), 0), 0)
     * pt_Stamp2 = ((0, (1, 0)), 0)
     * pt_Stamp3 = ((0, (0, 1)), 0)
     * pt_Stamp4 = (0, ((1, 0), 0))
     * pt_Stamp5 = (0, ((0, 1), 0))
     * pt_Stamp6 = (0, (0, (1, 0)))
     * pt_Stamp7 = (0, (0, (0, 1)))
     */
    TEST_SUCCESS(ITC_Stamp_fork(&pt_Stamp0, &pt_Stamp1));
    TEST_SUCCESS(ITC_Stamp_fork(&pt_Stamp2, &pt_Stamp3));
    TEST_SUCCESS(ITC_Stamp_fork(&pt_Stamp4, &pt_Stamp5));
    TEST_SUCCESS(ITC_Stamp_fork(&pt_Stamp6, &pt_Stamp7));

    /* Add some events */
    TEST_SUCCESS(ITC_Stamp_event(pt_Stamp1));
    TEST_SUCCESS(ITC_Stamp_event(pt_Stamp3));
    TEST_SUCCESS(ITC_Stamp_event(pt_Stamp5));
    TEST_SUCCESS(ITC_Stamp_event(pt_Stamp6));
    TEST_SUCCESS(ITC_Stamp_event(pt_Stamp7));
    TEST_SUCCESS(ITC_Stamp_event(pt_Stamp7));

    /* Too lasy to test ordering here... It's probably fine (TM) */

    /* Sum them back into to a seed Stamp while adding events in
     * arbitrary order */
    for (uint32_t u32_I = 0; u32_I < ARRAY_COUNT(rppt_JoinOrder); u32_I += 2)
    {
        /* Clone the Stamps for comparison */
        TEST_SUCCESS(
            ITC_Stamp_clone(*rppt_JoinOrder[u32_I], &pt_TmpStamp1));
        TEST_SUCCESS(
            ITC_Stamp_clone(*rppt_JoinOrder[u32_I + 1], &pt_TmpStamp2));

        /* Join 2 Stamps */
        TEST_SUCCESS(
            ITC_Stamp_join(rppt_JoinOrder[u32_I], rppt_JoinOrder[u32_I + 1]));

        /* Test the joined Stamp is greater-than or equal to both of the
         * source stamps */
        TEST_SUCCESS(
            ITC_Stamp_compare(
                *rppt_JoinOrder[u32_I],
                pt_TmpStamp1,
                &t_Result));
        TEST_ASSERT_TRUE(t_Result & (ITC_STAMP_COMPARISON_EQUAL |
                                     ITC_STAMP_COMPARISON_GREATER_THAN));
        TEST_SUCCESS(
            ITC_Stamp_compare(*rppt_JoinOrder[u32_I], pt_TmpStamp2, &t_Result));
        TEST_ASSERT_TRUE(t_Result & (ITC_STAMP_COMPARISON_EQUAL |
                                     ITC_STAMP_COMPARISON_GREATER_THAN));

        TEST_SUCCESS(ITC_Stamp_destroy(&pt_TmpStamp1));
        TEST_SUCCESS(ITC_Stamp_destroy(&pt_TmpStamp2));

        /* Add Event on every second iteration but not the last one */
        if ((u32_I % 4 == 0) && ((u32_I + 4) < ARRAY_COUNT(rppt_JoinOrder)))
        {
            TEST_SUCCESS(ITC_Stamp_event(*rppt_JoinOrder[u32_I]));
        }
    }

    /* clang-format off */
    /* Test the summed up Stamp has a seed ID with a
     * (1, 3, (0, (0, 0, 1), 3)) Event tree */
    TEST_ITC_ID_IS_SEED_ID(pt_Stamp0->pt_Id);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_Stamp0->pt_Event, 1);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp0->pt_Event->pt_Left, 3);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_Stamp0->pt_Event->pt_Right, 0);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_Stamp0->pt_Event->pt_Right->pt_Left, 0);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp0->pt_Event->pt_Right->pt_Left->pt_Left, 0);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp0->pt_Event->pt_Right->pt_Left->pt_Right, 1);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp0->pt_Event->pt_Right->pt_Right, 3);
    /* clang-format on */

    /* Add an event */
    TEST_SUCCESS(ITC_Stamp_event(pt_Stamp0));

    /* Test the summed up Stamp has a seed ID with a
     * (4) Event tree */
    TEST_ITC_ID_IS_SEED_ID(pt_Stamp0->pt_Id);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp0->pt_Event, 4);

    /* Split into Stamps with (1, 0) and (0, 1) IDs again */
    TEST_SUCCESS(ITC_Stamp_fork(&pt_Stamp0, &pt_Stamp1));

    /* Add Event */
    TEST_SUCCESS(ITC_Stamp_event(pt_Stamp1));

    /* Share the Event history through a peek Stamp */
    TEST_SUCCESS(ITC_Stamp_newPeek(pt_Stamp1, &pt_TmpStamp1));
    TEST_SUCCESS(ITC_Stamp_join(&pt_Stamp0, &pt_TmpStamp1));

    /* Test the Stamp IDs haven't changed but the Event history has been shared */
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_Stamp0->pt_Id);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_Stamp0->pt_Event, 4);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp0->pt_Event->pt_Left, 0);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp0->pt_Event->pt_Right, 1);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_Stamp1->pt_Id);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_Stamp1->pt_Event, 4);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp1->pt_Event->pt_Left, 0);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp1->pt_Event->pt_Right, 1);

    /* Join Stamps back into a Stamp with a seed ID */
    TEST_SUCCESS(ITC_Stamp_join(&pt_Stamp0, &pt_Stamp1));

    /* Test the Stamp has a seed ID but the same Event history */
    TEST_ITC_ID_IS_SEED_ID(pt_Stamp0->pt_Id);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_Stamp0->pt_Event, 4);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp0->pt_Event->pt_Left, 0);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp0->pt_Event->pt_Right, 1);

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp0));
}

/* Test creating a Stamp from an ID fails with invalid param */
void ITC_Stamp_Test_createStampFromIdFailInvalidParam(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_DummyStamp = NULL;
    ITC_Id_t *pt_DummyId = NULL;

    TEST_FAILURE(
        ITC_Stamp_newFromId(
            pt_DummyId,
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_newFromId(
            NULL,
            &pt_DummyStamp),
        ITC_STATUS_INVALID_PARAM);
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test creating a Stamp from an invalid ID fails with corrupt ID */
void ITC_Stamp_Test_createStampFromIdFailWithCorruptId(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Id_t *pt_Id;

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_newFromId(pt_Id, &pt_Stamp),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the Stamp */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Id);
    }
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test creating a Stamp from an ID succeeds */
void ITC_Stamp_Test_createStampFromIdSuccessful(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp = NULL;
    ITC_Id_t *pt_Id = NULL;

    /* Create a new ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id, NULL));

    /* Create the Stamp */
    TEST_SUCCESS(ITC_Stamp_newFromId(pt_Id, &pt_Stamp));

    /* Test the ID has been copied and an Event tree has been allocated */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 0);
    TEST_ASSERT_TRUE(pt_Id != pt_Stamp->pt_Id);
    TEST_ITC_ID_IS_SEED_ID(pt_Stamp->pt_Id);

    /* Deallocate everything */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test creating a Stamp from an ID and Event fails with invalid param */
void ITC_Stamp_Test_createStampFromIdAndEventFailInvalidParam(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_DummyStamp = NULL;
    ITC_Id_t *pt_DummyId = NULL;
    ITC_Event_t *pt_DummyEvent = NULL;

    TEST_FAILURE(
        ITC_Stamp_newFromIdAndEvent(
            pt_DummyId,
            pt_DummyEvent,
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_newFromIdAndEvent(
            NULL,
            pt_DummyEvent,
            &pt_DummyStamp),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_newFromIdAndEvent(
            pt_DummyId,
            NULL,
            &pt_DummyStamp),
        ITC_STATUS_INVALID_PARAM);
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test creating a Stamp from an invalid ID or Event fails with corrupt ID/Event */
void ITC_Stamp_Test_createStampFromIdFailWithCorruptEventAndId(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Event_t *pt_Event;
    ITC_Id_t *pt_Id;

    /* Create a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id, NULL));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Event);

        /* Test for the failure */
        TEST_FAILURE(ITC_Stamp_newFromIdAndEvent(
            pt_Id,
            pt_Event,
            &pt_Stamp),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Event);
    }

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* Create a valid Event */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event, NULL, 0));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid Id */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_newFromIdAndEvent(
                pt_Id,
                pt_Event,
                &pt_Stamp),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the Id */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Id);
    }

    /* Destroy the Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test creating a Stamp from an ID and Event succeeds */
void ITC_Stamp_Test_createStampFromIdAndEventSuccessful(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp = NULL;
    ITC_Id_t *pt_Id = NULL;
    ITC_Event_t *pt_Event = NULL;

    /* Create a new ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id, NULL));

    /* Create a new Event */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event, NULL, 42));

    /* Create the Stamp */
    TEST_SUCCESS(ITC_Stamp_newFromIdAndEvent(pt_Id, pt_Event, &pt_Stamp));

    /* Test the ID and Event trees have been copied */
    TEST_ASSERT_TRUE(pt_Id != pt_Stamp->pt_Id);
    TEST_ASSERT_TRUE(pt_Event != pt_Stamp->pt_Event);
    TEST_ITC_ID_IS_SEED_ID(pt_Stamp->pt_Id);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 42);

    /* Deallocate everything */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test creating a peek Stamp from an Event fails with invalid param */
void ITC_Stamp_Test_createPeekStampFromEventFailInvalidParam(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_DummyStamp = NULL;
    ITC_Event_t *pt_DummyEvent = NULL;

    TEST_FAILURE(
        ITC_Stamp_newPeekFromEvent(
            pt_DummyEvent,
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_newPeekFromEvent(
            NULL,
            &pt_DummyStamp),
        ITC_STATUS_INVALID_PARAM);
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test creating a Stamp from an invalid Event fails with corrupt Event */
void ITC_Stamp_Test_createPeekStampFromEventFailWithCorruptEvent(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Event_t *pt_Event;

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_newPeekFromEvent(pt_Event, &pt_Stamp),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Stamp */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Event);
    }
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test creating a peek Stamp from an Event succeeds */
void ITC_Stamp_Test_createPeekStampFromEventSuccessful(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp = NULL;
    ITC_Event_t *pt_Event = NULL;

    /* Create a new ID */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event, NULL, 100));

    /* Create the Stamp */
    TEST_SUCCESS(ITC_Stamp_newPeekFromEvent(pt_Event, &pt_Stamp));

    /* Test the Event has been copied and an ID tree has been allocated */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 100);
    TEST_ASSERT_TRUE(pt_Event != pt_Stamp->pt_Event);
    TEST_ITC_ID_IS_NULL_ID(pt_Stamp->pt_Id);

    /* Deallocate everything */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test getting the ID component of a Stamp fails with invalid param */
void ITC_Stamp_Test_getIdFromStampFailInvalidParam(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_DummyStamp = NULL;
    ITC_Id_t *pt_DummyId;

    TEST_FAILURE(
        ITC_Stamp_getId(
            pt_DummyStamp,
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_getId(
            NULL,
            &pt_DummyId),
        ITC_STATUS_INVALID_PARAM);
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test getting the ID component of a Stamp fails with corrupt stamp */
void ITC_Stamp_Test_getIdFromStampFailWithCorruptStamp(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Id_t *pt_DummyId;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_ASSERT_NOT_EQUAL(
            ITC_Stamp_getId(pt_Stamp, &pt_DummyId),
            /* Depending on the failure, different exceptions might be
             * returned */
            ITC_STATUS_SUCCESS);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test getting the ID component of a Stamp fails with corrupt Id or Event */
void ITC_Stamp_Test_getIdFromStampFailWithCorruptIdOrEvent(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Id_t *pt_DummyId;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_getId(pt_Stamp, &pt_DummyId),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_getId(pt_Stamp, &pt_DummyId),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test getting the ID component of a Stamp succeeds */
void ITC_Stamp_Test_getIdFromStampSuccessful(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp = NULL;
    ITC_Id_t *pt_Id = NULL;

    /* Create the Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Get the ID */
    TEST_SUCCESS(ITC_Stamp_getId(pt_Stamp, &pt_Id));

    /* Test the ID has been copied */
    TEST_ASSERT_TRUE(pt_Id != pt_Stamp->pt_Id);
    TEST_ITC_ID_IS_SEED_ID(pt_Id);

    /* Deallocate everything */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the ID component of a Stamp fails with invalid param */
void ITC_Stamp_Test_setIdOfStampFailInvalidParam(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_DummyStamp = NULL;
    ITC_Id_t *pt_DummyId = NULL;

    TEST_FAILURE(
        ITC_Stamp_setId(
            pt_DummyStamp,
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_setId(
            NULL,
            pt_DummyId),
        ITC_STATUS_INVALID_PARAM);
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the Id component of a Stamp fails with corrupt stamp */
void ITC_Stamp_Test_setIdFromStampFailWithCorruptStamp(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Id_t *pt_DummyId = NULL;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_ASSERT_NOT_EQUAL(
            ITC_Stamp_setId(pt_Stamp, pt_DummyId),
            /* Depending on the failure, different exceptions might be
             * returned */
            ITC_STATUS_SUCCESS);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the ID component of a Stamp with invalid components fails with
 * corrupt Id or Event */
void ITC_Stamp_Test_setIdOfStampWithInvalidComponentFailWithCorruptIdOrEvent(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Id_t *pt_DummyId = NULL;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_setId(pt_Stamp, pt_DummyId),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_setId(pt_Stamp, pt_DummyId),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the ID component of a Stamp fails with corrupt ID */
void ITC_Stamp_Test_setIdOfStampFailWithCorruptId(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Id_t *pt_Id;

    /* Create a Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_setId(pt_Stamp, pt_Id),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the Stamp */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Id);
    }

    /* Destroy the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the ID component of a Stamp succeeds */
void ITC_Stamp_Test_setIdOfStampSuccessful(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp = NULL;
    ITC_Id_t *pt_Id = NULL;

    /* Create the Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Create the ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));

    /* Set the ID */
    TEST_SUCCESS(ITC_Stamp_setId(pt_Stamp, pt_Id));

    /* Test the ID has been copied */
    TEST_ASSERT_TRUE(pt_Id != pt_Stamp->pt_Id);
    /* XXX: Deliberately not testing if addresses of the old and new
     * `pt_Stamp->pt_Id`s differ, as depending on the `malloc` implementation
     * they might end up being the same address (since the old ID is deallocated
     * first, followed immediately by the new ID allocation) */
    TEST_ITC_ID_IS_NULL_ID(pt_Stamp->pt_Id);

    /* Deallocate everything */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test getting the Event component of a Stamp fails with invalid param */
void ITC_Stamp_Test_getEventFromStampFailInvalidParam(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_DummyStamp = NULL;
    ITC_Event_t *pt_DummyEvent;

    TEST_FAILURE(
        ITC_Stamp_getEvent(
            pt_DummyStamp,
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_getEvent(
            NULL,
            &pt_DummyEvent),
        ITC_STATUS_INVALID_PARAM);
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test getting the Event component of a Stamp fails with corrupt stamp */
void ITC_Stamp_Test_getEventFromStampFailWithCorruptStamp(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Event_t *pt_DummyEvent;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_ASSERT_NOT_EQUAL(
            ITC_Stamp_getEvent(pt_Stamp, &pt_DummyEvent),
            /* Depending on the failure, different exceptions might be
             * returned */
            ITC_STATUS_SUCCESS);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test getting the Event component of a Stamp fails with corrupt Id or Event */
void ITC_Stamp_Test_getEventFromStampFailWithCorruptIdOrEvent(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Event_t *pt_DummyEvent;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_getEvent(pt_Stamp, &pt_DummyEvent),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_getEvent(pt_Stamp, &pt_DummyEvent),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test getting the Event component of a Stamp succeeds */
void ITC_Stamp_Test_getEventFromStampSuccessful(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp = NULL;
    ITC_Event_t *pt_Event = NULL;

    /* Create the Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));
    pt_Stamp->pt_Event->t_Count = 42;

    /* Get the Event */
    TEST_SUCCESS(ITC_Stamp_getEvent(pt_Stamp, &pt_Event));

    /* Test the Event has been copied */
    TEST_ASSERT_TRUE(pt_Event != pt_Stamp->pt_Event);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Event, 42);

    /* Deallocate everything */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the Event component of a Stamp fails with invalid param */
void ITC_Stamp_Test_setEventOfStampFailInvalidParam(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_DummyStamp = NULL;
    ITC_Event_t *pt_DummyEvent = NULL;

    TEST_FAILURE(
        ITC_Stamp_setEvent(
            pt_DummyStamp,
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_setEvent(
            NULL,
            pt_DummyEvent),
        ITC_STATUS_INVALID_PARAM);
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the Event component of a Stamp fails with corrupt stamp */
void ITC_Stamp_Test_setEventFromStampFailWithCorruptStamp(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Event_t *pt_DummyEvent = NULL;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_ASSERT_NOT_EQUAL(
            ITC_Stamp_setEvent(pt_Stamp, pt_DummyEvent),
            /* Depending on the failure, different exceptions might be
             * returned */
            ITC_STATUS_SUCCESS);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the Event component of a Stamp with invalid components fails with
 * corrupt Event or Event */
void ITC_Stamp_Test_setEventOfStampWithInvalidComponentFailWithCorruptIdOrEvent(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Event_t *pt_DummyEvent = NULL;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_setEvent(pt_Stamp, pt_DummyEvent),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_setEvent(pt_Stamp, pt_DummyEvent),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the Event component of a Stamp fails with corrupt Event */
void ITC_Stamp_Test_setEventOfStampFailWithCorruptEvent(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Event_t *pt_Event;

    /* Create a Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_setEvent(pt_Stamp, pt_Event),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Stamp */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Event);
    }

    /* Destroy the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
#else
  TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the Event component of a Stamp succeeds */
void ITC_Stamp_Test_setEventOfStampSuccessful(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp = NULL;
    ITC_Event_t *pt_Event = NULL;

    /* Create the Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Create the Event */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event, NULL, 100));

    /* Set the Event */
    TEST_SUCCESS(ITC_Stamp_setEvent(pt_Stamp, pt_Event));

    /* Test the Event has been copied */
    TEST_ASSERT_TRUE(pt_Event != pt_Stamp->pt_Event);
    /* XXX: Deliberately not testing if addresses of the old and new
     * `pt_Stamp->pt_Event`s differ, as depending on the `malloc` implementation
     * they might end up being the same address (since the old Event is
     * deallocated first, followed immediately by the new ID allocation) */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 100);

    /* Deallocate everything */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}
