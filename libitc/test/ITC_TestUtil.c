#include "ITC_TestUtil.h"

#include "ITC_Id_package.h"
#include "ITC_Event_package.h"

/******************************************************************************
 * Same as ITC_Id_newNull but enforces setting the parent
 ******************************************************************************/

ITC_Status_t ITC_TestUtil_newNullId(
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

/******************************************************************************
 * Same as ITC_Id_newSeed but enforces setting the parent
 ******************************************************************************/

ITC_Status_t ITC_TestUtil_newSeedId(
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

/******************************************************************************
 * Same as ITC_Event_new but enforces setting the parent and an
 ******************************************************************************/

ITC_Status_t ITC_TestUtil_newEvent(
    ITC_Event_t **ppt_Event,
    ITC_Event_t *pt_Parent,
    ITC_Event_Counter_t t_Count
)
{
    ITC_Status_t t_Status;

    t_Status = ITC_Event_new(ppt_Event);
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        (*ppt_Event)->pt_Parent = pt_Parent;
        (*ppt_Event)->t_Count = t_Count;
    }

    return t_Status;
}
