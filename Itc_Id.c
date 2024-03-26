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
    Itc_Status_t t_Status = ITC_STATUS_SUCCESS;
    ITC_Id_t *pt_Root = *ppt_Id;
    ITC_Id_t *pt_Tmp = NULL;

    if (pt_Root == NULL)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        while(t_Status == ITC_STATUS_SUCCESS && pt_Root != NULL)
        {
            if(pt_Root->pt_Left)
            {
                pt_Root = pt_Root->pt_Left;
            }
            else if(pt_Root->pt_Right)
            {
                pt_Root = pt_Root->pt_Right;
            }
            else
            {
                pt_Tmp = pt_Root->pt_Parent;

                if(pt_Tmp)
                {
                    if(pt_Tmp->pt_Left == pt_Root)
                    {
                        pt_Tmp->pt_Left = NULL;
                    }
                    else
                    {
                        pt_Tmp->pt_Right = NULL;
                    }
                }

                free(pt_Root);

                pt_Root = pt_Tmp;
            }
        }
    }

    return t_Status;
}

/******************************************************************************
 * Clone an existing ITC ID
 ******************************************************************************/

Itc_Status_t ITC_Id_clone(
    const ITC_Id_t const *pt_Id,
    ITC_Id_t **ppt_ClonedId
)
{
    Itc_Status_t t_Status = ITC_STATUS_SUCCESS;
    const ITC_Id_t *pt_Root = pt_Id;
    ITC_Id_t *pt_ClonedRoot;

    if (pt_Root == NULL)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        /* Allocate the root */
        t_Status = ITC_Id_alloc(ppt_ClonedId);

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            /* Initialise the cloned root pointer */
            pt_ClonedRoot = *ppt_ClonedId;

            /* Copy ownership */
            pt_ClonedRoot->b_IsOwner = pt_Root->b_IsOwner;
        }

        while(t_Status == ITC_STATUS_SUCCESS && pt_Root != NULL)
        {
            if (pt_Root->pt_Left && !pt_ClonedRoot->pt_Left)
            {
                /* Allocate left subtree */
                t_Status = ITC_Id_alloc(&pt_ClonedRoot->pt_Left);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    /* Set parent */
                    pt_ClonedRoot->pt_Left->pt_Parent = pt_ClonedRoot;

                    /* Advance into the tree */
                    pt_Root = pt_Root->pt_Left;
                    pt_ClonedRoot = pt_ClonedRoot->pt_Left;

                    /* Copy ownership */
                    pt_ClonedRoot->b_IsOwner = pt_Root->b_IsOwner;
                }
            }
            else if (pt_Root->pt_Right && !pt_ClonedRoot->pt_Right)
            {
                /* Allocate right subtree */
                t_Status = ITC_Id_alloc(&pt_ClonedRoot->pt_Right);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    /* Set parent */
                    pt_ClonedRoot->pt_Right->pt_Parent = pt_ClonedRoot;

                    /* Advance into the tree */
                    pt_Root = pt_Root->pt_Right;
                    pt_ClonedRoot = pt_ClonedRoot->pt_Right;

                    /* Copy ownership */
                    pt_ClonedRoot->b_IsOwner = pt_Root->b_IsOwner;
                }
            }
            else
            {
                /* Go up the tree */
                pt_Root = pt_Root->pt_Parent;
                pt_ClonedRoot = pt_ClonedRoot->pt_Parent;
            }
        }
    }

    if (t_Status != ITC_STATUS_SUCCESS)
    {
        /* Deallocate clone */
        ITC_Id_free(ppt_ClonedId);
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
    if (pt_Id->pt_Left)
    {
        t_Status = ITC_Id_free(&pt_Id->pt_Left);
    }
    /* Free the right subtree if it is present */
    if (pt_Id->pt_Right)
    {
        t_Status = ITC_Id_free(&pt_Id->pt_Right);
    }

    return t_Status;
}