#include "ITC_Id.h"
#include "ITC_Id_package.h"
#include "ITC_Id_Test.h"

#include "ITC_Test_package.h"
#include "MockITC_Port.h"

/******************************************************************************
 * Global variables
 ******************************************************************************/

ITC_Id_t gt_LeafNode;

ITC_Id_t gt_RootOfParentId;
ITC_Id_t gt_LeftLeafOfParentId;
ITC_Id_t gt_RightLeafOfParentId;

ITC_Id_t *gpt_ParentId;
ITC_Id_t *gpt_LeafId;

/******************************************************************************
 *  Public functions
 ******************************************************************************/

/* Init test */
void setUp(void)
{
    /* Setup the parent ID tree as a (0, 1) ID */
    gt_RootOfParentId.b_IsOwner = false;
    gt_LeftLeafOfParentId.b_IsOwner = false;
    gt_RightLeafOfParentId.b_IsOwner = true;

    gt_RootOfParentId.pt_Parent = NULL;

    gt_RootOfParentId.pt_Left = &gt_LeftLeafOfParentId;
    gt_LeftLeafOfParentId.pt_Parent = &gt_RootOfParentId;
    gt_LeftLeafOfParentId.pt_Left = NULL;
    gt_LeftLeafOfParentId.pt_Right = NULL;

    gt_RootOfParentId.pt_Right = &gt_RightLeafOfParentId;
    gt_RightLeafOfParentId.pt_Parent = &gt_RootOfParentId;
    gt_RightLeafOfParentId.pt_Left = NULL;
    gt_RightLeafOfParentId.pt_Right = NULL;

    gpt_ParentId = &gt_RootOfParentId;

    /* Setup the leaf as a NULL ID */
    gt_LeafNode.b_IsOwner = false;
    gt_LeafNode.pt_Parent = NULL;
    gt_LeafNode.pt_Left = NULL;
    gt_LeafNode.pt_Right = NULL;

    gpt_LeafId = &gt_LeafNode;
}

/* Fini test */
void tearDown(void) {}

/* Test destroying an ID calls ITC_Port_free */
void ITC_Id_Test_destroyIdCallsItcPortFree(void)
{
    /* Setup expectations */
    ITC_Port_free_ExpectAndReturn(gpt_ParentId->pt_Left, ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(gpt_ParentId->pt_Right,ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(gpt_ParentId, ITC_STATUS_SUCCESS);

    TEST_SUCCESS(ITC_Id_destroy(&gpt_ParentId));
}

/* Test destroying an ID continues even when calls to ITC_Port_free fail */
void ITC_Id_Test_destroyIdConinuesOnError(void)
{
    /* Setup expectations */
    ITC_Port_free_ExpectAndReturn(gpt_ParentId->pt_Left, ITC_STATUS_FAILURE);
    ITC_Port_free_ExpectAndReturn(gpt_ParentId->pt_Right, ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(gpt_ParentId, ITC_STATUS_SUCCESS);

    TEST_FAILURE(ITC_Id_destroy(&gpt_ParentId), ITC_STATUS_FAILURE);
}

/* Test creating an ID calls ITC_Port_malloc */
void ITC_Id_Test_createIdCallsItcPortMalloc(void)
{
    ITC_Id_t *pt_NewId;

    /* Setup expectations */
    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&gpt_LeafId);

    /* Test for new null ID */
    TEST_SUCCESS(ITC_Id_newNull(&pt_NewId));
    TEST_ASSERT_EQUAL_PTR(pt_NewId, gpt_LeafId);
    TEST_ITC_ID_IS_NULL_ID(pt_NewId);

    /* Setup expectations */
    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&gpt_LeafId);

    /* Test for new seed ID */
    TEST_SUCCESS(ITC_Id_newSeed(&pt_NewId));
    TEST_ASSERT_EQUAL_PTR(pt_NewId, gpt_LeafId);
    TEST_ITC_ID_IS_SEED_ID(pt_NewId);
}

/* Test failed cloning of an ID is properly cleaned up */
void ITC_Id_Test_clonedIdIsDestroyedOnFailure(void)
{
    ITC_Id_t *pt_ClonedId;

    ITC_Id_t t_NewId = {0};
    ITC_Id_t *pt_NewId = &t_NewId;

    /* Setup expectations */
    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&pt_NewId);

    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_FAILURE);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();

    ITC_Port_free_ExpectAndReturn(pt_NewId, ITC_STATUS_SUCCESS);

    /* Test cloning failure */
    TEST_FAILURE(ITC_Id_clone(gpt_ParentId, &pt_ClonedId), ITC_STATUS_FAILURE);
}

