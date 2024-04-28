/**
 * @file ITC_Stamp_Test.h
 * @brief Unit tests for the Interval Tree Clock's Stamp mechanism
 *
 * @copyright Copyright (c) 2024 libITC project. Released under AGPL-3.0
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

/******************************************************************************
 *  Private functions
 ******************************************************************************/

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

/******************************************************************************
 *  Global variables
 ******************************************************************************/

/**
 * @brief Table of constructors for varous types of invalid Stamps
 *  Each constructor must return an invalid ITC_Stamp_t**.
 *
 *  It is expected that a a destructor for the invalid Stamp exists at the
 *  corresponding index in `gpv_InvalidStampDestructorTable`
 */
void (*gpv_InvalidStampConstructorTable[])(ITC_Stamp_t **) =
{
    newInvalidStampWithNoID,
    newInvalidStampWithNoEvent,
};

/**
 * @brief Table of destructors for varous types of invalid Stamps
 *  Each destructor must fully deallocate the invalid Stamp.
 *
 *  It is expected that a a constructor for the invalid Stamp exists at the
 *  corresponding index in `gpv_InvalidStampConstructorTable`
 */
void (*gpv_InvalidStampDestructorTable[])(ITC_Stamp_t **) =
{
    /* Cast the funcion pointer to the type of the table
     * This is ugly but beats needlessly having to write a destructor
     * for each invalid Stamp */
    (void (*)(ITC_Stamp_t **))ITC_Stamp_destroy,
    (void (*)(ITC_Stamp_t **))ITC_Stamp_destroy,
};

/******************************************************************************
 *  Public functions
 ******************************************************************************/

/* Init test */
void setUp(void) {}

/* Fini test */
void tearDown(void) {}

/* Test destroying a Stamp fails with invalid param */
void ITC_Stamp_Test_destroyStampFailInvalidParam(void)
{
    ITC_Stamp_t *pt_Dummy = NULL;

    TEST_FAILURE(ITC_Stamp_destroy(NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(ITC_Stamp_destroy(&pt_Dummy), ITC_STATUS_INVALID_PARAM);
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
         u32_I < ARRAY_COUNT(gpv_InvalidStampConstructorTable);
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_newPeek(pt_Stamp, &pt_PeekStamp),
            ITC_STATUS_CORRUPT_STAMP);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
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
         u32_I < ARRAY_COUNT(gpv_InvalidStampConstructorTable);
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_clone(pt_Stamp, &pt_ClonedStamp),
            ITC_STATUS_CORRUPT_STAMP);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
}

/* Test cloning an Stamp succeeds */
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

/* Test forking a Stamp fails with invalid param */
void ITC_Stamp_Test_forkStampFailInvalidParam(void)
{
    ITC_Stamp_t *pt_DummyStamp = NULL;

    TEST_FAILURE(
        ITC_Stamp_fork(pt_DummyStamp, NULL, NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_fork(NULL, &pt_DummyStamp, NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_fork(NULL, NULL, &pt_DummyStamp), ITC_STATUS_INVALID_PARAM);
}

/* Test forking a Stamp fails with corrupt stamp */
void ITC_Stamp_Test_forkStampFailWithCorruptStamp(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_ForkedStamp1;
    ITC_Stamp_t *pt_ForkedStamp2;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < ARRAY_COUNT(gpv_InvalidStampConstructorTable);
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_fork(pt_Stamp, &pt_ForkedStamp1, &pt_ForkedStamp2),
            ITC_STATUS_CORRUPT_STAMP);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
}

/* Test forking a Stamp succeeds */
void ITC_Stamp_Test_forkStampSuccessful(void)
{
    ITC_Stamp_t *pt_OriginalStamp;
    ITC_Stamp_t *pt_ForkedStamp1;
    ITC_Stamp_t *pt_ForkedStamp2;

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_OriginalStamp));

    /* Fork the Stamp */
    TEST_SUCCESS(
        ITC_Stamp_fork(pt_OriginalStamp, &pt_ForkedStamp1, &pt_ForkedStamp2));

    /* Destroy the original Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_OriginalStamp));

    /* Test the ID was cloned and split and the Event history was cloned */
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_ForkedStamp1->pt_Id);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_ForkedStamp2->pt_Id);
    TEST_ASSERT_TRUE(pt_ForkedStamp1->pt_Event != pt_ForkedStamp2->pt_Event);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_ForkedStamp1->pt_Event, 0);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_ForkedStamp2->pt_Event, 0);

    /* Destroy the forked Stamps */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_ForkedStamp1));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_ForkedStamp2));
}

