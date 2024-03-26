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
            /* Save the current node as the next iteration parent node */
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
            /* Save the current node as the next iteration parent node */
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
    const ITC_Id_t const *pt_Id,
    ITC_Id_t **ppt_ClonedId
)
{
    tNode *current = original;
    tNode *cloneCurrent = newtNode(0);
    tNode *pre = NULL;
    tNode *clonePre = NULL;

    *clone = cloneCurrent;

    /* Inorder Morris traversal */
    while (current != NULL)
    {
        if (current->left == NULL)
        {
            cloneCurrent->data = current->data;

            current = current->right;
            cloneCurrent = cloneCurrent->right;
        }
        else
        {
            /* Esure the left node is not created again when we climb back */
            if(!cloneCurrent->left)
            {
                cloneCurrent->left = newtNode(0);
            }

            /* Find the inorder predecessor of current */
            pre = current->left;
            clonePre = cloneCurrent->left;
            while (pre->right != NULL
                   && pre->right != current)
            {
                /* Create a new right node if:
                 * - The pre->right has real node, not one used for climbing back
                 * - Only if this iteration will terminate due to
                 *   pre->right == NULL (i.e. this is the first time the
                 *   iteration is done) */
                if(!clonePre->right && pre->right && pre->right != current)
                {
                    clonePre->right = newtNode(0);
                }
                clonePre = clonePre->right;
                pre = pre->right;
            }

            /* Make current as the right child of its
               inorder predecessor */
            if (pre->right == NULL)
            {
                clonePre->right = cloneCurrent;
                cloneCurrent = cloneCurrent->left;

                pre->right = current;
                current = current->left;
            }

            /* Revert the changes made in the 'if' part to
               restore the original tree i.e., fix the right
               child of predecessor */
            else
            {
                cloneCurrent->data = current->data;

                /* Create a new node if it wasn't created in the while loop
                 * I believe this happens only on the first iteration that starts
                 * exploring the right subtree of the root (original) node */
                if (!cloneCurrent->right)
                {
                    cloneCurrent->right = newtNode(0);
                }

                clonePre->right = NULL;
                cloneCurrent = cloneCurrent->right;

                pre->right = NULL;
                current = current->right;
            }
        }
    }
}

/******************************************************************************
 * Initialise an ITC ID as a seed ID
 ******************************************************************************/

Itc_Status_t ITC_Id_init(
    ITC_Id_t *pt_Id
)
{
    Itc_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

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