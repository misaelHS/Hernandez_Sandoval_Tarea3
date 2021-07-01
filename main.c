
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_pit.h"
#include "fsl_debug_console.h"
#include "fsl_pit.h"
#include "fsl_clock.h"
#include "fsl_gpio.h"

#include "LEDRGB.h"

#define SW2 6u
#define SW3 4u

gpio_pin_config_t config_input = {
    kGPIO_DigitalInput,
    1,
};

const port_pin_config_t sw_gpio = {
    kPORT_PullUp,
    kPORT_FastSlewRate,
    kPORT_PassiveFilterDisable,
    kPORT_OpenDrainDisable,
    kPORT_LowDriveStrength,
    kPORT_MuxAsGpio,
    kPORT_UnlockRegister
  };

volatile bool pitIsrFlag = false;
volatile bool SW2_presionado = false;
volatile bool SW3_presionado = false;

void PORTA_IRQHandler(void){
	   GPIO_PortClearInterruptFlags(GPIOA, 1U << SW3);
	   SW3_presionado = true;
	   SDK_ISR_EXIT_BARRIER;
}

void PORTC_IRQHandler(void){
	   GPIO_PortClearInterruptFlags(GPIOC, 1U << SW2);
	   SW2_presionado = true;
	   SDK_ISR_EXIT_BARRIER;
}
void PIT0_IRQHandler(void)
{
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
    pitIsrFlag = true;
    __DSB();
}

int main(void){

	CLOCK_EnableClock(kCLOCK_PortA);
	CLOCK_EnableClock(kCLOCK_PortC);

	LedBegin();

	PORT_SetPinConfig(PORTC, SW2, &sw_gpio);
	PORT_SetPinConfig(PORTA, SW3, &sw_gpio);

	PORT_SetPinInterruptConfig(PORTC, SW2, kPORT_InterruptFallingEdge);
	PORT_SetPinInterruptConfig(PORTA, SW3, kPORT_InterruptFallingEdge);

	GPIO_PinInit(GPIOC, SW2, &config_input);
	GPIO_PinInit(GPIOA, SW3, &config_input);

	pit_config_t pitConfig;

	BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    PIT_Init(PIT, &pitConfig);
    PIT_SetTimerPeriod(PIT,kPIT_Chnl_0, USEC_TO_COUNT(1000000U, CLOCK_GetFreq(kCLOCK_BusClk)));
    PIT_EnableInterrupts(PIT, kPIT_Chnl_0 , kPIT_TimerInterruptEnable);
    EnableIRQ(PIT0_IRQn);
    PIT_StartTimer(PIT, kPIT_Chnl_0);

	NVIC_EnableIRQ(PORTA_IRQn);
	NVIC_EnableIRQ(PORTC_IRQn);
	NVIC_EnableIRQ(PIT0_IRQn);

	NVIC_SetPriority(PORTA_IRQn, 2);
	NVIC_SetPriority(PORTC_IRQn, 2);
	NVIC_SetPriority(PIT0_IRQn, 2);

	PIT_StartTimer(PIT, kPIT_Chnl_0);

	typedef enum {UNO, DOS, TRES, CUATRO} estados_externos_t;
	estados_externos_t estado_externo = UNO;

	typedef enum {Yellow, Red, Purple, Green, Blue, White} estados_internos_t;
	estados_internos_t estado_interno = yellow;

	while(true){
		switch (estado_externo) {
		    	case UNO: 	//CASO 1
		    		switch (estado_interno) {
		    				case Yellow:
		    					LedColor(yellow);
		    					if(pitIsrFlag==true){
		    						estado_interno = Red;
		    						pitIsrFlag=false;
		    					}
		    					break;
		    				case Red:
		    					LedColor(red);
		    				   	if(pitIsrFlag==true){
		    				   		estado_interno = Purple;
		    				   		pitIsrFlag=false;
		    				   	}
		    				   	break;
		    				case Purple:
		    					LedColor(purple);
		    					if(pitIsrFlag==true){
		    						estado_interno = Yellow;
		    						pitIsrFlag=false;
		    					}
		    				break;
		    				default:
		    					estado_interno = Yellow;
		    				break;

		    		}
		    	if(SW2_presionado){
		    		estado_externo = DOS;
		    		estado_interno = Green;
		    		SW2_presionado = false;
		    	}
		    	if(SW3_presionado){
		    		SW3_presionado = false;
		    	}
		    	break;

		    	case DOS: //CASO 2
		    		switch (estado_interno) {
		    		   	case Green:
		    		   		LedColor(green);
		    				if(pitIsrFlag==true){
		    					estado_interno = blue;
		    					pitIsrFlag=false;
		    				}
		    				break;
				    	case Blue:
				    		LedColor(blue);
		    				if(pitIsrFlag==true){
		    					estado_interno = Red;
		    					pitIsrFlag=false;
		    				 }
		    				 break;
	  			    	case Red:
	  			    		LedColor(red);
		    				 if(pitIsrFlag==true){
		    					 estado_interno = Green;
		    					 pitIsrFlag=false;
		    				 }
		    			  	 break;
		    	    	default:
		    				estado_interno = Green;
		    				break;
		    		}
		    		if(SW2_presionado){
		    	   		SW2_presionado = false;
		    		}
		    		if(SW3_presionado){
		    			estado_externo = TRES;
		    			estado_interno = Blue;
		    			SW3_presionado = false;
		        	}
			    	break;

		    	case TRES: //CASO 3
		    		switch (estado_interno) {
		    			case Blue:
		    				LedColor(blue);
		    				if(pitIsrFlag==true){
		    					estado_interno = White;
		    					pitIsrFlag=false;
		    				}
		    				break;
		    			case White:
		    				LedColor(white);
		    				if(pitIsrFlag==true){
		    					estado_interno = Red;
		    					pitIsrFlag=false;
		    				}
		    				break;
		    			case Red:
		    				LedColor(red);
		    				if(pitIsrFlag==true){
		    					estado_interno = Green;
		    					pitIsrFlag=false;
		    				}
		    				break;
		    				default:
		    				estado_interno = Blue;
		    				break;
		    			}
		    			if(SW2_presionado){
		    				SW2_presionado = false;
		    			}
		    			if(SW3_presionado){
		    				estado_externo = CUATRO;
		    				estado_interno = Green;
		    				SW3_presionado = false;
		    			}
		    			break;
		    	case CUATRO: //CASO 4
		    		switch (estado_interno) {
		    		   	case Green:
		    		   		LedColor(green);
		    				if(pitIsrFlag==true){
		    					estado_interno = Purple;
		    					pitIsrFlag=false;
		    				}
		    				break;
		    			case Purple:
		    				LedColor(purple);
		    				if(pitIsrFlag==true){
		    					estado_interno = Yellow;
		    					pitIsrFlag=false;
		    				}
		    				break;
		    			case Yellow:
		    				LedColor(yellow);
		    				if(pitIsrFlag==true){
		    					estado_interno = Green;
		    					pitIsrFlag=false;
		    				}
		    				break;
		    				default:
		    					estado_interno = Green;
		    				break;
		    			}
		    			if(SW2_presionado){
		    				SW2_presionado = false;
		    				estado_externo = UNO;
		    				estado_interno = Yellow;
		    			}
		    			if(SW3_presionado){
		    				SW3_presionado = false;
		    			}
		    			break;
		    			default:
		    			estado_externo = UNO;
		    			estado_interno = Yellow;
		    			break;
		}
	}
}

