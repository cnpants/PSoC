
#include <main.h>

extern int doorNotify;
extern int bleConnected;
extern CYBLE_CONN_HANDLE_T  connectionHandle;

int main()
{
	/* Start the components */
	InitializeSystems();
    
    for(;;)
    {
        /* CyBle_ProcessEvents() allows BLE stack to process pending events */
        CyBle_ProcessEvents();
        
        if (bleConnected)
        {
            uint8 button = SW2_Read();
            CYBLE_GATTS_HANDLE_VALUE_NTF_T buttonHandle;
                
            buttonHandle.attrHandle = CYBLE_BUTTONPRESS_BUTTONPRESS_CHAR_HANDLE;
            buttonHandle.value.len = 1;
            buttonHandle.value.val = &button;
                
            CyBle_GattsWriteAttributeValue(&buttonHandle, 0, &connectionHandle, 0);
            if (doorNotify)
                CyBle_GattsNotification(connectionHandle, &buttonHandle);
        }
        
    }
}


void InitializeSystems(void)
{
	/* Enable Global Interrupt */
	CyGlobalIntEnable;
	
	/* Start PWM. This PWM controls the LED status */
	LED_PWM_Start();
	
    /* Start CYBLE component and register generic event handler */
    CyBle_Start(GenericAppEventHandler);
	
    /* register the event handler for IAS specific events */
    CyBle_IasRegisterAttrCallback(iasEventHandler);
	
	/* Set drive mode of Alert LED pin to High-Z to shutdown LED */
	Alert_LED_SetDriveMode(Alert_LED_DM_ALG_HIZ);
}