/* Test failed splitting of an ID is properly cleaned up */
void ITC_Id_Test_splitIdsAreDestroyedOnFailure(void)
{
    ITC_Id_t *pt_SplitId1;
    ITC_Id_t *pt_SplitId2;

    ITC_Id_t t_NewId1 = {0};
    ITC_Id_t *pt_NewId1 = &t_NewId1;

    ITC_Id_t t_NewId2 = {0};
    ITC_Id_t *pt_NewId2 = &t_NewId2;

    /* Setup expectations */
    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&pt_NewId1);

    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_FAILURE);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();

    ITC_Port_free_ExpectAndReturn(pt_NewId1, ITC_STATUS_SUCCESS);

    /* Test failing to split a null ID */
    gpt_LeafId->b_IsOwner = false;
    TEST_FAILURE(
        ITC_Id_split(
            gpt_LeafId,
            &pt_SplitId1,
            &pt_SplitId2),
        ITC_STATUS_FAILURE);

    /* Setup expectations */
    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&pt_NewId1);

    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_FAILURE);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();

    ITC_Port_free_ExpectAndReturn(pt_NewId1, ITC_STATUS_SUCCESS);

    /* Test failing to split a seed ID */
    gpt_LeafId->b_IsOwner = true;
    TEST_FAILURE(
        ITC_Id_split(
            gpt_LeafId,
            &pt_SplitId1,
            &pt_SplitId2),
        ITC_STATUS_FAILURE);

    /* Setup expectations */
    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&pt_NewId1);

    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&pt_NewId2);

    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_FAILURE);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();

    ITC_Port_free_ExpectAndReturn(pt_NewId1, ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(pt_NewId2, ITC_STATUS_SUCCESS);

    /* Test failing to split a (0, 1) ID */
    TEST_FAILURE(
        ITC_Id_split(
            gpt_ParentId,
            &pt_SplitId1,
            &pt_SplitId2),
        ITC_STATUS_FAILURE);
}

/* Test failed normalisation of an ID is properly recovered from */
void ITC_Id_Test_normaliseIdIsRecoveredOnFailure(void)
{
    ITC_Id_t t_NewId1 = { 0 };
    ITC_Id_t t_NewId2 = { 0 };
    ITC_Id_t *pt_NewId1 = &t_NewId1;
    ITC_Id_t *pt_NewId2 = &t_NewId2;

    /* Setup expectations */
    ITC_Port_free_ExpectAndReturn(gpt_ParentId->pt_Left, ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(gpt_ParentId->pt_Right, ITC_STATUS_FAILURE);

    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&pt_NewId1);

    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&pt_NewId2);

    /* Test failing to normalise a (1, 1) ID */
    gpt_ParentId->pt_Left->b_IsOwner = true;
    gpt_ParentId->pt_Right->b_IsOwner = true;
    TEST_FAILURE(ITC_Id_normalise(gpt_ParentId), ITC_STATUS_FAILURE);

    /* Test the ID is the same but the children have been restored */
    TEST_ITC_ID_IS_SEED_SEED_ID(gpt_ParentId);
    TEST_ASSERT_EQUAL_PTR(pt_NewId2, gpt_ParentId->pt_Left);
    TEST_ASSERT_EQUAL_PTR(pt_NewId1, gpt_ParentId->pt_Right);
}

/* Test failed summing of two IDs is properly cleaned up */
void ITC_Id_Test_sumIdAreDestroyedOnFailure(void)
{
    ITC_Id_t *pt_SummedId;
    ITC_Id_t t_NewId1 = { 0 };
    ITC_Id_t *pt_NewId1 = &t_NewId1;
    ITC_Id_t t_NewId2 = { 0 };
    ITC_Id_t *pt_NewId2 = &t_NewId2;

    ITC_Id_t t_OtherId = gt_RootOfParentId;
    /* Mirror the global test ID */
    ITC_Id_t t_OtherIdLeftChild = gt_RightLeafOfParentId;
    ITC_Id_t t_OtherIdRightChild = gt_LeftLeafOfParentId;

    /* Fix pointers */
    t_OtherId.pt_Left = &t_OtherIdLeftChild;
    t_OtherId.pt_Right = &t_OtherIdRightChild;
    t_OtherIdLeftChild.pt_Parent = &t_OtherId;
    t_OtherIdRightChild.pt_Parent = &t_OtherId;

    /* Setup expectations */
    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&pt_NewId1);

    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&pt_NewId2);

    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_FAILURE);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();

    ITC_Port_free_ExpectAndReturn(pt_NewId2, ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(pt_NewId1, ITC_STATUS_SUCCESS);

    /* Test summing the IDs */
    TEST_FAILURE(
        ITC_Id_sum(
            gpt_ParentId,
            &t_OtherId,
            &pt_SummedId),
        ITC_STATUS_FAILURE);

    /* Setup expectations */
    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&pt_NewId1);

    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_SUCCESS);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();
    ITC_Port_malloc_ReturnThruPtr_ppv_Ptr((void **)&pt_NewId2);

    ITC_Port_malloc_ExpectAndReturn(NULL, sizeof(ITC_Id_t), ITC_STATUS_FAILURE);
    ITC_Port_malloc_IgnoreArg_ppv_Ptr();

    ITC_Port_free_ExpectAndReturn(pt_NewId2, ITC_STATUS_SUCCESS);
    ITC_Port_free_ExpectAndReturn(pt_NewId1, ITC_STATUS_SUCCESS);

    /* Test summing the IDs the other way around */
    TEST_FAILURE(
        ITC_Id_sum(
            &t_OtherId,
            gpt_ParentId,
            &pt_SummedId),
        ITC_STATUS_FAILURE);
}
