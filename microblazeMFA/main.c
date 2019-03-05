#include <stdio.h>
#include <xio.h>

#include "xparameters.h"
#include "cam_ctrl_header.h"
#include "vmodcam_header.h"
#include "globals.h"
#include "xgpio.h"
#include "xuartns550.h"
#include "bluetooth.h"

#define blDvmaCR		0x00000000 // Control Reg Offset
#define blDvmaFWR		0x00000004 // Frame Width Reg Offset
#define blDvmaFHR		0x00000008 // Frame Height Reg Offset
#define blDvmaFBAR	0x0000000c // Frame Base Addr Reg Offset
#define blDvmaFLSR	0x00000010 // Frame Line Stride Reg Offeset
#define blDvmaHSR		0x00000014 // H Sync Reg Offset
#define blDvmaHBPR	0x00000018 // H Back Porch Reg Offset
#define blDvmaHFPR	0x0000001c // H Front Porch Reg Offset
#define blDvmaHTR		0x00000020 // H Total Reg Offset
#define blDvmaVSR		0x00000024 // V Sync Reg Offset
#define blDvmaVBPR	0x00000028 // V Back Porch Reg Offset
#define blDvmaVFPR	0x0000002c // V Front Porch Reg Offset
#define blDvmaVTR		0x00000030 // V Total Reg Offset


int GpioInputExample(u16 DeviceId, u32 *DataRead);
u32 convertKey(u32 key);
#define printf xil_printf

XGpio switches;


void main() {

	xil_printf("Entering Security System\n");

	u32 lDvmaBaseAddress = XPAR_DVMA_0_BASEADDR;
	int posX, posY;
	int color;
	for(posX = 0; posX<2560; posX++){
			for(posY = 0; posY<720; posY++)
				XIo_Out16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(posY*2560+posX), (posX/40)<<4);
	}

	XIo_Out32(lDvmaBaseAddress + blDvmaHSR, 40); // hsync
	XIo_Out32(lDvmaBaseAddress + blDvmaHBPR, 260); // hbpr
	XIo_Out32(lDvmaBaseAddress + blDvmaHFPR, 1540); // hfpr
	XIo_Out32(lDvmaBaseAddress + blDvmaHTR, 1650); // htr
	XIo_Out32(lDvmaBaseAddress + blDvmaVSR, 5); // vsync
	XIo_Out32(lDvmaBaseAddress + blDvmaVBPR, 25); // vbpr
	XIo_Out32(lDvmaBaseAddress + blDvmaVFPR, 745); // vfpr
	XIo_Out32(lDvmaBaseAddress + blDvmaVTR, 750); // vtr

