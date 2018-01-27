#include <main.h>
#include "debug.h"
#include "stdio.h"
#include <project.h>

/* 'list_of_devices' is an array of type 'CYBLE_GAPC_ADV_REPORT_T' (defined in 
 * BLE_StackGap.h) that is used to store the reports of the peripheral devices
* being scanned by this Central device*/
CYBLE_GAPC_ADV_REPORT_T 	list_of_devices[CYBLE_MAX_ADV_DEVICES];

/* 'connectPeriphDevice' is a variable of type 'CYBLE_GAP_BD_ADDR_T' (defined in 
* BLE_StackGap.h) and is used to store address of the connected device. */
CYBLE_GAP_BD_ADDR_T 		connectPeriphDevice;

/* 'connHandle' is a variable of type 'CYBLE_CONN_HANDLE_T' (defined in 
* BLE_StackGatt.h) and is used to store the connection handle parameters after
* connecting with the peripheral device. */
CYBLE_CONN_HANDLE_T			connHandle;

/* 'deviceConnected' flag tells the status of connection with BLE peripheral Device */
uint8 deviceConnected = FALSE;

/* 'ble_state' stores the state of connection which is used for updating LEDs */
uint8 ble_state = BLE_DISCONNECTED;

/* 'address_store' stores the addresses returned by Scanning results */
uint8 address_store[10][6];

/* 'peripheralAddress' stores the addresses of device presently connected to */
uint8 peripheralAddress[6];

/* 'peripheralFound' flag tells whether the peripheral device with the required Address
 * has been found during scanning or not. */
uint8 peripheralFound = FALSE;

/* 'addedDevices' tells the number of devices that has been added to the list till now. */
uint8 addedDevices = FALSE;

/* 'iasLevel' stores the current alert level as set by Central device */
extern uint8 iasLevel;

/* 'restartScanning' flag indicates to application whether starting scan API has to be
* called or not */
uint8 restartScanning = FALSE;

/* 'deviceName' is used to store the Device Name of the device connected */
char deviceName[30];


void ApplicationEventHandler(uint32 event, void *eventparam)
{
	/* 'apiResult' is a variable of type 'CYBLE_API_RESULT_T' (defined in 
	* BLE_StackTypes.h) and is used to store the return value from BLE APIs. */
	CYBLE_API_RESULT_T 			apiResult;
	
	/* 'scan_report' is a variable of type 'CYBLE_GAPC_ADV_REPORT_T' (defined in 
	* BLE_StackGap.h) and is used to store report retuned from Scan results. */
	CYBLE_GAPC_ADV_REPORT_T		scan_report;
    CYBLE_GATTC_READ_RSP_PARAM_T *readResponse;
    CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;

	/* Local variable for Loop */
	uint16 i = FALSE;
        
	switch(event)
	{
		case CYBLE_EVT_STACK_ON:
			/* Set start scanning flag to allow calling the API in main loop */
			restartScanning = TRUE;
		break;
			
		case CYBLE_EVT_GAPC_SCAN_START_STOP:
			/* Add relevant code here pertaining to Starting/Stopping of Scan*/
			if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED || CYBLE_STATE_STOPPED)
			{
				ble_state = BLE_DISCONNECTED;
				
				if(!peripheralFound)
				{
					/* Restart Scanning */
					Status_LED_Write(1);
					restartScanning = TRUE;
				}
			}
			else
			{
				ble_state = BLE_SCANNING;
			}
		break;
            
        case CYBLE_EVT_TIMEOUT:
            restartScanning = TRUE;
		break;
            
		case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
			/* This event is generated whenever there is a device found*/
			if(CYBLE_STATE_CONNECTED != CyBle_GetState())	
			{
				/* If we are not connected to any peripheral device, then save the new device  
					information so to add it to our list */
				scan_report = *(CYBLE_GAPC_ADV_REPORT_T*) eventparam;
				
				/* Add the new device to existing list if not done yet, and compare the address with our
					required address.*/
				HandleScanDevices(&scan_report);
			}
			/* Update the LED status for BLE scanning mode*/
			ble_state = BLE_SCANNING;
		break;
			
 		case CYBLE_EVT_GATT_CONNECT_IND:

			/* When the peripheral device is connected, store the connection handle.*/
            connHandle = *(CYBLE_CONN_HANDLE_T *)eventparam;
            break;
			
		case CYBLE_EVT_GATT_DISCONNECT_IND:
			/* When the peripheral device is disconnected, reset variables*/
			break;
			
		case CYBLE_EVT_GAP_DEVICE_CONNECTED:
			/* The Device is connected now. Start Attributes discovery process.*/
			apiResult = CyBle_GattcStartDiscovery(connHandle);
			
			if(apiResult != CYBLE_ERROR_OK)
			{

			}
			/* Update the LED status for BLE discovery mode*/
			ble_state = BLE_SERVICE_DISCOVERY;
        break;
			
		case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
			/* Reset all saved peripheral Addresses */
			for(i=0;i<addedDevices;i++)
			{
				list_of_devices[i].peerBdAddr[0] = FALSE;
				list_of_devices[i].peerBdAddr[1] = FALSE;
				list_of_devices[i].peerBdAddr[2] = FALSE;
				list_of_devices[i].peerBdAddr[3] = FALSE;
				list_of_devices[i].peerBdAddr[4] = FALSE;
				list_of_devices[i].peerBdAddr[5] = FALSE;
			}
			/* Reset application Flags on BLE Disconnect */
			addedDevices = FALSE;
			peripheralFound = FALSE;
			deviceConnected = FALSE;
			iasLevel = FALSE;
			ble_state = BLE_DISCONNECTED;
            memset(deviceName, 0, sizeof(deviceName));
			
			/* Set the flag for rescanning after wakeup */
			restartScanning = TRUE;
			
			/* Update LED Status for Disconnection */
			HandleLEDs(ble_state);
			if(apiResult != CYBLE_ERROR_OK)
			{
			}
			break;
			
		case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
			/* This event is generated whenever the discovery procedure is complete*/
			
			/*Set the Device connected flag*/
			deviceConnected = TRUE;
			
			/* Update the LED status for BLE discovery mode*/
			ble_state = BLE_CONNECTED;
		break;
            
        case CYBLE_EVT_GATTC_READ_RSP:
            readResponse = (CYBLE_GATTC_READ_RSP_PARAM_T *) eventparam;
            
            printf("Value: %x \r\n", readResponse->value.val[0]);
        break;    
            
		default:
			
		break;
	}
}

