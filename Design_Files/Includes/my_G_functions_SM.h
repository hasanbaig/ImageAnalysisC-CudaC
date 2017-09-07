// Functions for GPU
//Author: Hasan Baig
//Email: hasan.baig@hotmail.com
//Web: http://www.hasanbaig.com

__global__ void im_open_G_SM
(
	int *Image_In, 
	int *Image_Out, 
	int *Image_Erode, 
	int *Temp	
)
{
	__shared__ int Image_SM [TILE_WIDTH_SCAN][TILE_WIDTH_SCAN];

	int bx = blockIdx.x;
	int by = blockIdx.y;

	int tx = threadIdx.x;
	int ty = threadIdx.y;

	int Row = (by * TILE_WIDTH_SCAN + ty) * WIDTH;
	int Col = bx * TILE_WIDTH_SCAN;

	Image_SM[ty][tx] = Image_In [Row + Col + tx];
	__syncthreads();

	// Perform Erosion

	if(Image_SM[ty][tx] == 1)
	{
		if(Image_SM[ty][(tx - 1)] == 1)
		{
			if(Image_SM[ty][tx + 1] == 1)
				Image_Erode[Row + Col + tx] = 1;
			else
				Image_Erode[Row + Col + tx] = 0;
		}
		else
			Image_Erode[Row + Col + tx] = 0;
	}
	else
		Image_Erode[Row + Col + tx] = 0;

	__syncthreads();

		Temp[Row + Col + tx] = Image_Erode[Row + Col + tx];

	__syncthreads();

	//Perform Dilation

		if(Temp[Row + Col + tx] == 1)
		{
			Image_Out[Row + Col + (tx-1)] = 1;
			Image_Out[Row + Col + tx]	 = 1;
			Image_Out[Row + Col + (tx+1)] = 1; 
		}
		else
		{
			if(Temp[Row + Col + (tx-1)] == 1)
				Image_Out[Row + Col + tx] = 1;
			else if(Temp[Row + Col + (tx+1)] == 1)
				Image_Out[Row + Col + tx] = 1;
			else
				Image_Out[Row + Col + tx] = 0;
		}	


}



//Shared memory usage: input Image >> Ouput Lengths of hor/ver axes
__global__ void maj_min_scan_G_SM_1
(
	int *Image, 
	int *hor_scan, 
	int *ver_scan
)
{

	__shared__ int Image_SM [TILE_WIDTH_SCAN][TILE_WIDTH_SCAN];
	
	int bx = blockIdx.x;
	int by = blockIdx.y;

	int tx = threadIdx.x;
	int ty = threadIdx.y;
	
	//Horizontal Scanning
	int Hor_Row = by * TILE_WIDTH_SCAN + ty;
	int Hor_Col = bx * TILE_WIDTH_SCAN + tx;

	//Vertical Scanning
	int Ver_Row = bx * TILE_WIDTH_SCAN + tx;
	int Ver_Col = (by * TILE_WIDTH_SCAN)*WIDTH; 

	int Temp_val_hor = 0;
	int Temp_val_ver = 0;

	int k;

//---------------- Calculating the lengths of each row and column of an object ----------------
	Image_SM[ty][tx] = Image [Hor_Row * WIDTH + Hor_Col];
	//Temp_val_ver += Image [Ver_Row + Ver_Col + k*WIDTH];
	__syncthreads();
	
	for(k=0;k<TILE_WIDTH_SCAN;k++)
	{
		Temp_val_hor += Image_SM[ty][k];
		Temp_val_ver += Image_SM[k][tx];
	}
	__syncthreads();
	hor_scan[by*WIDTH + bx + ty*BLOCKS_X_SCAN]		= Temp_val_hor;
	ver_scan[by*WIDTH + bx*TILE_WIDTH_SCAN + tx]	= Temp_val_ver;
//---------------------------------------------------------------------------------------------

}
//**********************************************************************



__global__ void maj_min_scan_G_SM_1_SU //SU --> Speed Up
(
	int *Image, 
	int *hor_scan, 
	int *ver_scan
)
{

	__shared__ int Image_SM [TILE_WIDTH_SCAN][TILE_WIDTH_SCAN];
	
	int bx = blockIdx.x;
	int by = blockIdx.y;

	int tx = threadIdx.x;
	int ty = threadIdx.y;
	
	//Horizontal Scanning
	int Hor_Row = by * TILE_WIDTH_SCAN + ty;
	int Hor_Col = bx * TILE_WIDTH_SCAN + tx;

	//Vertical Scanning
	int Ver_Row = bx * TILE_WIDTH_SCAN + tx;
	int Ver_Col = (by * TILE_WIDTH_SCAN)*WIDTH; 

	int Temp_val_hor = 0;
	int Temp_val_ver = 0;

	int k;

//---------------- Calculating the lengths of each row and column of an object ----------------
	Image_SM[ty][tx] = Image [Hor_Row * WIDTH + Hor_Col];
	//Temp_val_ver += Image [Ver_Row + Ver_Col + k*WIDTH];
	__syncthreads();
	
	for(k=0;k<8;k++)
	{
		Temp_val_hor += Image_SM[ty][2*k] + Image_SM[ty][2*k + 1];
		Temp_val_ver += Image_SM[2*k][tx] + Image_SM[2*k + 1][tx];
	}
	__syncthreads();
	hor_scan[by*WIDTH + bx + ty*BLOCKS_X_SCAN]		= Temp_val_hor;
	ver_scan[by*WIDTH + bx*TILE_WIDTH_SCAN + tx]	= Temp_val_ver;
//---------------------------------------------------------------------------------------------

}
//**********************************************************************

