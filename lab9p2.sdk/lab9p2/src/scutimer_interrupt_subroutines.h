/*File Name: LEDcounter_scutimer_interrupt.h */
/*
//ECE530 Spring 2022-2023
//Author: Jianjian Song
// A count-up counter is incremented by the Cortex A9 SCU Private Timer interrupt
// This is revision based on xscutimer_intr_example.c
//from C:\Xilinx\SDK\2016.2\data\embeddedsw\XilinxProcessorIPLib\drivers\scutimer_v2_1\examples
*/
#include <xparameters.h>
#include "xscutimer.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "xgpio.h"
#include "stdio.h"

/************************** Constant Definitions *****************************/
#define TIMER_DEVICE_ID        XPAR_XSCUTIMER_0_DEVICE_ID
#define GIC_DEVICE_ID        XPAR_SCUGIC_SINGLE_DEVICE_ID
#define TIMER_IRPT_INTR        XPAR_SCUTIMER_INTR
#define LEDS_DEVICE_ID        XPAR_LEDS_DEVICE_ID

#define ONE_TENTH 32500000
#define TIMER_LOAD_VALUE        ONE_TENTH

/************************** Variable Definitions *****************************/
XScuTimer TimerInstance;    /* Cortex A9 Scu Private Timer Instance */
XScuGic GICinstance;        /* GIC Interrupt Controller Instance */
XGpio LEDInst;    //LED instance

extern volatile int timerCount;

//Function prototypes
int SetupScuTimer(XScuTimer *TimerInstance, u16 timer_ID, unsigned int load_value, u16 timer_interrupt_id);

int SetupGIC(XScuGic *IntcInstancePtr, u16 gic_id, XScuTimer *TimerInstancePtr, u16 TimerIntrId);

void TimerIntrHandler(void *CallBackRef);

int SetupLEDs(XGpio *LEDdevice, int DeviceID);

int initTimer();