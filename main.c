
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_pit.h"
#include "fsl_clock.h"
#include "fsl_gpio.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define PIN21 21u	// DEFINICION DEL PIN21, PARA EL LED AZUL
#define PIN22 22u   // DEFINICION DEL PIN22, PARA EL LED ROJO
#define PIN26 26u	// DEFINICION DEL PIN26, PARA EL LED VERDE

#define PIN06 6u	// DEFINICION DEL SW2
#define PIN04 4u	// DEFINICION DEL SW3

typedef enum { 	// DEFINICION DE CASOS
	UNO,
	DOS,
	TRES,
	CUATRO,
	CINCO,
}Casos;

volatile uint32_t g_ButtonPress2 = false; // DEFINICION DE VARIABLE PARA EL SW2 PARA LA INTERRUPCION
volatile uint32_t g_ButtonPress3 = false; // DEFINICION DE VARIABLE PARA EL SW3 PARA LA INTERRUPCION
volatile bool pitIsrFlag = false; 		  // DEFINICION DE VARIABLE PARA EL PIT
volatile uint32_t i = false;


gpio_pin_config_t sw2_config = {	// CONFIGURACION DEL SW2 COMO ENTRADA
       kGPIO_DigitalInput,
       1,
   };

gpio_pin_config_t sw3_config = {	// CONFIGURACION DEL SW3 COMO ENTRADA
       kGPIO_DigitalInput,
       1,
   };

gpio_pin_config_t led1_config={  // DEFINICION DEL LED 1 COMO SALIDA A TRAVES DEL GPIO
		kGPIO_DigitalOutput,
		1,
};

gpio_pin_config_t led2_config={ // DEFINICION DEL LED 2 COMO SALIDA A TRAVES DEL GPIO
		kGPIO_DigitalOutput,
		1,
};

gpio_pin_config_t led3_config={ // DEFINICION DEL LED 3 COMO SALIDA A TRAVES DEL GPIO
		kGPIO_DigitalOutput,
		1,
};

void PORTC_IRQHandler(void)// CONFIGURACION DE LA INTERRUPCION DEL PUERTO C PIN 6
{
    GPIO_PortClearInterruptFlags(GPIOC, 1U << PIN06);
    g_ButtonPress2 = true;	// HABILITACION DE BANDERA DE INTERRUPCION
    SDK_ISR_EXIT_BARRIER;
}

void PORTA_IRQHandler(void)// CONFIGURACION DE LA INTERRUPCION DEL PUERTO PIN 4
{
    GPIO_PortClearInterruptFlags(GPIOA, 1U << PIN04);
    g_ButtonPress3 = true; // HABILITACION DE BANDERA DE INTERRUPCION
    SDK_ISR_EXIT_BARRIER;
}

void PIT0_IRQHandler(void)// CONFIGURACION DE LA INTERRUPCION DEL PIT CANAL 0
{
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
    pitIsrFlag = true;
    __DSB();
}

