//Integrated 
#include "iostream"
#include "conio.h" 
#include "time.h"
#include "D:/hasan_cuda_VC/CUDA_Stuff/defines.h"
#include "D:/hasan_cuda_VC/CUDA_Stuff/my_G_functions.h"

int main(void)
{
	int		*Image_In_dev,
			*Image_Out_dev, 
			*Image_Erode_dev, 
			*Temp_dev, 
			*dev_hor_scan,
			*dev_ver_scan, 
			*dev_hor_max_obj, 
			*dev_ver_max_obj;

	float	*dev_e_obj;
	
	int		Image_Out [Rows][Cols];
	int		Image_Erode[Rows][Cols];
	int		Temp[Rows][Cols];
	int		Image_size = Rows * Cols * sizeof(int);

	cudaEvent_t start, stop; 
	float time1,time2,time3;

	cudaEventCreate(&start); 
	cudaEventCreate(&stop);

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

	cudaMalloc( (void**)&Image_In_dev, Image_size );
	cudaMalloc( (void**)&Image_Out_dev, Image_size );
	cudaMalloc( (void**)&Image_Erode_dev, Image_size );
	cudaMalloc( (void**)&Temp_dev, Image_size );
	cudaMalloc( (void**)&dev_hor_scan, hor_scan_size );
	cudaMalloc( (void**)&dev_ver_scan, ver_scan_size );
	cudaMalloc( (void**)&dev_hor_max_obj, hor_max_obj_size );
	cudaMalloc( (void**)&dev_ver_max_obj, ver_max_obj_size);
	cudaMalloc( (void**)&dev_e_obj, e_obj_size );

	
	//************************** Noise Removing Kernel ************************
	
	cudaMemcpy( Image_In_dev, Noisy_Image, Image_size,cudaMemcpyHostToDevice );

	dim3 dimGrid_1(BLOCKS_X_MORPH, BLOCKS_Y_MORPH);								
	dim3 dimBlock_1(Threads_x_Morph, Threads_y_Morph);

	cudaEventRecord( start, 0 );
	im_open_G <<<dimGrid_1, dimBlock_1>>>
		(
			Image_In_dev, 
			Image_Out_dev, 
			Image_Erode_dev, 
			Temp_dev
		);

	cudaEventRecord( stop, 0 ); 
	cudaEventSynchronize( stop );
	cudaEventElapsedTime( &time1, start, stop ); 

	printf("\nTIME to Run Noise Removing Kernel = %f ms\n\n", time1);
	cudaEventDestroy( start ); 
	cudaEventDestroy( stop );


	cudaMemcpy( Image_Out, Image_Out_dev, Image_size , cudaMemcpyDeviceToHost );

	printf("Noise Free Image\n\n");
	for (i=0;i<Rows;i++)
	{
		for (j=0;j<Cols;j++)
		{
			printf("%d", Image_Out[i][j]);
			if(j==(Rows - 1))
				printf("\n");
		}
	}


	//*************************************************************************
	
	cudaEventCreate(&start); 
	cudaEventCreate(&stop);
	//************** Major/Minor Axes Length calculations Kernel ************** 
	dim3 dimGrid_2(BLOCKS_X_SCAN, BLOCKS_Y_SCAN);								
	dim3 dimBlock_2(Threads_x_Scan, Threads_y_Scan);	

	cudaEventRecord( start, 0 );
	maj_min_scan_G <<<dimGrid_2, dimBlock_2>>>
		(
			Image_Out_dev, 
			dev_hor_scan, 
			dev_ver_scan, 
			dev_hor_max_obj, 
			dev_ver_max_obj
		);

	cudaEventRecord( stop, 0 ); 
	cudaEventSynchronize( stop );
	cudaEventElapsedTime( &time2, start, stop ); 

	printf("\nTIME to Run Maj/Min Axes Kernel = %f ms\n\n", time2);
	cudaEventDestroy( start ); 
	cudaEventDestroy( stop );

	cudaMemcpy( hor_max_obj, dev_hor_max_obj, hor_max_obj_size , cudaMemcpyDeviceToHost ); 
	cudaMemcpy( ver_max_obj, dev_ver_max_obj, ver_max_obj_size , cudaMemcpyDeviceToHost ); 

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

	//*************************************************************************

	cudaEventCreate(&start); 
	cudaEventCreate(&stop);
	//******************* Eccentricity calculations Kernel ******************** 
	dim3 dimGrid_3(1, 1);								
	dim3 dimBlock_3(2, 2, 1);	

	cudaEventRecord( start, 0 );
	e_calc_G <<<dimGrid_3,dimBlock_3>>> 
		(
			dev_hor_max_obj, 
			dev_ver_max_obj, 
			dev_e_obj
		);

	cudaEventRecord( stop, 0 ); 
	cudaEventSynchronize( stop );
	cudaEventElapsedTime( &time3, start, stop ); 

	printf("\nTIME to Run Eccentricity Calculation Kernel = %f ms\n\n", time3);
	cudaEventDestroy( start ); 
	cudaEventDestroy( stop );

	cudaMemcpy( e_obj, dev_e_obj, e_obj_size , cudaMemcpyDeviceToHost ); 

	printf("\n"); 
	for(i=0;i<BLOCKS_Y_SCAN;i++)
	{
		for(j=0;j<BLOCKS_X_SCAN;j++)
		{
			printf ("e[%d][%d] = %f\n",i,j,e_obj[i][j]);
		}
	}

	//*************************************************************************


	system("pause");
	return 0;
}
