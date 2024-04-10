#include "ITC_Event.h"
#include "ITC_Event_Test.h"
#include "ITC_Event_Test_private.h"

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
 * @brief Create a new invalid Event with asymmetric root parent
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidEventWithAsymmetricRootParent(ITC_Event_t **ppt_Event)
{
    TEST_SUCCESS(newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(newEvent(&(*ppt_Event)->pt_Right, *ppt_Event, 1));
}

/**
 * @brief Create a new invalid Event with asymmetric nested parent
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidEventWithAsymmetricNestedParent(ITC_Event_t **ppt_Event)
{
    TEST_SUCCESS(newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(newEvent(&(*ppt_Event)->pt_Left, *ppt_Event, 1));
    TEST_SUCCESS(newEvent(&(*ppt_Event)->pt_Right, *ppt_Event, 2));
    TEST_SUCCESS(
        newEvent(&(*ppt_Event)->pt_Right->pt_Left, (*ppt_Event)->pt_Right, 3));
}

/**
 * @brief Create a new invalid Event with NULL parent pointer
 *
 * Use `destroyInvalidEventWithNullParentPointer` to deallocate the Event
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidEventWithNullParentPointer(ITC_Event_t **ppt_Event)
{
    TEST_SUCCESS(newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(newEvent(&(*ppt_Event)->pt_Left, NULL, 1));
    TEST_SUCCESS(newEvent(&(*ppt_Event)->pt_Right, *ppt_Event, 2));
}

/**
 * @brief Deallocate an invalid Event created with
 * `newInvalidEventWithNullParentPointer`
 *
 *
 * @param pt_Event (in) The pointer to the root if the Event.
 */
static void destroyInvalidEventWithNullParentPointer(ITC_Event_t **ppt_Event)
{
    /* Fix the damage so the Event can be properly deallocated */
    (*ppt_Event)->pt_Left->pt_Parent = *ppt_Event;
    TEST_SUCCESS(ITC_Event_destroy(ppt_Event));
}

/**
 * @brief Create a new invalid Event with invalid parent pointer
 *
 * Use `destroyInvalidEventWithInvalidParentPointer` to deallocate the Event
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidEventWithInvalidParentPointer(ITC_Event_t **ppt_Event)
{
    TEST_SUCCESS(newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(newEvent(&(*ppt_Event)->pt_Left, *ppt_Event, 1));
    TEST_SUCCESS(newEvent(&(*ppt_Event)->pt_Right, (*ppt_Event)->pt_Left, 2));
}

/**
 * @brief Deallocate an invalid Event created with
 * `newInvalidEventWithInvalidParentPointer`
 *
 * @param pt_Event (in) The pointer to the root if the Event.
 */
static void destroyInvalidEventWithInvalidParentPointer(ITC_Event_t **ppt_Event)
{
    /* Fix the damage so the Event can be properly deallocated */
    (*ppt_Event)->pt_Right->pt_Parent = *ppt_Event;
    TEST_SUCCESS(ITC_Event_destroy(ppt_Event));
}

/******************************************************************************
 *  Global variables
 ******************************************************************************/

/**
 * @brief Table of constructors for varous types of invalid Events
 *  Each constructor must return an invalid ITC_Event_t**.
 *
 *  It is expected that a a destructor for the invalid Event exists at the
 *  corresponding index in `gpv_InvalidEventDestructorTable`
 */
void (*gpv_InvalidEventConstructorTable[])(ITC_Event_t **) =
{
    newInvalidEventWithAsymmetricRootParent,
    newInvalidEventWithAsymmetricNestedParent,
    newInvalidEventWithNullParentPointer,
    newInvalidEventWithInvalidParentPointer,
};

/**
 * @brief Table of destructors for varous types of invalid Events
 *  Each destructor must fully deallocate the invalid Event.
 *
 *  It is expected that a a constructor for the invalid Event exists at the
 *  corresponding index in `gpv_InvalidEventConstructorTable`
 */
void (*gpv_InvalidEventDestructorTable[])(ITC_Event_t **) =
{
    /* Cast the funcion pointer to the type of the table
     * This is ugly but beats needlessly having to write a destructor
     * for each invalid Event */
    (void (*)(ITC_Event_t **))ITC_Event_destroy,
    (void (*)(ITC_Event_t **))ITC_Event_destroy,
    destroyInvalidEventWithNullParentPointer,
    destroyInvalidEventWithInvalidParentPointer,
};

/******************************************************************************
 *  Public functions
 ******************************************************************************/

/* Init test */
void setUp(void) {}

/* Fini test */
void tearDown(void) {}

/* Test destroying an Event fails with invalid param */
void ITC_Event_Test_destroyEventFailInvalidParam(void)
{
    ITC_Event_t *pt_Dummy = NULL;

    TEST_FAILURE(ITC_Event_destroy(NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(ITC_Event_destroy(&pt_Dummy), ITC_STATUS_INVALID_PARAM);
}

/* Test creating a NULL Event fails with invalid param */
void ITC_Event_Test_createNullEventFailInvalidParam(void)
{
    TEST_FAILURE(ITC_Event_new(NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test creating an Event succeeds */
void ITC_Event_Test_createEventSuccessful(void)
{
    ITC_Event_t *pt_Event;

    /* Create a new Event */
    TEST_SUCCESS(ITC_Event_new(&pt_Event));

    /* Test this is a leaf node with 0 events */
    TEST_ASSERT_FALSE(pt_Event->pt_Parent);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event, 0);

    /* Destroy the Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
}

/* Test cloning an Event fails with invalid param */
void ITC_Event_Test_cloneEventFailInvalidParam(void)
{
  ITC_Event_t *pt_DummyEvent = NULL;

  TEST_FAILURE(ITC_Event_clone(NULL, &pt_DummyEvent), ITC_STATUS_INVALID_PARAM);
  TEST_FAILURE(ITC_Event_clone(pt_DummyEvent, NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test cloning an Event fails with corrupt event */
void ITC_Event_Test_cloneEventFailWithCorruptEvent(void)
{
    ITC_Event_t *pt_Event;
    ITC_Event_t *pt_ClonedEvent;

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < ARRAY_COUNT(gpv_InvalidEventConstructorTable);
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Event_clone(pt_Event, &pt_ClonedEvent),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Event);
    }
}

/* Test cloning an Event succeeds */
void ITC_Event_Test_cloneEventSuccessful(void)
{
    ITC_Event_t *pt_OriginalEvent = NULL;
    ITC_Event_t *pt_ClonedEvent = NULL;

    /* Test cloning an Event */
    TEST_SUCCESS(newEvent(&pt_OriginalEvent, NULL, 0));
    TEST_SUCCESS(ITC_Event_clone(pt_OriginalEvent, &pt_ClonedEvent));
    TEST_ASSERT_TRUE(pt_OriginalEvent != pt_ClonedEvent);
    TEST_SUCCESS(ITC_Event_destroy(&pt_OriginalEvent));

    TEST_ASSERT_FALSE(pt_ClonedEvent->pt_Parent);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_ClonedEvent, 0);
    TEST_SUCCESS(ITC_Event_destroy(&pt_ClonedEvent));

    /* Test cloning a complex Event */
    TEST_SUCCESS(newEvent(&pt_OriginalEvent, NULL, 0));
    TEST_SUCCESS(newEvent(&pt_OriginalEvent->pt_Left, pt_OriginalEvent, 1));
    TEST_SUCCESS(newEvent(&pt_OriginalEvent->pt_Right, pt_OriginalEvent, 2));
    TEST_SUCCESS(ITC_Event_clone(pt_OriginalEvent, &pt_ClonedEvent));
    TEST_ASSERT_TRUE(pt_OriginalEvent != pt_ClonedEvent);
    TEST_EVENT_IS_PARENT_N_EVENT(pt_ClonedEvent, 0);
    TEST_ASSERT_TRUE(pt_OriginalEvent->pt_Left != pt_ClonedEvent->pt_Left);
    TEST_ASSERT_TRUE(pt_OriginalEvent->pt_Right != pt_ClonedEvent->pt_Right);
    TEST_SUCCESS(ITC_Event_destroy(&pt_OriginalEvent));

    TEST_ASSERT_FALSE(pt_ClonedEvent->pt_Parent);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_ClonedEvent->pt_Left, 1);
    TEST_ASSERT_TRUE(pt_ClonedEvent->pt_Left->pt_Parent == pt_ClonedEvent);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_ClonedEvent->pt_Right, 2);
    TEST_ASSERT_TRUE(pt_ClonedEvent->pt_Right->pt_Parent == pt_ClonedEvent);
    TEST_SUCCESS(ITC_Event_destroy(&pt_ClonedEvent));
}

/* Test cloning a subtree of an Event succeeds */
void ITC_Event_Test_cloneEventSubtreeSuccessful(void)
{
    ITC_Event_t *pt_OriginalEvent = NULL;
    ITC_Event_t *pt_ClonedEvent = NULL;

    /* Test cloning an Event subree */
    TEST_SUCCESS(newEvent(&pt_OriginalEvent, NULL, 0));
    TEST_SUCCESS(newEvent(&pt_OriginalEvent->pt_Left, pt_OriginalEvent, 1));
    TEST_SUCCESS(newEvent(&pt_OriginalEvent->pt_Right, pt_OriginalEvent, 2));
    TEST_SUCCESS(ITC_Event_clone(pt_OriginalEvent->pt_Left, &pt_ClonedEvent));
    TEST_ASSERT_TRUE(pt_OriginalEvent->pt_Left != pt_ClonedEvent);
    TEST_SUCCESS(ITC_Event_destroy(&pt_OriginalEvent));

    TEST_ASSERT_FALSE(pt_ClonedEvent->pt_Parent);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_ClonedEvent, 1);
    TEST_SUCCESS(ITC_Event_destroy(&pt_ClonedEvent));

    /* clang-format off */
    /* Test cloning a complex Event subree */
    TEST_SUCCESS(newEvent(&pt_OriginalEvent, NULL, 0));
    TEST_SUCCESS(newEvent(&pt_OriginalEvent->pt_Left, pt_OriginalEvent, 1));
    TEST_SUCCESS(newEvent(&pt_OriginalEvent->pt_Left->pt_Left, pt_OriginalEvent->pt_Left, 2));
    TEST_SUCCESS(newEvent(&pt_OriginalEvent->pt_Left->pt_Right, pt_OriginalEvent->pt_Left, 3));
    TEST_SUCCESS(newEvent(&pt_OriginalEvent->pt_Right, pt_OriginalEvent, 4));
    TEST_SUCCESS(ITC_Event_clone(pt_OriginalEvent->pt_Left, &pt_ClonedEvent));
    TEST_ASSERT_TRUE(pt_OriginalEvent->pt_Left != pt_ClonedEvent);
    TEST_EVENT_IS_PARENT_N_EVENT(pt_ClonedEvent, 1);
    TEST_ASSERT_TRUE(pt_OriginalEvent->pt_Left->pt_Left != pt_ClonedEvent->pt_Left);
    TEST_ASSERT_TRUE(pt_OriginalEvent->pt_Left->pt_Right != pt_ClonedEvent->pt_Right);
    TEST_SUCCESS(ITC_Event_destroy(&pt_OriginalEvent));
    /* clang-format on */

    TEST_ASSERT_FALSE(pt_ClonedEvent->pt_Parent);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_ClonedEvent->pt_Left, 2);
    TEST_ASSERT_TRUE(pt_ClonedEvent->pt_Left->pt_Parent == pt_ClonedEvent);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_ClonedEvent->pt_Right, 3);
    TEST_ASSERT_TRUE(pt_ClonedEvent->pt_Right->pt_Parent == pt_ClonedEvent);
    TEST_SUCCESS(ITC_Event_destroy(&pt_ClonedEvent));
}

/* Test normalising an Event fails with invalid param */
void ITC_Event_Test_normaliseEventFailInvalidParam(void)
{
    TEST_FAILURE(ITC_Event_normalise(NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test normalising an Event fails with corrupt event */
void ITC_Event_Test_normaliseEventFailWithCorruptEvent(void)
{
    ITC_Event_t *pt_Event;

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < ARRAY_COUNT(gpv_InvalidEventConstructorTable);
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Event);

        /* Test for the failure */
        TEST_FAILURE(ITC_Event_normalise(pt_Event), ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Event);
    }
}

void ITC_Event_Test_normaliseLeafEvent(void)
{
    ITC_Event_t *pt_Event = NULL;

    /* Create the 0 leaf event */
    TEST_SUCCESS(newEvent(&pt_Event, NULL, 0));
    /* Normalise the event */
    ITC_Event_normalise(pt_Event);
    /* Test this is still a 0 leaf event */
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event, 0);
    /* Destroy the event*/
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));

    /* Create the 1 leaf event */
    TEST_SUCCESS(newEvent(&pt_Event, NULL, 1));
    /* Normalise the event */
    ITC_Event_normalise(pt_Event);
    /* Test this is still a 1 leaf event */
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event, 1);
    /* Destroy the event*/
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
}

