
#include <main.h>

/* This variable stores the connection handle value after connection is made */
CYBLE_CONN_HANDLE_T  connectionHandle;

int bleConnected = 0;
int doorNotify;

void GenericAppEventHandler(uint32 event, void *eventParam)
{
    CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;
    
    switch(event)
	{
    /**********************************************************
    *                       General Events
    ***********************************************************/
	case CYBLE_EVT_STACK_ON: 
		/* Start BLE Advertisement after BLE Stack is ON */
        /* Pag naka start na, mag start na ning advertisement */
		CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
		
		break;
        
	case CYBLE_EVT_TIMEOUT:
		/* Timeout has occured */
        if(CYBLE_GAP_ADV_MODE_TO == *(uint8 *) eventParam)
        {
            /* Advertisement timeout has occured - stop advertisement */
            CyBle_GappStopAdvertisement();
        }
	break;
     
    /**********************************************************
    *                       GAP Events
    ***********************************************************/
    case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
		/* Restart Advertisement if the state is disconnected */
		if(CyBle_GetState() == CYBLE_STATE_DISCONNECTED )
		{
			CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
		}

        break;

    case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
        /* Put the device to discoverable mode so that remote can search it. */
        CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
        bleConnected = 0;
		
		/* Stop PWM as no LED status is required*/
		LED_PWM_Stop();
		
		/* Set drive mode of Alert LED pin to High-Z to shutdown LED */
		Alert_LED_SetDriveMode(Alert_LED_DM_ALG_HIZ);
        break;
    /**********************************************************
    *                       GATT Events
    ***********************************************************/
    case CYBLE_EVT_GATT_CONNECT_IND:
		/* Extract connection handle */
        connectionHandle = *(CYBLE_CONN_HANDLE_T *)eventParam;	
        bleConnected = 1;
		
		/* Start PWM for LED status control */
		LED_PWM_Start();
        break;
        
        case CYBLE_EVT_GATTS_WRITE_REQ:
        /* This event is generated when the connected Central */
        /* device sends a Write request. */
        /* The parameter ‘eventParam’ contains the data written */
        /* Extract the Write data sent by Client */
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
            /* 'CYBLE_BUTTONPRESS_BUTTONPRESS_BUTTONPRESSCCD_DESC_INDEX' is auto generated when creating a custom profile, this is 
            shown at PSoC_4_BLE_Central_IAS_with_Custom_Profile\BLE_Dongle_Peripheral_IAS.cydsn\Generated_Source\PSoC4\BLE_custom.h */
            if (wrReqParam->handleValPair.attrHandle == CYBLE_BUTTONPRESS_BUTTONPRESS_BUTTONPRESSCCD_DESC_INDEX)
            {
                CyBle_GattsWriteAttributeValue(&wrReqParam->handleValPair, 0, &connectionHandle, CYBLE_GATT_DB_LOCALLY_INITIATED);
                doorNotify = wrReqParam->handleValPair.value.val[0];
            }
            CyBle_GattsWriteRsp(connectionHandle);
            break;

    default:
        break;
	}
}

void iasEventHandler(uint32 event, void *eventParam)
{
	/* Local variables */
    uint8 alertLevel;
    CYBLE_IAS_CHAR_VALUE_T *iasWrCmdValueParam;
    
    if(event == CYBLE_EVT_IASS_WRITE_CHAR_CMD)
    {
        /* Check if the event received was for writing Alert level */
        iasWrCmdValueParam = (CYBLE_IAS_CHAR_VALUE_T *)eventParam;
            
		/* If size of received attribute is equal to size of Alert level data */
        if(CYBLE_IAS_ALERT_LEVEL_SIZE == iasWrCmdValueParam->value->len)
        {
			/* Extract the alert level value received from Client device*/
            alertLevel = *((iasWrCmdValueParam->value->val));
            
            if (alertLevel == BUTTON_PRESS_TRUE)
            {
               	Alert_LED_SetDriveMode(Alert_LED_DM_STRONG);
				LED_PWM_WriteCompare(LED_HIGH_ALERT); 
            }
            else
            {
                Alert_LED_SetDriveMode(Alert_LED_DM_ALG_HIZ);
				LED_PWM_WriteCompare(LED_NO_ALERT);
            }
        }
    }
}
