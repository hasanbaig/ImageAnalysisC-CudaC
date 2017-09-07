// Noise removing through morphological techniques on GPU
#include "iostream"
#include "conio.h" 
#include "time.h"
#include "D:/hasan_cuda_VC/CUDA_Stuff/defines.h"
#include "D:/hasan_cuda_VC/CUDA_Stuff/my_C_functions.h"
#include "D:/hasan_cuda_VC/CUDA_Stuff/my_G_functions_SM.h"
#include "C:/ProgramData/NVIDIA Corporation/NVIDIA GPU Computing SDK 4.0/C/common/inc/cutil.h"

int main (void)
{
	int *Image_In_dev, *Image_Out_dev;
	int Image_size = Rows * Cols * sizeof(int);
	int Image_Out [Rows][Cols];

	//for debugging
	int Image_Erode[Rows][Cols];
	int Temp[Rows][Cols];
	int *Image_Erode_dev;
	int *Temp_dev;

	cudaEvent_t start, stop; 
	float time;

	cudaEventCreate(&start); 
	cudaEventCreate(&stop);

	dim3 dimGrid(BLOCKS_Y_SCAN, BLOCKS_X_SCAN);								
	dim3 dimBlock(Threads_y_Scan, Threads_x_Scan);

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

	cudaMemcpy( Image_In_dev, Noisy_Image, Image_size,cudaMemcpyHostToDevice );
	//cudaMemcpy( Image_Out_dev, Image_Out, Image_size,cudaMemcpyHostToDevice );
	//cudaThreadSynchronize();
	cudaEventRecord( start, 0 );
	im_open_G_SM<<<dimGrid, dimBlock>>>(Image_In_dev, Image_Out_dev, Image_Erode_dev, Temp_dev);	
	//cudaThreadSynchronize();
	cudaEventRecord( stop, 0 ); 
	cudaEventSynchronize( stop ); 
	cudaEventElapsedTime( &time, start, stop ); 

	printf("\nTIME = %f \n\n", time);

//	cudaMemcpy( Image_Erode, Image_Erode_dev, Image_size , cudaMemcpyDeviceToHost );
//	cudaMemcpy( Temp, Temp_dev, Image_size , cudaMemcpyDeviceToHost );
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

	system ("pause");
	return 0;
}