void ITC_Event_Test_normaliseLeafEventSubtree(void)
{
    ITC_Event_t *pt_Event = NULL;

    /* Create the root event */
    TEST_SUCCESS(newEvent(&pt_Event, NULL, 0));
    /* Create the 0 leaf event */
    TEST_SUCCESS(newEvent(&pt_Event->pt_Left, pt_Event, 0));
    /* Create the 1 leaf event */
    TEST_SUCCESS(newEvent(&pt_Event->pt_Right, pt_Event, 1));

    /* Normalise the event subtree */
    ITC_Event_normalise(pt_Event->pt_Left);
    /* Test the whole event tree hasn't changed */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right, 1);

    /* Normalise the event subtree */
    ITC_Event_normalise(pt_Event->pt_Right);
    /* Test the whole event tree hasn't changed */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right, 1);

    /* Destroy the event*/
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
}

void ITC_Event_Test_normaliseParentEventWithLeafChildren(void)
{
    ITC_Event_t *pt_Event = NULL;

    /* Create the root event */
    TEST_SUCCESS(newEvent(&pt_Event, NULL, 1));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Left, pt_Event, 2));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Right, pt_Event, 3));

    /* Normalise the event */
    ITC_Event_normalise(pt_Event);
    /* Test the event has been normalised */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event, 3);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right, 1);

    /* Normalise the normalised event */
    ITC_Event_normalise(pt_Event);
    /* Test the event hasn't changed */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event, 3);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right, 1);

    /* Make the children event count equal */
    pt_Event->pt_Right->t_Count = pt_Event->pt_Left->t_Count;

    /* Normalise the event */
    ITC_Event_normalise(pt_Event);
    /* Test the event has been normalised */
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event, 3);

    /* Destroy the event*/
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
}

