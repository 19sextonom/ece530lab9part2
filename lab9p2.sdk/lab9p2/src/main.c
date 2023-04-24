/*
 * lab9p1.c
 * Read two tmp101 breakout boards from both PL and PS sides with ARM I2C modules
 */

#include <stdio.h>
#include "xparameters.h"
#include <xiicps.h>


#define TMP101_RESOLUTION_09BIT 0b00000000
#define TMP101_RESOLUTION_10BIT 0b00100000
#define TMP101_RESOLUTION_11BIT 0b01000000
#define TMP101_RESOLUTION_12BIT 0b01100000

#define TMP101_TEMPERATURE_REGISTER     0b00000000
#define TMP101_CONFIGURATION_REGISTER   0b00000001
#define TMP101_T_LOW_REGISTER           0b00000010
#define TMP101_T_HIGH_REGISTER          0b00000011

#define PS_I2C_DEVICE_ID XPAR_PS7_I2C_0_DEVICE_ID
#define PS_DATA_RATE 100000 // 100KHz

// TO DONE Revise the following address to be VDD address
#define PS_TMP101_ADDRESS 0b1001010    // jumper = vdd (JF)

// TO DONE Revise to configure 12 bit resolution for TMP101
#define PS_TMP101_RESOLUTION TMP101_RESOLUTION_12BIT

#define PL_I2C_DEVICE_ID XPAR_PS7_I2C_1_DEVICE_ID
#define PL_DATA_RATE 200000 // 200KHz

// TO DONE: Revise the following address to be ground address
#define PL_TMP101_ADDRESS 0b1001000    // jumper = ground (JB)

// TO DONE Revised the following to configure 10-bit resolution for TMP101
#define PL_TMP101_RESOLUTION TMP101_RESOLUTION_10BIT

#define DELAY_LOOP_COUNT 80000000
#define BUFFER_SIZE 2

XIicPs i2cPs;
XIicPs i2cPl;
u8 sendBuffer[BUFFER_SIZE];
u8 recvBuffer[BUFFER_SIZE];

/**
 * Configures an I2C TMP101 instance.
 * @param i2cInstance pointer to an I2C instance
 * @param i2cAddress address of I2C TMP101 chip
 * @param temperatureResolution resolution of TMP101 temperature as 0b0RR00000
 *      - RR = 00: 9 bit
 *      - RR = 01: 10 bit
 *      - RR = 10: 11 bit
 *      - RR = 11: 12 bit (based on TI datasheet)
 * @return XST_SUCCESS on success, XST_FAILURE otherwise
 */
int configureTmp101(XIicPs *i2cInstance, u8 i2cAddress, u8 temperatureResolution) {
    // TO DONE: Set resolution
    // NOTE: all other fields in the config register are desired as 0 (or don't cares),
    //  so we just write the resolution value directly.
    sendBuffer[0] = TMP101_CONFIGURATION_REGISTER;
    sendBuffer[1] = temperatureResolution;
    if (XST_FAILURE == XIicPs_MasterSendPolled(
            i2cInstance,
            sendBuffer,
            BUFFER_SIZE,
            i2cAddress
    )) {
        printf("-- Set Resolution Failed\r\n");
        return XST_FAILURE;
    }

    //TO DONE: set pointer back to 0x00 to point to the temperature value
    sendBuffer[0] = TMP101_TEMPERATURE_REGISTER;
    if (XST_FAILURE == XIicPs_MasterSendPolled(
            i2cInstance,
            sendBuffer,
            1,
            i2cAddress
    )) {
        printf("-- Set Pointer 0 Failed\r\n");
        return XST_FAILURE;
    }

    return XST_SUCCESS;
}

/**
 * Configures an I2C instance.
 *
 * @param	i2cInstance is a pointer to an I2C instance.
 * @param	i2cDeviceId is the ID of I2C device defined in xparamenters.h.
 * @param	i2cClockRate is clock frequency of I2C TMP101 in Hz.
 *
 * @return
 * 		- XST_SUCCESS if everything went well.
 * 		- XST_FAILURE if failed.
 */
int configureI2cInstance(XIicPs *i2cInstance, int i2cDeviceId, int i2cClockRate) {
    XIicPs_Config *i2cConfig;

    // TO DONE: Call lookup table to find I2C hardware module for i2cDeviceId
    i2cConfig = XIicPs_LookupConfig(i2cDeviceId);

    if (i2cConfig == NULL) {
        printf("-- Failed to find i2cDeviceId %d\r\n", i2cDeviceId);
        return XST_FAILURE;
    }

    // TO DONE: Initialize I2C instance. Return XST_FAILURE if failed.
    if (XIicPs_CfgInitialize(i2cInstance, i2cConfig, i2cConfig->BaseAddress) != XST_SUCCESS) {
        printf("-- Configuration Failed\r\n");
        return XST_FAILURE;
    }

    // Self test. Return XST_FAILURE if failed
    if (XIicPs_SelfTest(i2cInstance) != XST_SUCCESS) {
        printf("-- Self Test Failed\r\n");
        return XST_FAILURE;
    }

    // TO DONE: Set I2C clock frequency. Return XST_FAILURE if failed
    if (XIicPs_SetSClk(i2cInstance, i2cClockRate) != XST_SUCCESS) {
        printf("-- Self Clock Failed\r\n");
        return XST_FAILURE;
    }

    printf("waiting until bus is idle\r\n");
    s32 status1;
    do {
        status1 = XIicPs_BusIsBusy(i2cInstance);
//		printf("%d \r\n",status1);
    } while (status1);
    printf("bus is now idle\r\n");

    return XST_SUCCESS;
}

