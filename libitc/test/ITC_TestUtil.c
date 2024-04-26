#include "unity.h"

#include "ITC_Test_package.h"

#include "ITC_Id_package.h"
#include "ITC_Event_package.h"
#include "ITC_TestUtil.h"

/******************************************************************************
 *  Private functions
 ******************************************************************************/

/**
 * @brief Create a new invalid ID with root parent owner
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidIdWithRootParentOwner(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newSeedId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Left, *ppt_Id));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&(*ppt_Id)->pt_Right, *ppt_Id));
}

/**
 * @brief Create a new invalid ID with nested parent owner
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidIdWithNestedParentOwner(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Left, *ppt_Id));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&(*ppt_Id)->pt_Right, *ppt_Id));
    TEST_SUCCESS(
        ITC_TestUtil_newNullId(
            &(*ppt_Id)->pt_Right->pt_Left,
            (*ppt_Id)->pt_Right));
    TEST_SUCCESS(
        ITC_TestUtil_newSeedId(
            &(*ppt_Id)->pt_Right->pt_Right,
            (*ppt_Id)->pt_Right));
}

/**
 * @brief Create a new invalid ID with asymmetric root parent with only left child
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidIdWithAsymmetricRootParentLeft(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&(*ppt_Id)->pt_Left, *ppt_Id));
}

/**
 * @brief Create a new invalid ID with asymmetric root parent with only right child
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidIdWithAsymmetricRootParentRight(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Right, *ppt_Id));
}

/**
 * @brief Create a new invalid ID with asymmetric nested parent with only left child
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidIdWithAsymmetricNestedParentLeft(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Left, *ppt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Right, *ppt_Id));
    TEST_SUCCESS(
        ITC_TestUtil_newSeedId(
            &(*ppt_Id)->pt_Right->pt_Left,
            (*ppt_Id)->pt_Right));
}

/**
 * @brief Create a new invalid ID with asymmetric nested parent with only right child
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidIdWithAsymmetricNestedParentRight(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Left, *ppt_Id));
    TEST_SUCCESS(
        ITC_TestUtil_newNullId(
            &(*ppt_Id)->pt_Left->pt_Right,
            (*ppt_Id)->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Right, *ppt_Id));
}

/**
 * @brief Create a new invalid not normalised ID
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidNotNormalisedId(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Left, *ppt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Right, *ppt_Id));
}

/**
 * @brief Create a new invalid not normalised nested ID
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidNotNormalisedNestedId(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Left, *ppt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Right, *ppt_Id));
    TEST_SUCCESS(
        ITC_TestUtil_newSeedId(
            &(*ppt_Id)->pt_Right->pt_Left,
            (*ppt_Id)->pt_Right));
    TEST_SUCCESS(
        ITC_TestUtil_newSeedId(
            &(*ppt_Id)->pt_Right->pt_Right,
            (*ppt_Id)->pt_Right));
}

/**
 * @brief Create a new invalid ID with NULL parent pointer
 *
 * Use `destroyInvalidIdWithNullParentPointer` to deallocate the ID
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidIdWithNullParentPointer(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Left, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&(*ppt_Id)->pt_Right, *ppt_Id));
}

/**
 * @brief Deallocate an invalid ID created with
 * `newInvalidIdWithNullParentPointer`
 *
 *
 * @param pt_Id (in) The pointer to the root if the ID.
 */
static void destroyInvalidIdWithNullParentPointer(
    ITC_Id_t **ppt_Id
)
{
    /* Fix the damage so the ID can be properly deallocated */
    (*ppt_Id)->pt_Left->pt_Parent = *ppt_Id;
    TEST_SUCCESS(ITC_Id_destroy(ppt_Id));
}

/**
 * @brief Create a new invalid ID with invalid parent pointer
 *
 * Use `destroyInvalidIdWithInvalidParentPointer` to deallocate the ID
 *
 * @param pt_Id (out) The pointer to the ID
 */
static void newInvalidIdWithInvalidParentPointer(
    ITC_Id_t **ppt_Id
)
{
    TEST_SUCCESS(ITC_TestUtil_newNullId(ppt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Left, *ppt_Id));
    TEST_SUCCESS(
        ITC_TestUtil_newNullId(&(*ppt_Id)->pt_Right, (*ppt_Id)->pt_Left));
}

/**
 * @brief Deallocate an invalid ID created with
 * `newInvalidIdWithInvalidParentPointer`
 *
 * @param pt_Id (in) The pointer to the root if the ID.
 */
