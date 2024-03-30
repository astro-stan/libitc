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

// static void debugPrintTree(ITC_Id_t *pt_Root)
// {
//     if(ITC_ID_IS_LEAF_ID(pt_Root))
//     {
//         printf("%d", pt_Root->b_IsOwner);
//     }
//     else
//     {
//         printf("(");
//     }

//     if(pt_Root->pt_Left)
//     {
//         debugPrintTree(pt_Root->pt_Left);
//         printf(", ");
//     }
//     if(pt_Root->pt_Right)
//     {
//         debugPrintTree(pt_Root->pt_Right);
//     }

//     if(!ITC_ID_IS_LEAF_ID(pt_Root))
//     {
//         printf(")");
//     }
// }

/**
 * @brief Turns an ID into its string representation
 *
 * E.g 1 -> '1', 0 -> '0', (1, 0) -> '(1, 0)', etc
 *
 * @param pt_Id The ID
 * @param z_Str (in) A pointer to a string buffer. (out) A null terminated
 * string.
 * @param pu8_StrOffset (in) The start index in the buffer (usually 0).
 * (out) The length of the written data without the NULL termination byte.
 * @param u8_StrLength The total length of the buffer
 * @return ITC_Status_t Status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t snprintfId(
    const ITC_Id_t *const pt_Id,
    string_t z_Str,
    uint8_t *pu8_StrOffset,
    uint8_t u8_StrLength
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS;
    int8_t i8_PrintfRet = 0;

    /* TODO: Refactor this to work without recursion */

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        if(ITC_ID_IS_LEAF_ID(pt_Id))
        {
            i8_PrintfRet = (int8_t)snprintf(
                    &z_Str[*pu8_StrOffset],
                    u8_StrLength - *pu8_StrOffset,
                    "%" PRIu8,
                    pt_Id->b_IsOwner);

            if(i8_PrintfRet >= u8_StrLength - *pu8_StrOffset)
            {
                t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
            }
            else
            {
                *pu8_StrOffset += i8_PrintfRet;
                t_Status = ITC_STATUS_SUCCESS;
            }
        }
        else
        {
            i8_PrintfRet = (int8_t)snprintf(
                    &z_Str[*pu8_StrOffset],
                    u8_StrLength - *pu8_StrOffset,
                    "(");

            if(i8_PrintfRet >= u8_StrLength - *pu8_StrOffset)
            {
                t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
            }
            else
            {
                *pu8_StrOffset += i8_PrintfRet;
                t_Status = ITC_STATUS_SUCCESS;
            }
        }
    }

    if(pt_Id->pt_Left && t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = snprintfId(
            pt_Id->pt_Left, z_Str, pu8_StrOffset, u8_StrLength);

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            i8_PrintfRet = (int8_t)snprintf(
                &z_Str[*pu8_StrOffset],
                u8_StrLength - *pu8_StrOffset,
                ", ");

            if(i8_PrintfRet >= u8_StrLength - *pu8_StrOffset)
            {
                t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
            }
            else
            {
                *pu8_StrOffset += i8_PrintfRet;
                t_Status = ITC_STATUS_SUCCESS;
            }
        }
    }
    if(pt_Id->pt_Right && t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = snprintfId(
            pt_Id->pt_Right, z_Str, pu8_StrOffset, u8_StrLength);

    }

    if(!ITC_ID_IS_LEAF_ID(pt_Id) && t_Status == ITC_STATUS_SUCCESS)
    {
        i8_PrintfRet = (int8_t)snprintf(
                &z_Str[*pu8_StrOffset],
                u8_StrLength - *pu8_StrOffset,
                ")");

        if(i8_PrintfRet >= u8_StrLength - *pu8_StrOffset)
        {
            t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
        }
        else
        {
            *pu8_StrOffset += i8_PrintfRet;
            t_Status = ITC_STATUS_SUCCESS;
        }
    }

    return t_Status;
}


// static void compareIdToString(
//     const ITC_Id_t *const pt_Id,
//     cstring_t z_Str
// )
// {
//     char rc_Buffer[100];
//     uint8_t u8_Offset = 0;

//     /* Turn the ID into its string representation */
//     TEST_SUCCESS(snprintfId(pt_Id, rc_Buffer, &u8_Offset, sizeof(rc_Buffer)));

//     /* Compare string representations */
//     TEST_ASSERT_EQUAL_CHAR_ARRAY_MESSAGE(
//         z_Str,
//         rc_Buffer,
//         u8_Offset,
//         "ID doesn't match expected string representation");
// }

void setUp(void) {}

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

