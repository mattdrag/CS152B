

#include "xio.h"
#include "xparameters.h"
#include "globals.h"
#include <stdio.h>
#define printf xil_printf	/* A smaller footprint printf */
/*
        0    1    2
+----+----+----+----+
|    |  0 |  1 |  2 |      0
+----+----+----+----+
|    |  3 |  4 |  5 |      1
+----+----+----+----+
|    |  6 |  7 |  8 |      2
+----+----+----+----+

*/


// Scans pixels in CELL and returns a 1 if the cell is shaded, 0 otherwise
char processBlock(int blockno) {

	int BLOCK_WIDTH = (X_MAX-X_MIN)/3;
	int BLOCK_HEIGHT = (Y_MAX-Y_MIN)/3;
	int yStart,xStart;
	int posX,posY;
	// Determine which block we are in
	if (blockno == 0){
		xStart = X_MIN;
		yStart = Y_MIN;
	}
	else if(blockno == 1){
		xStart = X_MIN+BLOCK_WIDTH;
		yStart = Y_MIN;
	}
	else if(blockno == 2){
		xStart = X_MIN + 2*BLOCK_WIDTH;
		yStart = Y_MIN;
	}
	else if(blockno == 3){
		xStart = X_MIN;
		yStart = Y_MIN + BLOCK_HEIGHT;
	}
	else if(blockno == 4){
		xStart = X_MIN  +BLOCK_WIDTH;
		yStart = Y_MIN + BLOCK_HEIGHT;
	}
	else if(blockno == 5){
		xStart = X_MIN + 2*BLOCK_WIDTH;
		yStart = Y_MIN+BLOCK_HEIGHT;
	}
	else if(blockno == 6){
		xStart = X_MIN;
		yStart = Y_MIN+2*BLOCK_HEIGHT;
	}
	else if(blockno == 7){
		xStart = X_MIN+BLOCK_WIDTH;
		yStart = Y_MIN+2*BLOCK_HEIGHT;
	}
	else if(blockno == 8){
		xStart = X_MIN+2*BLOCK_WIDTH;
		yStart = Y_MIN+2*BLOCK_HEIGHT;
	}
	else{
		;
	}

	// Average the pixel color of the block
	int numPixels = BLOCK_WIDTH * BLOCK_HEIGHT;
	int avgR = 0;
	int avgG = 0;
	int avgB = 0;
	u16 color = 0;

	for (posY=yStart ; posY < yStart+BLOCK_HEIGHT; posY++) {
		for (posX = xStart ; posX < xStart+BLOCK_WIDTH; posX++) {
			color = XIo_In16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(posY*2560+posX));
			avgB += (color&0x000f);
			avgG += ((color&0x00f0)>>4);
			avgR += ((color&0x0f00)>>8);
		}
	}

	// draw boxes in each spot
	/*
	for (posY=yStart ; posY < yStart+BLOCK_HEIGHT; posY++) {
			for (posX = xStart ; posX < xStart+BLOCK_WIDTH; posX++) {
				XIo_Out16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*(posY*2560+posX), 0xf00);
		}
	}
	*/

	avgR /= numPixels;
	avgG /= numPixels;
	avgB /= numPixels;

	//GREEN
	if (avgG > avgR && avgG > avgB){
		return 0;
	}
	//RED
	else if (avgR > avgG && avgR > avgB){
		return 1;
	}
	//BLUE
	else if (avgB > avgG && avgB > avgR){
		return 2;
	}
	else{
		return 0;
	}
}


int process() {

	int i;
	int grid[9];

	for (i = 0; i < NUM_CELLS; i++){
		grid[i] = 0;
	}

	// draw dot in center of each box (9 dots)
	/*
	int i,j;
	for(i = X_MIN; i<X_MAX; i+=(X_MAX-X_MIN)/3){
		for(j = Y_MIN; j<Y_MAX; j+=(Y_MAX-Y_MIN)/3){
			XIo_Out16(XPAR_DDR2_SDRAM_MPMC_BASEADDR + 2*((j+(Y_MAX-Y_MIN)/6)*2560+(i+(X_MAX-X_MIN)/3)), 0x0f0);
		}
	}
	*/

	for (i = 0; i < NUM_CELLS; i++){
		grid[i] = processBlock(i);
	}


	//////////////CHECK GRID COLORS///////////////////
	// 0 = green
	//1 = red
	//2 = blue

	//Detect the Letter "L"
	if (grid[0] == 1 && grid[1] == 0 && grid[2] == 0 && grid[3] == 1 && grid[4] == 0 && grid[5] == 0 && grid[6] == 1 && grid[7] == 1 && grid[8] == 0){
		//xil_printf("User Luke Found\n");
		return 1;
	}
	//Detect the Letter "D"
	else if(grid[0] == 1 && grid[1] == 1 && grid[2] == 0 && grid[3] == 1 && grid[4] == 0 && grid[5] == 1 && grid[6] == 1 && grid[7] == 1 && grid[8] == 0){
		//xil_printf("User David Found\n");
		return 2;
	}
	//Detect the Letter "H"
	else if (grid[0] == 1 && grid[1] == 0 && grid[2] == 1 && grid[3] == 1 && grid[4] == 1 && grid[5] == 1 && grid[6] == 1 && grid[7] == 0 && grid[8] == 1){
		//xil_printf("User Hatt Found\n");
		return 3;
	}
	else{
		return 0;
	}

}



