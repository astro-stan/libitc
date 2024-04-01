#include "unity.h"

#include "ITC_Id.h"
#include "ITC_Id_Test.h"

#include "ITC_Test_package.h"

#include <stdio.h>
#include <inttypes.h>

/******************************************************************************
 *  Global variables
 ******************************************************************************/

/**
 * @brief Same as ITC_Id_newNull but enforces setting the parent
 *
 * @param ppt_Id (out) The pointer to the NULL ID
 * @param pt_Parent The pointer to the parent ID. Otherwise NULL
 */
static ITC_Status_t newNull(
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

/**
 * @brief Same as ITC_Id_newSeed but enforces setting the parent
 *
 * @param ppt_Id (out) The pointer to the seed ID
 * @param pt_Parent The pointer to the parent ID. Otherwise NULL
 */
static ITC_Status_t newSeed(
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

/* Init test */
void setUp(void) {}

/* Fini test */
void tearDown(void) {}

/* Test destroying an ID fails with invalid param */
void ITC_Id_Test_destroyIdFailInvalidParam(void)
{
    TEST_FAILURE(ITC_Id_destroy(NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test creating a NULL ID fails with invalid param */
void ITC_Id_Test_createNullIdFailInvalidParam(void)
{
    TEST_FAILURE(ITC_Id_newNull(NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test creating a NULL ID succeeds */
void ITC_Id_Test_createNullIdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new NULL ID */
    TEST_SUCCESS(newNull(&pt_Id, NULL));

    /* Test this is a NULL ID */
    TEST_IS_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test creating a seed ID fails with invalid param */
void ITC_Id_Test_createSeedIdFailInvalidParam(void)
{
    TEST_FAILURE(ITC_Id_newSeed(NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test creating a Seed ID succeeds */
void ITC_Id_Test_createSeedIdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new seed ID */
    TEST_SUCCESS(newSeed(&pt_Id, NULL));

    /* Test this is a seed ID */
    TEST_IS_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test cloning an ID fails with invalid param */
void ITC_Id_Test_cloneIdFailInvalidParam(void)
{
  ITC_Id_t *pt_DummyId = NULL;

  TEST_FAILURE(ITC_Id_clone(NULL, &pt_DummyId), ITC_STATUS_INVALID_PARAM);
  TEST_FAILURE(ITC_Id_clone(pt_DummyId, NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test cloning an ID succeeds */
void ITC_Id_Test_cloneIdSuccessful(void)
{
    ITC_Id_t *pt_OriginalId = NULL;
    ITC_Id_t *pt_ClonedId = NULL;

    /* Test cloning seed ID */
    TEST_SUCCESS(newSeed(&pt_OriginalId, NULL));
    TEST_SUCCESS(ITC_Id_clone(pt_OriginalId, &pt_ClonedId));
    TEST_ASSERT_TRUE(pt_OriginalId != pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));

    TEST_ASSERT_FALSE(pt_ClonedId->pt_Parent);
    TEST_IS_SEED_ID(pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_ClonedId));

    /* Test cloning null ID */
    TEST_SUCCESS(newNull(&pt_OriginalId, NULL));
    TEST_SUCCESS(ITC_Id_clone(pt_OriginalId, &pt_ClonedId));
    TEST_ASSERT_TRUE(pt_OriginalId != pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));

    TEST_ASSERT_FALSE(pt_ClonedId->pt_Parent);
    TEST_IS_NULL_ID(pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_ClonedId));

    /* Test cloning a complex ID */
    TEST_SUCCESS(newNull(&pt_OriginalId, NULL));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(newSeed(&pt_OriginalId->pt_Right, pt_OriginalId));
    TEST_SUCCESS(ITC_Id_clone(pt_OriginalId, &pt_ClonedId));
    TEST_ASSERT_TRUE(pt_OriginalId != pt_ClonedId);
    TEST_IS_NOT_LEAF_ID(pt_ClonedId);
    TEST_ASSERT_TRUE(pt_OriginalId->pt_Left != pt_ClonedId->pt_Left);
    TEST_ASSERT_TRUE(pt_OriginalId->pt_Right != pt_ClonedId->pt_Right);
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));

    TEST_ASSERT_FALSE(pt_ClonedId->pt_Parent);
    TEST_IS_NULL_ID(pt_ClonedId->pt_Left);
    TEST_ASSERT_TRUE(pt_ClonedId->pt_Left->pt_Parent == pt_ClonedId);
    TEST_IS_SEED_ID(pt_ClonedId->pt_Right);
    TEST_ASSERT_TRUE(pt_ClonedId->pt_Right->pt_Parent == pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_ClonedId));
}

/* Test cloning a subtree of an ID succeeds */
void ITC_Id_Test_cloneIdSubtreeSuccessful(void)
{
    ITC_Id_t *pt_OriginalId = NULL;
    ITC_Id_t *pt_ClonedId = NULL;

    /* Test cloning seed subree ID */
    TEST_SUCCESS(newNull(&pt_OriginalId, NULL));
    TEST_SUCCESS(newSeed(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Right, pt_OriginalId));
    TEST_SUCCESS(ITC_Id_clone(pt_OriginalId->pt_Left, &pt_ClonedId));
    TEST_ASSERT_TRUE(pt_OriginalId->pt_Left != pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));

    TEST_ASSERT_FALSE(pt_ClonedId->pt_Parent);
    TEST_IS_SEED_ID(pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_ClonedId));

    /* Test cloning null subree ID */
    TEST_SUCCESS(newNull(&pt_OriginalId, NULL));
    TEST_SUCCESS(newSeed(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Right, pt_OriginalId));
    TEST_SUCCESS(ITC_Id_clone(pt_OriginalId->pt_Right, &pt_ClonedId));
    TEST_ASSERT_TRUE(pt_OriginalId->pt_Right != pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));

    TEST_ASSERT_FALSE(pt_ClonedId->pt_Parent);
    TEST_IS_NULL_ID(pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_ClonedId));

    /* Test cloning a complex ID subree */
    TEST_SUCCESS(newNull(&pt_OriginalId, NULL));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left->pt_Left, pt_OriginalId->pt_Left));
    TEST_SUCCESS(newSeed(&pt_OriginalId->pt_Left->pt_Right, pt_OriginalId->pt_Left));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Right, pt_OriginalId));
    TEST_SUCCESS(ITC_Id_clone(pt_OriginalId->pt_Left, &pt_ClonedId));
    TEST_ASSERT_TRUE(pt_OriginalId->pt_Left != pt_ClonedId);
    TEST_IS_NOT_LEAF_ID(pt_ClonedId);
    TEST_ASSERT_TRUE(pt_OriginalId->pt_Left->pt_Left != pt_ClonedId->pt_Left);
    TEST_ASSERT_TRUE(pt_OriginalId->pt_Left->pt_Right != pt_ClonedId->pt_Right);
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));

    TEST_ASSERT_FALSE(pt_ClonedId->pt_Parent);
    TEST_IS_NULL_ID(pt_ClonedId->pt_Left);
    TEST_ASSERT_TRUE(pt_ClonedId->pt_Left->pt_Parent == pt_ClonedId);
    TEST_IS_SEED_ID(pt_ClonedId->pt_Right);
    TEST_ASSERT_TRUE(pt_ClonedId->pt_Right->pt_Parent == pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_ClonedId));
}

