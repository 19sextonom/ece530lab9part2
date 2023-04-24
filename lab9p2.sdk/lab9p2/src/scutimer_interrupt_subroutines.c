/*File Name: LEDcounter_scutimer_interrupt.c */
/*
// ECE530 Spring 2022-2023
// Author: Jianjian Song
// A count-up counter is incremented by the Cortex A9 SCU Private Timer interrupt
// This is revision based on xscutimer_intr_example.c
// from C:\Xilinx\SDK\2016.2\data\embeddedsw\XilinxProcessorIPLib\drivers\scutimer_v2_1\examples
*/

#include "scutimer_interrupt_subroutines.h"

#define ENDCOUNT 200
volatile int TimerCount=0;

int interrupt_main(void)
{
    int Status;

    printf("SCU Timer Interrupt Counter Example\r\n");
    // Initialize LEDs
    Status = SetupLEDs(&LEDInst, LEDS_DEVICE_ID);
    if(Status != XST_SUCCESS) return XST_FAILURE;

    Status = SetupScuTimer(&TimerInstance, TIMER_DEVICE_ID, TIMER_LOAD_VALUE, TIMER_IRPT_INTR);
    if (Status != XST_SUCCESS) {
        xil_printf("SCU Timer Enable Failed\r\n");
        return XST_FAILURE;     }

    Status = SetupGIC(&GICinstance, GIC_DEVICE_ID,
                      &TimerInstance, TIMER_IRPT_INTR);
    if (Status != XST_SUCCESS) {
        xil_printf("SCU Timer Interrupt Example Test Failed\r\n");
        return XST_FAILURE;     }

    while (1) {
        if (TimerCount == ENDCOUNT) {
            XScuTimer_Stop(&TimerInstance);
            break;    }
    }
// Disconnect and disable the interrupt for the Timer.
    XScuGic_Disconnect(&GICinstance, TIMER_IRPT_INTR);

    xil_printf("\r\nSCU Timer Interrupt counter Example Is Successful!\r\n");
    return XST_SUCCESS;
}	//end main()

int SetupLEDs(XGpio *LEDdevice, int DeviceID) {
    int status;
    // Initialize LEDs
    status = XGpio_Initialize(LEDdevice, DeviceID);
    if(status != XST_SUCCESS) return XST_FAILURE;
    // Set LEDs direction to outputs
    XGpio_SetDataDirection(&LEDInst, 1, 0x00);
    return XST_SUCCESS;
} //end SetupLEDs()

int SetupScuTimer(XScuTimer *Timer, u16 timer_ID, unsigned int Load_value, u16 timer_interrupt_id) {
    int Status;
    XScuTimer_Config *ConfigPtr;

// Initialize the Scu Private Timer driver.
    ConfigPtr = XScuTimer_LookupConfig(timer_ID);

//This is where the virtual address would be used, this example uses physical address.
    Status = XScuTimer_CfgInitialize(Timer, ConfigPtr, ConfigPtr->BaseAddr);
    if (Status != XST_SUCCESS) return XST_FAILURE;

// Perform a self-test to ensure that the hardware was built correctly.
    Status = XScuTimer_SelfTest(Timer);
    if (Status != XST_SUCCESS) return XST_FAILURE;

    //Enable Auto reload mode.
    XScuTimer_EnableAutoReload(Timer);
//Load the timer counter register.
    XScuTimer_LoadTimer(Timer, Load_value);
// Enable the timer interrupts for timer mode.
    XScuTimer_EnableInterrupt(Timer);

//Start the timer counter and then wait for it to timeout a number of times.
    XScuTimer_Start(Timer);
    return XST_SUCCESS;
} //end SetupScuTimer()

/*****************************************************************************/
/*
* This function sets up GIC for timer interrupt.
*
* @param	IntcInstancePtr is a pointer to the instance of XScuGic driver.
* @param	TimerInstancePtr is a pointer to the instance of XScuTimer
*		driver.
* @param	TimerIntrId is the Interrupt Id of the XScuTimer device.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
* @note		None.
******************************************************************************/
int SetupGIC(XScuGic *IntcInstancePtr, u16 gic_id, XScuTimer *TimerInstancePtr, u16 TimerIntrId)
{
    int Status;

    XScuGic_Config *IntcConfig;
//Level 2: configure GIC (generic interrupt controller)
//Initialize the interrupt controller driver so that it is ready to use.
    IntcConfig = XScuGic_LookupConfig(gic_id);
    if (NULL == IntcConfig) return XST_FAILURE;

    Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
                                   IntcConfig->CpuBaseAddress);
    if (Status != XST_SUCCESS) return XST_FAILURE;

// Connect the device driver handler that will be called when an
//interrupt for the device occurs, the handler defined above performs
// the specific interrupt processing for the device.
    Status = XScuGic_Connect(IntcInstancePtr, TimerIntrId,
                             (Xil_ExceptionHandler)TimerIntrHandler, (void *)TimerInstancePtr);
    if (Status != XST_SUCCESS)  return Status;
//Level 1: to tie interrupt to an ARM CPU
//Enable the interrupt for the device.
    XScuGic_Enable(IntcInstancePtr, TimerIntrId);

// Connect the interrupt controller interrupt handler to the hardware
//interrupt handling logic in the processor.
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
                                 (Xil_ExceptionHandler)XScuGic_InterruptHandler, IntcInstancePtr);
    /*
     * Enable interrupts in the Processor.
     */
    Xil_ExceptionEnable();

    return XST_SUCCESS;
}	//end SetupGIC()

/***************************************************************/
/*
* This function is the Interrupt handler for the Timer interrupt of the
* Timer device. It is called on the expiration of the timer counter in
* interrupt context.
*
* @param	CallBackRef is a pointer to the callback function.
*
* @return	None.
*
* @note		None.
*
****************************************************************/
void TimerIntrHandler(void *CallBackRef)
{
    //CallBackRef was initialized to be pointer to Timer instance
    XScuTimer *TimerInstancePtr = (XScuTimer *) CallBackRef;

    /*
     * Check if the timer counter has expired, checking is not necessary
     * since that's the reason this function is executed, this just shows
     * how the callback reference can be used as a pointer to the instance
     * of the timer counter that expired, increment a shared variable so
     * the main thread of execution can see the timer expired.
     */
    if (XScuTimer_IsExpired(TimerInstancePtr)) {
        XScuTimer_ClearInterruptStatus(TimerInstancePtr);
        TimerCount = TimerCount+1;
        XGpio_DiscreteWrite(&LEDInst, 1, TimerCount);
        xil_printf("\r\nTimerCount = %d", TimerCount);
    }
}	//end TimerIntrHandler()

