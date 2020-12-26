
#include "camera_app.h"
#include "xtime_l.h"
#include <stdio.h>

camera_config_t camera_config;
Xuint16 recordings[32][1920*1080];

// Main function. Initializes the devices and configures VDMA
int main() {

	camera_config_init(&camera_config);
	fmc_imageon_enable(&camera_config);
	camera_loop(&camera_config);

	return 0;
}


// Initialize the camera configuration data structure
void camera_config_init(camera_config_t *config) {

    config->uBaseAddr_IIC_FmcIpmi =  XPAR_FMC_IPMI_ID_EEPROM_0_BASEADDR;   // Device for reading HDMI board IPMI EEPROM information
    config->uBaseAddr_IIC_FmcImageon = XPAR_FMC_IMAGEON_IIC_0_BASEADDR;    // Device for configuring the HDMI board

    // Uncomment when using VITA Camera for Video input
    config->uBaseAddr_VITA_SPI = XPAR_ONSEMI_VITA_SPI_0_S00_AXI_BASEADDR;  // Device for configuring the Camera sensor
    config->uBaseAddr_VITA_CAM = XPAR_ONSEMI_VITA_CAM_0_S00_AXI_BASEADDR;  // Device for receiving Camera sensor data

    // Uncomment as Video Hardware pipeline IP cores are added
    config->uDeviceId_RGBYCC = XPAR_RGB2YCRCB_0_DEVICE_ID;

    // Uncomment when using the TPG for Video input
   // config->uBaseAddr_TPG_PatternGenerator = XPAR_V_TPG_0_S_AXI_CTRL_BASEADDR; // TPG Device

    config->uDeviceId_VTC_tpg   = XPAR_V_TC_0_DEVICE_ID;                        // Video Timer Controller (VTC) ID
    config->uDeviceId_VDMA_HdmiFrameBuffer = XPAR_AXI_VDMA_0_DEVICE_ID;         // VDMA ID
    config->uBaseAddr_MEM_HdmiFrameBuffer = XPAR_DDR_MEM_BASEADDR + 0x10000000; // VDMA base address for Frame buffers
    config->uNumFrames_HdmiFrameBuffer = XPAR_AXIVDMA_0_NUM_FSTORES;            // NUmber of VDMA Frame buffers

    return;
}


void print_frame_rate(camera_config_t *config) {

	Xuint32 vdma_MM2S_DMACR;
	XTime start_t, end_t;

	vdma_MM2S_DMACR = XAxiVdma_ReadReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_TX_OFFSET+XAXIVDMA_CR_OFFSET);
	XAxiVdma_WriteReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_TX_OFFSET+XAXIVDMA_CR_OFFSET, vdma_MM2S_DMACR | 0x00FE0010); // Make the frame count value 255 frames and set FrameCntEn
	XTime_GetTime(&start_t); 		// get time before 255 frames start processing
	while (!(XAxiVdma_ReadReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_TX_OFFSET+XAXIVDMA_SR_OFFSET) & 0x00000001)) { // wait for halted bit to be high when 255 frames are done
		// wait until 255 frames have been processed
	}
	XTime_GetTime(&end_t); // get time after 255 frames processed
	double time_in_seconds = ((double) (end_t-start_t)) / COUNTS_PER_SECOND; // get time in seconds for 255 frames to process
	printf("\n\rFRAME RATE: %lf fps\n\r", (255.0 / time_in_seconds)); // divide to get FPS

}