/* Test spliting an ID fails with invalid param */
void ITC_Id_Test_splitIdFailInvalidParam(void)
{
  ITC_Id_t *pt_DummyId = NULL;

  TEST_FAILURE(
    ITC_Id_split(NULL, &pt_DummyId, &pt_DummyId), ITC_STATUS_INVALID_PARAM);
  TEST_FAILURE(
    ITC_Id_split(pt_DummyId, NULL, &pt_DummyId), ITC_STATUS_INVALID_PARAM);
  TEST_FAILURE(
    ITC_Id_split(pt_DummyId, &pt_DummyId, NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test splitting a NULL and seed IDs succeeds */
void ITC_Id_Test_splitNullAndSeedIdsSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* Create a new NULL ID */
    TEST_SUCCESS(newNull(&pt_OriginalId, NULL));

    /* Split the NULL ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriginalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (0, 0) */
    TEST_IS_NULL_ID(pt_SplitId1);
    TEST_IS_NULL_ID(pt_SplitId2);

    /* Test the original is still a NULL ID */
    TEST_IS_NULL_ID(pt_OriginalId);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));

    /* Change the ID into a seed ID */
    pt_OriginalId->b_IsOwner = true;

    /* Split the seed ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriginalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match ((1, 0), (0, 1)) */
    TEST_IS_SEED_NULL_ID(pt_SplitId1);
    TEST_IS_NULL_SEED_ID(pt_SplitId2);

    /* Test the original is still a seed ID */
    TEST_IS_SEED_ID(pt_OriginalId);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a NULL and seed ID subtrees succeeds */
void ITC_Id_Test_splitNullAndSeedIdSubtreesSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* Create a new NULL ID */
    TEST_SUCCESS(newNull(&pt_OriginalId, NULL));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(newSeed(&pt_OriginalId->pt_Right, pt_OriginalId));

    /* Split the NULL ID */
    TEST_SUCCESS(ITC_Id_split(
        pt_OriginalId->pt_Left, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (0, 0) */
    TEST_IS_NULL_ID(pt_SplitId1);
    TEST_IS_NULL_ID(pt_SplitId2);

    /* Test the original is still a NULL ID */
    TEST_IS_NULL_ID(pt_OriginalId->pt_Left);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));

    /* Split the seed ID */
    TEST_SUCCESS(ITC_Id_split(
        pt_OriginalId->pt_Right, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match ((1, 0), (0, 1)) */
    TEST_IS_SEED_NULL_ID(pt_SplitId1);
    TEST_IS_NULL_SEED_ID(pt_SplitId2);

    /* Test the original is still a seed ID */
    TEST_IS_SEED_ID(pt_OriginalId->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a (0, 1) and (1, 0) ID succeeds */
void ITC_Id_Test_split01And10IdsSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* Create a new (0, 1) ID */
    TEST_SUCCESS(newNull(&pt_OriginalId, NULL));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(newSeed(&pt_OriginalId->pt_Right, pt_OriginalId));

    /* Split the (0, 1) ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriginalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match ((0, (1, 0)), (0, (0, 1))) */
    TEST_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_IS_NULL_ID(pt_SplitId1->pt_Left);
    TEST_IS_SEED_NULL_ID(pt_SplitId1->pt_Right);
    TEST_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_IS_NULL_ID(pt_SplitId2->pt_Left);
    TEST_IS_NULL_SEED_ID(pt_SplitId2->pt_Right);

    /* Test the original is still a (0, 1) ID */
    TEST_IS_NULL_ID(pt_OriginalId->pt_Left);
    TEST_IS_SEED_ID(pt_OriginalId->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));

    /* Change the ID into (1, 0) ID */
    pt_OriginalId->pt_Left->b_IsOwner = true;
    pt_OriginalId->pt_Right->b_IsOwner = false;

    /* Split the (1, 0) ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriginalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (((1, 0), 0), ((0, 1), 0)) */
    TEST_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_IS_SEED_NULL_ID(pt_SplitId1->pt_Left);
    TEST_IS_NULL_ID(pt_SplitId1->pt_Right);
    TEST_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_IS_NULL_SEED_ID(pt_SplitId2->pt_Left);
    TEST_IS_NULL_ID(pt_SplitId2->pt_Right);

    /* Test the original is still a (1, 0) ID */
    TEST_IS_SEED_ID(pt_OriginalId->pt_Left);
    TEST_IS_NULL_ID(pt_OriginalId->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a (0, 1) and (1, 0) ID subtrees succeeds */
void ITC_Id_Test_split01And10IdSubtreesSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* clang-format off */
    /* Create a new (0, 1) ID */
    TEST_SUCCESS(newNull(&pt_OriginalId, NULL));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Right, pt_OriginalId));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Right->pt_Left, pt_OriginalId->pt_Right));
    TEST_SUCCESS(newSeed(&pt_OriginalId->pt_Right->pt_Right, pt_OriginalId->pt_Right));
    /* clang-format on */

    /* Split the (0, 1) ID */
    TEST_SUCCESS(
        ITC_Id_split(pt_OriginalId->pt_Right, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match ((0, (1, 0)), (0, (0, 1))) */
    TEST_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_IS_NULL_ID(pt_SplitId1->pt_Left);
    TEST_IS_SEED_NULL_ID(pt_SplitId1->pt_Right);
    TEST_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_IS_NULL_ID(pt_SplitId2->pt_Left);
    TEST_IS_NULL_SEED_ID(pt_SplitId2->pt_Right);

    /* Test the original is still a (0, 1) ID */
    TEST_IS_NULL_ID(pt_OriginalId->pt_Right->pt_Left);
    TEST_IS_SEED_ID(pt_OriginalId->pt_Right->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));

    /* Change the ID into (1, 0) ID */
    pt_OriginalId->pt_Right->pt_Left->b_IsOwner = true;
    pt_OriginalId->pt_Right->pt_Right->b_IsOwner = false;

    /* Split the (1, 0) ID */
    TEST_SUCCESS(
        ITC_Id_split(pt_OriginalId->pt_Right, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (((1, 0), 0), ((0, 1), 0)) */
    TEST_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_IS_SEED_NULL_ID(pt_SplitId1->pt_Left);
    TEST_IS_NULL_ID(pt_SplitId1->pt_Right);
    TEST_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_IS_NULL_SEED_ID(pt_SplitId2->pt_Left);
    TEST_IS_NULL_ID(pt_SplitId2->pt_Right);

    /* Test the original is still a (1, 0) ID */
    TEST_IS_SEED_ID(pt_OriginalId->pt_Right->pt_Left);
    TEST_IS_NULL_ID(pt_OriginalId->pt_Right->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a (0, (1, 0)) ID succeeds */
void ITC_Id_Test_split010RIdSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* clang-format off */
    /* Create a new (0, (1, 0)) ID */
    TEST_SUCCESS(newNull(&pt_OriginalId, NULL));

    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left, pt_OriginalId));

    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Right, pt_OriginalId));
    TEST_SUCCESS(newSeed(&pt_OriginalId->pt_Right->pt_Left, pt_OriginalId->pt_Right));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Right->pt_Right, pt_OriginalId->pt_Right));
    /* clang-format on */

    /* Split the (0, (1, 0)) ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriginalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match ((0, ((1, 0), 0)), (0, ((0, 1), 0))) */
    TEST_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_IS_NULL_ID(pt_SplitId1->pt_Left);
    TEST_IS_NOT_LEAF_ID(pt_SplitId1->pt_Right);
    TEST_IS_SEED_NULL_ID(pt_SplitId1->pt_Right->pt_Left);
    TEST_IS_NULL_ID(pt_SplitId1->pt_Right->pt_Right);

    TEST_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_IS_NULL_ID(pt_SplitId2->pt_Left);
    TEST_IS_NOT_LEAF_ID(pt_SplitId2->pt_Right);
    TEST_IS_NULL_SEED_ID(pt_SplitId2->pt_Right->pt_Left);
    TEST_IS_NULL_ID(pt_SplitId2->pt_Right->pt_Right);

    /* Test the original is still a (0, (1, 0)) ID */
    TEST_IS_NULL_ID(pt_OriginalId->pt_Left);
    TEST_IS_SEED_NULL_ID(pt_OriginalId->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a ((0, 1), 0) ID succeeds */
void ITC_Id_Test_split010LIdSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* clang-format off */
    /* Create a new ((0, 1), 0) ID */
    TEST_SUCCESS(newNull(&pt_OriginalId, NULL));

    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left->pt_Left, pt_OriginalId->pt_Left));
    TEST_SUCCESS(newSeed(&pt_OriginalId->pt_Left->pt_Right, pt_OriginalId->pt_Left));

    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Right, pt_OriginalId));
    /* clang-format on */

    /* Split the ((0, 1), 0) ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriginalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (((0, (1, 0)), 0), ((0, (0, 1)), 0)) */
    TEST_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_IS_NOT_LEAF_ID(pt_SplitId1->pt_Left);
    TEST_IS_NULL_ID(pt_SplitId1->pt_Left->pt_Left);
    TEST_IS_SEED_NULL_ID(pt_SplitId1->pt_Left->pt_Right);
    TEST_IS_NULL_ID(pt_SplitId1->pt_Right);

    TEST_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_IS_NOT_LEAF_ID(pt_SplitId2->pt_Left);
    TEST_IS_NULL_ID(pt_SplitId2->pt_Left->pt_Left);
    TEST_IS_NULL_SEED_ID(pt_SplitId2->pt_Left->pt_Right);
    TEST_IS_NULL_ID(pt_SplitId2->pt_Right);

    /* Test the original is still a ((0, 1), 0) ID */
    TEST_IS_NULL_SEED_ID(pt_OriginalId->pt_Left);
    TEST_IS_NULL_ID(pt_OriginalId->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a ((1, 0), (0, 1)) ID succeeds */
void ITC_Id_Test_split1001IdSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* clang-format off */
    /* Create a new ((1, 0), (0, 1)) ID */
    TEST_SUCCESS(newNull(&pt_OriginalId, NULL));

    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(newSeed(&pt_OriginalId->pt_Left->pt_Left, pt_OriginalId->pt_Left));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left->pt_Right, pt_OriginalId->pt_Left));

    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Right, pt_OriginalId));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Right->pt_Left, pt_OriginalId->pt_Right));
    TEST_SUCCESS(newSeed(&pt_OriginalId->pt_Right->pt_Right, pt_OriginalId->pt_Right));
    /* clang-format on */

    /* Split the ((1, 0), (0, 1)) ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriginalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (((1, 0), 0), (0, (0, 1))) */
    TEST_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_IS_SEED_NULL_ID(pt_SplitId1->pt_Left);
    TEST_IS_NULL_ID(pt_SplitId1->pt_Right);
    TEST_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_IS_NULL_ID(pt_SplitId2->pt_Left);
    TEST_IS_NULL_SEED_ID(pt_SplitId2->pt_Right);

    /* Test the original is still a ((1, 0), (0, 1)) ID */
    TEST_IS_SEED_NULL_ID(pt_OriginalId->pt_Left);
    TEST_IS_NULL_SEED_ID(pt_OriginalId->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a ((1, 0), (0, 1)) ID subtree succeeds */
void ITC_Id_Test_split1001IdSubtreeSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* clang-format off */
    /* Create a new ((1, 0), (0, 1)) ID */
    TEST_SUCCESS(newNull(&pt_OriginalId, NULL));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Right, pt_OriginalId));

    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left->pt_Left, pt_OriginalId->pt_Left));
    TEST_SUCCESS(newSeed(&pt_OriginalId->pt_Left->pt_Left->pt_Left, pt_OriginalId->pt_Left->pt_Left));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left->pt_Left->pt_Right, pt_OriginalId->pt_Left->pt_Left));

    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left->pt_Right, pt_OriginalId->pt_Left));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left->pt_Right->pt_Left, pt_OriginalId->pt_Left->pt_Right));
    TEST_SUCCESS(newSeed(&pt_OriginalId->pt_Left->pt_Right->pt_Right, pt_OriginalId->pt_Left->pt_Right));
    /* clang-format on */

    /* Split the ((1, 0), (0, 1)) ID */
    TEST_SUCCESS(
        ITC_Id_split(pt_OriginalId->pt_Left, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (((1, 0), 0), (0, (0, 1))) */
    TEST_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_IS_SEED_NULL_ID(pt_SplitId1->pt_Left);
    TEST_IS_NULL_ID(pt_SplitId1->pt_Right);
    TEST_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_IS_NULL_ID(pt_SplitId2->pt_Left);
    TEST_IS_NULL_SEED_ID(pt_SplitId2->pt_Right);

    /* Test the original is still a ((1, 0), (0, 1)) ID */
    TEST_IS_SEED_NULL_ID(pt_OriginalId->pt_Left->pt_Left);
    TEST_IS_NULL_SEED_ID(pt_OriginalId->pt_Left->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a ((0, (1, 0)), ((0, 1), 0)) ID succeeds */
void ITC_Id_Test_split010010IdSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* clang-format off */
    /* Create a new ((0, (1, 0)), ((0, 1), 0)) ID */
    TEST_SUCCESS(newNull(&pt_OriginalId, NULL));

    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left->pt_Left, pt_OriginalId->pt_Left));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left->pt_Right, pt_OriginalId->pt_Left));
    TEST_SUCCESS(newSeed(&pt_OriginalId->pt_Left->pt_Right->pt_Left, pt_OriginalId->pt_Left->pt_Right));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Left->pt_Right->pt_Right, pt_OriginalId->pt_Left->pt_Right));

    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Right, pt_OriginalId));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Right->pt_Left, pt_OriginalId->pt_Right));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Right->pt_Left->pt_Left, pt_OriginalId->pt_Right->pt_Left));
    TEST_SUCCESS(newSeed(&pt_OriginalId->pt_Right->pt_Left->pt_Right, pt_OriginalId->pt_Right->pt_Left));
    TEST_SUCCESS(newNull(&pt_OriginalId->pt_Right->pt_Right, pt_OriginalId->pt_Right));
    /* clang-format on */

    /* Split the ((0, (1, 0)), ((0, 1), 0)) ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriginalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (((0, (1, 0)), 0), (0, ((0, 1), 0))) */
    TEST_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_IS_NOT_LEAF_ID(pt_SplitId1->pt_Left);
    TEST_IS_NULL_ID(pt_SplitId1->pt_Left->pt_Left);
    TEST_IS_SEED_NULL_ID(pt_SplitId1->pt_Left->pt_Right);
    TEST_IS_NULL_ID(pt_SplitId1->pt_Right);

    TEST_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_IS_NULL_ID(pt_SplitId2->pt_Left);
    TEST_IS_NOT_LEAF_ID(pt_SplitId2->pt_Right);
    TEST_IS_NULL_SEED_ID(pt_SplitId2->pt_Right->pt_Left);
    TEST_IS_NULL_ID(pt_SplitId2->pt_Right->pt_Right);

    /* Test the original is still a ((0, (1, 0)), ((0, 1), 0)) ID */
    TEST_IS_NULL_ID(pt_OriginalId->pt_Left->pt_Left);
    TEST_IS_SEED_NULL_ID(pt_OriginalId->pt_Left->pt_Right);
    TEST_IS_NULL_SEED_ID(pt_OriginalId->pt_Right->pt_Left);
    TEST_IS_NULL_ID(pt_OriginalId->pt_Right->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test normalising an ID fails with invalid param */
void ITC_Id_Test_normaliseIdFailInvalidParam(void)
{
    TEST_FAILURE(ITC_Id_normalise(NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test normalising NULL and seed IDs succeeds */
void ITC_Id_Test_normaliseNullAndSeedIdsSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new NULL ID */
    TEST_SUCCESS(newNull(&pt_Id, NULL));

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test this is still a NULL ID */
    TEST_IS_NULL_ID(pt_Id);

    /* Change ID into a seed ID */
    pt_Id->b_IsOwner = true;

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test this is still a seed ID */
    TEST_IS_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising NULL and seed ID subtrees succeeds */
void ITC_Id_Test_normaliseNullAndSeedIdSubtreesSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new NULL and seed ID subtrees */
    TEST_SUCCESS(newNull(&pt_Id, NULL));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right, pt_Id));

    /* Normalise the NULL ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id->pt_Right));

    /* Test the whole ID hasn't changed */
    TEST_IS_SEED_NULL_ID(pt_Id);

    /* Normalise the seed ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id->pt_Left));

    /* Test the whole ID hasn't changed */
    TEST_IS_SEED_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (1, 0) and (0, 1) IDs succeeds */
void ITC_Id_Test_normalise10And01IdsSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new (1, 0) ID */
    TEST_SUCCESS(newNull(&pt_Id, NULL));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right, pt_Id));

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test this is still a (1, 0) ID */
    TEST_IS_SEED_NULL_ID(pt_Id);

    /* Switch the (1, 0) ID into a (0, 1) ID */
    pt_Id->pt_Left->b_IsOwner = false;
    pt_Id->pt_Right->b_IsOwner = true;

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test this is still a (1, 0) ID */
    TEST_IS_NULL_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (1, 0) and (0, 1) ID subtrees succeeds */