__global__ void maj_min_scan_G_SM_2
(
	int *hor_scan, 
	int *ver_scan, 
	int *hor_max_obj, 
	int *ver_max_obj
)
{
	__shared__ int hor_scan_SM[TILE_WIDTH_SCAN][BLOCKS_X_SCAN];
	__shared__ int ver_scan_SM [BLOCKS_Y_SCAN][TILE_WIDTH_SCAN];
	
	int bx = blockIdx.x;
	int by = blockIdx.y;

	int tx = threadIdx.x;
	int ty = threadIdx.y;
	
	//Horizontal Scanning
	int Row = by * TILE_WIDTH_SCAN + ty;
	int Col = bx * TILE_WIDTH_SCAN + tx;

	int k;

	hor_scan_SM[ty][bx]	= hor_scan[by*WIDTH + bx + ty*BLOCKS_X_SCAN];
	ver_scan_SM[by][tx]	= ver_scan[by*WIDTH + bx*TILE_WIDTH_SCAN + tx];
	__syncthreads();

	//------------------------ Scanning Major and Minor Axes of an object -------------------------
	for(k=0;k<TILE_WIDTH_SCAN;k++)
	{
		if(hor_scan_SM[k][bx] > hor_max_obj[by*BLOCKS_Y_SCAN + bx])
			hor_max_obj[by*BLOCKS_Y_SCAN + bx] = hor_scan_SM[k][bx];
	

		if(ver_scan_SM[by][k] > ver_max_obj[by*BLOCKS_Y_SCAN + bx])
			ver_max_obj[by*BLOCKS_Y_SCAN + bx] = ver_scan_SM[by][k];
		
	else
		{
			hor_max_obj[by*BLOCKS_Y_SCAN + bx] = hor_max_obj[by*BLOCKS_Y_SCAN + bx];
			ver_max_obj[by*BLOCKS_Y_SCAN + bx] = ver_max_obj[by*BLOCKS_Y_SCAN + bx];
		}
		
	}	
}
//**********************************************************************



__global__ void maj_min_scan_G_SM
(
	int *Image, 
	int *hor_max_obj,
	int *ver_max_obj
)
{

	__shared__ int Image_SM [TILE_WIDTH_SCAN][TILE_WIDTH_SCAN]; // 17*17*4 = 1156B are utilized
	__shared__ int hor_scan [TILE_WIDTH_SCAN][2];
	__shared__ int ver_scan [2][TILE_WIDTH_SCAN];
	
	int bx = blockIdx.x;
	int by = blockIdx.y;

	int tx = threadIdx.x;
	int ty = threadIdx.y;
	
	//Horizontal Scanning
	int Hor_Row = by * TILE_WIDTH_SCAN + ty;
	int Hor_Col = bx * TILE_WIDTH_SCAN + tx;

	int Temp_val_hor = 0;
	int Temp_val_ver = 0;

	int k;

//---------------- Calculating the lengths of each row and column of an object ----------------
	Image_SM[ty][tx] = Image [Hor_Row*WIDTH + Hor_Col];
	//Temp_val_ver += Image [Ver_Row + Ver_Col + k*WIDTH];
	__syncthreads();
	
	for(k=0;k<TILE_WIDTH_SCAN;k++)
	{
		Temp_val_hor += Image_SM[ty][k];
		Temp_val_ver += Image_SM[k][tx];
	}
	__syncthreads();
	hor_scan[ty][bx]	= Temp_val_hor;
	ver_scan[by][tx]	= Temp_val_ver;
//---------------------------------------------------------------------------------------------

//------------------------ Scanning Major and Minor Axes of an object -------------------------
	for(k=0;k<TILE_WIDTH_SCAN;k++)
	{
		if(hor_scan[k][bx] > hor_max_obj[by*BLOCKS_Y_SCAN + bx])
			hor_max_obj[by*BLOCKS_Y_SCAN + bx] = hor_scan[k][bx];
	

		if(ver_scan[by][k] > ver_max_obj[by*BLOCKS_Y_SCAN + bx])
			ver_max_obj[by*BLOCKS_Y_SCAN + bx] = ver_scan[by][k];
		
	else
		{
			hor_max_obj[by*BLOCKS_Y_SCAN + bx] = hor_max_obj[by*BLOCKS_Y_SCAN + bx];
			ver_max_obj[by*BLOCKS_Y_SCAN + bx] = ver_max_obj[by*BLOCKS_Y_SCAN + bx];
		}
		
	}	
}
//**********************************************************************





