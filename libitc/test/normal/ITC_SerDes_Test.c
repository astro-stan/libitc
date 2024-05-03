/**
 * @file ITC_SerDes_Test.h
 * @brief Unit tests for the Interval Tree Clock's serialisation and
 * deserialisation mechanism
 *
 * @copyright Copyright (c) 2024 libITC project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#include "ITC_SerDes.h"
#include "ITC_SerDes_Test.h"

#include "ITC_Stamp.h"
#include "ITC_Test_package.h"

#include <stdint.h>

/******************************************************************************
 *  Private functions
 ******************************************************************************/

/******************************************************************************
 *  Global variables
 ******************************************************************************/

/******************************************************************************
 *  Public functions
 ******************************************************************************/

/* Init test */
void setUp(void) {}

/* Fini test */
void tearDown(void) {}

/* Test serialising a Stamp fails with invalid param */
void ITC_SerDes_Test_serialiseStampFailInvalidParam(void)
{
    ITC_Stamp_t *pt_Dummy = NULL;
    uint8_t ru8_Buffer[100];
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    TEST_FAILURE(
        ITC_SerDes_serialiseStamp(
            pt_Dummy,
            &ru8_Buffer[0],
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_serialiseStamp(
            NULL,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_serialiseStamp(
            pt_Dummy,
            NULL,
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);
}

/* Test deserialising a Stamp fails with invalid param */
void ITC_SerDes_Test_deserialiseStampFailInvalidParam(void)
{
    ITC_Stamp_t *pt_Dummy = NULL;
    uint8_t ru8_Buffer[100] = { 0 };

    TEST_FAILURE(
        ITC_SerDes_deserialiseStamp(
            &ru8_Buffer[0],
            sizeof(ru8_Buffer),
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_deserialiseStamp(
            NULL,
            sizeof(ru8_Buffer),
            &pt_Dummy),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_deserialiseStamp(
            &ru8_Buffer[0],
            0,
            &pt_Dummy),
        ITC_STATUS_INVALID_PARAM);
}