void ITC_Id_Test_normalise10And01IdSubtreesSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new (1, 0) ID subree */
    TEST_SUCCESS(newNull(&pt_Id, NULL));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Left->pt_Left, pt_Id->pt_Left));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left->pt_Right, pt_Id->pt_Left));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right, pt_Id));

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id->pt_Left));

    /* Test the whole ID hasn't changed */
    TEST_IS_NOT_LEAF_ID(pt_Id);
    TEST_IS_SEED_NULL_ID(pt_Id->pt_Left);
    TEST_IS_NULL_ID(pt_Id->pt_Right);

    /* Switch the (1, 0) ID into a (0, 1) ID */
    pt_Id->pt_Left->pt_Left->b_IsOwner = false;
    pt_Id->pt_Left->pt_Right->b_IsOwner = true;

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id->pt_Left));

    /* Test the whole ID hasn't changed */
    TEST_IS_NOT_LEAF_ID(pt_Id);
    TEST_IS_NULL_SEED_ID(pt_Id->pt_Left);
    TEST_IS_NULL_ID(pt_Id->pt_Right);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (1, 1) and (0, 0) IDs succeeds */
void ITC_Id_Test_normalise11And00IdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new (1, 1) ID */
    TEST_SUCCESS(newNull(&pt_Id, NULL));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Right, pt_Id));

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a seed ID */
    TEST_IS_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* Create a new (0, 0) ID */
    TEST_SUCCESS(newNull(&pt_Id, NULL));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right, pt_Id));

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a NULL ID */
    TEST_IS_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (1, 1) and (0, 0) ID subtrees succeeds */
