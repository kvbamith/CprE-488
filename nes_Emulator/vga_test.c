/*****************************************************************************
 * Joseph Zambreno
 * Phillip Jones
 * Department of Electrical and Computer Engineering
 * Iowa State University
 *****************************************************************************/

/*****************************************************************************
 * vga_test.c - example VGA out using a v_tc timing controller and vdma
 * module.
 *
 *
 * NOTES:
 * 10/11/13 by JAZ::Design created.
 * 1/15/2018 by PHJ: Update XVtc_Enable, new function that take only one arg
 * 1/16/2018 by PHJ: Upaded to provide students with an option for directly
                     accessing registers via pointers.
 *****************************************************************************/


#include <stdio.h>
#include "xparameters.h"
#include "xvtc.h"
#include "xaxivdma.h"


u16 test_image[480][640];
u32 *VTC_CR = (u32*) 0x43C00000;
u32 ENABLE_VTC = 0x00000001;



int main() {

	XVtc Vtc;
    XVtc_Config *VtcCfgPtr;

    int i, j;

    printf("starting vga\n\r");

    // Enable VTC module: Using high-level functions provided by Vendor
    VtcCfgPtr = XVtc_LookupConfig(XPAR_AXI_VDMA_0_DEVICE_ID);
    XVtc_CfgInitialize(&Vtc, VtcCfgPtr, VtcCfgPtr->BaseAddress);
    XVtc_EnableGenerator(&Vtc);

    // Challenge: Can you rewrite the Enable VTC module code by directly accessing
    // the VTC registers using pointers?  (See VTC data sheet, and xparameters.h)

    VTC_CR[0]   |=  ENABLE_VTC;    // You: Declare VTC_CR and ENABLE_VTC appropriately (before main() )



    // Initialize Test image for VDMA transfer to VGA monitor
    for (i = 0; i < 480; i++) {
      for (j = 0; j < 640; j++) {

        if (j < 213) {
           test_image[i][j] = 0x000F; // red pixels
        }
        else if(j < 426 ) {
          test_image[i][j] = 0x00F0; // green pixels
        }
        else if (j < 638){
          test_image[i][j] = 0x0F00; // blue pixels
        }
        else {
        	test_image[i][j] = 0x0000; // 2 columns of black to account for dark color issues
        }

      }
    }

    Xil_DCacheFlush();

    // Set up VDMA config registers
	#define CHANGE_ME 0

    // Simple function abstraction by Vendor for writing VDMA registers
    XAxiVdma_WriteReg(XPAR_AXI_VDMA_0_BASEADDR, XAXIVDMA_CR_OFFSET,  0x3);  // Read Channel: VDMA MM2S Circular Mode and Start bits set, VDMA MM2S Control
    XAxiVdma_WriteReg(XPAR_AXI_VDMA_0_BASEADDR, XAXIVDMA_HI_FRMBUF_OFFSET, 0x0);  // Read Channel: VDMA MM2S Reg_Index
    XAxiVdma_WriteReg(XPAR_AXI_VDMA_0_BASEADDR, XAXIVDMA_MM2S_ADDR_OFFSET + XAXIVDMA_START_ADDR_OFFSET, (u32)test_image );  // Read Channel: VDMA MM2S Frame buffer Start Addr 1
    XAxiVdma_WriteReg(XPAR_AXI_VDMA_0_BASEADDR, XAXIVDMA_MM2S_ADDR_OFFSET + XAXIVDMA_STRD_FRMDLY_OFFSET, 0x500);  // Read Channel: VDMA MM2S FRM_Delay, and Stride
    XAxiVdma_WriteReg(XPAR_AXI_VDMA_0_BASEADDR, XAXIVDMA_MM2S_ADDR_OFFSET + XAXIVDMA_HSIZE_OFFSET, 0x500);  // Read Channel: VDMA MM2S HSIZE
    XAxiVdma_WriteReg(XPAR_AXI_VDMA_0_BASEADDR, XAXIVDMA_MM2S_ADDR_OFFSET + XAXIVDMA_VSIZE_OFFSET, 0x1E0);  // Read Channel: VDMA MM2S VSIZE  (Note: Also Starts VDMA transaction)


    // Low-level register acess using pointers
    // Alternative approach for configuring VDMA registers: Instead of using the abstracted function can you configure and start the VDMA using pointers to directly configure VDMA registers
    // YOU: Declare VDMA_MM2S_XXX (before main) and set values "CHANGE_ME" appropriately, before main()

    // VDMA_MM2S_CR[0]             = CHANGE_ME; // Read Channel: VDMA MM2S Circular/Park Mode and enable the channel
    // VDMA_MM2S_REG_INDEX[0]      = CHANGE_ME; // Read Channel: VDMA MM2S Reg_Index
    // VDMA_MM2S_START_ADDRESS1[0] = CHANGE_ME; // Read channel: VDMA MM2S Frame buffer Start Add 1
    // VDMA_MM2S_FRMDLY_STRIDE[0]  = CHANGE_ME; // Read channel: VDMA MM2S FRM_Delay, and Stride
    // VDMA_MM2S_HSIZE[0]          = CHANGE_ME; // Read channel: VDMA MM2S HSIZE
    // VDMA_MM2S_VSIZE[0]          = CHANGE_ME; // Read channel: VDMA MM2S VSIZE  (Note: Also Starts VDMA transaction)

    printf("done\n\r");



    return 0;
}