void ITC_Id_Test_memleak(void)
{
    	/* Constructed binary tree is
    			1
    		  / \
    	    2	 3
    	  / \
    	4	 5
       /
      6
       \
        7
    */
    ITC_Id_t *pt_Original;
    // ITC_Id_t *pt_Clone;
    // ITC_Id_t *pt_Split1 = NULL;
    // ITC_Id_t *pt_Split2 = NULL;
    char rc_Buffer[100];
    uint8_t u8_BufferOffset = 0;

    for (int i = 0; i < 1; i++)
    {
        // ITC_Id_new(&pt_Root, NULL);
        // pt_Root->b_IsOwner = true;

        // ITC_Id_new(&pt_Root->pt_Left, pt_Root);
        // pt_Root->pt_Left->b_IsOwner = false;

        // ITC_Id_new(&pt_Root->pt_Right, pt_Root);
        // pt_Root->pt_Right->b_IsOwner = true;

        // ITC_Id_new(&pt_Root->pt_Left->pt_Left, pt_Root->pt_Left);
        // pt_Root->pt_Left->pt_Left->b_IsOwner = false;

        // ITC_Id_new(&pt_Root->pt_Left->pt_Right, pt_Root->pt_Left);
        // pt_Root->pt_Left->pt_Right->b_IsOwner = true;

        // ITC_Id_new(
        //     &pt_Root->pt_Left->pt_Left->pt_Left,
        //     pt_Root->pt_Left->pt_Left);
        // pt_Root->pt_Left->pt_Left->pt_Left->b_IsOwner = false;

        // ITC_Id_new(
        //     &pt_Root->pt_Left->pt_Left->pt_Left->pt_Right,
        //     pt_Root->pt_Left->pt_Left->pt_Left);
        // pt_Root->pt_Left->pt_Left->pt_Left->pt_Right->b_IsOwner = true;

        printf("Testing split(i, i)\n");

        TEST_SUCCESS(ITC_Id_newNull(&pt_Original));

        ITC_Id_newSeed(&pt_Original->pt_Left);
        pt_Original->pt_Left->pt_Parent = pt_Original;

        ITC_Id_newNull(&pt_Original->pt_Right);
        pt_Original->pt_Right->pt_Parent = pt_Original;

        ITC_Id_newNull(&pt_Original->pt_Right->pt_Right);
        pt_Original->pt_Right->pt_Right->pt_Parent = pt_Original->pt_Right;

        ITC_Id_newNull(&pt_Original->pt_Right->pt_Left);
        pt_Original->pt_Right->pt_Left->pt_Parent = pt_Original->pt_Right;

        ITC_Id_newNull(&pt_Original->pt_Right->pt_Right->pt_Left);
        pt_Original->pt_Right->pt_Right->pt_Left->pt_Parent = pt_Original->pt_Right->pt_Right;

        ITC_Id_newSeed(&pt_Original->pt_Right->pt_Right->pt_Right);
        pt_Original->pt_Right->pt_Right->pt_Right->pt_Parent = pt_Original->pt_Right->pt_Right;

        snprintfId(
            pt_Original,
            &rc_Buffer[0],
            &u8_BufferOffset,
            sizeof(rc_Buffer));

        ITC_Id_destroy(&pt_Original);

        printf("%s", &rc_Buffer[0]);

        // printf("\nInit = ");
        // snprintfId(pt_Original);

        // ITC_Id_split(pt_Original, &pt_Split1, &pt_Split2);
        // ITC_Id_destroy(&pt_Original);
        // printf("\nGive = ");
        // snprintfId(pt_Split2);
        // printf("\nKeep = ");
        // snprintfId(pt_Split1);

        // ITC_Id_destroy(&pt_Split1);
        // ITC_Id_destroy(&pt_Split2);


        // printf("\n\nTesting split(i, 0), split(0, i) and split(1)\n");

        // ITC_Id_newSeed(&pt_Original);
        // printf("\nInit = ");
        // snprintfId(pt_Original);

        // ITC_Id_split(pt_Original, &pt_Split1, &pt_Split2);
        // ITC_Id_destroy(&pt_Original);
        // printf("\nGive = ");
        // snprintfId(pt_Split2);
        // printf("\nKeep = ");
        // snprintfId(pt_Split1);
        // ITC_Id_destroy(&pt_Split2);

        // ITC_Id_clone(pt_Split1, &pt_Original);
        // ITC_Id_destroy(&pt_Split1);

        // ITC_Id_split(pt_Original, &pt_Split1, &pt_Split2);
        // ITC_Id_destroy(&pt_Original);
        // printf("\nGive = ");
        // snprintfId(pt_Split1);
        // printf("\nKeep = ");
        // snprintfId(pt_Split2);
        // ITC_Id_destroy(&pt_Split1);

        // ITC_Id_clone(pt_Split2, &pt_Original);
        // ITC_Id_destroy(&pt_Split2);

        // ITC_Id_split(pt_Original, &pt_Split1, &pt_Split2);
        // ITC_Id_destroy(&pt_Original);
        // printf("\nGive = ");
        // snprintfId(pt_Split2);
        // printf("\nKeep = ");
        // snprintfId(pt_Split1);
        // ITC_Id_destroy(&pt_Split2);

        // ITC_Id_clone(pt_Split1, &pt_Original);
        // ITC_Id_destroy(&pt_Split1);

        // ITC_Id_split(pt_Original, &pt_Split1, &pt_Split2);

        // ITC_Id_destroy(&pt_Original);
        // printf("\nGive = ");
        // snprintfId(pt_Split2);
        // printf("\nKeep = ");
        // snprintfId(pt_Split1);

        // ITC_Id_destroy(&pt_Split1);
        // ITC_Id_destroy(&pt_Split2);
    }
}