__global__ void maj_min_scan_G_SM_SU
(
	int *Image, 
	int *hor_max_obj,
	int *ver_max_obj
)
{

	__shared__ int Image_SM [TILE_WIDTH_SCAN][TILE_WIDTH_SCAN]; // 17*17*4 = 1156B are utilized
	__shared__ int hor_scan [TILE_WIDTH_SCAN][2];
	__shared__ int ver_scan [2][TILE_WIDTH_SCAN];
	
	int bx = blockIdx.x;
	int by = blockIdx.y;

	int tx = threadIdx.x;
	int ty = threadIdx.y;
	
	//Horizontal Scanning
	int Hor_Row = by * TILE_WIDTH_SCAN + ty;
	int Hor_Col = bx * TILE_WIDTH_SCAN + tx;

	int Temp_val_hor = 0;
	int Temp_val_ver = 0;

	int k;

//---------------- Calculating the lengths of each row and column of an object ----------------
	Image_SM[ty][tx] = Image [Hor_Row*WIDTH + Hor_Col];
	//Temp_val_ver += Image [Ver_Row + Ver_Col + k*WIDTH];
	__syncthreads();
	
	for(k=0;k<8;k++)
	{
		Temp_val_hor += Image_SM[ty][2*k] + Image_SM[ty][2*k + 1];
		Temp_val_ver += Image_SM[2*k][tx] + Image_SM[2*k + 1][tx];
	}
	__syncthreads();
	hor_scan[ty][bx]	= Temp_val_hor;
	ver_scan[by][tx]	= Temp_val_ver;
//---------------------------------------------------------------------------------------------

//------------------------ Scanning Major and Minor Axes of an object -------------------------
	for(k=0;k<TILE_WIDTH_SCAN;k++)
	{
		if(hor_scan[k][bx] > hor_max_obj[by*BLOCKS_Y_SCAN + bx])
			hor_max_obj[by*BLOCKS_Y_SCAN + bx] = hor_scan[k][bx];
	

		if(ver_scan[by][k] > ver_max_obj[by*BLOCKS_Y_SCAN + bx])
			ver_max_obj[by*BLOCKS_Y_SCAN + bx] = ver_scan[by][k];
		
	else
		{
			hor_max_obj[by*BLOCKS_Y_SCAN + bx] = hor_max_obj[by*BLOCKS_Y_SCAN + bx];
			ver_max_obj[by*BLOCKS_Y_SCAN + bx] = ver_max_obj[by*BLOCKS_Y_SCAN + bx];
		}
		
	}	
}
//**********************************************************************





//************* GPU Kernel for Eccentricity Calculations ***************
__global__ void e_calc_G_SM 
(
	int		*hor_max_obj,
	int		*ver_max_obj, 
	float	*e_obj
)
{

	__shared__ int hor_max_obj_SM[2][2];
	__shared__ int ver_max_obj_SM[2][2];
	
	float a,b,c,d;
	
	int bx = blockIdx.x;
	int by = blockIdx.y;

	int Row = by*2;
	int Col = bx;

	hor_max_obj_SM [by][bx] = hor_max_obj[Row + Col];
	ver_max_obj_SM [by][bx] = ver_max_obj[Row + Col];

	if(hor_max_obj_SM[by][bx] == ver_max_obj_SM[by][bx])
		e_obj[Row + Col]  = 0.0;
	
	else if (hor_max_obj_SM[by][bx]  > ver_max_obj_SM[by][bx] )
		{
			a = hor_max_obj_SM[by][bx] /2.0;
			b = ver_max_obj_SM[by][bx] /2.0;
			d = pow(a,2) - pow(b,2);
			c = sqrt(d);
			e_obj[Row + Col]  = c/a;
		}

	else if (hor_max_obj_SM[by][bx] < ver_max_obj_SM[by][bx] )
		{
			b = hor_max_obj_SM[by][bx] /2.0;
			a = ver_max_obj_SM[by][bx] /2.0;
			d = pow(a,2) - pow(b,2);
			c = sqrt(d);
			e_obj[Row + Col]  = c/a;
		}
}
//**********************************************************************

