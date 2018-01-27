
#if !defined (BLEAPPLICATIONS_H)
#define BLEAPPLICATIONS_H
#include <project.h>

/*************************Pre-processor directives****************************/
/* 'ENABLE_LOW_POWER_MODE' pre-processor directive enables the low power mode 
* handling in the firmware, ensuring low power consumption during project usage.
* To disable, comment the following #define. 
* If disabled, prevent usage of the project with coin cell */
#define ENABLE_LOW_POWER_MODE
/****************************************************************************/
	
/* BLE State Macros used for LED status updates*/
#define BLE_DISCONNECTED				0x01
#define BLE_SCANNING					0x02
#define BLE_SERVICE_DISCOVERY			0x03
#define BLE_CONNECTED					0x04

/* Size of IAS Data */
#define IAS_ATTR_SIZE					0x01

/* LED State Macros*/
#define LED_OFF							0x01
#define LED_ON							0x00
	
/* Counter default values used for LED status update during various 
* states BLE */
#ifdef ENABLE_LOW_POWER_MODE
#define LED_SCANNING_COUNTER_VALUE		60
#define LED_DISCOVERY_COUNTER_VALUE		5
#else
#define LED_SCANNING_COUNTER_VALUE		10000
#define LED_DISCOVERY_COUNTER_VALUE		1000	
#endif

/* User button related Macros for Debouncing and Detection*/
#define USER_BUTTON_PUSHED_MASK			0x01
#define	USER_BUTTON_DETECTED_MASK		0x02
#define SWITCH_DEBOUNCE_DELAY			4

/**************************Function Declarations*****************************/
void CheckButtonStatus(void);
void HandleLEDs(uint8 state);
void SetAlertLevel(uint8* alertLevel);
void HandleLowPowerMode(void);
void MyISR(void);
void CheckSwitchStatus(void);
/****************************************************************************/

#endif
/* [] END OF FILE */