/**
 * Reads temperature of a TMP101.
 * @param i2cInstance pointer to an I2C instance
 * @param temperature returned floating point temperature value
 * @param i2cAddress address of I2C TMP101 chip.
 * @return XST_SUCCESS on success, XST_FAILURE otherwise
 */
int ReadTemperature(XIicPs *i2cInstance, float *temperature, int i2cAddress, int resolution) {
    u8 temp[2] = {0x00, 0x00};    // 2 byte temperature
    // set pointer back to 0x00 to point to the temperature value
    if (XIicPs_MasterSendPolled(i2cInstance, temp, 1, i2cAddress) == XST_FAILURE) {
    	printf("-- Setting pointer to 0x00\r\n");
        return XST_FAILURE;
    }

    // TO DONE: Read temperature. Return XST_FAILURE if failed
    if (XIicPs_MasterRecvPolled(i2cInstance, temp, BUFFER_SIZE, i2cAddress) == XST_FAILURE) {
    	printf("-- Reading from device Failed\r\n");
    	return XST_FAILURE;
    }

    // TO DONE: Convert temperature to floating number
    // NOTE: IDK if I did this right. Not sure I understand how the data is coming in
    *temperature = (float) temp[0] + (temp[1] >> 4)/16.0;// & 1)/2.0 + ((temp[1] >> 6) & 1)/4.0 + ((temp[1] >> 5) & 1)/8.0 + ((temp[1] >> 4) & 1)/16.0;
    // + (0x80 && temp[1]) / 2; + (0x40 & temp[1])/4 + (0x20 & temp[1])/8 + (0x10 & temp[1])/16 ;
//    u8 *temp_ptr = (u8 *) temperature;
//    temp_ptr[1] = temp[0];
//    temp_ptr[0] = 0x0F & (temp[1] >> 4);

    return XST_SUCCESS;
}

int main() {
    float tempPs, tempPl;
    unsigned int loopCount;
    printf("-- Start Lab 9 Part 1 EH and OS--\r\n\r\n");

    // TO DONE: configure I2C instance and tmp101 board on PS port
    if (XST_FAILURE == configureI2cInstance(
            &i2cPs,
            XPAR_PS7_I2C_0_DEVICE_ID,   // TODO: may need to swap dev id 0 vs 1 here and below
            PS_DATA_RATE
    )) {
        printf("Failed to configure I2C instance on PS.\r\n");
    }

    // TO DONE: set resolution
    if (configureTmp101(&i2cPs, PS_TMP101_ADDRESS, PS_TMP101_RESOLUTION) == XST_FAILURE) {
        printf("Failed to configure TMP101 on PS.\r\n");
    }

    //TO DONE: configure I2C instance and tmp101 board on PL port
    if (XST_FAILURE == configureI2cInstance(
            &i2cPl,
            XPAR_PS7_I2C_1_DEVICE_ID,   // TODO: may need to swap dev id 0 vs 1 here and above
            PL_DATA_RATE
    )) {
        printf("Failed to configure I2C instance on PL.\r\n");
    }

    // TO DONE: set resolution
    if (configureTmp101(&i2cPl, PL_TMP101_ADDRESS, PL_TMP101_RESOLUTION) == XST_FAILURE) {
        printf("Failed to configure TMP101 on PL.\r\n");
    }

    while (1) {
        printf("in loop\r\n");

        // TO DONE: Read tmp101 board and calculate temperature value on PS port
        if (ReadTemperature(&i2cPs, &tempPs, PS_TMP101_ADDRESS, PS_TMP101_RESOLUTION) == XST_FAILURE){
        	printf("Failed to read TMP101 from PS on bottom row of Connector JF.\r\n");
        }

        // TO DONE: Read tmp101 board and calculate temperature value on PL port
        if (ReadTemperature(&i2cPl, &tempPl, PL_TMP101_ADDRESS, PL_TMP101_RESOLUTION) == XST_FAILURE){
        	printf("Failed to read TMP101 from PL on top row of Connector JB.\r\n");
        }

        // TO DONE: Display temperature in floating point number with 4 digits after decimal point
        // NOTE: printing floating numbers is not supported by xil_printf().
        // Use printf() from <stdio.h> to print floating points.
        // Mixing Xil_printf() with printf() may cause some printf() statements to be dropped.
        printf("PS Temperature: %8.4f degrees\r\n", tempPs);
        printf("PL Temperature: %8.4f degrees\r\n", tempPl);

        // delay loop to pause for a while
        for (loopCount = 0; loopCount < DELAY_LOOP_COUNT; loopCount++);    //delay time

    }
}
