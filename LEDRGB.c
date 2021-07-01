
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_pit.h"
#include "fsl_debug_console.h"
#include "fsl_pit.h"
#include "fsl_clock.h"
#include "fsl_gpio.h"

#include "LEDRGB.h"

void LedBegin(){

	gpio_pin_config_t led_config={
		kGPIO_DigitalOutput,
		1,
	};

	CLOCK_EnableClock(kCLOCK_PortB);
	CLOCK_EnableClock(kCLOCK_PortE);

	PORT_SetPinMux(PORTB, LED_BLUE, kPORT_MuxAsGpio);
	PORT_SetPinMux(PORTB, LED_RED, kPORT_MuxAsGpio);
	PORT_SetPinMux(PORTE, LED_GREEN, kPORT_MuxAsGpio);

	GPIO_PinInit(GPIOB, LED_BLUE, &led_config);
	GPIO_PinInit(GPIOB, LED_RED, &led_config);
	GPIO_PinInit(GPIOE, LED_GREEN, &led_config);

}


void LedOff(){
	GPIO_PortSet(GPIOB, 1U << LED_BLUE);
	GPIO_PortSet(GPIOB, 1U << LED_RED);
	GPIO_PortSet(GPIOB, 1U << LED_GREEN);
}

void LedColor(int color){
	switch (color){
		case blue:
			LedOff();
			GPIO_PortClear(GPIOB, 1U << LED_BLUE);

		break;

		case red:
			LedOff();
			GPIO_PortClear(GPIOB, 1U << LED_RED);

		break;
		case green:
			LedOff();
			GPIO_PortClear(GPIOE, 1U << LED_GREEN);

		break;
		case white:
			LedOff();
			GPIO_PortClear(GPIOB, 1U << LED_BLUE);
			GPIO_PortClear(GPIOB, 1U << LED_RED);
			GPIO_PortClear(GPIOE, 1U << LED_GREEN);

		break;
		case purple:
			LedOff();
			GPIO_PortClear(GPIOB, 1U << LED_BLUE);
			GPIO_PortClear(GPIOB, 1U << LED_RED);

		break;
		case yellow:
			LedOff();
			GPIO_PortClear(GPIOB, 1U << LED_RED);
			GPIO_PortClear(GPIOE, 1U << LED_GREEN);

		break;
		default:
			LedOff();
		break;
	}
}

