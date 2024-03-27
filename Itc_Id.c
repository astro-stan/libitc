/**
 * @file Itc_Id.c
 * @author Stan
 * @brief Implementation of the Interval Tree Clock's ID mechanism
 *
 * @copypt_Right Copypt_Right (c) 2024
 *
 */
#include "Itc_Id.h"
#include "Itc_Id_package.h"

#include <stdlib.h>

/******************************************************************************
 * Public functions
 ******************************************************************************/

/******************************************************************************
 * Allocate a new ITC ID
 ******************************************************************************/

Itc_Status_t ITC_Id_alloc(
    ITC_Id_t **ppt_Id,
    ITC_Id_t *pt_Parent
)
{
    ITC_Id_t *pt_Alloc = (ITC_Id_t*)calloc(1, sizeof(ITC_Id_t));

    /* Allocation failed */
    if (!pt_Alloc)
    {
        return ITC_STATUS_INSUFFICIENT_RESOURCES;
    }

    /* Assign the parent */
    if (pt_Parent)
    {
        pt_Alloc->pt_Parent = pt_Parent;
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
    Itc_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Id_t *pt_Current = *ppt_Id; /* The current element */
    ITC_Id_t *pt_Tmp = NULL;

    if (pt_Current == NULL)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        while(t_Status == ITC_STATUS_SUCCESS && pt_Current != NULL)
        {
            if(pt_Current->pt_Left)
            {
                /* Advance into left subtree */
                pt_Current = pt_Current->pt_Left;
            }
            else if(pt_Current->pt_Right)
            {
                /* Advance into right subtree */
                pt_Current = pt_Current->pt_Right;
            }
            else
            {
                /* Remember the parent element */
                pt_Tmp = pt_Current->pt_Parent;

                if(pt_Tmp)
                {
                    /* Remove the current element address from the parent */
                    if(pt_Tmp->pt_Left == pt_Current)
                    {
                        pt_Tmp->pt_Left = NULL;
                    }
                    else
                    {
                        pt_Tmp->pt_Right = NULL;
                    }
                }

                /* Free the current element */
                free(pt_Current);

                /* Go up the tree */
                pt_Current = pt_Tmp;
            }
        }
    }

    return t_Status;
}

/******************************************************************************
 * Clone an existing ITC ID
 ******************************************************************************/

Itc_Status_t ITC_Id_clone(
    const ITC_Id_t *const pt_Id,
    ITC_Id_t **ppt_ClonedId
)
{
    Itc_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    const ITC_Id_t *pt_Root = pt_Id; /* The current root */
    ITC_Id_t *pt_ClonedRoot; /* The current cloned root */

    if (pt_Root == NULL)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        /* Allocate the root */
        t_Status = ITC_Id_alloc(ppt_ClonedId, NULL);

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
                t_Status =
                    ITC_Id_alloc(&pt_ClonedRoot->pt_Left, pt_ClonedRoot);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
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
                t_Status =
                    ITC_Id_alloc(&pt_ClonedRoot->pt_Right, pt_ClonedRoot);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
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

    /* Something went wrong */
    if (t_Status != ITC_STATUS_SUCCESS)
    {
        /* Deallocate clone */
        ITC_Id_free(ppt_ClonedId);
    }

    return t_Status;
}

/******************************************************************************
 * Allocate a new ITC ID and initialise it as a seed ID ([1])
 ******************************************************************************/

Itc_Status_t ITC_Id_newSeed(
    ITC_Id_t **ppt_Id
)
{
    Itc_Status_t t_Status; /* The current status */

    t_Status = ITC_Id_alloc(ppt_Id, NULL);

    /* Initialise as stamp seed */
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        (*ppt_Id)->b_IsOwner = 1;
        (*ppt_Id)->pt_Left = NULL;
        (*ppt_Id)->pt_Right = NULL;
    }

    return t_Status;
}