static void destroyInvalidIdWithInvalidParentPointer(
    ITC_Id_t **ppt_Id
)
{
    /* Fix the damage so the ID can be properly deallocated */
    (*ppt_Id)->pt_Right->pt_Parent = *ppt_Id;
    TEST_SUCCESS(ITC_Id_destroy(ppt_Id));
}

/**
 * @brief Create a new invalid Event with asymmetric root parent
 * with only left child
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidEventWithAsymmetricRootParentLeft(
    ITC_Event_t **ppt_Event
)
{
    TEST_SUCCESS(ITC_TestUtil_newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Left, *ppt_Event, 1));
}

/**
 * @brief Create a new invalid Event with asymmetric root parent
 * with only right child
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidEventWithAsymmetricRootParentRight(
    ITC_Event_t **ppt_Event
)
{
    TEST_SUCCESS(ITC_TestUtil_newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Right, *ppt_Event, 1));
}

/**
 * @brief Create a new invalid Event with asymmetric nested parent
 * with only left child
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidEventWithAsymmetricNestedParentLeft(
    ITC_Event_t **ppt_Event
)
{
    TEST_SUCCESS(ITC_TestUtil_newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Left, *ppt_Event, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Right, *ppt_Event, 2));
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &(*ppt_Event)->pt_Right->pt_Left,
            (*ppt_Event)->pt_Right,
            3));
}

/**
 * @brief Create a new invalid Event with asymmetric nested parent
 * with only right child
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidEventWithAsymmetricNestedParentRight(
    ITC_Event_t **ppt_Event
)
{
    TEST_SUCCESS(ITC_TestUtil_newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Left, *ppt_Event, 0));
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &(*ppt_Event)->pt_Left->pt_Right,
            (*ppt_Event)->pt_Left,
            3));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Right, *ppt_Event, 2));
}

/**
 * @brief Create a new invalid not normalised Event
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidNotNormalisedEvent(
    ITC_Event_t **ppt_Event
)
{
    TEST_SUCCESS(ITC_TestUtil_newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Left, *ppt_Event, 1));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Right, *ppt_Event, 2));
}

/**
 * @brief Create a new invalid not normalised nested Event
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidNotNormalisedNestedEvent(
    ITC_Event_t **ppt_Event
)
{
    TEST_SUCCESS(ITC_TestUtil_newEvent(ppt_Event, NULL, 1));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Left, *ppt_Event, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Right, *ppt_Event, 2));
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &(*ppt_Event)->pt_Right->pt_Left,
            (*ppt_Event)->pt_Right,
            2));
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &(*ppt_Event)->pt_Right->pt_Right,
            (*ppt_Event)->pt_Right,
            2));
}

/**
 * @brief Create a new invalid Event with NULL parent pointer
 *
 * Use `destroyInvalidEventWithNullParentPointer` to deallocate
 * the Event
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidEventWithNullParentPointer(
    ITC_Event_t **ppt_Event
)
{
    TEST_SUCCESS(ITC_TestUtil_newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Left, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Right, *ppt_Event, 2));
}

/**
 * @brief Deallocate an invalid Event created with
 * `newInvalidEventWithNullParentPointer`
 *
 *
 * @param pt_Event (in) The pointer to the root if the Event.
 */
static void destroyInvalidEventWithNullParentPointer(
    ITC_Event_t **ppt_Event
)
{
    /* Fix the damage so the Event can be properly deallocated */
    (*ppt_Event)->pt_Left->pt_Parent = *ppt_Event;
    TEST_SUCCESS(ITC_Event_destroy(ppt_Event));
}

/**
 * @brief Create a new invalid Event with invalid parent pointer
 *
 * Use `destroyInvalidEventWithInvalidParentPointer` to deallocate
 * the Event
 *
 * @param pt_Event (out) The pointer to the Event
 */
static void newInvalidEventWithInvalidParentPointer(
    ITC_Event_t **ppt_Event
)
{
    TEST_SUCCESS(ITC_TestUtil_newEvent(ppt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&(*ppt_Event)->pt_Left, *ppt_Event, 1));
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &(*ppt_Event)->pt_Right,
            (*ppt_Event)->pt_Left,
            2));
}

/**
 * @brief Deallocate an invalid Event created with
 * `newInvalidEventWithInvalidParentPointer`
 *
 * @param pt_Event (in) The pointer to the root if the Event.
 */
static void destroyInvalidEventWithInvalidParentPointer(
    ITC_Event_t **ppt_Event
)
{
    /* Fix the damage so the Event can be properly deallocated */
    (*ppt_Event)->pt_Right->pt_Parent = *ppt_Event;
    TEST_SUCCESS(ITC_Event_destroy(ppt_Event));
}

/******************************************************************************
 *  Global variables
 ******************************************************************************/

