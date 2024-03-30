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

/* Test splitting a Seed ID succeeds */
void ITC_Id_Test_splitSeedIdSuccessful(void)
{
    ITC_Id_t *pt_OriignalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* Create a new seed ID */
    TEST_SUCCESS(newSeed(&pt_OriignalId, NULL));

    /* Split the seed ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriignalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match ((1, 0), (0, 1)) */
    TEST_IS_SEED_NULL_ID(pt_SplitId1);
    TEST_IS_NULL_SEED_ID(pt_SplitId2);

    /* Test the original is still a seed ID */
    TEST_IS_SEED_ID(pt_OriignalId);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriignalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a NULL ID succeeds */
void ITC_Id_Test_splitNullIdSuccessful(void)
{
    ITC_Id_t *pt_OriignalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* Create a new NULL ID */
    TEST_SUCCESS(newNull(&pt_OriignalId, NULL));

    /* Split the NULL ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriignalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (0, 0) */
    TEST_IS_NULL_ID(pt_SplitId1);
    TEST_IS_NULL_ID(pt_SplitId2);

    /* Test the original is still a NULL ID */
    TEST_IS_NULL_ID(pt_OriignalId);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriignalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a (0, 1) ID succeeds */
void ITC_Id_Test_split01IdSuccessful(void)
{
    ITC_Id_t *pt_OriignalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* Create a new (0, 1) ID */
    TEST_SUCCESS(newNull(&pt_OriignalId, NULL));
    TEST_SUCCESS(newNull(&pt_OriignalId->pt_Left, pt_OriignalId));
    TEST_SUCCESS(newSeed(&pt_OriignalId->pt_Right, pt_OriignalId));

    /* Split the (0, 1) ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriignalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match ((0, (1, 0)), (0, (0, 1))) */
    TEST_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_IS_NULL_ID(pt_SplitId1->pt_Left);
    TEST_IS_SEED_NULL_ID(pt_SplitId1->pt_Right);
    TEST_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_IS_NULL_ID(pt_SplitId2->pt_Left);
    TEST_IS_NULL_SEED_ID(pt_SplitId2->pt_Right);

    /* Test the original is still a (0, 1) ID */
    TEST_IS_NULL_ID(pt_OriignalId->pt_Left);
    TEST_IS_SEED_ID(pt_OriignalId->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriignalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a (1, 0) ID succeeds */
void ITC_Id_Test_split10IdSuccessful(void)
{
    ITC_Id_t *pt_OriignalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* Create a new (1, 0) ID */
    TEST_SUCCESS(newNull(&pt_OriignalId, NULL));
    TEST_SUCCESS(newSeed(&pt_OriignalId->pt_Left, pt_OriignalId));
    TEST_SUCCESS(newNull(&pt_OriignalId->pt_Right, pt_OriignalId));

    /* Split the (1, 0) ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriignalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (((1, 0), 0), ((0, 1), 0)) */
    TEST_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_IS_SEED_NULL_ID(pt_SplitId1->pt_Left);
    TEST_IS_NULL_ID(pt_SplitId1->pt_Right);
    TEST_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_IS_NULL_SEED_ID(pt_SplitId2->pt_Left);
    TEST_IS_NULL_ID(pt_SplitId2->pt_Right);

    /* Test the original is still a (1, 0) ID */
    TEST_IS_SEED_ID(pt_OriignalId->pt_Left);
    TEST_IS_NULL_ID(pt_OriignalId->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriignalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a ((1, 0), (0, 1)) ID succeeds */
void ITC_Id_Test_split1001IdSuccessful(void)
{
    ITC_Id_t *pt_OriignalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* clang-format off */
    /* Create a new ((1, 0), (0, 1)) ID */
    TEST_SUCCESS(newNull(&pt_OriignalId, NULL));

    TEST_SUCCESS(newNull(&pt_OriignalId->pt_Left, pt_OriignalId));
    TEST_SUCCESS(newSeed(&pt_OriignalId->pt_Left->pt_Left, pt_OriignalId->pt_Left));
    TEST_SUCCESS(newNull(&pt_OriignalId->pt_Left->pt_Right, pt_OriignalId->pt_Left));

    TEST_SUCCESS(newNull(&pt_OriignalId->pt_Right, pt_OriignalId));
    TEST_SUCCESS(newNull(&pt_OriignalId->pt_Right->pt_Left, pt_OriignalId->pt_Right));
    TEST_SUCCESS(newSeed(&pt_OriignalId->pt_Right->pt_Right, pt_OriignalId->pt_Right));
    /* clang-format on */

    /* Split the ((1, 0), (0, 1)) ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriignalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (((1, 0), 0), (0, (0, 1))) */
    TEST_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_IS_SEED_NULL_ID(pt_SplitId1->pt_Left);
    TEST_IS_NULL_ID(pt_SplitId1->pt_Right);
    TEST_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_IS_NULL_ID(pt_SplitId2->pt_Left);
    TEST_IS_NULL_SEED_ID(pt_SplitId2->pt_Right);

    /* Test the original is still a ((1, 0), (0, 1)) ID */
    TEST_IS_SEED_NULL_ID(pt_OriignalId->pt_Left);
    TEST_IS_NULL_SEED_ID(pt_OriignalId->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriignalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a ((0, (1, 0)), ((0, 1), 0)) ID succeeds */
void ITC_Id_Test_split010010IdSuccessful(void)
{
    ITC_Id_t *pt_OriignalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* clang-format off */
    /* Create a new ((0, (1, 0)), ((0, 1), 0)) ID */
    TEST_SUCCESS(newNull(&pt_OriignalId, NULL));

    TEST_SUCCESS(newNull(&pt_OriignalId->pt_Left, pt_OriignalId));
    TEST_SUCCESS(newNull(&pt_OriignalId->pt_Left->pt_Left, pt_OriignalId->pt_Left));
    TEST_SUCCESS(newNull(&pt_OriignalId->pt_Left->pt_Right, pt_OriignalId->pt_Left));
    TEST_SUCCESS(newSeed(&pt_OriignalId->pt_Left->pt_Right->pt_Left, pt_OriignalId->pt_Left->pt_Right));
    TEST_SUCCESS(newNull(&pt_OriignalId->pt_Left->pt_Right->pt_Right, pt_OriignalId->pt_Left->pt_Right));

    TEST_SUCCESS(newNull(&pt_OriignalId->pt_Right, pt_OriignalId));
    TEST_SUCCESS(newNull(&pt_OriignalId->pt_Right->pt_Left, pt_OriignalId->pt_Right));
    TEST_SUCCESS(newNull(&pt_OriignalId->pt_Right->pt_Left->pt_Left, pt_OriignalId->pt_Right->pt_Left));
    TEST_SUCCESS(newSeed(&pt_OriignalId->pt_Right->pt_Left->pt_Right, pt_OriignalId->pt_Right->pt_Left));
    TEST_SUCCESS(newNull(&pt_OriignalId->pt_Right->pt_Right, pt_OriignalId->pt_Right));
    /* clang-format on */

    /* Split the ((0, (1, 0)), ((0, 1), 0)) ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriignalId, &pt_SplitId1, &pt_SplitId2));

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
    TEST_IS_NULL_ID(pt_OriignalId->pt_Left->pt_Left);
    TEST_IS_SEED_NULL_ID(pt_OriignalId->pt_Left->pt_Right);
    TEST_IS_NULL_SEED_ID(pt_OriignalId->pt_Right->pt_Left);
    TEST_IS_NULL_ID(pt_OriignalId->pt_Right->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriignalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}