void ITC_Event_Test_normaliseParentEventSubtreeWithLeafChildren(void)
{
    ITC_Event_t *pt_Event = NULL;

    /* Create the root event */
    TEST_SUCCESS(newEvent(&pt_Event, NULL, 2));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Left, pt_Event, 2));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Left->pt_Left, pt_Event->pt_Left, 4));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Left->pt_Right, pt_Event->pt_Left, 1));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Right, pt_Event, 1));

    /* Normalise the event subtree */
    ITC_Event_normalise(pt_Event->pt_Left);
    /* Test the event subtree has been normalised */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event->pt_Left, 3);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left->pt_Left, 3);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left->pt_Right, 0);
    /* Test the rest of the tree hasn't changed */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event, 2);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right, 1);

    /* Normalise the normalised event subtree*/
    ITC_Event_normalise(pt_Event->pt_Left);
    /* Test the event subtree has been normalised */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event->pt_Left, 3);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left->pt_Left, 3);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left->pt_Right, 0);
    /* Test the rest of the tree hasn't changed */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event, 2);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right, 1);

    /* Make the children event count equal */
    pt_Event->pt_Left->pt_Right->t_Count =
        pt_Event->pt_Left->pt_Left->t_Count;

    /* Normalise the event */
    ITC_Event_normalise(pt_Event->pt_Left);
    /* Test the event has been normalised */
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left, 6);
    /* Test the rest of the tree hasn't changed */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event, 2);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right, 1);

    /* Destroy the event*/
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
}