int main(void)
{

	const port_pin_config_t porta_pin6_config = {
	    kPORT_PullUp,                   /* Internal pull-up resistor is enabled */
	    kPORT_FastSlewRate,             /* Fast slew rate is configured */
	    kPORT_PassiveFilterDisable,     /* Passive filter is disabled, PARA REBOTES DE SW */
	    kPORT_OpenDrainDisable,         /* Open drain is disabled */
	    kPORT_LowDriveStrength,         /* High drive strength is configured, 2 - 8 mA SALIDA EN CORRIENTE DE PINES */
	    kPORT_MuxAsGpio,                /* Pin is configured as PTA4 */
	    kPORT_UnlockRegister            /* Pin Control Register fields [15:0] are not locked */
	  };

	const port_pin_config_t porta_pin4_config = {
	    kPORT_PullUp,                   /* Internal pull-up resistor is enabled */
	    kPORT_FastSlewRate,             /* Fast slew rate is configured */
	    kPORT_PassiveFilterDisable,     /* Passive filter is disabled, PARA REBOTES DE SW */
	    kPORT_OpenDrainDisable,         /* Open drain is disabled */
	    kPORT_LowDriveStrength,         /* High drive strength is configured, 2 - 8 mA SALIDA EN CORRIENTE DE PINES */
	    kPORT_MuxAsGpio,                /* Pin is configured as PTA4 */
	    kPORT_UnlockRegister            /* Pin Control Register fields [15:0] are not locked */
	  };

	Casos Estado  = UNO;

//---------
	CLOCK_EnableClock(kCLOCK_PortA); // HABILITACION DEL CLOCK DEL PUERTO A, PARA EL USO DE SW1
	CLOCK_EnableClock(kCLOCK_PortB); // HABILITACION DEL CLOCK DEL PUERTO B, PARA EL USO DE LED AZUL Y LED ROJO
	CLOCK_EnableClock(kCLOCK_PortC); // HABILITACION DEL CLOCK DEL PUERTO C, PARA EL USO DE SW2
	CLOCK_EnableClock(kCLOCK_PortE); // HABILITACION DEL CLOCK DEL PUERTO A, PARA EL USO DE LED VERDE

	PORT_SetPinConfig(PORTC, PIN06, &porta_pin6_config); //
	PORT_SetPinInterruptConfig(PORTC, PIN06, kPORT_InterruptFallingEdge); //
	PORT_SetPinConfig(PORTA, PIN04, &porta_pin4_config); //
	PORT_SetPinInterruptConfig(PORTA, PIN04, kPORT_InterruptFallingEdge); //

	PORT_SetPinMux(PORTB, PIN21, kPORT_MuxAsGpio); //CONFIGURACION DEL PIN21 DEL PUERTO B MULTIPLEXADO, PARA USO DEL LED AZUL
	PORT_SetPinMux(PORTB, PIN22, kPORT_MuxAsGpio); //CONFIGURACION DEL PIN22 DEL PUERTO B MULTIPLEXADO, PARA USO DEL LED ROJO
	PORT_SetPinMux(PORTE, PIN26, kPORT_MuxAsGpio); //CONFIGURACION DEL PIN26 DEL PUERTO E MULTIPLEXADO, PARA USO DEL LED VERDE

	GPIO_PinInit(GPIOB, PIN21, &led1_config); // CONFIGURACION DEL GPIO DEL PIN21, PUERTO B, COMO SALIDA
	GPIO_PinInit(GPIOB, PIN22, &led2_config); // CONFIGURACION DEL GPIO DEL PIN22, PUERTO B, COMO SALIDA
	GPIO_PinInit(GPIOE, PIN26, &led3_config); // CONFIGURACION DEL GPIO DEL PIN26, PUERTO E, COMO SALIDA
	GPIO_PinInit(GPIOC, PIN06, &sw2_config);  // CONFIGURACION DEL GPIO DEL PIN26, PUERTO E, COMO ENTRADA
	GPIO_PinInit(GPIOA, PIN04, &sw3_config);  // CONFIGURACION DEL GPIO DEL PIN26, PUERTO E, COMO ENTRADA

	NVIC_EnableIRQ(PORTC_IRQn); 	 // HABILITACION DEL VECTOR DE INTERRUPCION PUERTO C
	NVIC_SetPriority(PORTC_IRQn, 2); // PRIORIDAD DE LA INTERRUPCION C
	NVIC_EnableIRQ(PORTA_IRQn); 	 // HABILITACION DEL VECTOR DE INTERRUPCION PUERTO A
	NVIC_SetPriority(PORTA_IRQn, 1); // PRIORIDAD DE LA INTERRUPCION A

    pit_config_t pitConfig; // VARIABLE DE TIPO PIT_CONFIG
    uint32_t freq=0;		// VARIABLE PARA ESTABLECER LA FRECUENCIA

/*FUNCIONES ASOCIADAS A LA TARJETA
 *CODIGO INICIAL QUE PERMITE LA MANITPULACION DE LA TARJETA
 *
 *BOARD_INITPINS AGRUPA LOS PINES QUE UTILIZA EL SDK
 *BOARD_BOOTCLOCKRUN MANIPULA EL SISTEMA DE RELOJ DE LA K64F PARA TRABAJAR A 120MHz
 *BOARD_INITDEBUGCONSOLE CONFIGURA LA SALIDA DEL DEBUG QUE SE UTILIZA EN LA K64 (POR DECIR UART)
 */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

 //  PIT_GetDefaultConfig(&pitConfig); //ESTABLECE CONFIGURACION POR DEFECTO

//INICIALIZACION
    PIT_Init(PIT, &pitConfig);

//EL CLOCKBUS TRABAJA A LA MITAD DEL CORE/2, EN ESTE CASO 120MHz
    freq = CLOCK_GetFreq(kCLOCK_BusClk);

//SE ESTABLECE EL PERIODO PASANDO COMO ARGUMENTO LA DIRECCION BASE DEL PIT, EL CANAL, UNA MACRO FUNCION
//LA MACRO TRANSFORMA EL DATO A UN VALOR EN SEGUNDO, ES DECIR, PROPORCIONA EL TIEMPO EN 1 SEGUNDO
    PIT_SetTimerPeriod(PIT,kPIT_Chnl_0, USEC_TO_COUNT(1000000U, freq));

//HABILITAR INTERRUPCIONES EN EL PERIFERICO
    PIT_EnableInterrupts(PIT, kPIT_Chnl_0 , kPIT_TimerInterruptEnable);

//HABILITAR INTERRUPCIONES EN EL NVIC
    //EnableIRQ(PIT0_IRQn);  //FUNCION QUE HABILITA LA INTERRUPCION SIN PRIORIDAD
    NVIC_EnableIRQ(PIT0_IRQn); 	//HABILITACION DEL VECTOR DE INTERRUCION PARA EL PIT
    NVIC_SetPriority(PIT0_IRQn,3);	//ESTABLECE PRIORIDAD DE LE INTERRUPCION DEL PIT

   PIT_StartTimer(PIT, kPIT_Chnl_0); //INICIA LA MARCHA DEL TIMER PIT

    while (true)
    {
    	PIT_StartTimer(PIT, kPIT_Chnl_0);
    	if(g_ButtonPress2){
    		Estado  = DOS;
  //  		g_ButtonPress3=false;

    	}
    	if(g_ButtonPress3){
    		g_ButtonPress2=false;
    		i=true;
    		Estado  = TRES;
    	}

    	switch (Estado) {
    					case UNO:

    						if (true == pitIsrFlag){
    						     GPIO_PortSet(GPIOB, 1u << PIN21);	// AMARILLO
    						     GPIO_PortClear(GPIOB, 1u << PIN22);
    						     GPIO_PortClear(GPIOE, 1u << PIN26);
    						     pitIsrFlag = false;
    						    }

    						if (true == pitIsrFlag){
					            GPIO_PortSet(GPIOE, 1u << PIN26);	// ROJO
					            GPIO_PortSet(GPIOB, 1u << PIN21);
					        	GPIO_PortClear(GPIOB, 1u << PIN22);
					            pitIsrFlag = false;
   						        }
    						if (true == pitIsrFlag){
					        	GPIO_PortClear(GPIOB, 1u << PIN21);// MORADO
					        	GPIO_PortClear(GPIOB, 1u << PIN22);
					        	GPIO_PortSet(GPIOE, 1u << PIN26);
					            pitIsrFlag = false;
   						        }

    					break;

    					case DOS:
    						g_ButtonPress2=false;
    						if(true == pitIsrFlag){
    							GPIO_PortClear(GPIOE, 1u << PIN26); //VERDE
    							GPIO_PortSet(GPIOB, 1u << PIN21);
    							GPIO_PortSet(GPIOB, 1u << PIN22);
    							pitIsrFlag = false;
				        		}

    						if(true == pitIsrFlag){
    							GPIO_PortClear(GPIOB, 1u << PIN21);	// AZUL
    							GPIO_PortSet(GPIOB, 1u << PIN22);
    							GPIO_PortSet(GPIOE, 1u << PIN26);
    							pitIsrFlag = false;
				        		}

    						if(true == pitIsrFlag){
    							GPIO_PortClear(GPIOB, 1u << PIN22);	// ROJO
    							GPIO_PortSet(GPIOE, 1u << PIN26);
    							GPIO_PortSet(GPIOB, 1u << PIN21);
    							pitIsrFlag = false;
				        		}

    					break;

    					case TRES:
    						g_ButtonPress3=false;
    						if(true == pitIsrFlag){
    						   	GPIO_PortSet(GPIOE, 1u << PIN26); //AZUL
    						   	GPIO_PortClear(GPIOB, 1u << PIN21);	//
    						   	GPIO_PortSet(GPIOB, 1u << PIN22);
    						   	pitIsrFlag = false;
    						    }

    						 if(true == pitIsrFlag){
    						   	GPIO_PortClear(GPIOB, 1u << PIN21);	// BLANCO
    						   	GPIO_PortClear(GPIOB, 1u << PIN22);
    						   	GPIO_PortClear(GPIOE, 1u << PIN26);
    						  	pitIsrFlag = false;
    						    }

    						 if(true == pitIsrFlag){
    						   	GPIO_PortClear(GPIOB, 1u << PIN22);	// ROJO
    						    GPIO_PortSet(GPIOE, 1u << PIN26);
    						    GPIO_PortSet(GPIOB, 1u << PIN21);
    						    pitIsrFlag = false;
    							}

    						 if(g_ButtonPress3 & (i==1)){
    							 g_ButtonPress3=false;
    						     	    Estado  = CUATRO;
    						     	}

    					break;
    					case CUATRO:
    						i=false;
    						g_ButtonPress2=false;
    						if(true == pitIsrFlag){
    							GPIO_PortClear(GPIOE, 1u << PIN26);//VERDE
    							GPIO_PortSet(GPIOB, 1u << PIN21);
    							GPIO_PortSet(GPIOB, 1u << PIN22);
    							pitIsrFlag = false;
    					    	}

    					    if(true == pitIsrFlag){
    					       	GPIO_PortClear(GPIOB, 1u << PIN21);	// MORADO
    					       	GPIO_PortClear(GPIOB, 1u << PIN22);
    					       	GPIO_PortSet(GPIOE, 1u << PIN26);
    					    	pitIsrFlag = false;
    					    	}

    					    if(true == pitIsrFlag){
    					       	GPIO_PortClear(GPIOB, 1u << PIN22);	// AMARILLO
    					        GPIO_PortClear(GPIOE, 1u << PIN26);
    					        GPIO_PortSet(GPIOB, 1u << PIN21);
    					        pitIsrFlag = false;
    					    	}

    					    if(g_ButtonPress2){
    					    	Estado  = CINCO;
	    						}
    					break;
    					case CINCO:
    						g_ButtonPress2=false;
    						g_ButtonPress3=false;
    						GPIO_PortSet(GPIOB, 1u << PIN21);//BLUE
    			    		GPIO_PortSet(GPIOB, 1u << PIN22);//RED
    			    		GPIO_PortSet(GPIOE, 1u << PIN26);//GREE

    			    		Estado = UNO;

    			    	break;

    					default:
    						GPIO_PortSet(GPIOB, 1u << PIN21);//BLUE
    						GPIO_PortSet(GPIOB, 1u << PIN22);//RED
    						GPIO_PortSet(GPIOE, 1u << PIN26);//GREE

    						Estado = UNO;
    					break;
    				}
    }

}