void ITC_Id_Test_normalise11And00IdSubtreesSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new (1, 1) ID subtree (whole tree (1, (1, 1)) ) */
    TEST_SUCCESS(newNull(&pt_Id, NULL));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right, pt_Id));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Right->pt_Left, pt_Id->pt_Right));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Right->pt_Right, pt_Id->pt_Right));

    /* Normalise the ID subtree */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id->pt_Right));

    /* Test the targeted subtree has been normalised but the rest of the tree
     * is untouched */
    TEST_IS_SEED_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* Create a new (0, 0) ID subtree (whole tree ((0, 0), 0) ) */
    TEST_SUCCESS(newNull(&pt_Id, NULL));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left->pt_Left, pt_Id->pt_Left));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left->pt_Right, pt_Id->pt_Left));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right, pt_Id));

    /* Normalise the ID subtree */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id->pt_Left));

    /* Test the targeted subtree has been normalised but the rest of the tree
     * is untouched */
    TEST_IS_NULL_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (0, (1, 1)) and ((1, 1), 0) and IDs succeeds */
void ITC_Id_Test_normalise011And110IdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new (0, (1, 1)) ID */
    TEST_SUCCESS(newNull(&pt_Id, NULL));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right, pt_Id));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Right->pt_Left, pt_Id->pt_Right));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Right->pt_Right, pt_Id->pt_Right));

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a (0, 1) ID */
    TEST_IS_NULL_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* Create a new ((1, 1), 0) ID */
    TEST_SUCCESS(newNull(&pt_Id, NULL));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Left->pt_Left, pt_Id->pt_Left));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Left->pt_Right, pt_Id->pt_Left));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right, pt_Id));

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a (1, 0) ID */
    TEST_IS_SEED_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (1, (1, 1)) and ((1, 1), 1) and IDs succeeds */
