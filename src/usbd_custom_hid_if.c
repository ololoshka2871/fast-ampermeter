/**
 ******************************************************************************
 * @file           : usbd_custom_hid_if.c
 * @brief          : USB Device Custom HID interface file.
 ******************************************************************************
 * COPYRIGHT(c) 2015 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of STMicroelectronics nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_hid_if.h"

#include "HidSensorSpec.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

__ALIGN_BEGIN static const uint8_t
    CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END = {

        0x06, 0x00, 0xFF, // Usage Page (Vendor Defined 0xFF00)
        0x09, 0x01,       // Usage (0x01)
        0xA1, 0x01,       // Collection (Application)
        0x09, 0x02,       //   Usage (0x02)
        0x75, 0x80,       //   Report Size (4 * sizeof(float) * 8)
        0x95, 0x01,       //   Report Count (1)
        0x81, 0x02, //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No
                    //   Null Position)
        0xC0,       // End Collection

#if 0
    HID_USAGE_PAGE_SENSOR,
    HID_USAGE_SENSOR_TYPE_ENVIRONMENTAL_TEMPERATURE,
    HID_COLLECTION(Physical),

        //feature reports (xmit/receive)
        HID_USAGE_PAGE_SENSOR,
        HID_USAGE_SENSOR_PROPERTY_SENSOR_CONNECTION_TYPE,  // NAry
        HID_LOGICAL_MIN_8(0),
        HID_LOGICAL_MAX_8(2),
        HID_REPORT_SIZE(8),
        HID_REPORT_COUNT(1),
        HID_COLLECTION(Logical),
            HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_INTEGRATED_SEL,
            HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_ATTACHED_SEL,
            HID_USAGE_SENSOR_PROPERTY_CONNECTION_TYPE_PC_EXTERNAL_SEL,
            HID_FEATURE(Data_Arr_Abs),
        HID_END_COLLECTION,
        HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE,
        HID_LOGICAL_MIN_8(0),
        HID_LOGICAL_MAX_8(5),
        HID_REPORT_SIZE(8),
        HID_REPORT_COUNT(1),
        HID_COLLECTION(Logical),
            HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_NO_EVENTS_SEL,
            HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_ALL_EVENTS_SEL,
            HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_THRESHOLD_EVENTS_SEL,
            HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_NO_EVENTS_WAKE_SEL,
            HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_ALL_EVENTS_WAKE_SEL,
            HID_USAGE_SENSOR_PROPERTY_REPORTING_STATE_THRESHOLD_EVENTS_WAKE_SEL,
            HID_FEATURE(Data_Arr_Abs),
        HID_END_COLLECTION,
        HID_USAGE_SENSOR_PROPERTY_POWER_STATE,
        HID_LOGICAL_MIN_8(0),
        HID_LOGICAL_MAX_8(5),
        HID_REPORT_SIZE(8),
        HID_REPORT_COUNT(1),
        HID_COLLECTION(Logical),
            HID_USAGE_SENSOR_PROPERTY_POWER_STATE_UNDEFINED_SEL,
            HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D0_FULL_POWER_SEL,
            HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D1_LOW_POWER_SEL,
            HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D2_STANDBY_WITH_WAKE_SEL,
            HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D3_SLEEP_WITH_WAKE_SEL,
            HID_USAGE_SENSOR_PROPERTY_POWER_STATE_D4_POWER_OFF_SEL,
            HID_FEATURE(Data_Arr_Abs),
        HID_END_COLLECTION,
        HID_USAGE_SENSOR_STATE,
        HID_LOGICAL_MIN_8(0),
        HID_LOGICAL_MAX_8(6),
        HID_REPORT_SIZE(8),
        HID_REPORT_COUNT(1),
        HID_COLLECTION(Logical),
            HID_USAGE_SENSOR_STATE_UNKNOWN_SEL,
            HID_USAGE_SENSOR_STATE_READY_SEL,
            HID_USAGE_SENSOR_STATE_NOT_AVAILABLE_SEL,
            HID_USAGE_SENSOR_STATE_NO_DATA_SEL,
            HID_USAGE_SENSOR_STATE_INITIALIZING_SEL,
            HID_USAGE_SENSOR_STATE_ACCESS_DENIED_SEL,
            HID_USAGE_SENSOR_STATE_ERROR_SEL,
            HID_FEATURE(Data_Arr_Abs),
        HID_END_COLLECTION,
        HID_USAGE_SENSOR_PROPERTY_REPORT_INTERVAL,
        HID_LOGICAL_MIN_8(0),
        HID_LOGICAL_MAX_32(0xFF,0xFF,0xFF,0xFF),
        HID_REPORT_SIZE(32),
        HID_REPORT_COUNT(1),
        HID_UNIT_EXPONENT(0),
        HID_FEATURE(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA(HID_USAGE_SENSOR_DATA_ENVIRONMENTAL_TEMPERATURE,HID_USAGE_SENSOR_DATA_MOD_CHANGE_SENSITIVITY_ABS),
        HID_LOGICAL_MIN_8(0),
        HID_LOGICAL_MAX_16(0xFF,0xFF),
        HID_REPORT_SIZE(16),
        HID_REPORT_COUNT(1),
        HID_FEATURE(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA(HID_USAGE_SENSOR_DATA_ENVIRONMENTAL_TEMPERATURE,HID_USAGE_SENSOR_DATA_MOD_MAX),
        HID_LOGICAL_MIN_16(0x01,0x80), //    LOGICAL_MINIMUM (-32767)
        HID_LOGICAL_MAX_16(0xFF,0x7F), //    LOGICAL_MAXIMUM (32767)
        HID_REPORT_SIZE(16),
        HID_REPORT_COUNT(1),
        HID_UNIT_EXPONENT(0x0E), // scale default unit “Celsius” to provide 2 digits past the decimal point
        HID_FEATURE(Data_Var_Abs),
        HID_USAGE_SENSOR_DATA(HID_USAGE_SENSOR_DATA_ENVIRONMENTAL_TEMPERATURE,HID_USAGE_SENSOR_DATA_MOD_MIN),
        HID_LOGICAL_MIN_16(0x01,0x80), //    LOGICAL_MINIMUM (-32767)
        HID_LOGICAL_MAX_16(0xFF,0x7F), //    LOGICAL_MAXIMUM (32767)
        HID_REPORT_SIZE(16),
        HID_REPORT_COUNT(1),
        HID_UNIT_EXPONENT(0x0E), // scale default unit “Celsius” to provide 2 digits past the decimal point
        HID_FEATURE(Data_Var_Abs),

        //input reports (transmit)
        HID_USAGE_PAGE_SENSOR,
        HID_USAGE_SENSOR_STATE,
        HID_LOGICAL_MIN_8(0),
        HID_LOGICAL_MAX_8(6),
        HID_REPORT_SIZE(8),
        HID_REPORT_COUNT(1),
        HID_COLLECTION(Logical),
            HID_USAGE_SENSOR_STATE_UNKNOWN_SEL,
            HID_USAGE_SENSOR_STATE_READY_SEL,
            HID_USAGE_SENSOR_STATE_NOT_AVAILABLE_SEL,
            HID_USAGE_SENSOR_STATE_NO_DATA_SEL,
            HID_USAGE_SENSOR_STATE_INITIALIZING_SEL,
            HID_USAGE_SENSOR_STATE_ACCESS_DENIED_SEL,
            HID_USAGE_SENSOR_STATE_ERROR_SEL,
            HID_INPUT(Data_Arr_Abs),
        HID_END_COLLECTION,
        HID_USAGE_SENSOR_EVENT,
        HID_LOGICAL_MIN_8(0),
        HID_LOGICAL_MAX_8(16),
        HID_REPORT_SIZE(8),
        HID_REPORT_COUNT(1),

        HID_COLLECTION(Logical),
            HID_USAGE_SENSOR_EVENT_UNKNOWN_SEL,
            HID_USAGE_SENSOR_EVENT_STATE_CHANGED_SEL,
            HID_USAGE_SENSOR_EVENT_PROPERTY_CHANGED_SEL,
            HID_USAGE_SENSOR_EVENT_DATA_UPDATED_SEL,
            HID_USAGE_SENSOR_EVENT_POLL_RESPONSE_SEL,
            HID_USAGE_SENSOR_EVENT_CHANGE_SENSITIVITY_SEL,
            HID_INPUT(Data_Arr_Abs),
        HID_END_COLLECTION,
        HID_USAGE_SENSOR_DATA_ENVIRONMENTAL_TEMPERATURE,
        HID_LOGICAL_MIN_16(0x01,0x80), //    LOGICAL_MINIMUM (-32767)
        HID_LOGICAL_MAX_16(0xFF,0x7F), //    LOGICAL_MAXIMUM (32767)
        HID_REPORT_SIZE(16),
        HID_REPORT_COUNT(1),
        HID_UNIT_EXPONENT(0x0E), // scale default unit “Celsius” to provide 2 digits past the decimal point
        HID_INPUT(Data_Var_Abs),

    HID_END_COLLECTION
#endif
};

/* USB handler declaration */
/* Handle for USB Full Speed IP */
USBD_HandleTypeDef *hUsbDevice_0;

