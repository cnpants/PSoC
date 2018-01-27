#include <main.h>

#include <debug.h>
/* 'CYBLE_BUTTONPRESS_BUTTONPRESS_CHAR_HANDLE' is the characteristic handle UUID 
which  writes the changed value from the Server/Dongle copied from 
PSoC_4_BLE_Central_IAS_with_Custom_Profile\BLE_Dongle_Peripheral_IAS.cydsn\Generated_Source\PSoC4\BLE_custom.h */
#define CYBLE_BUTTONPRESS_BUTTONPRESS_CHAR_HANDLE   (0x0011u)

extern uint8 ble_state;
extern uint8 deviceConnected;
extern uint8 peripheralFound;
extern uint8 restartScanning;

/* 'connectPeriphDevice' is a variable of type 'CYBLE_GAP_BD_ADDR_T' (defined in 
* BLE_StackGap.h) and is used to store address of the connected device. */
extern CYBLE_GAP_BD_ADDR_T connectPeriphDevice;
extern CYBLE_CONN_HANDLE_T connHandle;
extern char deviceName[15];

int main()
{
    /* Initialize the system */
	InitializeSystem();
	
    for(;;)
    {
        /*Process Event callback to handle BLE events. The events generated and 
		* used for this application are inside the 'ApplicationEventHandler' routine */
		CyBle_ProcessEvents();

		/* Handle BLE Status LED */
		HandleLEDs(ble_state);
		
		if(peripheralFound)
		{
			/* If the desired peripheral is found, then connect to that peripheral */
			CyBle_GapcConnectDevice(&connectPeriphDevice);
            printf("%s \r\n", deviceName);

			/* Call CyBle_ProcessEvents() once to process the Connect request sent above. */
			CyBle_ProcessEvents();
		
			/* Reset flag to prevent resending the Connect command */
			peripheralFound = FALSE;
		}
		if(deviceConnected)
		{
			/* Check if the button was pressed and update the Alert Level Accordingly.*/
			CheckSwitchStatus();
            CyBle_GattcReadCharacteristicValue(connHandle, CYBLE_BUTTONPRESS_BUTTONPRESS_CHAR_HANDLE);
		}
		/* If rescanning flag is TRUE, the restart the scan */
		if(restartScanning)
		{
			/* Reset flag to prevent calling multiple Start Scan commands */
			restartScanning = FALSE;
			
			/* Start Fast BLE Scanning. This API will only take effect after calling
			* CyBle_ProcessEvents()*/
			CyBle_GapcStartScan(CYBLE_SCANNING_FAST);
		}
	}
}

void InitializeSystem(void)
{
	/* Enable global interrupts. */
	CyGlobalIntEnable; 
	
	/* Start BLE component with appropriate Event handler function */
	CyBle_Start(ApplicationEventHandler);

    UART_Start();
    UART_SpiUartClearTxBuffer();
	/* Load the BD address of peripheral device to which we have to connect */
	LoadPeripheralDeviceData(); 

	/* Start the Button ISR and indicate the ISR routine */
	isr_button_StartEx(MyISR);
}
/* [] END OF FILE */
