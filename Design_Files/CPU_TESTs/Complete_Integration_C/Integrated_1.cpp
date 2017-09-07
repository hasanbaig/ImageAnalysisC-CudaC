//Integrated complete project on C # 1

#include "iostream"
#include "conio.h" 
#include "time.h"
#include "D:/Academics/3. MS_Course_Work/3a. Computer Graphics (GPU)/Project/CUDA_Stuff/defines.h"
#include "D:/Academics/3. MS_Course_Work/3a. Computer Graphics (GPU)/Project/CUDA_Stuff/my_C_functions.h"

int main (void)
{
	int temp[Rows][Cols];
	int img_erode[Rows][Cols];
	int Image_Out[Rows][Cols];
	int SE[3] = {1,1,1};

	

	printf("Original Image with Noise\n\n");
	for (i=0;i<Rows;i++)
	{
		for (j=0;j<Cols;j++)
		{
			printf("%d", Noisy_Image[i][j]);
			if(j==(Rows - 1))
				printf("\n");
		}
	}
	st1 = time (NULL);
	printf("\n\nTime Before running Morph-Function\n");
	printf (ctime(&st1));
	for(m=0;m<Loop_C;m++)
		im_open_C(Noisy_Image, SE, Image_Out);
	
	end1 = time (NULL);
	printf("\nTime after running Morph-Function\n");
	printf (ctime(&end1));

	st2 = time (NULL);
	printf("\n\nTime Before running Maj/Min-Function\n");
	printf (ctime(&st2));
	for(m=0;m<Loop_C;m++)
		maj_min_scan_C(Image_Out, hor_scan, ver_scan, hor_max_obj, ver_max_obj);
	
	end2 = time (NULL);
	printf("\nTime after running Maj/Min-Function\n");
	printf (ctime(&end2));

	st3 = time (NULL);
	printf("\n\nTime Before running Eccentricity-Function\n");
	printf (ctime(&st3));
	for(m=0;m<Loop_G;m++)
		e_calc_C (hor_max_obj, ver_max_obj, e_obj);

	end3 = time (NULL);
	printf("\nTime after running Eccentricity-Function\n");
	printf (ctime(&end3));






	//----------------------------------------------------------------------
	printf("\n\nNoise Free Image\n\n");
	for (i=0;i<Rows;i++)
	{
		for (j=0;j<Cols;j++)
		{
			printf("%d", Image_Out[i][j]);
			if(j==(Rows - 1))
				printf("\n");
		}
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

	printf("\n"); 
	for(i=0;i<BLOCKS_Y_SCAN;i++)
	{
		for(j=0;j<BLOCKS_X_SCAN;j++)
		{
			printf ("e[%d][%d] = %f\n",i,j,e_obj[i][j]);
		}
	}

	//----------------------------------------------------------------------

	system("pause");
	return 0;
}