/******************************************************************************
 * Table of constructors for varous types of invalid IDs
 ******************************************************************************/

void (*const gpv_InvalidIdConstructorTable[])(ITC_Id_t **) =
{
    newInvalidIdWithAsymmetricRootParentLeft,
    newInvalidIdWithAsymmetricRootParentRight,
    newInvalidIdWithAsymmetricNestedParentLeft,
    newInvalidIdWithAsymmetricNestedParentRight,
    newInvalidIdWithRootParentOwner,
    newInvalidIdWithNestedParentOwner,
    newInvalidIdWithNullParentPointer,
    newInvalidIdWithInvalidParentPointer,
    /* Normalisation related invalid IDs.
     * If adding more constructors before this point,
     * be sure to update `FIRST_NORMALISATION_RELATED_INVALID_ID_INDEX` */
    newInvalidNotNormalisedId,
    newInvalidNotNormalisedNestedId,
};

/******************************************************************************
 * Table of destructors for varous types of invalid IDs
 ******************************************************************************/

void (*const gpv_InvalidIdDestructorTable[])(ITC_Id_t **) =
{
    /* Cast the funcion pointer to the type of the table
     * This is ugly but beats needlessly having to write a destructor
     * for each invalid ID */
    (void (*)(ITC_Id_t **))ITC_Id_destroy,
    (void (*)(ITC_Id_t **))ITC_Id_destroy,
    (void (*)(ITC_Id_t **))ITC_Id_destroy,
    (void (*)(ITC_Id_t **))ITC_Id_destroy,
    (void (*)(ITC_Id_t **))ITC_Id_destroy,
    (void (*)(ITC_Id_t **))ITC_Id_destroy,
    destroyInvalidIdWithNullParentPointer,
    destroyInvalidIdWithInvalidParentPointer,
    /* Normalisation related invalid IDs.
     * If adding more destructors before this point,
     * be sure to update `FIRST_NORMALISATION_RELATED_INVALID_ID_INDEX` */
    (void (*)(ITC_Id_t **))ITC_Id_destroy,
    (void (*)(ITC_Id_t **))ITC_Id_destroy,
};

/******************************************************************************
 * The size of the `gpv_InvalidIdDestructorTable` and `gpv_InvalidIdConstructorTable` arrays.
 ******************************************************************************/

const uint32_t gu32_InvalidIdTablesSize =
    ARRAY_COUNT(gpv_InvalidIdConstructorTable);

/******************************************************************************
 * Table of constructors for varous types of invalid Events
 ******************************************************************************/

void (*const gpv_InvalidEventConstructorTable[])(ITC_Event_t **) =
{
    newInvalidEventWithAsymmetricRootParentLeft,
    newInvalidEventWithAsymmetricRootParentRight,
    newInvalidEventWithAsymmetricNestedParentLeft,
    newInvalidEventWithAsymmetricNestedParentRight,
    newInvalidEventWithNullParentPointer,
    newInvalidEventWithInvalidParentPointer,
    /* Normalisation related invalid Events.
     * If adding more constructors before this point,
     * be sure to update `FIRST_NORMALISATION_RELATED_INVALID_EVENT_INDEX` */
    newInvalidNotNormalisedEvent,
    newInvalidNotNormalisedNestedEvent,
};


/******************************************************************************
 * Table of destructors for varous types of invalid Events
 ******************************************************************************/

void (*const gpv_InvalidEventDestructorTable[])(ITC_Event_t **) =
{
    /* Cast the funcion pointer to the type of the table
     * This is ugly but beats needlessly having to write a destructor
     * for each invalid Event */
    (void (*)(ITC_Event_t **))ITC_Event_destroy,
    (void (*)(ITC_Event_t **))ITC_Event_destroy,
    (void (*)(ITC_Event_t **))ITC_Event_destroy,
    (void (*)(ITC_Event_t **))ITC_Event_destroy,
    destroyInvalidEventWithNullParentPointer,
    destroyInvalidEventWithInvalidParentPointer,
    /* Normalisation related invalid Events.
     * If adding more destructors before this point,
     * be sure to update `FIRST_NORMALISATION_RELATED_INVALID_EVENT_INDEX` */
    (void (*)(ITC_Event_t **))ITC_Event_destroy,
    (void (*)(ITC_Event_t **))ITC_Event_destroy,
};

/******************************************************************************
 * The size of the `gpv_InvalidEventConstructorTable` and `gpv_InvalidEventDestructorTable` arrays.
 ******************************************************************************/

const uint32_t gu32_InvalidEventTablesSize =
    ARRAY_COUNT(gpv_InvalidEventConstructorTable);

/******************************************************************************
 *  Public functions
 ******************************************************************************/

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