/* Test joining two Stamps fails with invalid param */
void ITC_Stamp_Test_joinStampsFailInvalidParam(void)
{
    ITC_Stamp_t *pt_DummyStamp = NULL;

    TEST_FAILURE(
        ITC_Stamp_join(pt_DummyStamp, NULL, NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_join(NULL, pt_DummyStamp, NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_join(NULL, NULL, &pt_DummyStamp), ITC_STATUS_INVALID_PARAM);
}

/* Test joining two Stamps fails with corrupt stamp */
void ITC_Stamp_Test_joinStampsFailWithCorruptStamp(void)
{
    ITC_Stamp_t *pt_Stamp1;
    ITC_Stamp_t *pt_Stamp2;
    ITC_Stamp_t *pt_JoinedStamp;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < ARRAY_COUNT(gpv_InvalidStampConstructorTable);
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp1);

        /* Construct the other Stamp */
        TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp2));

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_join(pt_Stamp1, pt_Stamp2, &pt_JoinedStamp),
            ITC_STATUS_CORRUPT_STAMP);
        /* And the other way around */
        TEST_FAILURE(
            ITC_Stamp_join(pt_Stamp2, pt_Stamp1, &pt_JoinedStamp),
            ITC_STATUS_CORRUPT_STAMP);

        /* Destroy the Stamps */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp1);
        TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp2));
    }
}

