/**
 * @file ITC_Event_Test.h
 * @brief Unit tests for the Interval Tree Clock's Event mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#include "ITC_Event.h"
#include "ITC_Event_package.h"
#include "ITC_Event_Test.h"

#include "ITC_Test_package.h"
#include "MockITC_Port.h"

/******************************************************************************
 * Global variables
 ******************************************************************************/

ITC_Event_t gt_LeafNode;

ITC_Event_t gt_RootOfParentEvent;
ITC_Event_t gt_LeftLeafOfParentEvent;
ITC_Event_t gt_RightLeafOfParentEvent;

ITC_Event_t *gpt_ParentEvent;
ITC_Event_t *gpt_LeafEvent;

/******************************************************************************
 *  Public functions
 ******************************************************************************/

/* Init test */
void setUp(void)
{
    /* Setup the parent Event tree as a (0, 0, 1) Event */
    gt_RootOfParentEvent.t_Count = 0;
    gt_LeftLeafOfParentEvent.t_Count = 0;
    gt_RightLeafOfParentEvent.t_Count = 1;

    gt_RootOfParentEvent.pt_Parent = NULL;

    gt_RootOfParentEvent.pt_Left = &gt_LeftLeafOfParentEvent;
    gt_LeftLeafOfParentEvent.pt_Parent = &gt_RootOfParentEvent;
    gt_LeftLeafOfParentEvent.pt_Left = NULL;
    gt_LeftLeafOfParentEvent.pt_Right = NULL;

    gt_RootOfParentEvent.pt_Right = &gt_RightLeafOfParentEvent;
    gt_RightLeafOfParentEvent.pt_Parent = &gt_RootOfParentEvent;
    gt_RightLeafOfParentEvent.pt_Left = NULL;
    gt_RightLeafOfParentEvent.pt_Right = NULL;

    gpt_ParentEvent = &gt_RootOfParentEvent;

    /* Setup the leaf as a new Event tree */
    gt_LeafNode.t_Count = 0;
    gt_LeafNode.pt_Parent = NULL;
    gt_LeafNode.pt_Left = NULL;
    gt_LeafNode.pt_Right = NULL;

    gpt_LeafEvent = &gt_LeafNode;
}

/* Fini test */
void tearDown(void) {}

