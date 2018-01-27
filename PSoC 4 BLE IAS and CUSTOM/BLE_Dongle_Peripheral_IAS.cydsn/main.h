
#if !defined(MAIN_H)
#define MAIN_H
#include <project.h>
#include <math.h>
#include <BLEApplications.h>

/***************************************
*        API Constants
***************************************/
/* Alert Level values of IAS service as per BLE SIG spec*/

#define BUTTON_PRESS_TRUE    (1u)
#define BUTTON_PRESS_FALSE  (0u)
    
/* PWM compare values for various LED status */
#define LED_NO_ALERT       (1000u)
#define LED_HIGH_ALERT     (0u)

/* Local Macros */
#define TRUE				1
#define FALSE				0

/***************************************
*        Function Prototypes
***************************************/
void InitializeSystems(void);

#endif
/* [] END OF FILE */