void ITC_Event_Test_normaliseComplexEvent(void)
{
    ITC_Event_t *pt_Event = NULL;

    /* clang-format off */
    /* Create the complex event */
    TEST_SUCCESS(newEvent(&pt_Event, NULL, 1));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Left, pt_Event, 2));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Left->pt_Left, pt_Event->pt_Left, 2));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Left->pt_Right, pt_Event->pt_Left, 2));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Right, pt_Event, 3));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Right->pt_Left, pt_Event->pt_Right, 4));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Right->pt_Right, pt_Event->pt_Right, 3));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Right->pt_Right->pt_Left, pt_Event->pt_Right->pt_Right, 3));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Right->pt_Right->pt_Right, pt_Event->pt_Right->pt_Right, 2));
    /* clang-format on */

    /* Normalise the event */
    ITC_Event_normalise(pt_Event);
    /* Test the event has been normalised */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event, 5);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left, 0);
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event->pt_Right, 3);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right->pt_Left, 0);
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event->pt_Right->pt_Right, 1);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right->pt_Right->pt_Left, 1);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right->pt_Right->pt_Right, 0);

    /* Normalise the normalised event */
    ITC_Event_normalise(pt_Event);
    /* Test the event hasn't changed */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event, 5);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left, 0);
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event->pt_Right, 3);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right->pt_Left, 0);
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event->pt_Right->pt_Right, 1);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right->pt_Right->pt_Left, 1);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right->pt_Right->pt_Right, 0);

    /* Destroy the event*/
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
}

