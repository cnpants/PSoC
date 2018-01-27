
#include <main.h>




void MyISR(void)
{
	/* Clear Pending interrupts */
	isr_button_ClearPending();
	
	User_Button_ClearInterrupt();
}
/* [] END OF FILE */