/* Test joining two Stamps succeeds */
void ITC_Stamp_Test_joinStampsSuccessful(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_ForkedStamp1;
    ITC_Stamp_t *pt_ForkedStamp2;

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Fork the Stamp */
    TEST_SUCCESS(
        ITC_Stamp_fork(pt_Stamp, &pt_ForkedStamp1, &pt_ForkedStamp2));

    /* Destroy the original Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));

    /* Join the Stamps */
    TEST_SUCCESS(
        ITC_Stamp_join(pt_ForkedStamp1, pt_ForkedStamp2, &pt_Stamp));

    /* Destroy the forked Stamps */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_ForkedStamp1));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_ForkedStamp2));

    /* Test the ID is a seed ID and the Event history is a leaf with 0 events */
    TEST_ITC_ID_IS_SEED_ID(pt_Stamp->pt_Id);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 0);

    /* Destroy the original Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
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
         u32_I < ARRAY_COUNT(gpv_InvalidStampConstructorTable);
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_FAILURE(ITC_Stamp_event(pt_Stamp), ITC_STATUS_CORRUPT_STAMP);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
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
    TEST_ASSERT_TRUE(t_Result == ITC_STAMP_COMPARISON_GREATER_THAN);

    /* Create a new peek Stamp */
    TEST_SUCCESS(ITC_Stamp_newPeek(pt_Stamp, &pt_PeekStamp));

    /* Attempt to inflate the peek Stamp */
    TEST_SUCCESS(ITC_Stamp_event(pt_PeekStamp));

    /* Test the Event counter has not changed */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_PeekStamp->pt_Event, 1);
    TEST_SUCCESS(ITC_Stamp_compare(pt_PeekStamp, pt_Stamp, &t_Result));
    TEST_ASSERT_TRUE(t_Result == ITC_STAMP_COMPARISON_EQUAL);

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
    TEST_ASSERT_TRUE(t_Result == ITC_STAMP_COMPARISON_GREATER_THAN);

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
         u32_I < ARRAY_COUNT(gpv_InvalidStampConstructorTable);
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp1);

        /* Construct the other Stamp */
        TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp2));

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_compare(pt_Stamp1, pt_Stamp2, &t_Result),
            ITC_STATUS_CORRUPT_STAMP);
        /* And the other way around */
        TEST_FAILURE(
            ITC_Stamp_compare(pt_Stamp2, pt_Stamp1, &t_Result),
            ITC_STATUS_CORRUPT_STAMP);

        /* Destroy the Stamps */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp1);
        TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp2));
    }
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
    ITC_Stamp_t *pt_OriginalStamp;
    ITC_Stamp_t *pt_SplitStamp1;
    ITC_Stamp_t *pt_SplitStamp11;
    ITC_Stamp_t *pt_SplitStamp12;
    ITC_Stamp_t *pt_SplitStamp111;
    ITC_Stamp_t *pt_SplitStamp121;
    ITC_Stamp_t *pt_SplitStamp112;
    ITC_Stamp_t *pt_SplitStamp122;
    ITC_Stamp_t *pt_SplitStamp2;
    ITC_Stamp_t *pt_SplitStamp21;
    ITC_Stamp_t *pt_SplitStamp22;
    ITC_Stamp_t *pt_SplitStamp211;
    ITC_Stamp_t *pt_SplitStamp221;
    ITC_Stamp_t *pt_SplitStamp212;
    ITC_Stamp_t *pt_SplitStamp222;

    ITC_Stamp_t *pt_SummedStamp = NULL;
    ITC_Stamp_t *pt_TmpStamp = NULL;

    ITC_Stamp_Comparison_t t_Result;

    /* Create the initial stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_OriginalStamp));

    /* Split into Stamps with (1, 0) and (0, 1) IDs */
    TEST_SUCCESS(
        ITC_Stamp_fork(pt_OriginalStamp, &pt_SplitStamp1, &pt_SplitStamp2));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_OriginalStamp));

    /* Add some events */
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp1));
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp1));
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp2));

    /* Test stamp ordering */
    TEST_SUCCESS(ITC_Stamp_compare(pt_SplitStamp1, pt_SplitStamp2, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_CONCURRENT, t_Result);

    /* Split into Stamps with IDs:
     * - ((1, 0), 0)
     * - (0, (1, 0))
     * - ((0, 1), 0)
     * - (0, (0, 1))
     */
    TEST_SUCCESS(
        ITC_Stamp_fork(pt_SplitStamp1, &pt_SplitStamp11, &pt_SplitStamp21));
    TEST_SUCCESS(
        ITC_Stamp_fork(pt_SplitStamp2, &pt_SplitStamp12, &pt_SplitStamp22));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp1));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp2));

    /* Add some events */
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp11));
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp22));

    /* Test stamp ordering */
    TEST_SUCCESS(ITC_Stamp_compare(pt_SplitStamp11, pt_SplitStamp22, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_CONCURRENT, t_Result);
    TEST_SUCCESS(ITC_Stamp_compare(pt_SplitStamp11, pt_SplitStamp21, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);
    TEST_SUCCESS(ITC_Stamp_compare(pt_SplitStamp12, pt_SplitStamp22, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_LESS_THAN, t_Result);
    TEST_SUCCESS(ITC_Stamp_compare(pt_SplitStamp12, pt_SplitStamp21, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_CONCURRENT, t_Result);

    /* Split into Stamps with IDs:
     * - (((1, 0), 0), 0)
     * - (0, ((1, 0), 0))
     * - ((0, (1, 0)), 0)
     * - (0, (0, (1, 0)))
     * - (((0, 1), 0), 0)
     * - (0, ((0, 1), 0))
     * - ((0, (0, 1)), 0)
     * - (0, (0, (0, 1)))
     */
    TEST_SUCCESS(ITC_Stamp_fork(
        pt_SplitStamp11, &pt_SplitStamp111, &pt_SplitStamp211));
    TEST_SUCCESS(ITC_Stamp_fork(
        pt_SplitStamp12, &pt_SplitStamp112, &pt_SplitStamp212));
    TEST_SUCCESS(ITC_Stamp_fork(
        pt_SplitStamp21, &pt_SplitStamp121, &pt_SplitStamp221));
    TEST_SUCCESS(ITC_Stamp_fork(
        pt_SplitStamp22, &pt_SplitStamp122, &pt_SplitStamp222));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp11));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp12));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp21));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp22));

    /* Add some events */
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp211));
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp212));
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp222));
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp222));
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp122));
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp111));

    /* Too lasy to test ordering here... It's probably fine (TM) */

    /* Sum them back into to a seed Stamp while adding events in
     * arbitrary order */

    TEST_SUCCESS(ITC_Stamp_join(pt_SplitStamp222, pt_SplitStamp121, &pt_SummedStamp));

    /* Test the joined Stamp is greater than both of the source stamps */
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_SplitStamp222, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_SplitStamp121, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp222));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp121));

    /* Add some events */
    TEST_SUCCESS(ITC_Stamp_event(pt_SummedStamp));
    TEST_SUCCESS(ITC_Stamp_event(pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_clone(pt_SummedStamp, &pt_TmpStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_join(pt_SplitStamp211, pt_TmpStamp, &pt_SummedStamp));

    /* Test the joined Stamp is greater than both of the source stamps */
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_SplitStamp211, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_TmpStamp, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp211));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_TmpStamp));

    /* Add some events */
    TEST_SUCCESS(ITC_Stamp_event(pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_clone(pt_SummedStamp, &pt_TmpStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_join(pt_SplitStamp122, pt_TmpStamp, &pt_SummedStamp));

    /* Test the joined Stamp is greater than both of the source stamps */
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_SplitStamp122, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_TmpStamp, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp122));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_TmpStamp));

    /* Add some events */
    TEST_SUCCESS(ITC_Stamp_event(pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_clone(pt_SummedStamp, &pt_TmpStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_join(pt_SplitStamp111, pt_TmpStamp, &pt_SummedStamp));

    /* Test the joined Stamp is greater than both of the source stamps */
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_SplitStamp111, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_TmpStamp, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp111));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_TmpStamp));

    TEST_SUCCESS(ITC_Stamp_clone(pt_SummedStamp, &pt_TmpStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_join(pt_SplitStamp221, pt_TmpStamp, &pt_SummedStamp));

    /* Test the joined Stamp is greater than or equal to both of the
     * source stamps */
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_SplitStamp221, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_TmpStamp, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_EQUAL, t_Result);

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp221));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_TmpStamp));

    /* Add some events */
    TEST_SUCCESS(ITC_Stamp_event(pt_SummedStamp));
    TEST_SUCCESS(ITC_Stamp_event(pt_SummedStamp));
    TEST_SUCCESS(ITC_Stamp_event(pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_clone(pt_SummedStamp, &pt_TmpStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_join(pt_SplitStamp212, pt_TmpStamp, &pt_SummedStamp));

    /* Test the joined Stamp is greater than both of the source stamps */
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_SplitStamp212, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_TmpStamp, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp212));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_TmpStamp));

    TEST_SUCCESS(ITC_Stamp_clone(pt_SummedStamp, &pt_TmpStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_join(pt_SplitStamp112, pt_TmpStamp, &pt_SummedStamp));

    /* Test the joined Stamp is greater than or equal to both of the
     * source stamps */
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_SplitStamp112, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_TmpStamp, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_EQUAL, t_Result);

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp112));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_TmpStamp));

    /* clang-format off */
    /* Test the summed up Stamp has a seed ID with a
     * (1, 3, (0, (0, 0, 1), 8)) Event tree */
    TEST_ITC_ID_IS_SEED_ID(pt_SummedStamp->pt_Id);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_SummedStamp->pt_Event, 1);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SummedStamp->pt_Event->pt_Left, 3);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_SummedStamp->pt_Event->pt_Right, 0);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_SummedStamp->pt_Event->pt_Right->pt_Left, 0);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SummedStamp->pt_Event->pt_Right->pt_Left->pt_Left, 0);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SummedStamp->pt_Event->pt_Right->pt_Left->pt_Right, 1);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SummedStamp->pt_Event->pt_Right->pt_Right, 8);
    /* clang-format on */

    /* Add an event */
    TEST_SUCCESS(ITC_Stamp_event(pt_SummedStamp));

    /* Test the summed up Stamp has a seed ID with a
     * (9) Event tree */
    TEST_ITC_ID_IS_SEED_ID(pt_SummedStamp->pt_Id);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SummedStamp->pt_Event, 9);

    /* Split into Stamps with (1, 0) and (0, 1) IDs again */
    TEST_SUCCESS(ITC_Stamp_fork(
        pt_SummedStamp, &pt_SplitStamp1, &pt_SplitStamp2));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));

    /* Add Event */
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp1));

    /* Share the Event history */
    TEST_SUCCESS(ITC_Stamp_newPeek(pt_SplitStamp1, &pt_SummedStamp));
    TEST_SUCCESS(ITC_Stamp_join(pt_SummedStamp, pt_SplitStamp2, &pt_TmpStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp2));

    /* Test the Stamps have the different IDs but the same Event history */
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_SplitStamp1->pt_Id);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_SplitStamp1->pt_Event, 9);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SplitStamp1->pt_Event->pt_Left, 1);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SplitStamp1->pt_Event->pt_Right, 0);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_TmpStamp->pt_Id);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_TmpStamp->pt_Event, 9);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_TmpStamp->pt_Event->pt_Left, 1);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_TmpStamp->pt_Event->pt_Right, 0);

    TEST_SUCCESS(ITC_Stamp_join(pt_SplitStamp1, pt_TmpStamp, &pt_SummedStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp1));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_TmpStamp));

    /* Test the Stamps has a seed ID but the same Event history */
    TEST_ITC_ID_IS_SEED_ID(pt_SummedStamp->pt_Id);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_SummedStamp->pt_Event, 9);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SummedStamp->pt_Event->pt_Left, 1);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SummedStamp->pt_Event->pt_Right, 0);

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));
}
