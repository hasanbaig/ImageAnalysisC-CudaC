//GPU implementation of Major Minor Axes Scan
#include "iostream"
#include "conio.h" 
#include "time.h"
#include "D:/hasan_cuda_VC/CUDA_Stuff/defines.h"
#include "D:/hasan_cuda_VC/CUDA_Stuff/my_G_functions.h"
#include "C:/ProgramData/NVIDIA Corporation/NVIDIA GPU Computing SDK 4.0/C/common/inc/cutil.h"



int main (void)
{
	int *dev_Image, *dev_hor_scan, *dev_ver_scan, *dev_hor_max_obj, *dev_ver_max_obj;
	
	cudaEvent_t start, stop; 
	float time;

	unsigned int timer = 0;
	cutCreateTimer( &timer );


	cudaEventCreate(&start); 
	cudaEventCreate(&stop);

	printf("\nMatrix Image \n\n");
	for (i=0;i<(TILE_WIDTH_SCAN*BLOCKS_Y_SCAN);i++)
	{
		for (j=0;j<WIDTH;j++)
		{
			printf("%d", Image[i][j]);
		}
		printf("\n"); 
	}

	dim3 dimGrid(BLOCKS_X_SCAN, BLOCKS_Y_SCAN);								
	dim3 dimBlock(Threads_x_Scan, Threads_y_Scan);	


	cudaMalloc( (void**)&dev_Image, Image_size );
	cudaMalloc( (void**)&dev_hor_scan, hor_scan_size);
	cudaMalloc( (void**)&dev_ver_scan, ver_scan_size );
	cudaMalloc( (void**)&dev_hor_max_obj, hor_max_obj_size );
	cudaMalloc( (void**)&dev_ver_max_obj, ver_max_obj_size );

	cudaMemcpy( dev_Image, Image, Image_size,cudaMemcpyHostToDevice );
	cudaMemcpy( dev_hor_max_obj, hor_max_obj, hor_max_obj_size,cudaMemcpyHostToDevice );
	cudaMemcpy( dev_ver_max_obj, ver_max_obj, ver_max_obj_size,cudaMemcpyHostToDevice );
	
	//cudaThreadSynchronize();
	cudaEventRecord( start, 0 );

	for(m=0;m<Loop_G;m++)
	maj_min_scan_G<<<dimGrid, dimBlock>>>(dev_Image, dev_hor_scan, dev_ver_scan, dev_hor_max_obj, dev_ver_max_obj);
	
	//cudaThreadSynchronize();
	cudaEventRecord( stop, 0 ); 
	cudaEventSynchronize( stop ); 
	cudaEventElapsedTime( &time, start, stop ); 

	printf("\nTIME = %f ms\n\n", time);
	//printf("\nTIME = %f ms\n\n", cutGetTimerValue( timer ));
	cudaEventDestroy( start ); 
	cudaEventDestroy( stop );

	cudaMemcpy( hor_scan, dev_hor_scan, hor_scan_size , cudaMemcpyDeviceToHost ); 
	cudaMemcpy( ver_scan, dev_ver_scan, ver_scan_size , cudaMemcpyDeviceToHost ); 
	cudaMemcpy( hor_max_obj, dev_hor_max_obj, hor_max_obj_size , cudaMemcpyDeviceToHost ); 
	cudaMemcpy( ver_max_obj, dev_ver_max_obj, ver_max_obj_size , cudaMemcpyDeviceToHost ); 

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

	printf("\n Horizontal Axes Max Values \n");
	printf(" ---------------------\n\n");
	for (i=0;i<BLOCKS_Y_SCAN;i++)
	{
		for (j=0;j<BLOCKS_X_SCAN;j++)
			printf("Obj[%d]=%d ",i+j, hor_max_obj[i][j]);

		printf("\n"); 
	}

	printf("\n Vertical Axes Max Values \n");
	printf(" ---------------------\n\n");
	for (i=0;i<BLOCKS_Y_SCAN;i++)
	{
		for (j=0;j<BLOCKS_X_SCAN;j++)
			printf("Obj[%d]=%d ",i+j, ver_max_obj[i][j]);

		printf("\n"); 
	}
	
	//**********************************************
	system ("pause");
	return 0;
}