void ITC_Id_Test_normalise111And111IdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new (1, (1, 1)) ID */
    TEST_SUCCESS(newNull(&pt_Id, NULL));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right, pt_Id));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Right->pt_Left, pt_Id->pt_Right));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Right->pt_Right, pt_Id->pt_Right));

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a seed ID */
    TEST_IS_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* Create a new ((1, 1), 1) ID */
    TEST_SUCCESS(newNull(&pt_Id, NULL));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Left->pt_Left, pt_Id->pt_Left));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Left->pt_Right, pt_Id->pt_Left));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Right, pt_Id));

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a seed ID */
    TEST_IS_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (1, (0, 0)) and ((0, 0), 1) and IDs succeeds */
void ITC_Id_Test_normalise100And001IdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new (1, (0, 0)) ID */
    TEST_SUCCESS(newNull(&pt_Id, NULL));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right, pt_Id));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right->pt_Left, pt_Id->pt_Right));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right->pt_Right, pt_Id->pt_Right));

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a (1, 0) ID */
    TEST_IS_SEED_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* Create a new ((0, 0), 1) ID */
    TEST_SUCCESS(newNull(&pt_Id, NULL));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left->pt_Left, pt_Id->pt_Left));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left->pt_Right, pt_Id->pt_Left));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Right, pt_Id));

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a (0, 1) ID */
    TEST_IS_NULL_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (0, (0, 0)) and ((0, 0), 0) and IDs succeeds */
