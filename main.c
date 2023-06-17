// Fake Typing bare metal sample application
// On the serial port, fakes

#include "consoleUtils.h"
#include <stdint.h>
#include "hw_types.h"      // For HWREG(...) macro
// My hardware abstraction modules
#include "serial.h"
#include "timer.h"
#include "wdtimer.h"
#include "leds.h"
#include "joystick.h"
// My application's modules
#include "fakeTyper.h"
#include <stdint.h>
/******************************************************************************
 **              MACROS FOR THE RESET SOURCE REGISTER 
 ******************************************************************************/
#define SRC_RST_REG_BASE  			(0x44E00F00)  		/*base address*/
#define SRC_RST_REG_OFFSET			(0x08)		  		/*offset*/
#define ICEPICK_EVENT_BIT			(0x09)				/*ice pick reset, global warm event initiated by the emulation */
#define EXTERNAL_WARM_EVENT_BIT		(0x05)				/*external warm reset event*/
#define WDT1_EVENT_BIT				(0x04)				/*watchdog timer reset event*/
#define GLOBAL_WARM_SW_EVENT_BIT	(0x01)				/*global warm software reset event*/
#define GLOBAL_COLD_SW_EVENT_BIT	(0x00)				/*power-on (cold) reset event*/
/*mask for clearing all events from the source reset register*/
#define CLEAR_SRC_RST_REG_MASK		((1 << ICEPICK_EVENT_BIT) + (1 << EXTERNAL_WARM_EVENT_BIT) + (1 << WDT1_EVENT_BIT) + (1 << GLOBAL_WARM_SW_EVENT_BIT) + (1 << GLOBAL_COLD_SW_EVENT_BIT))

/*read value from the source reset register and clears all the occured events*/
static uint32_t readResetSourceRegister(void);
/*displays all events from the value of the source reset register */
static void displayResetEvents (int32_t regValue);
/*display help message if asked from the user*/
static void displayHelpMessage(void);
/*
Commands:
 ? : Display this help message
 0-9 : Set speed 0 (slow) to 9 (fast)
 x : Stop hitting the watchdog
 JOY : Up (faster), Down (slower) 
 */
static void displayHelpMessage(void)
{
	ConsoleUtilsPrintf("\nCommands:\n");
	ConsoleUtilsPrintf("?\t: Display this help message\n");
	ConsoleUtilsPrintf("0-9\t: Set speed 0 (slow) to 9 (fast)\n");
	ConsoleUtilsPrintf("x\t: Stop hitting the watchdog\n");
	ConsoleUtilsPrintf("JOY\t: Up (faster), Down (slower)\n");
	
}


/******************************************************************************
 **              SERIAL PORT HANDLING
 ******************************************************************************/
static volatile uint8_t s_rxByte = 0;
static void serialRxIsrCallback(uint8_t rxByte) {
	s_rxByte = rxByte;
}

static void doBackgroundSerialWork(void)
{
	if (s_rxByte != 0) {
		// Tell a joke
		switch (s_rxByte)
		{
		case '?':
			displayHelpMessage();
			break;
		case '0':
			LEDs_SetBouncingSpeedTo(0);
			break;
		case '1':
			LEDs_SetBouncingSpeedTo(1);
			break;
		case '2':
			LEDs_SetBouncingSpeedTo(2);
			break;
		case '3':
			LEDs_SetBouncingSpeedTo(3);
			break;
		case '4':
			LEDs_SetBouncingSpeedTo(4);
			break;
		case '5':
			LEDs_SetBouncingSpeedTo(5);
			break;
		case '6':
			LEDs_SetBouncingSpeedTo(6);
			break;
		case '7':
			LEDs_SetBouncingSpeedTo(7);
			break;
		case '8':
			LEDs_SetBouncingSpeedTo(8);
			break;
		case '9':
			LEDs_SetBouncingSpeedTo(9);
			break;
		case 'x':
			Watchdog_StopHittingWatchDog();
			ConsoleUtilsPrintf("\nNo longer hitting the watchdog.\n");
			break;
		default:
			ConsoleUtilsPrintf("\nInvalid Command.\n");
			displayHelpMessage();
			break;
		}
		s_rxByte = 0;
	}
}

/******************************************************************************
 **              MAIN
 ******************************************************************************/
int main(void)
{
	// Initialization
	Serial_init(serialRxIsrCallback);
	
	Timer_init();
	Watchdog_init();
	FakeTyper_init();
	Joystick_Init();
	LEDs_Init();

	// Setup callbacks from hardware abstraction modules to application:
	Serial_setRxIsrCallback(serialRxIsrCallback);
	Timer_setTimerIsrCallback(FakeTyper_notifyOnTimeIsr);
	Timer_setTimerIsrCallback(LEDs_NotifyOnTimerIsr);
	
	// Display startup messages to console:
	ConsoleUtilsPrintf("LightBouncer:\n\tby Abay Kulamkadyr\n\t------------------\n");
	
	uint32_t resetSrcReg_value = readResetSourceRegister();
	displayResetEvents(resetSrcReg_value);
	displayHelpMessage();
	
	// Main loop:
	while(1) {
		// Handle background processing
		doBackgroundSerialWork();
		FakeTyper_doBackgroundWork();
		Joystick_Read();
		// Timer ISR signals intermittent background activity.
		if(Timer_isIsrFlagSet()) {
			Timer_clearIsrFlag();
			/*hit the watchdog if the user hasn't typed 'x' character*/
			Watchdog_hit();
		}
	}
}
void displayResetEvents (int32_t regValue)
{
	if((regValue & (1 << ICEPICK_EVENT_BIT)) != 0) {

		ConsoleUtilsPrintf("Reset Source (0x200): IcePick (warm), ");
	}

	if((regValue & (1 << EXTERNAL_WARM_EVENT_BIT)) != 0) {
		
		ConsoleUtilsPrintf("Reset Source (0x20): External(warm), ");

	}
	if((regValue & (1 << WDT1_EVENT_BIT)) != 0) {
		
		ConsoleUtilsPrintf("Reset Source (0x10): WatchDog timer, ");
		
	}
	if((regValue & (1 << GLOBAL_WARM_SW_EVENT_BIT)) != 0) {
		
		ConsoleUtilsPrintf("Reset Source (0x02): Global warm software, ");
		
	}
	if((regValue & (1 << GLOBAL_COLD_SW_EVENT_BIT)) != 0) {

		ConsoleUtilsPrintf("Reset Source (0x01): Power-on (cold), ");
		
	}
	ConsoleUtilsPrintf("\n");
}
uint32_t readResetSourceRegister(void)
{
	uint32_t regValue = HWREG ( SRC_RST_REG_BASE + SRC_RST_REG_OFFSET );
	/*clear the registers's bits by writing 1s*/
	HWREG(SRC_RST_REG_BASE + SRC_RST_REG_OFFSET) |= CLEAR_SRC_RST_REG_MASK;
	return regValue;
}