void HandleScanDevices(CYBLE_GAPC_ADV_REPORT_T* scanReport)
{
	uint16 i;
	uint8 newDevice = TRUE;

	if((addedDevices < CYBLE_MAX_ADV_DEVICES))
	{
		for(i = 0; i < addedDevices; i++)
		{
			/* Initialize the peerBdAddr element of our list.*/
			list_of_devices[i].peerBdAddr = &address_store[i][0];
			
			/* In this for loop, compare the new device address with the existing addresses in the list to 
				determine if the address is new or not. If the address exists in the list, then the device 
				is not new.*/
			if(FALSE == memcmp(list_of_devices[i].peerBdAddr, scanReport->peerBdAddr, ADV_ADDR_LEN))
			{
				newDevice = FALSE;
				break;
			}
		}
		
		if(newDevice)
		{
			/* If the device address is new, then add the device to our existing list and compare the address
				with our expected address to see if the desired peripheral is advertising or not.*/
			list_of_devices[addedDevices].peerBdAddr = &address_store[addedDevices][0];
			/* Store the data*/
			list_of_devices[addedDevices].dataLen = scanReport->dataLen;
			list_of_devices[addedDevices].data = scanReport->data;
			list_of_devices[addedDevices].eventType = scanReport->eventType;
			/* Record the address type, Public or Random, of the advertising peripheral.*/
			list_of_devices[addedDevices].peerAddrType = scanReport->peerAddrType;
			
			/* Save the BD addresses */
            for (i=0; i<6; i++)
            {
                list_of_devices[addedDevices].peerBdAddr[i] = scanReport->peerBdAddr[i];
            }
			list_of_devices[addedDevices].rssi = scanReport->rssi;
			
			/* If the new BD address found matches the desired BD address, the Dongle has been found*/
			if(FALSE == memcmp(peripheralAddress, scanReport->peerBdAddr, ADV_ADDR_LEN))
			{
                /* 'n' variable is used to count the characters for the deviceName */
                uint16 n = FALSE;
				/* Save the connected device BD Address and Type*/
                for (i=0; i<6; i++)
                {
                    connectPeriphDevice.bdAddr[i] = scanReport->peerBdAddr[i];
                }
				
				connectPeriphDevice.type = list_of_devices[addedDevices].peerAddrType;
                n = (uint8)scanReport->data[3];
                printf("%d \r\n", n);
                
                for (i=0; i < n-1; i++)
                {
                    deviceName[i] = scanReport->data[5+i];
                }
                
				/* Set the flag to notify application of a connected peripheral device */
				peripheralFound = TRUE;

				/* Stop existing BLE Scan */
				CyBle_GapcStopScan();
			}
			
			addedDevices++;
		}
		
		newDevice = TRUE;
	}
}

void LoadPeripheralDeviceData(void)
{
		peripheralAddress[5] = 0x00;
		peripheralAddress[4] = 0xA0;
		peripheralAddress[3] = 0x50;
		peripheralAddress[2] = 0x65;
		peripheralAddress[1] = 0x43;
		peripheralAddress[0] = 0x25;
}
/* [] END OF FILE */
