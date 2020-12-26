
#include "camera_app.h"
#include "xtime_l.h"


camera_config_t camera_config;

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
//    config->uDeviceId_CFA = XPAR_CFA_0_DEVICE_ID;
//    config->uDeviceId_CRES = XPAR_CRESAMPLE_0_DEVICE_ID;
//    config->uDeviceId_RGBYCC = XPAR_RGB2YCRCB_0_DEVICE_ID;

    // Uncomment when using the TPG for Video input
    //config->uBaseAddr_TPG_PatternGenerator = XPAR_V_TPG_0_S_AXI_CTRL_BASEADDR; // TPG Device

    config->uDeviceId_VTC_tpg   = XPAR_V_TC_0_DEVICE_ID;                        // Video Timer Controller (VTC) ID
    config->uDeviceId_VDMA_HdmiFrameBuffer = XPAR_AXI_VDMA_0_DEVICE_ID;         // VDMA ID
    config->uBaseAddr_MEM_HdmiFrameBuffer = XPAR_DDR_MEM_BASEADDR + 0x10000000; // VDMA base address for Frame buffers
    config->uNumFrames_HdmiFrameBuffer = XPAR_AXIVDMA_0_NUM_FSTORES;            // NUmber of VDMA Frame buffers

    return;
}


// Main (SW) processing loop. Recommended to have an explicit exit condition
void camera_loop(camera_config_t *config) {

	Xuint32 parkptr;
	Xuint32 vdma_S2MM_DMACR, vdma_MM2S_DMACR;
	int i, j, k, m;

	uint8_t bayer_image[1080][1920];
	uint8_t red[1080][1920];
	uint8_t green[1080][1920];
	uint8_t blue[1080][1920];
	uint8_t Y[1080][1920];
	uint8_t Cb[1080][1920];
	uint8_t Cr[1080][1920];

	xil_printf("Entering main SW processing loop\r\n");


	// Grab the DMA parkptr, and update it to ensure that when parked, the S2MM side is on frame 0, and the MM2S side on frame 1
	parkptr = XAxiVdma_ReadReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_PARKPTR_OFFSET);
	parkptr &= ~XAXIVDMA_PARKPTR_READREF_MASK;
	parkptr &= ~XAXIVDMA_PARKPTR_WRTREF_MASK;
	parkptr |= 0x1001;
	XAxiVdma_WriteReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_PARKPTR_OFFSET, parkptr);


	// Grab the DMA Control Registers, and clear circular park mode.
	vdma_MM2S_DMACR = XAxiVdma_ReadReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_TX_OFFSET+XAXIVDMA_CR_OFFSET);
	XAxiVdma_WriteReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_TX_OFFSET+XAXIVDMA_CR_OFFSET, vdma_MM2S_DMACR & ~XAXIVDMA_CR_TAIL_EN_MASK);
	vdma_S2MM_DMACR = XAxiVdma_ReadReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_RX_OFFSET+XAXIVDMA_CR_OFFSET);
	XAxiVdma_WriteReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_RX_OFFSET+XAXIVDMA_CR_OFFSET, vdma_S2MM_DMACR & ~XAXIVDMA_CR_TAIL_EN_MASK);



	// Pointers to the S2MM memory frame and M2SS memory frame
	volatile Xuint16 *pS2MM_Mem = (Xuint16 *)XAxiVdma_ReadReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_S2MM_ADDR_OFFSET+XAXIVDMA_START_ADDR_OFFSET);
	volatile Xuint16 *pMM2S_Mem = (Xuint16 *)XAxiVdma_ReadReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_MM2S_ADDR_OFFSET+XAXIVDMA_START_ADDR_OFFSET+4);


	xil_printf("Start processing 1000 frames!\r\n");
	xil_printf("pS2MM_Mem = %X\n\r", pS2MM_Mem);
	xil_printf("pMM2S_Mem = %X\n\r", pMM2S_Mem);

	XTime start_t, end_t;
	XTime_GetTime(&start_t);
	// Run for 1000 frames before going back to HW mode
	for (k = 0; k < 10; k++) {

		for (i = 0; i < 1080; i++) {
			for (j = 0; j < 1920; j++) {
				bayer_image[i][j] = (uint8_t) pS2MM_Mem[i*1920 +j] & 0x00FF;
			}
		}

		/* 1920*1080-i-1*/
		for (i = 0; i < 1080; i++) {
			for (j = 0; j < 1920; j++) {

				int i_minus_1 = i-1;
				int i_plus_1 = i+1;
				int j_minus_1 = j-1;
				int j_plus_1 = j+1;

				if(i == 0 ){
					if(j == 0){ 						//top left corner
						i_minus_1 = i+1;
						j_minus_1 = j+1;
					}
					else if(j == 1919){ 				//top right corner
						i_minus_1 = i+1;
						j_plus_1 = j-1;
					}
					else{
						i_minus_1 = i+1;				//top row except the corners
					}
				}
				else if(i == 1079){
					if(j == 0){ 						//bottom left corner
						j_minus_1 = j+1;
						i_plus_1 = i-1;
					}
					else if(j == 1919){ 				//bottom right corner
						i_plus_1 = i-1;
						j_plus_1 = j-1;
					}
					else{
						i_plus_1 = i-1;					//bottom row without the corners
					}
				}
				else if (j == 0 && i > 0 && i < 1079)
				{
					j_minus_1 = j+1;					//left column without the corners
				}

				else if (j == 1919 && i > 0 && i < 1079)
				{
					j_plus_1 = j-1;						//right column without the corners
				}


				if(i % 2 == 0  && j % 2 == 0){			//red pixel
					red[i][j] = bayer_image[i][j];
					
					blue[i][j] =  (bayer_image[i_minus_1][j_minus_1]
								 + bayer_image[i_plus_1][j_minus_1]
								 + bayer_image[i_minus_1][j_plus_1]
								 + bayer_image[i_plus_1][j_plus_1])/4;
					
					green[i][j] =  (bayer_image[i_plus_1][j]
								  + bayer_image[i_minus_1][j]
								  + bayer_image[i][j_minus_1]
								  + bayer_image[i][j_plus_1])/4;
				}
				else if (i % 2 == 1 && j % 2 == 1) {	 // blue pixel
					blue[i][j] = bayer_image[i][j];
					
					red[i][j] =  (bayer_image[i_minus_1][j_minus_1] ]
								+ bayer_image[i_plus_1][j_minus_1]
								+ bayer_image[i_minus_1][j_plus_1]
								+ bayer_image[i_plus_1][j_plus_1])/4;

					green[i][j] =  (bayer_image[i_plus_1][j]
								  + bayer_image[i_minus_1][j]
								  + bayer_image[i][j_minus_1]
								  + bayer_image[i][j_plus_1])/4;
				}
				else if (i % 2 == 0 && j % 2 == 1) {	//green pixel with red on left and right
					green[i][j] = bayer_image[i][j];
					red[i][j] =  (bayer_image[i][j_minus_1]  + bayer_image[i][j_plus_1])/2;
					blue[i][j] =  (bayer_image[i_minus_1][j] + bayer_image[i_plus_1][j])/2;
				}
				else if (i % 2 == 1 && j % 2 == 0) {	//green pixel with red on up and down
					green[i][j] = bayer_image[i][j];
					blue[i][j] =  (bayer_image[i][j_minus_1]  + bayer_image[i][j_plus_1])/2;
					red[i][j] =  (bayer_image[i_minus_1][j] + bayer_image[i_plus_1][j])/2;
				}

				Y[i][j] = (uint8_t)(0.183*red[i][j] + 0.614*green[i][j] + 0.062*blue[i][j] + 16);
				Cb[i][j] = (uint8_t)(-0.101*red[i][j] - 0.338*green[i][j] + 0.439*blue[i][j] + 128);
				Cr[i][j] = (uint8_t)(0.439*red[i][j] - 0.399*green[i][j] - 0.040*blue[i][j] + 128);

			}
		}

		for (m = 0; m < 1920*1080-1; m += 2) {
			int temp_i = m / 1920;
			int temp_j = m % 1920;
			pMM2S_Mem[m] = ((uint16_t)Cb[temp_i][temp_j] << 8) | Y[temp_i][temp_j];
			pMM2S_Mem[m+1] = ((uint16_t)Cr[temp_i][temp_j] << 8) | Y[temp_i][temp_j];
		}



	}
	XTime_GetTime(&end_t);
	printf("\n\rFrame Rate: %lf fps\n\r\n\r\n\r", (10.0 * COUNTS_PER_SECOND / (end_t-start_t)));
	// 0.547788 fps^


	// Grab the DMA Control Registers, and re-enable circular park mode.
	vdma_MM2S_DMACR = XAxiVdma_ReadReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_TX_OFFSET+XAXIVDMA_CR_OFFSET);
	XAxiVdma_WriteReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_TX_OFFSET+XAXIVDMA_CR_OFFSET, vdma_MM2S_DMACR | XAXIVDMA_CR_TAIL_EN_MASK);
	vdma_S2MM_DMACR = XAxiVdma_ReadReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_RX_OFFSET+XAXIVDMA_CR_OFFSET);
	XAxiVdma_WriteReg(config->vdma_hdmi.BaseAddr, XAXIVDMA_RX_OFFSET+XAXIVDMA_CR_OFFSET, vdma_S2MM_DMACR | XAXIVDMA_CR_TAIL_EN_MASK);


	xil_printf("Main SW processing loop complete!\r\n");

	sleep(5);


	sleep(1);


	return;
}