void ITC_Event_Test_normaliseComplexEventSubtree(void)
{
    ITC_Event_t *pt_Event = NULL;

    /* clang-format off */
    /* Create the complex event */
    TEST_SUCCESS(newEvent(&pt_Event, NULL, 1));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Left, pt_Event, 1));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Left->pt_Left, pt_Event->pt_Left, 3));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Left->pt_Left->pt_Left, pt_Event->pt_Left->pt_Left, 3));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Left->pt_Left->pt_Left->pt_Left, pt_Event->pt_Left->pt_Left->pt_Left, 3));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Left->pt_Left->pt_Left->pt_Right, pt_Event->pt_Left->pt_Left->pt_Left, 2));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Left->pt_Left->pt_Right, pt_Event->pt_Left->pt_Left, 4));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Left->pt_Right, pt_Event->pt_Left, 2));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Left->pt_Right->pt_Left, pt_Event->pt_Left->pt_Right, 2));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Left->pt_Right->pt_Right, pt_Event->pt_Left->pt_Right, 2));
    TEST_SUCCESS(newEvent(&pt_Event->pt_Right, pt_Event, 1));

    /* Normalise the event subtree */
    ITC_Event_normalise(pt_Event->pt_Left);
    /* Test the event subtree has been normalised */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event->pt_Left, 5);
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event->pt_Left->pt_Left, 3);
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event->pt_Left->pt_Left->pt_Left, 1);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left->pt_Left->pt_Left->pt_Left, 1);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left->pt_Left->pt_Left->pt_Right, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left->pt_Left->pt_Right, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left->pt_Right, 0);
    /* Test the rest of the tree hasn't changed */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event, 1);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right, 1);

    /* Normalise the normalised event subtree */
    ITC_Event_normalise(pt_Event->pt_Left);
    /* Test the event subtree hasn't changed */
    /* Test the event subtree has been normalised */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event->pt_Left, 5);
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event->pt_Left->pt_Left, 3);
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event->pt_Left->pt_Left->pt_Left, 1);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left->pt_Left->pt_Left->pt_Left, 1);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left->pt_Left->pt_Left->pt_Right, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left->pt_Left->pt_Right, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left->pt_Right, 0);
    /* Test the rest of the tree hasn't changed */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_Event, 1);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right, 1);
    /* clang-format on */

    /* Destroy the event*/
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
}

/* Test joining Events fails with invalid param */
void ITC_Event_Test_joinEventFailInvalidParam(void)
{
    ITC_Event_t *pt_Dummy = NULL;

    TEST_FAILURE(
        ITC_Event_join(pt_Dummy, NULL, NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Event_join(NULL, pt_Dummy, NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Event_join(NULL, NULL, &pt_Dummy), ITC_STATUS_INVALID_PARAM);
}

/* Test joining an Event fails with corrupt Event */
void ITC_Event_Test_joinEventFailWithCorruptEvent(void)
{
    ITC_Event_t *pt_Event1;
    ITC_Event_t *pt_Event2;
    ITC_Event_t *pt_JoinEvent;

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < ARRAY_COUNT(gpv_InvalidEventConstructorTable);
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Event1);

        /* Construct the other Event */
        TEST_SUCCESS(newEvent(&pt_Event2, NULL, 0));

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Event_join(pt_Event1, pt_Event2, &pt_JoinEvent),
            ITC_STATUS_CORRUPT_EVENT);
        /* And the other way around */
        TEST_FAILURE(
            ITC_Event_join(pt_Event2, pt_Event1, &pt_JoinEvent),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Events */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Event1);
        TEST_SUCCESS(ITC_Event_destroy(&pt_Event2));
    }
}

/* Test joining two identical leaf events */
void ITC_Event_Test_joinTwoIdenticalLeafEvents(void)
{
    ITC_Event_t *pt_Event1;
    ITC_Event_t *pt_Event2;
    ITC_Event_t *pt_JoinEvent;

    /* Construct the original Events */
    TEST_SUCCESS(newEvent(&pt_Event1, NULL, 1));
    TEST_SUCCESS(newEvent(&pt_Event2, NULL, 1));

    /* Test joining the events */
    TEST_SUCCESS(ITC_Event_join(pt_Event1, pt_Event2, &pt_JoinEvent));
    /* Test the joined event is a leaf with 1 counter */
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent, 1);

    /* Destroy the Events */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event1));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event2));
    TEST_SUCCESS(ITC_Event_destroy(&pt_JoinEvent));
}

