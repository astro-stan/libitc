/**
 * @file Itc_Id.c
 * @author Stan
 * @brief Implementation of the Interval Tree Clock's ID mechanism
 *
 * @copypt_Right Copypt_Right (c) 2024
 *
 */
#include "Itc_Id.h"

#include <stdlib.h>

/******************************************************************************
 * Public functions
 ******************************************************************************/

/******************************************************************************
 * Allocate a new ITC ID
 ******************************************************************************/

Itc_Status_t ITC_Id_alloc(
    ITC_Id_t **ppt_Id
)
{
    ITC_Id_t *pt_Alloc = (ITC_Id_t*)calloc(1, sizeof(ITC_Id_t));

    /* Allocation failed */
    if (!pt_Alloc)
    {
        return ITC_STATUS_INSUFFICIENT_RESOURCES;
    }

    /* Return the pointer to the allocated memory */
    *ppt_Id = pt_Alloc;

    return ITC_STATUS_SUCCESS;
}

/******************************************************************************
 * Free an allocated ITC ID
 ******************************************************************************/

Itc_Status_t ITC_Id_free(
    ITC_Id_t **ppt_Id
)
{
    ITC_Id_t *pt_Node = *ppt_Id; /* The currently explored node */
    ITC_Id_t *pt_Up = NULL;    /* The parent node */
    ITC_Id_t *pt_Swap = NULL;  /* Used for swapping pointers */

    /* Perform a depth-first walk of the tree
     * Uses the pt_Left subtree pointer to keep track of the parent node */
    while (pt_Node)
    {
        if (pt_Node->pt_Left)
        {
            pt_Swap = pt_Node->pt_Left;
            /* Save the parent address (obtained in the previous iteration) */
            pt_Node->pt_Left = pt_Up;
            /* Save the pt_Current node as the next iteration parent node */
            pt_Up = pt_Node;
            /* Explore the left subtree next */
            pt_Node = pt_Swap;
        }
        else if (pt_Node->pt_Right)
        {
            pt_Swap = pt_Node->pt_Right;
            /* Save the parent address (obtained in the previous iteration) */
            pt_Node->pt_Left = pt_Up;
            /* Forget the right subtree (it will be deallocated later) */
            pt_Node->pt_Right = NULL;
            /* Save the pt_Current node as the next iteration parent node */
            pt_Up = pt_Node;
            /* Explore the right subtree next */
            pt_Node = pt_Swap;
        }
        else
        {
            /* If the initially given tree had no subtrees */
            if (!pt_Up)
            {
                free(pt_Node);
                pt_Node = NULL;
            }
            /* Iterate over the parent nodes */
            while (pt_Up)
            {
                free(pt_Node);
                if (pt_Up->pt_Right)
                {
                    /* The right subtree of the parent hasn't been explored
                     * yet */
                    pt_Node = pt_Up->pt_Right;
                    pt_Up->pt_Right = NULL;
                    break;
                }
                else
                {
                    /* Move to the parent */
                    pt_Node = pt_Up;
                    pt_Up = pt_Up->pt_Left;
                }
            }
        }
    }

    /* Set pt_Id to NULL */
    *ppt_Id = NULL;

    /* Always succeeds */
    return ITC_STATUS_SUCCESS;
}

/******************************************************************************
 * Clone an existing ITC ID
 ******************************************************************************/