void ITC_Id_Test_normalise000And000IdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new (0, (0, 0)) ID */
    TEST_SUCCESS(newNull(&pt_Id, NULL));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right, pt_Id));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right->pt_Left, pt_Id->pt_Right));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right->pt_Right, pt_Id->pt_Right));

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a NULL ID */
    TEST_IS_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* Create a new ((0, 0), 0) ID */
    TEST_SUCCESS(newNull(&pt_Id, NULL));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left->pt_Left, pt_Id->pt_Left));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left->pt_Right, pt_Id->pt_Left));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right, pt_Id));

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a NULL ID */
    TEST_IS_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (((1, 1), 1), (1, 1)) ID succeeds */
void ITC_Id_Test_normalise11111IdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* clang-format off */
    /* Create a new (((1, 1), 1), (1, 1)) ID */
    TEST_SUCCESS(newNull(&pt_Id, NULL));

    TEST_SUCCESS(newNull(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left->pt_Left, pt_Id->pt_Left));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Left->pt_Left->pt_Left, pt_Id->pt_Left->pt_Left));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Left->pt_Left->pt_Right, pt_Id->pt_Left->pt_Left));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Left->pt_Right, pt_Id->pt_Left));

    TEST_SUCCESS(newNull(&pt_Id->pt_Right, pt_Id));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Right->pt_Left, pt_Id->pt_Right));
    TEST_SUCCESS(newSeed(&pt_Id->pt_Right->pt_Right, pt_Id->pt_Right));
    /* clang-format on */

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a seed ID */
    TEST_IS_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a ((0, 0), ((0, 0), 0)) ID succeeds */
void ITC_Id_Test_normalise00000IdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* clang-format off */
    /* Create a new ((0, 0), ((0, 0), 0)) ID */
    TEST_SUCCESS(newNull(&pt_Id, NULL));

    TEST_SUCCESS(newNull(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left->pt_Left, pt_Id->pt_Left));
    TEST_SUCCESS(newNull(&pt_Id->pt_Left->pt_Right, pt_Id->pt_Left));

    TEST_SUCCESS(newNull(&pt_Id->pt_Right, pt_Id));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right->pt_Left, pt_Id->pt_Right));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right->pt_Left->pt_Left, pt_Id->pt_Right->pt_Left));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right->pt_Left->pt_Right, pt_Id->pt_Right->pt_Left));
    TEST_SUCCESS(newNull(&pt_Id->pt_Right->pt_Right, pt_Id->pt_Right));
    /* clang-format on */

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a seed ID */
    TEST_IS_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}