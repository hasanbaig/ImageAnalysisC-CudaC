//C implementation of Major Minor Axes Scan
//Author: Hasan Baig
//Email: hasan.baig@hotmail.com
//Web: http://www.hasanbaig.com

#include "iostream"
#include "conio.h" 
#include "time.h"
#include "../defines.h"
#include "../my_C_functions.h"

int main(void)
{
//	time_t st,end;
	
	int hor_scan[TILE_WIDTH_SCAN*BLOCKS_Y_SCAN][BLOCKS_X_SCAN];
	int ver_scan[BLOCKS_Y_SCAN][WIDTH];

	int hor_max_obj[BLOCKS_Y_SCAN][BLOCKS_X_SCAN] = {0};
	int ver_max_obj[BLOCKS_Y_SCAN][BLOCKS_X_SCAN] = {0};


	for(i=0;i<Rows;i++)
	{
		for(j=0;j<Cols;j++)
			printf("%d",Image[i][j]);
		printf("\n");
	}

	st1 = time (NULL);
	printf("\n\nTime Before running Noise-Function\n");
	printf (ctime(&st1));
	
	int c1 = clock();	
	for(m=0;m<Loop_C;m++)
	maj_min_scan_C(Image, hor_scan, ver_scan,	hor_max_obj, ver_max_obj);
	
	int c2 = clock();
	printf(" C1 = %d , C2 = %d , Diff = %d \n", c1, c2, c2-c1);
	
	end1 = time (NULL);
	printf("\nTime after running Noise-Function\n");
	printf (ctime(&end1));

	//*************** For debugging ****************
	printf("\n Horizontal Scan \n");
	printf(" ---------------\n\n");
	for (i=0;i<(TILE_WIDTH_SCAN*BLOCKS_Y_SCAN);i++)
	{ 
		for (j=0;j<BLOCKS_X_SCAN;j++)
			printf("%d ", hor_scan[i][j]);
		
		printf("\n"); 
	}

	printf("\n Vertical Scan \n");
	printf(" -------------\n\n");
	for (i=0;i<BLOCKS_Y_SCAN;i++)
	{
		for (j=0;j<WIDTH;j++)
			printf("%d ", ver_scan[i][j]);

		printf("\n"); 
	}

	printf("\n Horizontal Max Values \n");
	printf(" ---------------------\n\n");
	for (i=0;i<BLOCKS_Y_SCAN;i++)
	{
		for (j=0;j<BLOCKS_X_SCAN;j++)
			printf("%d ", hor_max_obj[i][j]);

		printf("\n"); 
	}

	printf("\n Vertical Max Values \n");
	printf(" ---------------------\n\n");
	for (i=0;i<BLOCKS_Y_SCAN;i++)
	{
		for (j=0;j<BLOCKS_X_SCAN;j++)
			printf("%d ", ver_max_obj[i][j]);

		printf("\n"); 
	}

	//**********************************************
	
	system ("pause");
	return 0;
}
