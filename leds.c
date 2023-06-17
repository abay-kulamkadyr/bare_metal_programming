// GPIO LED demo
#include "leds.h"
#include "consoleUtils.h"
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "gpio_v2.h"
#include "hw_types.h"      // For HWREG(...) macro

/*****************************************************************************
 **                INTERNAL MACRO DEFINITIONS
 *****************************************************************************/
#define LED_GPIO_BASE           (SOC_GPIO_1_REGS)
#define LED0_PIN (21)
#define LED1_PIN (22)
#define LED2_PIN (23)
#define LED3_PIN (24)
#define LED_NUM  (4)
#define LED_MASK ((1<<LED0_PIN) | (1<<LED1_PIN) | (1<<LED2_PIN) | (1<<LED3_PIN))
#define DEFAULT_SPEED 5
//#define DELAY_TIME 0x4000000		// Delay with MMU enabled
#define DELAY_TIME 0x40000		// Delay witouth MMU and cache

static int32_t led_speed;
static int32_t flash_time_period;
static int32_t current_pin_to_flash; 
static _Bool 	reachedLastPin = false;
static int32_t counter;
static int32_t indexToPinDictionary[LED_NUM] = {LED0_PIN, LED1_PIN, LED2_PIN, LED3_PIN};
/*****************************************************************************
 **                INTERNAL FUNCTION PROTOTYPES
 *****************************************************************************/
static void displayBouncingPattern(void);
static void turnLEDOn(uint32_t pinNumber);
static inline void calculatePeriodFromSpeed(int32_t speed);
static inline uint32_t mapIndexToPinNumber(uint32_t ledPinNum);
static uint32_t incrementCurrAndGetNextPin();
/*****************************************************************************
 **                INTERNAL FUNCTION DEFINITIONS
 *****************************************************************************/

static inline void calculatePeriodFromSpeed(int32_t speed)
{
	flash_time_period = 2 << (9-speed);
}
static inline uint32_t mapIndexToPinNumber(uint32_t ledPinNum)
{
	if (ledPinNum>=0 && ledPinNum < LED_NUM){
		return indexToPinDictionary[ledPinNum];
	} else {
		ConsoleUtilsPrintf("Invalid pin number\n");
	}
	return -1;
}
static uint32_t incrementCurrAndGetNextPin()
{
	if(!reachedLastPin) {
		current_pin_to_flash++;
		if(current_pin_to_flash % LED_NUM == 0) {
			current_pin_to_flash-=2;
			reachedLastPin = true;
		}
	} else {
		current_pin_to_flash--;
		if(current_pin_to_flash < 0)
		{
			current_pin_to_flash+=2;
			reachedLastPin = false;

		}
	}
	return mapIndexToPinNumber(current_pin_to_flash);
}
void LEDs_Init(void)
{
	/* Enabling functional clocks for GPIO1 instance. */
	GPIO1ModuleClkConfig();

	/* Selecting GPIO1[23] pin for use. */
	//GPIO1Pin23PinMuxSetup();

	/* Enabling the GPIO module. */
	GPIOModuleEnable(LED_GPIO_BASE);

	/* Resetting the GPIO module. */
	GPIOModuleReset(LED_GPIO_BASE);

	/* Setting the GPIO pin as an output pin. */
	GPIODirModeSet(LED_GPIO_BASE,
			LED0_PIN,
			GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
			LED1_PIN,
			GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
			LED2_PIN,
			GPIO_DIR_OUTPUT);
	GPIODirModeSet(LED_GPIO_BASE,
			LED3_PIN,
			GPIO_DIR_OUTPUT);

	led_speed = DEFAULT_SPEED;
	current_pin_to_flash = 0;
	counter = 0;
	calculatePeriodFromSpeed(led_speed);
}
static void turnLEDOn(uint32_t pinNumber)
{
	/*Turn off all LEDs first*/

	/* Driving a logic LOW on the GPIO pin. */
	GPIOPinWrite(LED_GPIO_BASE,
			LED0_PIN,
			GPIO_PIN_LOW);
	/* Driving a logic LOW on the GPIO pin. */
	GPIOPinWrite(LED_GPIO_BASE,
			LED1_PIN,
			GPIO_PIN_LOW);
	/* Driving a logic LOW on the GPIO pin. */
	GPIOPinWrite(LED_GPIO_BASE,
			LED2_PIN,
			GPIO_PIN_LOW);
	/* Driving a logic LOW on the GPIO pin. */
	GPIOPinWrite(LED_GPIO_BASE,
			LED3_PIN,
			GPIO_PIN_LOW);

	/* Driving a logic HIGH on the GPIO pin. */
	GPIOPinWrite(LED_GPIO_BASE,
			pinNumber,
			GPIO_PIN_HIGH);
}
void LEDs_NotifyOnTimerIsr(void)
{
	displayBouncingPattern();
}
static void displayBouncingPattern(void)
{
	counter++;
	if(counter >= flash_time_period){
		turnLEDOn(mapIndexToPinNumber(current_pin_to_flash));
		incrementCurrAndGetNextPin();
		counter = 0;
	}
}
void LEDs_SetBouncingSpeedTo(int32_t speed)
{
	if(speed >= 0 && speed <= 9) {
		led_speed = speed;
		calculatePeriodFromSpeed (led_speed);
		ConsoleUtilsPrintf("\nSetting LED speed to %d\n", led_speed);

	} else {
		ConsoleUtilsPrintf("\nSpeed must be in the range [0,9] \n");
	}
}
void LEDs_BouncingSpeedUp(void)
{
	if(led_speed >= 0 && led_speed < 9) {
		led_speed++;
		calculatePeriodFromSpeed (led_speed);
	}
	ConsoleUtilsPrintf("\nSetting LED speed to %d\n", led_speed);
}
void LEDs_BouncingSpeedDown(void)
{
	if(led_speed > 0 && led_speed <= 9) {
		led_speed--;
		calculatePeriodFromSpeed (led_speed);
	}
	ConsoleUtilsPrintf("\nSetting LED speed to %d\n", led_speed);
}




