/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "xil_printf.h"
#include "axi_ppm.h"
#include "xparameters.h"
#include "xil_types.h"
#include "xstatus.h"
#include "xil_io.h"

#define AXI_PPM_BASEADDR 0x43C00000


int main()
{
    init_platform();

    print("START\n\r"); // begin program
    int i = 0; // increment/decrement variable for record/rewind
    int j = 0; // increment/decrement variable for play/reverse
	int imax = 0; // used to determine whether to start playing from beginning of recording or last rewind spot
    int frame_detect = 0; // stores previous value of slv_reg1 frame counter

    /* Arrays to store channel counts for record and play */
    uint32_t idleRecord[1000];
    uint32_t CH1Record[1000];
    uint32_t CH2Record[1000];
    uint32_t CH3Record[1000];
    uint32_t CH4Record[1000];
    uint32_t CH5Record[1000];
    uint32_t CH6Record[1000];

	uint32_t *button_ptr = (uint32_t*) XPAR_AXI_GPIO_2_BASEADDR;
	uint32_t *switch_ptr = (uint32_t*) XPAR_AXI_GPIO_1_BASEADDR;

	uint32_t slv_reg0_value= AXI_PPM_mReadReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG0_OFFSET); // initial value read
	uint32_t slv_reg1_value = AXI_PPM_mReadReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG1_OFFSET); // initial value read

	frame_detect = slv_reg1_value; // initialize

    while (!(*button_ptr & 0x1)) {

    	/************** READING SLAVE REGISTER VALUES ****************/

		slv_reg1_value = AXI_PPM_mReadReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG1_OFFSET); // frame counter


		/* CAPTURE REGISTERS */
		uint32_t slv_reg10_count= AXI_PPM_mReadReg(AXI_PPM_BASEADDR,AXI_PPM_S00_AXI_SLV_REG10_OFFSET);
		uint32_t slv_reg11_count=AXI_PPM_mReadReg(AXI_PPM_BASEADDR,AXI_PPM_S00_AXI_SLV_REG11_OFFSET);
		uint32_t slv_reg12_count=AXI_PPM_mReadReg(AXI_PPM_BASEADDR,AXI_PPM_S00_AXI_SLV_REG12_OFFSET);
		uint32_t slv_reg13_count=AXI_PPM_mReadReg(AXI_PPM_BASEADDR,AXI_PPM_S00_AXI_SLV_REG13_OFFSET);
		uint32_t slv_reg14_count=AXI_PPM_mReadReg(AXI_PPM_BASEADDR,AXI_PPM_S00_AXI_SLV_REG14_OFFSET);
		uint32_t slv_reg15_count=AXI_PPM_mReadReg(AXI_PPM_BASEADDR,AXI_PPM_S00_AXI_SLV_REG15_OFFSET);

		/* GENERATE REGISTERS */
		uint32_t slv_reg2_value= AXI_PPM_mReadReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG2_OFFSET);
		uint32_t slv_reg3_count= AXI_PPM_mReadReg(AXI_PPM_BASEADDR,AXI_PPM_S00_AXI_SLV_REG3_OFFSET);
		uint32_t slv_reg4_count=AXI_PPM_mReadReg(AXI_PPM_BASEADDR,AXI_PPM_S00_AXI_SLV_REG4_OFFSET);
		uint32_t slv_reg5_count=AXI_PPM_mReadReg(AXI_PPM_BASEADDR,AXI_PPM_S00_AXI_SLV_REG5_OFFSET);
		uint32_t slv_reg6_count=AXI_PPM_mReadReg(AXI_PPM_BASEADDR,AXI_PPM_S00_AXI_SLV_REG6_OFFSET);
		uint32_t slv_reg7_count=AXI_PPM_mReadReg(AXI_PPM_BASEADDR,AXI_PPM_S00_AXI_SLV_REG7_OFFSET);
		uint32_t slv_reg8_count=AXI_PPM_mReadReg(AXI_PPM_BASEADDR,AXI_PPM_S00_AXI_SLV_REG8_OFFSET);
		uint32_t slv_reg9_count=AXI_PPM_mReadReg(AXI_PPM_BASEADDR,AXI_PPM_S00_AXI_SLV_REG9_OFFSET);


		/******************** BUTTON AND SWITCH INPUTS *********************/


		if (*switch_ptr & 0x1) { // SW0 for SOFTWARE RELAY MODE
			slv_reg0_value =  0x00000001;
			if (*switch_ptr & 0x2) { // SW1 for SOFTWARE DEBUG MODE
						printf("Software Debug Mode: \n");
						printf("Idle: %d\n",slv_reg3_count);
						printf("CH1: %d\n",slv_reg4_count);
						printf("CH2: %d\n",slv_reg5_count);
						printf("CH3: %d\n",slv_reg6_count);
						printf("CH4: %d\n",slv_reg7_count);
						printf("CH5: %d\n",slv_reg8_count);
						printf("CH6: %d\n\n\n",slv_reg9_count);
			}

			if (*switch_ptr & 0x4) { // SW2 for SOFTWARE RECORD MODE
				if (*button_ptr & 0x2) { // BTND for RECORD
					if (frame_detect != slv_reg1_value) { // Record new frames only
						frame_detect = slv_reg1_value;
						idleRecord[i] = slv_reg2_value;
						CH1Record[i] = slv_reg10_count;
						CH2Record[i] = slv_reg11_count;
						CH3Record[i] = slv_reg12_count;
						CH4Record[i] = slv_reg13_count;
						CH5Record[i] = slv_reg14_count;
						CH6Record[i] = slv_reg15_count;
						printf("Recording: %d\n", i);
						i++;
						imax = i;
						if(i >= 999){
							i = 0;
						}
						j = 0;
					}


				}

				if (*button_ptr & 0x10) { // BTNU for REWIND
					if (frame_detect != slv_reg1_value) {
						frame_detect = slv_reg1_value;
						i--;
						if(i <= 0){
							i = 0;
						}
						printf("Rewinding: %d\n", i);
						j = i;
					}
				}
			}

			if (*switch_ptr & 0x8) { // SW3 for SOFTWARE PLAY MODE
				if (*button_ptr & 0x8) { // BTNR for PLAY
					if (frame_detect != slv_reg1_value) {
						frame_detect = slv_reg1_value;
						AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG0_OFFSET, slv_reg0_value); // reg0(0) <= 1
						AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG3_OFFSET, idleRecord[j]); // reg3 <= idle count
						AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG4_OFFSET, CH1Record[j]); //reg4 <= reg10 - CH1
						AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG5_OFFSET, CH2Record[j]); //reg5 <= reg11 - CH2
						AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG6_OFFSET, CH3Record[j]); //reg6 <= reg12 - CH3
						AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG7_OFFSET, CH4Record[j]); //reg7 <= reg13 - CH4
						AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG8_OFFSET, CH5Record[j]); //reg8 <= reg14 - CH5
						AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG9_OFFSET, CH6Record[j]); //reg9 <= reg15 - CH6
						printf("Playing: %d\n", j);
						j++;
						if(j >= imax){
							j = 0;
						}
					}
				}
				if (*button_ptr & 0x4) { // BTNL for REVERSE PLAY
					if (frame_detect != slv_reg1_value) {
						frame_detect = slv_reg1_value;
						AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG0_OFFSET, slv_reg0_value); // reg0(0) <= 1
						AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG3_OFFSET, idleRecord[j]); // reg3 <= idle count
						AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG4_OFFSET, CH1Record[j]); //reg4 <= reg10 - CH1
						AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG5_OFFSET, CH2Record[j]); //reg5 <= reg11 - CH2
						AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG6_OFFSET, CH3Record[j]); //reg6 <= reg12 - CH3
						AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG7_OFFSET, CH4Record[j]); //reg7 <= reg13 - CH4
						AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG8_OFFSET, CH5Record[j]); //reg8 <= reg14 - CH5
						AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG9_OFFSET, CH6Record[j]); //reg9 <= reg15 - CH6
						j--;
						if(j <= 0){
							j = 0;
						}
						printf("Reversing Play: %d\n", j);
					}
				}
			}
			else { // Output regular values if software play is not on
				AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG0_OFFSET, slv_reg0_value); // reg0(0) <= 1
				AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG3_OFFSET, slv_reg2_value); // reg3 <= idle count
				AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG4_OFFSET, slv_reg10_count); //reg4 <= reg10 - CH1
				AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG5_OFFSET, slv_reg11_count); //reg5 <= reg11 - CH2
				AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG6_OFFSET, slv_reg12_count); //reg6 <= reg12 - CH3
				AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG7_OFFSET, slv_reg13_count); //reg7 <= reg13 - CH4
				AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG8_OFFSET, slv_reg14_count); //reg8 <= reg14 - CH5
				AXI_PPM_mWriteReg(AXI_PPM_BASEADDR, AXI_PPM_S00_AXI_SLV_REG9_OFFSET, slv_reg15_count); //reg9 <= reg15 - CH6
			}
		}
		else { // HARDWARE RELAY MODE
			slv_reg0_value =  0x00000000;
			if (*switch_ptr & 0x2) {
					printf("Hardware Debug Mode: \n");
					printf("Idle: %d\n",slv_reg2_value);
					printf("CH1: %d\n",slv_reg10_count);
					printf("CH2: %d\n",slv_reg11_count);
					printf("CH3: %d\n",slv_reg12_count);
					printf("CH4: %d\n",slv_reg13_count);
					printf("CH5: %d\n",slv_reg14_count);
					printf("CH6: %d\n\n\n",slv_reg15_count);
			}
		}


    }
    slv_reg0_value =  0x00000000;

    cleanup_platform();
    return 0;
}
