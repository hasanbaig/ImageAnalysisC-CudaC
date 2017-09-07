<<<<<<< HEAD
//GPU implementation of Eccentricity Calculation
//Author: Hasan Baig
//Email: hasan.baig@hotmail.com 
//Web: http://www.hasanbaig.com 

#include "iostream"
#include "conio.h" 
#include "time.h"
#include "../defines.h"
#include "../my_G_functions.h"
#include "../my_G_functions_SM.h"



int main (void)
{
	int *dev_Image, *dev_hor_scan, *dev_ver_scan, *dev_hor_max_obj, *dev_ver_max_obj;
	float *dev_e_obj;
	
	cudaEvent_t start, stop; 
	float time;

	cudaEventCreate(&start); 
	cudaEventCreate(&stop);

	cudaMalloc( (void**)&dev_Image, Image_size );
	cudaMalloc( (void**)&dev_hor_scan, hor_scan_size);
	cudaMalloc( (void**)&dev_ver_scan, ver_scan_size );
	cudaMalloc( (void**)&dev_hor_max_obj, hor_max_obj_size );
	cudaMalloc( (void**)&dev_ver_max_obj, ver_max_obj_size );
	cudaMalloc( (void**)&dev_e_obj, e_obj_size );

	cudaMemcpy( dev_Image, Image, Image_size,cudaMemcpyHostToDevice );
	cudaMemcpy( dev_hor_max_obj, hor_max_obj, hor_max_obj_size,cudaMemcpyHostToDevice );
	cudaMemcpy( dev_ver_max_obj, ver_max_obj, ver_max_obj_size,cudaMemcpyHostToDevice );

	dim3 dimGrid(2, 2);								
	dim3 dimBlock(17,17,1);	
	
	maj_min_scan_G<<<dimGrid, dimBlock>>>(dev_Image, dev_hor_scan, dev_ver_scan, dev_hor_max_obj, dev_ver_max_obj);


	dim3 dimGrid_e(2, 2);								
	dim3 dimBlock_e(1, 1, 1);	


	//cudaThreadSynchronize();
	cudaEventRecord( start, 0 );

	e_calc_G_SM<<<dimGrid_e,dimBlock_e>>>(dev_hor_max_obj, dev_ver_max_obj, dev_e_obj);
	
	//cudaThreadSynchronize();
	cudaEventRecord( stop, 0 ); 
	cudaEventSynchronize( stop );
	cudaEventElapsedTime( &time, start, stop ); 

	printf("\nTIME = %f ms\n\n", time);
	cudaEventDestroy( start ); 
	cudaEventDestroy( stop );

	cudaMemcpy( hor_scan, dev_hor_scan, hor_scan_size , cudaMemcpyDeviceToHost ); 
	cudaMemcpy( ver_scan, dev_ver_scan, ver_scan_size , cudaMemcpyDeviceToHost ); 
	cudaMemcpy( hor_max_obj, dev_hor_max_obj, hor_max_obj_size , cudaMemcpyDeviceToHost ); 
	cudaMemcpy( ver_max_obj, dev_ver_max_obj, ver_max_obj_size , cudaMemcpyDeviceToHost ); 
	cudaMemcpy( e_obj, dev_e_obj, e_obj_size , cudaMemcpyDeviceToHost ); 


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
=======
//GPU implementation of Eccentricity Calculation
#include "iostream"
#include "conio.h" 
#include "time.h"
#include "D:/hasan_cuda_VC/CUDA_Stuff/defines.h"
#include "D:/hasan_cuda_VC/CUDA_Stuff/my_G_functions.h"
#include "D:/hasan_cuda_VC/CUDA_Stuff/my_G_functions_SM.h"



int main (void)
{
	int *dev_Image, *dev_hor_scan, *dev_ver_scan, *dev_hor_max_obj, *dev_ver_max_obj;
	float *dev_e_obj;
	
	cudaEvent_t start, stop; 
	float time;

	cudaEventCreate(&start); 
	cudaEventCreate(&stop);

	cudaMalloc( (void**)&dev_Image, Image_size );
	cudaMalloc( (void**)&dev_hor_scan, hor_scan_size);
	cudaMalloc( (void**)&dev_ver_scan, ver_scan_size );
	cudaMalloc( (void**)&dev_hor_max_obj, hor_max_obj_size );
	cudaMalloc( (void**)&dev_ver_max_obj, ver_max_obj_size );
	cudaMalloc( (void**)&dev_e_obj, e_obj_size );

	cudaMemcpy( dev_Image, Image, Image_size,cudaMemcpyHostToDevice );
	cudaMemcpy( dev_hor_max_obj, hor_max_obj, hor_max_obj_size,cudaMemcpyHostToDevice );
	cudaMemcpy( dev_ver_max_obj, ver_max_obj, ver_max_obj_size,cudaMemcpyHostToDevice );

	dim3 dimGrid(2, 2);								
	dim3 dimBlock(17,17,1);	
	
	maj_min_scan_G<<<dimGrid, dimBlock>>>(dev_Image, dev_hor_scan, dev_ver_scan, dev_hor_max_obj, dev_ver_max_obj);


	dim3 dimGrid_e(2, 2);								
	dim3 dimBlock_e(1, 1, 1);	


	//cudaThreadSynchronize();
	cudaEventRecord( start, 0 );

	e_calc_G_SM<<<dimGrid_e,dimBlock_e>>>(dev_hor_max_obj, dev_ver_max_obj, dev_e_obj);
	
	//cudaThreadSynchronize();
	cudaEventRecord( stop, 0 ); 
	cudaEventSynchronize( stop );
	cudaEventElapsedTime( &time, start, stop ); 

	printf("\nTIME = %f ms\n\n", time);
	cudaEventDestroy( start ); 
	cudaEventDestroy( stop );

	cudaMemcpy( hor_scan, dev_hor_scan, hor_scan_size , cudaMemcpyDeviceToHost ); 
	cudaMemcpy( ver_scan, dev_ver_scan, ver_scan_size , cudaMemcpyDeviceToHost ); 
	cudaMemcpy( hor_max_obj, dev_hor_max_obj, hor_max_obj_size , cudaMemcpyDeviceToHost ); 
	cudaMemcpy( ver_max_obj, dev_ver_max_obj, ver_max_obj_size , cudaMemcpyDeviceToHost ); 
	cudaMemcpy( e_obj, dev_e_obj, e_obj_size , cudaMemcpyDeviceToHost ); 


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
>>>>>>> ac3cb0f79f55229281d36df36921a63db1d105b7
}