/* Test destroying an Event calls ITC_Port_free */
void ITC_Event_Test_destroyEventCallsItcPortFree(void)
{
    /* Setup expectations */
    ITC_Port_free_ExpectAndReturn(gpt_ParentEvent->pt_Left, ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(gpt_ParentEvent->pt_Right,ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(gpt_ParentEvent, ITC_STATUS_SUCCESS);

    TEST_SUCCESS(ITC_Event_destroy(&gpt_ParentEvent));
}

/* Test destroying an Event continues even when calls to ITC_Port_free fail */
void ITC_Event_Test_destroyEventConinuesOnError(void)
{
    /* Setup expectations */
    ITC_Port_free_ExpectAndReturn(gpt_ParentEvent->pt_Left, ITC_STATUS_FAILURE);
    ITC_Port_free_ExpectAndReturn(gpt_ParentEvent->pt_Right,ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(gpt_ParentEvent, ITC_STATUS_SUCCESS);

    TEST_FAILURE(ITC_Event_destroy(&gpt_ParentEvent), ITC_STATUS_FAILURE);
}

/* Test creating an Event calls ITC_Port_malloc */
void ITC_Event_Test_createEventCallsItcPortMalloc(void)
{
    ITC_Event_t *pt_NewEvent;

    /* Setup expectations */
    ITC_Port_malloc_ExpectAndReturn(
        NULL, sizeof(ITC_Event_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&gpt_LeafEvent);

    /* Test for new leaf Event */
    TEST_SUCCESS(ITC_Event_new(&pt_NewEvent));
    TEST_ASSERT_EQUAL_PTR(pt_NewEvent, gpt_LeafEvent);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_NewEvent, 0);
}

/* Test failed cloning of an Event is properly cleaned up */
void ITC_Event_Test_clonedEventIsDestroyedOnFailure(void)
{
    ITC_Event_t *pt_ClonedEvent;

    ITC_Event_t t_NewEvent = {0};
    ITC_Event_t *pt_NewEvent = &t_NewEvent;

    /* Setup expectations */
    ITC_Port_malloc_ExpectAndReturn(
        NULL, sizeof(ITC_Event_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&pt_NewEvent);

    ITC_Port_malloc_ExpectAndReturn(
        NULL, sizeof(ITC_Event_t), ITC_STATUS_FAILURE);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();

    ITC_Port_free_ExpectAndReturn(pt_NewEvent, ITC_STATUS_SUCCESS);

    /* Test cloning failure */
    TEST_FAILURE(
        ITC_Event_clone(gpt_ParentEvent, &pt_ClonedEvent), ITC_STATUS_FAILURE);
}

/* Test failed normalisation of an Event is properly recovered from */
void ITC_Event_Test_normaliseEventIsRecoveredOnFailure(void)
{
    /* Setup expectations */
    ITC_Port_free_ExpectAndReturn(
        gpt_ParentEvent->pt_Left, ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(
        gpt_ParentEvent->pt_Right, ITC_STATUS_FAILURE);

    /* Test failing to normalise a (0, 1, 1) Event */
    gpt_ParentEvent->pt_Left->t_Count = 1;
    gpt_ParentEvent->pt_Right->t_Count = 1;
    TEST_FAILURE(ITC_Event_normalise(gpt_ParentEvent), ITC_STATUS_FAILURE);

    /* Test the Event was normalised properly, even though one of the children
     * deallocations failed */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(gpt_ParentEvent, 1);
}

/* Test failed summing of two Events is properly cleaned up */
void ITC_Event_Test_joinedEventIsDestroyedOnFailure(void)
{
    ITC_Event_t rt_NewEvent[3] = { 0 };
    ITC_Event_t *rpt_NewEvent[] = {
        &rt_NewEvent[0],
        &rt_NewEvent[1],
        &rt_NewEvent[2],
    };

    ITC_Event_t rt_CopiedParentEvent[3] = { 0 };
    ITC_Event_t *rpt_CopiedParentEvent[] = {
        &rt_CopiedParentEvent[0],
        &rt_CopiedParentEvent[1],
        &rt_CopiedParentEvent[2],
    };
    ITC_Event_t rt_CopiedLeafEvent[3] = { 0 };
    ITC_Event_t *rpt_CopiedLeafEvent[] = {
        &rt_CopiedLeafEvent[0],
        &rt_CopiedLeafEvent[1],
        &rt_CopiedLeafEvent[2],
    };

    /* Setup expectations */

    /* Expect parent event copy */
    for (uint32_t u32_I = 0;
         u32_I < ARRAY_COUNT(rpt_CopiedParentEvent);
         u32_I++)
    {
        ITC_Port_malloc_ExpectAndReturn(
            NULL, sizeof(ITC_Event_t), ITC_STATUS_SUCCESS);
        ITC_Port_malloc_IgnoreArg_ppv_Ptr();
        ITC_Port_malloc_ReturnThruPtr_ppv_Ptr(
            (void **)&rpt_CopiedParentEvent[u32_I]);

    }

    /* Expect leaf event copy */
    ITC_Port_malloc_ExpectAndReturn(
        NULL, sizeof(ITC_Event_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&rpt_CopiedLeafEvent[0]);

    /* Expect copied event node extension */
    for (uint32_t u32_I = 1;
         u32_I < ARRAY_COUNT(rpt_CopiedLeafEvent);
         u32_I++)
    {
        ITC_Port_malloc_ExpectAndReturn(
            NULL, sizeof(ITC_Event_t), ITC_STATUS_SUCCESS);
        ITC_Port_malloc_IgnoreArg_ppv_Ptr();
        ITC_Port_malloc_ReturnThruPtr_ppv_Ptr(
            (void **)&rpt_CopiedLeafEvent[u32_I]);
    }

    /* Expect joined event allocation */
    for (uint32_t u32_I = 0;
         u32_I < ARRAY_COUNT(rpt_NewEvent);
         u32_I++)
    {
        /* Fail on the last allocation call */
        if (u32_I == ARRAY_COUNT(rpt_NewEvent) - 1)
        {
            ITC_Port_malloc_ExpectAndReturn(
                NULL, sizeof(ITC_Event_t), ITC_STATUS_FAILURE);
        }
        else
        {
            ITC_Port_malloc_ExpectAndReturn(
                NULL, sizeof(ITC_Event_t), ITC_STATUS_SUCCESS);
            ITC_Port_malloc_ReturnThruPtr_ppv_Ptr(
                (void **)&rpt_NewEvent[u32_I]);
        }

        ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    }

    /* Expect copied parent event deallocation */
    ITC_Port_free_ExpectAndReturn(rpt_CopiedParentEvent[1], ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(rpt_CopiedParentEvent[2], ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(rpt_CopiedParentEvent[0], ITC_STATUS_SUCCESS);

    /* Expect copied leaf event deallocation */
    ITC_Port_free_ExpectAndReturn(rpt_CopiedLeafEvent[1], ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(rpt_CopiedLeafEvent[2], ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(rpt_CopiedLeafEvent[0], ITC_STATUS_SUCCESS);

    /* Expect joined event deallocation */
    ITC_Port_free_ExpectAndReturn(rpt_NewEvent[1], ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(rpt_NewEvent[0], ITC_STATUS_SUCCESS);

    /* Test failing to join the Events */
    TEST_FAILURE(
        ITC_Event_join(&gpt_ParentEvent, &gpt_LeafEvent),
        ITC_STATUS_FAILURE);
}

/* Test successful joining of two Events is properly cleaned up */
void ITC_Event_Test_joinOriginalAndCopiedEventsAreDestroyedOnSucess(void)
{
    ITC_Event_t t_OtherEvent = gt_LeafNode;
    ITC_Event_t *pt_OtherEvent = &t_OtherEvent;

    ITC_Event_t t_NewEvent1 = { 0 };
    ITC_Event_t *pt_NewEvent1 = &t_NewEvent1;
    ITC_Event_t t_NewEvent2 = { 0 };
    ITC_Event_t *pt_NewEvent2 = &t_NewEvent2;
    ITC_Event_t t_NewEvent3 = { 0 };
    ITC_Event_t *pt_NewEvent3 = &t_NewEvent3;

    /* Setup expectations */
    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Event_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&pt_NewEvent1);

    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Event_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&pt_NewEvent2);

    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Event_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&pt_NewEvent3);

    ITC_Port_free_ExpectAndReturn(pt_NewEvent1, ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(pt_NewEvent2, ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(gpt_LeafEvent, ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(pt_OtherEvent, ITC_STATUS_SUCCESS);

    /* Test joining the Events */
    TEST_SUCCESS(ITC_Event_join(&gpt_LeafEvent, &pt_OtherEvent));
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(gpt_LeafEvent, 0);
}

/* Test failed maximise an Event is properly recovered from */
void ITC_Event_Test_maximiseEventIsRecoveredOnFailure(void)
{
    /* Setup expectations */
    ITC_Port_free_ExpectAndReturn(
        gpt_ParentEvent->pt_Left, ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(
        gpt_ParentEvent->pt_Right, ITC_STATUS_FAILURE);

    /* Test failing to maximise a (0, 3, 0) Event */
    gpt_ParentEvent->pt_Left->t_Count = 3;
    gpt_ParentEvent->pt_Right->t_Count = 0;
    TEST_FAILURE(ITC_Event_maximise(gpt_ParentEvent), ITC_STATUS_FAILURE);

    /* Test the Event was maximised properly, even though one of the children
     * deallocations failed */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(gpt_ParentEvent, 3);
}

/* Test failed fill of an Event is properly recovered from */
void ITC_Event_Test_fillEventIsRecoveredOnFailure(void)
{
    ITC_Event_t rt_ClonedParentEvent[3] = {0};
    ITC_Event_t *pt_ClonedParentEvent[3] = {
        &rt_ClonedParentEvent[0],
        &rt_ClonedParentEvent[1],
        &rt_ClonedParentEvent[2],
    };

    /* Create the ID to work with */
    ITC_Id_t t_SeedId = {0};

    bool b_WasFilled;

    /* Assign interval ownership */
    t_SeedId.b_IsOwner = true;

    /* Expect the source event to be cloned before the operation starts */
    for (uint32_t u32_I = 0; u32_I < ARRAY_COUNT(rt_ClonedParentEvent); u32_I++)
    {
        ITC_Port_malloc_ExpectAndReturn(
            NULL, sizeof(ITC_Event_t), ITC_STATUS_SUCCESS);
        ITC_Port_malloc_IgnoreArg_ppv_Ptr();
        ITC_Port_malloc_ReturnThruPtr_ppv_Ptr(
            (void **)&pt_ClonedParentEvent[u32_I]);
    }

    /* Setup expectation to fail second deallocation */
    ITC_Port_free_ExpectAndReturn(gpt_ParentEvent->pt_Left, ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(
        gpt_ParentEvent->pt_Right, ITC_STATUS_FAILURE);

    /* The operation failed, the original event is in a corrupted state
     * expect it to be destroyed */
    ITC_Port_free_ExpectAndReturn(gpt_ParentEvent, ITC_STATUS_SUCCESS);

    /* Test failing to fill a (0, 3, 0) Event with a seed ID */
    gpt_ParentEvent->pt_Left->t_Count = 3;
    gpt_ParentEvent->pt_Right->t_Count = 0;
    TEST_FAILURE(
        ITC_Event_fill(&gpt_ParentEvent, &t_SeedId, &b_WasFilled),
        ITC_STATUS_FAILURE);

    /* Test the clone of the original event was returned */
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(gpt_ParentEvent, 0);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(gpt_ParentEvent->pt_Left, 3);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(gpt_ParentEvent->pt_Right, 0);
    TEST_ASSERT_EQUAL_PTR(gpt_ParentEvent, pt_ClonedParentEvent[0]);
    TEST_ASSERT_EQUAL_PTR(gpt_ParentEvent->pt_Left, pt_ClonedParentEvent[1]);
    TEST_ASSERT_EQUAL_PTR(gpt_ParentEvent->pt_Right, pt_ClonedParentEvent[2]);
}

/* Test failed grow of an Event is properly recovered from */
void ITC_Event_Test_growEventIsRecoveredOnFailure(void)
{
    ITC_Event_t rt_ClonedParentEvent[1] = {0};
    ITC_Event_t *pt_ClonedParentEvent[1] = {
        &rt_ClonedParentEvent[0],
    };

    ITC_Event_t t_NewEvent1 = { 0 };
    ITC_Event_t *pt_NewEvent1 = &t_NewEvent1;

    /* Create the ID to work with */
    ITC_Id_t t_ParentId = { 0 };
    ITC_Id_t t_NestedSeedId = { 0 };
    ITC_Id_t t_NestedNullId = { 0 };

    /* Assign interval ownerships */
    t_ParentId.b_IsOwner = false;
    t_NestedSeedId.b_IsOwner = true;
    t_NestedNullId.b_IsOwner = false;

    /* Connect the parent ID tree */
    t_ParentId.pt_Left = &t_NestedSeedId;
    t_ParentId.pt_Right = &t_NestedNullId;
    t_NestedSeedId.pt_Parent = &t_ParentId;
    t_NestedNullId.pt_Parent = &t_ParentId;

    /* Expect the source event to be cloned before the operation starts */
    for (uint32_t u32_I = 0; u32_I < ARRAY_COUNT(rt_ClonedParentEvent); u32_I++)
    {
        ITC_Port_malloc_ExpectAndReturn(
            NULL, sizeof(ITC_Event_t), ITC_STATUS_SUCCESS);
        ITC_Port_malloc_IgnoreArg_ppv_Ptr();
        ITC_Port_malloc_ReturnThruPtr_ppv_Ptr(
            (void **)&pt_ClonedParentEvent[u32_I]);
    }

    /* Setup expectation to fail second allocation */
    ITC_Port_malloc_ExpectAndReturn(
        NULL, sizeof(ITC_Event_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&pt_NewEvent1);
    ITC_Port_malloc_ExpectAndReturn(
        NULL, sizeof(ITC_Event_t), ITC_STATUS_FAILURE);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();

    /* Setup expectation to destroy the first allocation */
    ITC_Port_free_ExpectAndReturn(&t_NewEvent1, ITC_STATUS_SUCCESS);

    /* The operation failed, the original event might be in a corrupted state
     * expect it to be destroyed */
    ITC_Port_free_ExpectAndReturn(gpt_LeafEvent, ITC_STATUS_SUCCESS);

    /* Test failing to grow a (0) Event with a (1, 0) ID */
    TEST_FAILURE(
        ITC_Event_grow(&gpt_LeafEvent, &t_ParentId), ITC_STATUS_FAILURE);

    /* Test the clone of the original event was returned */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(gpt_LeafEvent, 0);
    TEST_ASSERT_EQUAL_PTR(gpt_LeafEvent, pt_ClonedParentEvent[0]);
}