/* Test joining two identical leaf event subtrees */
void ITC_Event_Test_joinTwoIdenticalLeafEventSubtrees(void)
{
    ITC_Event_t *pt_Event1;
    ITC_Event_t *pt_Event2;
    ITC_Event_t *pt_JoinEvent;

    /* Construct the original Events */
    TEST_SUCCESS(newEvent(&pt_Event1, NULL, 1));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Left, pt_Event1, 0));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Right, pt_Event1, 1));
    TEST_SUCCESS(newEvent(&pt_Event2, NULL, 1));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Left, pt_Event2, 1));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Right, pt_Event2, 0));

    /* Test joining the events */
    TEST_SUCCESS(
        ITC_Event_join(pt_Event1->pt_Right, pt_Event2->pt_Left, &pt_JoinEvent));
    /* Test the joined event is a leaf with 1 counter */
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent, 1);

    /* Destroy the Events */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event1));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event2));
    TEST_SUCCESS(ITC_Event_destroy(&pt_JoinEvent));
}

/* Test joining two different leaf events */
void ITC_Event_Test_joinTwoDifferentLeafEvents(void)
{
    ITC_Event_t *pt_Event1;
    ITC_Event_t *pt_Event2;
    ITC_Event_t *pt_JoinEvent;

    /* Construct the original Events */
    TEST_SUCCESS(newEvent(&pt_Event1, NULL, 4));
    TEST_SUCCESS(newEvent(&pt_Event2, NULL, 2));

    /* Test joining the events */
    TEST_SUCCESS(ITC_Event_join(pt_Event1, pt_Event2, &pt_JoinEvent));

    /* Test the joined event is a leaf with the bigger event counter */
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent, 4);

    /* Destroy the joined event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_JoinEvent));

    /* Test joining the events the other way around */
    TEST_SUCCESS(ITC_Event_join(pt_Event2, pt_Event1, &pt_JoinEvent));

    /* Test the joined event is a leaf with the bigger event counter */
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent, 4);

    /* Destroy the Events */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event1));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event2));
    TEST_SUCCESS(ITC_Event_destroy(&pt_JoinEvent));
}

/* Test joining two different leaf event subtrees */
void ITC_Event_Test_joinTwoDifferentLeafEventSubtrees(void)
{
    ITC_Event_t *pt_Event1;
    ITC_Event_t *pt_Event2;
    ITC_Event_t *pt_JoinEvent;

    /* Construct the original Events */
    TEST_SUCCESS(newEvent(&pt_Event1, NULL, 1));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Left, pt_Event1, 0));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Right, pt_Event1, 4));
    TEST_SUCCESS(newEvent(&pt_Event2, NULL, 2));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Left, pt_Event2, 2));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Right, pt_Event2, 0));

    /* Test joining the events */
    TEST_SUCCESS(
        ITC_Event_join(pt_Event1->pt_Right, pt_Event2->pt_Left, &pt_JoinEvent));

    /* Test the joined event is a leaf with the bigger event counter */
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent, 4);

    /* Destroy the joined event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_JoinEvent));

    /* Test joining the events the other way around */
    TEST_SUCCESS(
        ITC_Event_join(pt_Event2->pt_Left, pt_Event1->pt_Right, &pt_JoinEvent));

    /* Test the joined event is a leaf with the bigger event counter */
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent, 4);

    /* Destroy the Events */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event1));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event2));
    TEST_SUCCESS(ITC_Event_destroy(&pt_JoinEvent));
}

/* Test joining a leaf and a parent event */
void ITC_Event_Test_joinALeafAndAParentEvents(void)
{
    ITC_Event_t *pt_Event1;
    ITC_Event_t *pt_Event2;
    ITC_Event_t *pt_JoinEvent;

    /* Construct the original Events */
    TEST_SUCCESS(newEvent(&pt_Event1, NULL, 4));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Left, pt_Event1, 0));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Right, pt_Event1, 6));
    TEST_SUCCESS(newEvent(&pt_Event2, NULL, 2));

    /* Test joining the events */
    TEST_SUCCESS(ITC_Event_join(pt_Event1, pt_Event2, &pt_JoinEvent));

    /* Test the joined event is a (4, 0, 6) event */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_JoinEvent, 4);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Left, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Right, 6);

    /* Destroy the joined event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_JoinEvent));

    /* Test joining the events the other way around */
    TEST_SUCCESS(ITC_Event_join(pt_Event2, pt_Event1, &pt_JoinEvent));

    /* Test the joined event is a (4, 0, 6) event */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_JoinEvent, 4);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Left, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Right, 6);

    /* Destroy the Events */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event1));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event2));
    TEST_SUCCESS(ITC_Event_destroy(&pt_JoinEvent));
}