Itc_Status_t ITC_Id_clone(
    ITC_Id_t *pt_Id,
    ITC_Id_t **ppt_ClonedId
)
{

    /* TODO: allocation failures are NOT handled properly below (WIP)
     * Recovering from allocation failure is tricky because Morris traversal
     * modifies both the original and cloned trees, creating temporary
     * loops between the right leaf of a node (node->right) and its parent. */
    Itc_Status_t t_Status = ITC_STATUS_SUCCESS;
    bool b_AllocFailed = false;

    ITC_Id_t *pt_Current = pt_Id;
    ITC_Id_t *pt_ClonedCurrent = NULL;
    ITC_Id_t *pt_Predecessor = NULL;
    ITC_Id_t *pt_ClonedPredecessor = NULL;

    /* Allocate the root of the clone */
    t_Status = ITC_Id_alloc(&pt_ClonedCurrent);

    if (t_Status = ITC_STATUS_SUCCESS)
    {
        *ppt_ClonedId = pt_ClonedCurrent;
    }

    /* Perform the Morris traversal */
    while (pt_Current != NULL)
    {
        if (pt_Current->pt_Left == NULL && t_Status == ITC_STATUS_SUCCESS)
        {
            /* Clone the data */
            pt_ClonedCurrent->b_IsOwner = pt_Current->b_IsOwner;

            pt_Current = pt_Current->pt_Right;
            pt_ClonedCurrent = pt_ClonedCurrent->pt_Right;
        }
        else
        {
            /* Esure the left node is not created again when climbing back */
            if(!pt_ClonedCurrent->pt_Left && !b_AllocFailed)
            {
                t_Status = ITC_Id_alloc(&pt_ClonedCurrent->pt_Left);
                if (t_Status != ITC_STATUS_SUCCESS)
                {
                    b_AllocFailed = true;
                }
            }

            /* Find the inorder predecessor of pt_Current and create it for
             * the pt_ClonedCurrent */
            pt_Predecessor = pt_Current->pt_Left;

            /* Prevous allocation might have failed somewhere, but we still
             * need to cleanup. Proceed carefully. */
            if(pt_ClonedCurrent->pt_Left)
            {
                pt_ClonedPredecessor = pt_ClonedCurrent->pt_Left;
            }
            while (pt_Predecessor->pt_Right != NULL &&
                   pt_Predecessor->pt_Right != pt_Current)
            {
                /* Ensure the right node is only created on the first
                * iteration (i.e. when termination occurs due to
                * pt_Predecessor->pt_Right == NULL) and only for real nodes,
                * not ones used for climbing back */
                if(!b_AllocFailed &&
                   !pt_ClonedPredecessor->pt_Right &&
                   pt_Predecessor->pt_Right &&
                   pt_Predecessor->pt_Right != pt_Current)
                {
                    t_Status =
                        ITC_Id_alloc(&pt_ClonedPredecessor->pt_Right);
                    if (t_Status !=ITC_STATUS_SUCCESS)
                    {
                        b_AllocFailed = true;
                    }
                }

                pt_Predecessor = pt_Predecessor->pt_Right;
                /* Prevous allocation might have failed somewhere, but we still
                 * need to cleanup. Proceed carefully. */
                if (pt_ClonedPredecessor->pt_Right)
                {
                    pt_ClonedPredecessor = pt_ClonedPredecessor->pt_Right;
                }
            }

            /* Make pt_Current and pt_ClonedCurrent the right childs of
             * their inorder predecessors */
            if (pt_Predecessor->pt_Right == NULL)
            {
                pt_Predecessor->pt_Right = pt_Current;
                pt_Current = pt_Current->pt_Left;

                pt_ClonedPredecessor->pt_Right = pt_ClonedCurrent;
                pt_ClonedCurrent = pt_ClonedCurrent->pt_Left;
            }
            /* On the next iteration fix the right child of
             * predecessors */
            else
            {
                /* Clone the data */
                pt_ClonedCurrent->b_IsOwner = pt_Current->b_IsOwner;

                pt_Predecessor->pt_Right = NULL;
                pt_Current = pt_Current->pt_Right;

                pt_ClonedPredecessor->pt_Right = NULL;

                /* Create a new node if it wasn't created in the while
                    * loop above */
                if (!pt_ClonedCurrent->pt_Right)
                {
                    pt_ClonedCurrent->pt_Right = newtNode(0);
                    t_Status =
                        ITC_Id_alloc(&pt_ClonedCurrent->pt_Right);
                }
                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    pt_ClonedCurrent = pt_ClonedCurrent->pt_Right;
                }
            }
        }
    }

    return t_Status;
}

/******************************************************************************
 * Initialise an ITC ID as a seed ID
 ******************************************************************************/

Itc_Status_t ITC_Id_init(
    ITC_Id_t *pt_Id
)
{
    Itc_Status_t t_Status = ITC_STATUS_SUCCESS; /* The pt_Current status */

    /* Set the ID to be the owner of the interval */
    pt_Id->b_IsOwner = 1;

    /* Free the left subtree if it is present */
    t_Status = ITC_Id_free(&pt_Id->pt_Left);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Free the right subtree if it is present */
        t_Status = ITC_Id_free(&pt_Id->pt_Right);
    }

    return t_Status;
}