extern USBD_HandleTypeDef hUsbDeviceFS;

/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state);
/* USER CODE BEGIN 2 */
/* USER CODE END 2 */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS = {
    CUSTOM_HID_ReportDesc_FS,
    CUSTOM_HID_Init_FS,
    CUSTOM_HID_DeInit_FS,
    CUSTOM_HID_OutEvent_FS,
};

/* Private functions ---------------------------------------------------------*/
/**
 * @brief  CUSTOM_HID_Init_FS
 *         Initializes the CUSTOM HID media low layer
 * @param  None
 * @retval Result of the operation: USBD_OK if all operations are OK else
 * USBD_FAIL
 */
static int8_t CUSTOM_HID_Init_FS(void) {
  hUsbDevice_0 = &hUsbDeviceFS;
  /* USER CODE BEGIN 4 */
  return (0);
  /* USER CODE END 4 */
}

/**
 * @brief  CUSTOM_HID_DeInit_FS
 *         DeInitializes the CUSTOM HID media low layer
 * @param  None
 * @retval Result of the operation: USBD_OK if all operations are OK else
 * USBD_FAIL
 */
static int8_t CUSTOM_HID_DeInit_FS(void) {
  /* USER CODE BEGIN 5 */
  return (0);
  /* USER CODE END 5 */
}

/**
 * @brief  CUSTOM_HID_OutEvent_FS
 *         Manage the CUSTOM HID class events
 * @param  event_idx: event index
 * @param  state: event state
 * @retval Result of the operation: USBD_OK if all operations are OK else
 * USBD_FAIL
 */
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t event_idx, uint8_t state) {
  /* USER CODE BEGIN 6 */
  return (0);
  /* USER CODE END 6 */
}

/* USER CODE BEGIN 7 */
/**
 * @brief  USBD_CUSTOM_HID_SendReport_FS
 *         Send the report to the Host
 * @param  report: the report to be sent
 * @param  len: the report length
 * @retval Result of the operation: USBD_OK if all operations are OK else
 * USBD_FAIL
 */

int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t *report, uint16_t len) {
  return hUsbDevice_0 ? USBD_CUSTOM_HID_SendReport(hUsbDevice_0, report, len)
                      : USBD_FAIL;
}
/* USER CODE END 7 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