/* Test joining a leaf and a parent event subtrees */
void ITC_Event_Test_joinALeafAndAParentEventSubtrees(void)
{
    ITC_Event_t *pt_Event1;
    ITC_Event_t *pt_Event2;
    ITC_Event_t *pt_JoinEvent;

    /* clang-format off */
    /* Construct the original Events */
    TEST_SUCCESS(newEvent(&pt_Event1, NULL, 4));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Left, pt_Event1, 4));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Left->pt_Left, pt_Event1->pt_Left, 0));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Left->pt_Right, pt_Event1->pt_Left, 6));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Right, pt_Event1, 0));

    TEST_SUCCESS(newEvent(&pt_Event2, NULL, 2));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Left, pt_Event2, 0));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Right, pt_Event2, 2));
    /* clang-format on */

    /* Test joining the events */
    TEST_SUCCESS(
        ITC_Event_join(pt_Event1->pt_Left, pt_Event2->pt_Right, &pt_JoinEvent));

    /* Test the joined event is a (4, 0, 6) event */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_JoinEvent, 4);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Left, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Right, 6);

    /* Destroy the joined event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_JoinEvent));

    /* Test joining the events the other way around */
    TEST_SUCCESS(
        ITC_Event_join(pt_Event2->pt_Right, pt_Event1->pt_Left, &pt_JoinEvent));

    /* Test the joined event is a (4, 0, 6) event */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_JoinEvent, 4);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Left, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Right, 6);

    /* Destroy the Events */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event1));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event2));
    TEST_SUCCESS(ITC_Event_destroy(&pt_JoinEvent));
}

/* Test joining two identical parent events */
void ITC_Event_Test_joinTwoIdenticalParentEvents(void)
{
    ITC_Event_t *pt_Event1;
    ITC_Event_t *pt_Event2;
    ITC_Event_t *pt_JoinEvent;

    /* Construct the original Events */
    TEST_SUCCESS(newEvent(&pt_Event1, NULL, 1));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Left, pt_Event1, 0));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Right, pt_Event1, 3));
    TEST_SUCCESS(newEvent(&pt_Event2, NULL, 1));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Left, pt_Event2, 0));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Right, pt_Event2, 3));

    /* Test joining the events */
    TEST_SUCCESS(ITC_Event_join(pt_Event1, pt_Event2, &pt_JoinEvent));

    /* Test the joined event is a (4, 0, 6) event */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_JoinEvent, 1);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Left, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Right, 3);

    /* Destroy the Events */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event1));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event2));
    TEST_SUCCESS(ITC_Event_destroy(&pt_JoinEvent));
}

/* Test joining two mirrored parent events */
void ITC_Event_Test_joinTwoMirroredParentEvents(void)
{
    ITC_Event_t *pt_Event1;
    ITC_Event_t *pt_Event2;
    ITC_Event_t *pt_JoinEvent;

    /* Construct the original Events */
    TEST_SUCCESS(newEvent(&pt_Event1, NULL, 1));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Left, pt_Event1, 0));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Right, pt_Event1, 3));
    TEST_SUCCESS(newEvent(&pt_Event2, NULL, 1));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Left, pt_Event2, 3));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Right, pt_Event2, 0));

    /* Test joining the events */
    TEST_SUCCESS(ITC_Event_join(pt_Event1, pt_Event2, &pt_JoinEvent));

    /* Test the joined event is a leaf event with 4 events */
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent, 4);

    /* Destroy the joined event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_JoinEvent));

    /* Test joining the events the other way around */
    TEST_SUCCESS(ITC_Event_join(pt_Event2, pt_Event1, &pt_JoinEvent));

    /* Test the joined event is a leaf event with 4 events */
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent, 4);

    /* Destroy the Events */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event1));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event2));
    TEST_SUCCESS(ITC_Event_destroy(&pt_JoinEvent));
}

