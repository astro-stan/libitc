#include "unity.h"

#include "ITC_Id_Test.h"
#include "ITC_Id.h"
#include "ITC_Id_package.h"
#include "ITC_Test_helpers.h"

#include <stdio.h>

/******************************************************************************
 *  Global variables
 ******************************************************************************/

/* The initial seed */
const ITC_Id_t *gpt_Seed = NULL;

static void debugPrintTree(ITC_Id_t *pt_Root)
{
    if(ITC_ID_IS_LEAF_NODE(pt_Root))
    {
        printf("%d", pt_Root->b_IsOwner);
    }
    else
    {
        printf("(");
    }

    if(pt_Root->pt_Left)
    {
        debugPrintTree(pt_Root->pt_Left);
        printf(", ");
    }
    if(pt_Root->pt_Right)
    {
        debugPrintTree(pt_Root->pt_Right);
    }

    if(!ITC_ID_IS_LEAF_NODE(pt_Root))
    {
        printf(")");
    }
}


void setUp(void)
{
    TEST_SUCCESS(ITC_Id_newSeed(&gpt_Seed));
}

void tearDown(void)
{
    TEST_SUCCESS(ITC_Id_destroy(&gpt_Seed));
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
    ITC_Id_t *pt_Split1 = NULL;
    ITC_Id_t *pt_Split2 = NULL;

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

        printf("\nInit = ");
        debugPrintTree(pt_Original);

        ITC_Id_split(pt_Original, &pt_Split1, &pt_Split2);
        ITC_Id_destroy(&pt_Original);
        printf("\nGive = ");
        debugPrintTree(pt_Split2);
        printf("\nKeep = ");
        debugPrintTree(pt_Split1);

        ITC_Id_destroy(&pt_Split1);
        ITC_Id_destroy(&pt_Split2);


        printf("\n\nTesting split(i, 0), split(0, i) and split(1)\n");

        ITC_Id_newSeed(&pt_Original);
        printf("\nInit = ");
        debugPrintTree(pt_Original);

        ITC_Id_split(pt_Original, &pt_Split1, &pt_Split2);
        ITC_Id_destroy(&pt_Original);
        printf("\nGive = ");
        debugPrintTree(pt_Split2);
        printf("\nKeep = ");
        debugPrintTree(pt_Split1);
        ITC_Id_destroy(&pt_Split2);

        ITC_Id_clone(pt_Split1, &pt_Original);
        ITC_Id_destroy(&pt_Split1);

        ITC_Id_split(pt_Original, &pt_Split1, &pt_Split2);
        ITC_Id_destroy(&pt_Original);
        printf("\nGive = ");
        debugPrintTree(pt_Split1);
        printf("\nKeep = ");
        debugPrintTree(pt_Split2);
        ITC_Id_destroy(&pt_Split1);

        ITC_Id_clone(pt_Split2, &pt_Original);
        ITC_Id_destroy(&pt_Split2);

        ITC_Id_split(pt_Original, &pt_Split1, &pt_Split2);
        ITC_Id_destroy(&pt_Original);
        printf("\nGive = ");
        debugPrintTree(pt_Split2);
        printf("\nKeep = ");
        debugPrintTree(pt_Split1);
        ITC_Id_destroy(&pt_Split2);

        ITC_Id_clone(pt_Split1, &pt_Original);
        ITC_Id_destroy(&pt_Split1);

        ITC_Id_split(pt_Original, &pt_Split1, &pt_Split2);

        ITC_Id_destroy(&pt_Original);
        printf("\nGive = ");
        debugPrintTree(pt_Split2);
        printf("\nKeep = ");
        debugPrintTree(pt_Split1);

        ITC_Id_destroy(&pt_Split1);
        ITC_Id_destroy(&pt_Split2);
    }
}