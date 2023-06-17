#include "joystick.h"
#include "leds.h"
// GPIO Button demo
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "gpio_v2.h"
#include "hw_types.h"      // For HWREG(...) macro
#include "watchdog.h"
#include "uart_irda_cir.h"
#include "consoleUtils.h"
#include <stdint.h>

#define JOYSTICK_UP_GPIO_BASE       (SOC_GPIO_0_REGS)
#define JOYSTICK_DOWN_GPIO_BASE     (SOC_GPIO_1_REGS)

#define UP_PIN                      26
#define DOWN_PIN                    14
#define UP_INDEX                    0
#define DOWN_INDEX                  1
#define NUM_PINS                    2

struct pin_info_t {
    int32_t base;
    uint8_t pin;
};
static struct pin_info_t pins_info [2];
static void initializeJoystickPins(void);

static void initializeJoystickPins(void)
{
    /* Enabling functional clocks for GPIO0, GPIO1 instance. */
	GPIO0ModuleClkConfig();
    GPIO1ModuleClkConfig();
 
    for(int i=0; i < NUM_PINS; i++)
    {
        /* Enabling the GPIO module. */ 
        GPIOModuleEnable(pins_info[i].base);
        /* Resetting the GPIO module. */
        GPIOModuleReset(pins_info[i].base);
        /* Setting the GPIO pin as an input pin. */
        GPIODirModeSet( pins_info[i].base,
                        pins_info[i].pin,
                        GPIO_DIR_INPUT);
    }
}


void Joystick_Init(void)
{
    pins_info[UP_INDEX].base =      JOYSTICK_UP_GPIO_BASE;
    pins_info[DOWN_INDEX].base =    JOYSTICK_DOWN_GPIO_BASE;
    pins_info[UP_INDEX].pin =       UP_PIN;
    pins_info[DOWN_INDEX].pin =     DOWN_PIN;
    
    initializeJoystickPins();
}
void Joystick_Read(void)
{
    if(GPIOPinRead(pins_info[UP_INDEX].base, pins_info[UP_INDEX].pin) == 0)
    {
        /*holding the joystick has no effect*/
        while(GPIOPinRead(pins_info[UP_INDEX].base, pins_info[UP_INDEX].pin) == 0);
        LEDs_BouncingSpeedUp();
    }
    if(GPIOPinRead(pins_info[DOWN_INDEX].base,pins_info[DOWN_INDEX].pin) == 0)
    {   
        /*holding the joystick has no effect*/
        while(GPIOPinRead(pins_info[DOWN_INDEX].base,pins_info[DOWN_INDEX].pin) == 0);
        LEDs_BouncingSpeedDown();
    }
}