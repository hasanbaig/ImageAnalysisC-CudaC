//C implementation of Eccentricity Calculations
//Author: Hasan Baig
//Email: hasan.baig@hotmail.com
//Web: http://www.hasanbaig.com

#include "iostream"
#include "conio.h" 
#include "time.h"
#include "../defines.h"
#include "../my_C_functions.h"


int main (void)
{
	maj_min_scan_C(Image, hor_scan, ver_scan,	hor_max_obj, ver_max_obj);

	printf("\n\nNoise Free Image\n\n");
	for (i=0;i<Rows;i++)
	{
		for (j=0;j<Cols;j++)
		{
			printf("%d", Image[i][j]);
			if(j==(Rows - 1))
				printf("\n");
		}
	}

	st1 = time (NULL);
	printf("\n\nTime Before running Eccentricity-Function\n");
	printf (ctime(&st1));
	
	int c1 = clock();
	for(m=0;m<Loop_C;m++)
	e_calc_C (hor_max_obj, ver_max_obj, e_obj);
	int c2 = clock();
	printf(" C1 = %d , C2 = %d , Diff = %d \n", c1, c2, c2-c1);

	end1 = time (NULL);
	printf("\nTime after running Eccentricity-Function\n");
	printf (ctime(&end1));
	// Debugging-------

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

	//-----------------

	printf("\n"); 
	for(i=0;i<BLOCKS_Y_SCAN;i++)
	{
		for(j=0;j<BLOCKS_X_SCAN;j++)
		{
			printf ("e[%d][%d] = %f\n",i,j,e_obj[i][j]);
		}
	}

	system("pause");
	return 0;
}