/*
__global__ void maj_min_scan_G_SM_B1
(
	int *Image, 
	int *hor_scan, 
	int *ver_scan
)
{

	__shared__ int Image_SM [TILE_WIDTH_SCAN_B][TILE_WIDTH_SCAN_B]; // 17*17*4 = 1156B are utilized
	
	int bx = blockIdx.x;
	int by = blockIdx.y;

	int tx = threadIdx.x;
	int ty = threadIdx.y;
	
	//Horizontal Scanning
	int Hor_Row = by * TILE_WIDTH_SCAN_B + ty;
	int Hor_Col = bx * TILE_WIDTH_SCAN_B + tx;

	int Temp_val_hor = 0;
	int Temp_val_ver = 0;

	int k;

//---------------- Calculating the lengths of each row and column of an object ----------------
	Image_SM[ty][tx] = Image [Hor_Row*(WIDTH + 2) + Hor_Col];
	//Temp_val_ver += Image [Ver_Row + Ver_Col + k*WIDTH];
	__syncthreads();
	
	for(k=0;k<TILE_WIDTH_SCAN_B;k++)
	{
		Temp_val_hor += Image_SM[ty][k];
		Temp_val_ver += Image_SM[k][tx];
	}
	__syncthreads();

	if(bx < 2)
		hor_scan[by*(TILE_WIDTH_SCAN_B*2) + ty*2]	+= Temp_val_hor;
	else
		hor_scan[by*(TILE_WIDTH_SCAN_B*2) + 1 + ty*2]	+= Temp_val_hor;

	if(by < 2)
		ver_scan[bx*(TILE_WIDTH_SCAN_B) + tx] += Temp_val_ver;
	else
		ver_scan[(WIDTH + 2) + bx*(TILE_WIDTH_SCAN_B) + tx] += Temp_val_ver;
//---------------------------------------------------------------------------------------------
}




__global__ void maj_min_scan_G_SM_B
(
	int *Image, 
	int *hor_max_obj,
	int *ver_max_obj
)
{

	__shared__ int Image_SM [TILE_WIDTH_SCAN_B][TILE_WIDTH_SCAN_B]; // 17*17*4 = 1156B are utilized
	__shared__ int hor_scan [36][2];
	__shared__ int ver_scan [2][36];
	
	int bx = blockIdx.x;
	int by = blockIdx.y;

	int tx = threadIdx.x;
	int ty = threadIdx.y;
	
	//Horizontal Scanning
	int Hor_Row = by * TILE_WIDTH_SCAN_B + ty;
	int Hor_Col = bx * TILE_WIDTH_SCAN_B + tx;

	//__constant__ int Temp_val_hor[TILE_WIDTH_SCAN_B][BLOCKS_X_SCAN_B] = {0};
	//__constant__ int Temp_val_ver[BLOCKS_Y_SCAN_B][TILE_WIDTH_SCAN_B] = {0};

	int k;

//---------------- Calculating the lengths of each row and column of an object ----------------
	Image_SM[ty][tx] = Image [Hor_Row*(WIDTH + 2) + Hor_Col];
	//Temp_val_ver += Image [Ver_Row + Ver_Col + k*WIDTH];
	__syncthreads();
	
	for(k=0;k<TILE_WIDTH_SCAN_B;k++)
	{
		Temp_val_hor[ty][bx] += Image_SM[ty][k];
		Temp_val_ver[by][tx] += Image_SM[k][tx];
	}
	__syncthreads();
	if(bx % 2 == 0)
	hor_scan[by*9 + ty][bx]	= Temp_val_hor[ty][bx] + Temp_val_hor[ty][bx+1];

	if(by % 2 == 0)
	ver_scan[by][bx*9 + tx]	= Temp_val_ver[by][bx] + Temp_val_ver[by+1][bx];
//---------------------------------------------------------------------------------------------

//------------------------ Scanning Major and Minor Axes of an object -------------------------
	for(k=0;k<18;k++)
	{
		if((bx < 2) && (by < 2))
		{
			if(hor_scan[by*18 + k][bx] > hor_max_obj[by*BLOCKS_Y_SCAN + bx])
				hor_max_obj[by*BLOCKS_Y_SCAN + bx] = hor_scan[by*18 + k][bx];
	

			if(ver_scan[by][bx*18 + k] > ver_max_obj[by*BLOCKS_Y_SCAN + bx])
				ver_max_obj[by*BLOCKS_Y_SCAN + bx] = ver_scan[by][bx*18 + k];
		}
	else
		{
			hor_max_obj[by*BLOCKS_Y_SCAN + bx] = hor_max_obj[by*BLOCKS_Y_SCAN + bx];
			ver_max_obj[by*BLOCKS_Y_SCAN + bx] = ver_max_obj[by*BLOCKS_Y_SCAN + bx];
		}
		
	}	
}
*/
//**********************************************************************
