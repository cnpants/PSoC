#include <main.h>
/* 'connHandle' contains the connection handle parameter and used for BLE API to
* send data to particular connected device */
extern CYBLE_CONN_HANDLE_T			connHandle;

/* 'ble_state' contains the present status of BLE, used for LED state update */
extern uint8 ble_state;

/* 'iasLevel' contains the alert level that is sent to the connected peripheral 
* device */
uint8 iasLevel = FALSE;

void CheckSwitchStatus(void)
{
    if (User_Button_Read() == FALSE)
    {
        iasLevel = TRUE;
    }
    else
    {
        iasLevel = FALSE;
    }
    SetAlertLevel(&iasLevel);
}


void HandleLEDs(uint8 state)
{
	static uint16 led_scanning_counter = TRUE;
	static uint16 led_discovery_counter = TRUE;
	
	switch(state)
	{
		case BLE_DISCONNECTED:
			/* Set LED to OFF for Disconnected state*/
			Status_LED_Write(LED_OFF);
		break;
		
		case BLE_SCANNING:
			/* Run internal software timer to blink status LED, 
			* indicating ongoing BLE scan */
			if((--led_scanning_counter) == FALSE)
			{
				led_scanning_counter = LED_SCANNING_COUNTER_VALUE;
				Status_LED_Write(!Status_LED_Read());
			}
		break;
			
		case BLE_SERVICE_DISCOVERY:
			/* Run internal software timer to blink status LED, 
			* indicating BLE discovery procedure with connected device */
			if((--led_discovery_counter) == FALSE)
			{
				led_scanning_counter = LED_DISCOVERY_COUNTER_VALUE;
				Status_LED_Write(!Status_LED_Read());
			}
		break;
		
		case BLE_CONNECTED:
			/* Set the LED to ON to indicate BLE connected state */
			Status_LED_Write(LED_ON);
		break;
		
		default:
		
		break;
	}
}


void SetAlertLevel(uint8* alertLevel)
{
	CYBLE_API_RESULT_T 			apiResult;
	
	/* Set the IAS characteristic with provided alert level. Alert Level can be either
	* NO alert, MID alert or HIGH alert */
	apiResult = CyBle_IascSetCharacteristicValue(connHandle, CYBLE_IAS_ALERT_LEVEL ,IAS_ATTR_SIZE, alertLevel);
	
	if(apiResult != CYBLE_ERROR_OK)
	{
		
	}	
}

/* [] END OF FILE */
