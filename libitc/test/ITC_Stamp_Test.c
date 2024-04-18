#include "ITC_Stamp.h"
#include "ITC_Stamp_Test.h"

#include "ITC_Event_package.h"
#include "ITC_Id_package.h"
#include "ITC_Port.h"

#include "ITC_Test_package.h"

/******************************************************************************
 *  Private functions
 ******************************************************************************/

/**
 * @brief Same as ITC_Event_new but enforces setting the parent and an
 * event count
 *
 * @param ppt_Event (out) The pointer to the Event
 * @param pt_Parent The pointer to the parent Event. Otherwise NULL
 */
static ITC_Status_t newEvent(
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
        newEvent(&pt_Stamp->pt_Event->pt_Left, pt_Stamp->pt_Event, 0));
    TEST_SUCCESS(
        newEvent(&pt_Stamp->pt_Event->pt_Right, pt_Stamp->pt_Event, 3));

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
        newEvent(&pt_Stamp1->pt_Event->pt_Left, pt_Stamp1->pt_Event, 0));
    TEST_SUCCESS(
        newEvent(&pt_Stamp1->pt_Event->pt_Right, pt_Stamp1->pt_Event, 3));

    pt_Stamp2->pt_Event->t_Count = 1;
    TEST_SUCCESS(
        newEvent(&pt_Stamp2->pt_Event->pt_Left, pt_Stamp2->pt_Event, 0));
    TEST_SUCCESS(
        newEvent(&pt_Stamp2->pt_Event->pt_Right, pt_Stamp2->pt_Event, 2));

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
