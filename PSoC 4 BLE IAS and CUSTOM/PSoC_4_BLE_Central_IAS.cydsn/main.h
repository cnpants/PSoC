
#if !defined (MAIN_H)
#define MAIN_H
#include <project.h>
#include <BLEclient.h>
#include <string.h>
#include <BLEApplications.h>
#include <HandleLowPower.h>

/* Macros for Logical comparisons */
#define FALSE						0
#define TRUE						1

/**************************Function Declarations*****************************/
void InitializeSystem(void);
void ClientEventHandler(uint32 event, void* eventparam);
void ApplicationEventHandler(uint32 event, void *eventparam);
/****************************************************************************/

#endif

/* [] END OF FILE */