void camera_interface(camera_config_t *config) {


	int l_pressed = 0;
	int r_pressed = 0;
	int j = 0;
	int i = 0;
	int photo_count = 0;
	XTime start_t, end_t;


	// Pointers to the S2MM memory frame and M2SS memory frame
	volatile Xuint16 *pS2MM_Mem = (Xuint16 *)XAxiVdma_ReadReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_S2MM_ADDR_OFFSET+XAXIVDMA_START_ADDR_OFFSET);
	volatile Xuint16 *pMM2S_Mem = (Xuint16 *)XAxiVdma_ReadReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_MM2S_ADDR_OFFSET+XAXIVDMA_START_ADDR_OFFSET+4);
	

	uint32_t *button_ptr = (uint32_t*) XPAR_AXI_GPIO_0_BASEADDR;
	uint32_t *switch_ptr = (uint32_t*) XPAR_AXI_GPIO_1_BASEADDR;

	while (!(*switch_ptr & 0x1)) { 					// exit back to HW mode if SW0 is on
			if (*button_ptr & 0x1) { 				// BTNC for capturing an image
				if (j > 31) { 						// max number of images is 32
					j = 31;
					xil_printf("MEMORY FULL\n\r");
				}
				for(i = 0; i < 1080*1920; i++){ 	// capture frame into recordings array
					recordings[j][i] = pS2MM_Mem[i];
				}
				XTime_GetTime(&start_t);
				XTime_GetTime(&end_t);
				while ((end_t - start_t) / COUNTS_PER_SECOND < 2) { // display image for 2 sec
					XTime_GetTime(&end_t);
				}
				photo_count = j; 					// get number of photos saved in array for later use
				j++;
			}

			if (*switch_ptr & 0x2) { 				// SW1 for playback mode
				if (*button_ptr & 0x4) { 			// BTNL for previous image
					if (l_pressed == 0) { 			// if button is held down, don't keep cycling, only on single presses
						j--;
						if (j < 0) { 				// if cycling through images goes below first image, reset to first image
							j = 0;
						}
						for(i = 0; i < 1080*1920; i++){ 		// display specific image
							pMM2S_Mem[i] = recordings[j][i];
						}
						l_pressed = 1;
					}
				}
				else {
					l_pressed = 0;
				}
				if (*button_ptr & 0x8) {  // BTNR for next image
					if (r_pressed == 0) {  // if button is held down, don't keep cycling, only on single presses
						j++;
						if (j > photo_count) { // if cycling through images goes above number captured, last captured is to be displayed
							j = photo_count;
						}
						for(i = 0; i < 1080*1920; i++){ // display specific image
							pMM2S_Mem[i] = recordings[j][i];
						}
						r_pressed = 1;
					}
				}
				else {
					r_pressed = 0;
				}

			}
			else {
				for(i = 0; i < 1080*1920; i++){ // when not in playback mode, display current frames for capturing
					pMM2S_Mem[i] = pS2MM_Mem[i];
				}
			}

		}
}


// Main (SW) processing loop. Recommended to have an explicit exit condition
void camera_loop(camera_config_t *config) {

	Xuint32 parkptr;
	Xuint32 vdma_S2MM_DMACR, vdma_MM2S_DMACR;

	// Grab the DMA parkptr, and update it to ensure that when parked, the S2MM side is on frame 0, and the MM2S side on frame 1
	parkptr = XAxiVdma_ReadReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_PARKPTR_OFFSET);
	parkptr &= ~XAXIVDMA_PARKPTR_READREF_MASK;
	parkptr &= ~XAXIVDMA_PARKPTR_WRTREF_MASK;
	parkptr |= 0x1;
	XAxiVdma_WriteReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_PARKPTR_OFFSET, parkptr);


	// Grab the DMA Control Registers, and clear circular park mode.
	vdma_MM2S_DMACR = XAxiVdma_ReadReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_TX_OFFSET+XAXIVDMA_CR_OFFSET);
	XAxiVdma_WriteReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_TX_OFFSET+XAXIVDMA_CR_OFFSET, vdma_MM2S_DMACR & (~XAXIVDMA_CR_TAIL_EN_MASK));
	vdma_S2MM_DMACR = XAxiVdma_ReadReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_RX_OFFSET+XAXIVDMA_CR_OFFSET);
	XAxiVdma_WriteReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_RX_OFFSET+XAXIVDMA_CR_OFFSET, vdma_S2MM_DMACR & (~XAXIVDMA_CR_TAIL_EN_MASK));


	/** uncommnet to print out frame rate, this will cause a halt in pipeline after printing **/
	//print_frame_rate(&camera_config);


	camera_interface(&camera_config);


	// Grab the DMA Control Registers, and re-enable circular park mode.
	vdma_MM2S_DMACR = XAxiVdma_ReadReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_TX_OFFSET+XAXIVDMA_CR_OFFSET);
	XAxiVdma_WriteReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_TX_OFFSET+XAXIVDMA_CR_OFFSET, vdma_MM2S_DMACR | XAXIVDMA_CR_TAIL_EN_MASK);
	vdma_S2MM_DMACR = XAxiVdma_ReadReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_RX_OFFSET+XAXIVDMA_CR_OFFSET);
	XAxiVdma_WriteReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_RX_OFFSET+XAXIVDMA_CR_OFFSET, vdma_S2MM_DMACR | XAXIVDMA_CR_TAIL_EN_MASK);


	return;
}