/* Test joining two different parent events */
void ITC_Event_Test_joinTwoDifferentParentEvents(void)
{
    ITC_Event_t *pt_Event1;
    ITC_Event_t *pt_Event2;
    ITC_Event_t *pt_JoinEvent;

    /* Construct the original Events */
    TEST_SUCCESS(newEvent(&pt_Event1, NULL, 2));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Left, pt_Event1, 4));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Right, pt_Event1, 0));
    TEST_SUCCESS(newEvent(&pt_Event2, NULL, 1));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Left, pt_Event2, 0));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Right, pt_Event2, 6));

    /* Test joining the events */
    TEST_SUCCESS(ITC_Event_join(pt_Event1, pt_Event2, &pt_JoinEvent));

    /* Test the joined event is a (2, 5, 0) event */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_JoinEvent, 6);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Left, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Right, 1);

    /* Destroy the joined event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_JoinEvent));

    /* Test joining the events the other way around */
    TEST_SUCCESS(ITC_Event_join(pt_Event2, pt_Event1, &pt_JoinEvent));

    /* Test the joined event is a (4, 0, 6) event */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_JoinEvent, 6);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Left, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Right, 1);

    /* Destroy the Events */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event1));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event2));
    TEST_SUCCESS(ITC_Event_destroy(&pt_JoinEvent));
}

/* Test joining two complex events */
void ITC_Event_Test_joinTwoComplexEvents(void)
{
    ITC_Event_t *pt_Event1;
    ITC_Event_t *pt_Event2;
    ITC_Event_t *pt_JoinEvent;

    /* clang-format off */
    /* Construct the original Events */
    TEST_SUCCESS(newEvent(&pt_Event1, NULL, 2));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Left, pt_Event1, 4));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Right, pt_Event1, 0));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Right->pt_Left, pt_Event1->pt_Right, 0));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Right->pt_Right, pt_Event1->pt_Right, 1));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Right->pt_Left->pt_Left, pt_Event1->pt_Right->pt_Left, 3));
    TEST_SUCCESS(newEvent(&pt_Event1->pt_Right->pt_Left->pt_Right, pt_Event1->pt_Right->pt_Left, 0));

    TEST_SUCCESS(newEvent(&pt_Event2, NULL, 1));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Left, pt_Event2, 0));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Left->pt_Left, pt_Event2->pt_Left, 3));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Left->pt_Left->pt_Left, pt_Event2->pt_Left->pt_Left, 4));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Left->pt_Left->pt_Right, pt_Event2->pt_Left->pt_Left, 0));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Left->pt_Right, pt_Event2->pt_Left, 0));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Right, pt_Event2, 6));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Right->pt_Left, pt_Event2->pt_Right, 0));
    TEST_SUCCESS(newEvent(&pt_Event2->pt_Right->pt_Right, pt_Event2->pt_Right, 2));
    /* clang-format on */

    /* Test joining the events */
    TEST_SUCCESS(ITC_Event_join(pt_Event1, pt_Event2, &pt_JoinEvent));

    /* Test the joined event is (6, (0, (0, 2, 0), 0), (1, 0, 2)) event */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_JoinEvent, 6);
    TEST_EVENT_IS_PARENT_N_EVENT(pt_JoinEvent->pt_Left, 0);
    TEST_EVENT_IS_PARENT_N_EVENT(pt_JoinEvent->pt_Left->pt_Left, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Left->pt_Left->pt_Left, 2);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Left->pt_Left->pt_Right, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Left->pt_Right, 0);
    TEST_EVENT_IS_PARENT_N_EVENT(pt_JoinEvent->pt_Right, 1);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Right->pt_Left, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Right->pt_Right, 2);

    /* Destroy the joined event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_JoinEvent));

    /* Test joining the events the other way around */
    TEST_SUCCESS(ITC_Event_join(pt_Event2, pt_Event1, &pt_JoinEvent));

    /* Test the joined event is (6, (0, (0, 2, 0), 0), (1, 0, 2)) event */
    TEST_EVENT_IS_PARENT_N_EVENT(pt_JoinEvent, 6);
    TEST_EVENT_IS_PARENT_N_EVENT(pt_JoinEvent->pt_Left, 0);
    TEST_EVENT_IS_PARENT_N_EVENT(pt_JoinEvent->pt_Left->pt_Left, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Left->pt_Left->pt_Left, 2);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Left->pt_Left->pt_Right, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Left->pt_Right, 0);
    TEST_EVENT_IS_PARENT_N_EVENT(pt_JoinEvent->pt_Right, 1);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Right->pt_Left, 0);
    TEST_EVENT_IS_LEAF_N_EVENT(pt_JoinEvent->pt_Right->pt_Right, 2);

    /* Destroy the Events */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event1));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event2));
    TEST_SUCCESS(ITC_Event_destroy(&pt_JoinEvent));
}