/*
	XIo_Out32(lDvmaBaseAddress + blDvmaFWR, 0x00000500); // frame width
	XIo_Out32(lDvmaBaseAddress + blDvmaFHR, 0x000002D0); // frame height
	XIo_Out32(lDvmaBaseAddress + blDvmaFBAR, XPAR_DDR2_SDRAM_MPMC_BASEADDR); // frame base addr
	XIo_Out32(lDvmaBaseAddress + blDvmaFLSR, 0x00000A00); // frame line stride
	XIo_Out32(lDvmaBaseAddress + blDvmaCR, 0x00000003); // dvma enable, dfl enable

	CamIicCfg(XPAR_CAM_IIC_0_BASEADDR, CAM_CFG_640x480P);
	CamIicCfg(XPAR_CAM_IIC_1_BASEADDR, CAM_CFG_640x480P);
	CamCtrlInit(XPAR_CAM_CTRL_0_BASEADDR, CAM_CFG_640x480P, 640*2);
	CamCtrlInit(XPAR_CAM_CTRL_1_BASEADDR, CAM_CFG_640x480P, 0);
*/

	for(posX = 0; posX<2560; posX++)
		for(posY = 0; posY<720; posY++)
			XIo_Out16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(posY*2560+posX), 0x0000);

	// Initialize the Switch (Set the data direction to be input)
	int num = 0;
	int switch_val = 0;
	XGpio_Initialize(&switches, XPAR_SWITCH_DEVICE_ID);
	XGpio_SetDataDirection(&switches, 1, 0xFFFFFFFF);	// set to input
	int status = 0;

	XUartNs550Format BTFormat = {
			 .BaudRate = XUN_NS16550_MAX_RATE,
			 .DataBits = XUN_FORMAT_8_BITS,
			 .Parity   = XUN_FORMAT_NO_PARITY,
			 .StopBits = XUN_FORMAT_1_STOP_BIT,
		};

	status = XUartNs550_Initialize(&BT_UART, XPAR_BT_DEVICE_ID);
	if (status != XST_SUCCESS){
		xil_printf("GPIO failed to initialize\r\n");
		return XST_FAILURE;
	}
	XUartNs550_SetDataFormat(&BT_UART, &BTFormat);

	int bytes_sent = 0;

	while(1){

		// draw box on the screen where the user inputs a character/letter
		// draw top rectangle
		for(posX = X_MIN-BORDER_WIDTH; posX<=X_MAX+BORDER_WIDTH; posX++){
			for(posY = Y_MIN-BORDER_WIDTH; posY<=Y_MIN; posY++){
				XIo_Out16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(posY*2560+posX), 0xf00);
			}
		}
		// draw bottom rect
		for(posX = X_MIN-BORDER_WIDTH; posX<=X_MAX+BORDER_WIDTH; posX++){
			for(posY = Y_MAX; posY<=Y_MAX+BORDER_WIDTH; posY++){
				XIo_Out16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(posY*2560+posX), 0xf00);
			}
		}
		// draw left rect
		for(posX = X_MIN-BORDER_WIDTH; posX<X_MIN; posX++){
			for(posY = Y_MIN; posY<=Y_MAX; posY++){
				XIo_Out16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(posY*2560+posX), 0xf00);
			}
		}
		// draw right rect
		for(posX = X_MAX; posX<=X_MAX+BORDER_WIDTH; posX++){
			for(posY = Y_MIN; posY<=Y_MAX+BORDER_WIDTH; posY++){
				XIo_Out16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(posY*2560+posX), 0xf00);
			}
		}
		switch_val = XGpio_DiscreteRead(&switches, 1);
		// if our switch is set
		if (switch_val == 1){
			// process the image to find Letter/User
			num = process();
			if (num == 1 || num == 2 || num == 3){
				break;
			}
		}
	}

	int red = 0;
	int green = 0;
	int blue = 0;
	u16 pixel = XIo_In16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(260*2560+811));
	blue += (pixel&0x000f);
	green += ((pixel&0x00f0)>>4);
	red += ((pixel&0x0f00)>>8);

	int sum[4] = { 0, 0, 0, 0 };
	int i;

	for (i = 0; i < 4; i++){
		u16 pixel = XIo_In16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(125*(i+1)*2560+811));
		blue = (pixel&0x000f);
		green = ((pixel&0x00f0)>>4);
		red = ((pixel&0x0f00)>>8);
		sum[i] = (red + green + blue) % 10;
	}

	for (i = 0; i < 4; i++){
		if (sum[i] == 0)
			sum[i]++;
	}


	for (i = 0; i < 4; i++){
		if (sum[i] == sum[i+1])
			sum[i+1]++;
		if (sum[i+1] == 10)
			sum[i+1] = 1;
	}

	char pin[4];
	for (i = 0; i < 4; i++){
		pin[i] = sum[i]+48;
	}
	//xil_printf("sum 0 is %d\n sum 1 is %d\n sum 2 is %d\n sum 3 is %d\n", sum[0], sum[1], sum[2], sum[3]);
	//xil_printf("pin 0 is %c\n pin 1 is %c\n pin 2 is %c\n pin 3 is %c\n", pin[0], pin[1], pin[2], pin[3]);
	xil_printf("Check your phone for a 4 digit pin \n");
	bytes_sent = BTSend(pin, 4);
	xil_printf("User Detected. Enter your Pin:\n");

	//password cant contain duplicates, cant contain zeroes

	u32 key1 = 0;
	key1 = keypad();


	u32 key2 = key1;
	while(key2 == key1 || key2 == 0)
		key2 = keypad();

	u32 key3 = key2;
	while(key3 == key2 || key3 == 0)
		key3 = keypad();

	u32 key4 = key3;
	while(key4 == key3 || key4 == 0)
		key4 = keypad();

	key1 = convertKey(key1);
	key2 = convertKey(key2);
	key3 = convertKey(key3);
	key4 = convertKey(key4);

	if (key1 == sum[0] && key2 == sum[1] && key3 == sum[2] && key4 == sum[3]){
		// Luke's pin is 3
		if (num == 1 ){
			xil_printf("Welcome Luke\nYou may now access your information.");
		}
		// David's pin is 2
		else if (num == 2){
			xil_printf("Welcome David\nYou may now access your information.");
		}
		// Matt's pin is 1
		else if (num == 3 ){
			xil_printf("Welcome Matt\nYou may now access your information.");
		}
	}
	else{
		xil_printf("Wrong pin. Access Denied");
	}





	//xil_printf("sum 0 is %d\n sum 1 is %d\n sum 2 is %d\n sum 3 is %d\n", sum[0], sum[1], sum[2], sum[3]);

}


u32 convertKey(u32 key) {
	u32 actual_key = 0;
	switch(key){
	case 3:
			actual_key = 1;
			break;
	case 7:
			actual_key = 2;
			break;
	case 11:
			actual_key = 3;
			break;
	case 2:
			actual_key = 4;
			break;
	case 6:
			actual_key = 5;
			break;
	case 10:
			actual_key = 6;
			break;
	case 1:
			actual_key = 7;
			break;
	case 5:
			actual_key = 8;
			break;
	case 9:
			actual_key = 9;
			break;
	default:
			break;
	}

	return actual_key;
}

