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