
#if !defined (BLECLIENT_H)
#define BLECLIENT_H
#include <project.h>

/* Maximum number of devices this project will keep track of */
#define CYBLE_MAX_ADV_DEVICES        10u
	
/* BD Address Length*/
#define ADV_ADDR_LEN				0x06
	
/* Public Address indication used for Scanning */
#define PUBLIC_ADDR_TYPE			0x00
	
/* Random Address indication used for Scanning */
#define RANDOM_ADDR_TYPE			0x01

/**************************Function Declarations*****************************/
void HandleScanDevices(CYBLE_GAPC_ADV_REPORT_T* scanReport);
void LoadPeripheralDeviceData(void);
/****************************************************************************/

#endif
/* [] END OF FILE */
