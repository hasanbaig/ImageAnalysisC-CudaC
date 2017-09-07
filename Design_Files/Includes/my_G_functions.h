
//*********************** GPU Kernel for Noise Elimination *************
//Author: Hasan Baig
//Email: hasan.baig@hotmail.com
//Web: http://www.hasanbaig.com

__global__ void im_open_G
(
	int *Image_In, 
	int *Image_Out, 
	int *Image_Erode, 
	int *Temp
)
{

	int bx = blockIdx.x;
	int by = blockIdx.y;

	int tx = threadIdx.x;

	int Row = by*WIDTH;
	int Col = bx*TILE_WIDTH_MORPH;

	// Perform Erosion

		if(Image_In[Row + Col + tx] == 1)
		{
			if(Image_In[Row + Col + (tx-1)] == 1)
			{
				if(Image_In[Row + Col + (tx+1)] == 1)
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
//**********************************************************************




//********** GPU Kernel for Major/Minor Axes Lengths Scanning **********
__global__ void Image_Scan
(
	int *Image, 
	int *A, 
	int *B
)
{
	int bx = blockIdx.x;
	int by = blockIdx.y;

	int tx = threadIdx.x;
	int ty = threadIdx.y;

	int Hor_Row = (by * TILE_WIDTH_SCAN + ty) * WIDTH;
	int Hor_Col = bx * TILE_WIDTH_SCAN;

	//Vertical Scanning
	int Ver_Row = bx * TILE_WIDTH_SCAN + tx;
	int Ver_Col = (by * TILE_WIDTH_SCAN)*WIDTH; 
	
	int k;
		
	int	A_val = 0;
	int B_val = 0;

	for(k=0;k<TILE_WIDTH_SCAN;k++)
		{
			A_val += Image [Hor_Row + Hor_Col + k];
			B_val += Image [Ver_Row + Ver_Col + k*WIDTH];
		
		}
			
		A[by*WIDTH + bx + ty*BLOCKS_X_SCAN]		= A_val;
		B[by*WIDTH + bx*TILE_WIDTH_SCAN + tx]	= B_val;	

		__syncthreads();
			
}
//**********************************************************************




//******** GPU Kernel for Major/Minor Axes Length calculations *********
__global__ void maj_min_scan_G
(
	int *Image, 
	int *hor_scan, 
	int *ver_scan,
	int *hor_max_obj,
	int *ver_max_obj	
)
{
	int bx = blockIdx.x;
	int by = blockIdx.y;

	int tx = threadIdx.x;
	int ty = threadIdx.y;

	//Horizontal Scanning
	int Hor_Row = (by * TILE_WIDTH_SCAN + ty) * WIDTH;
	int Hor_Col = bx * TILE_WIDTH_SCAN;

	//Vertical Scanning
	int Ver_Row = bx * TILE_WIDTH_SCAN + tx;
	int Ver_Col = (by * TILE_WIDTH_SCAN)*WIDTH; 

	int Temp_val_hor = 0;
	int Temp_val_ver = 0;

	int k;

//---------------- Calculating the lengths of each row and column of an object ----------------
	for(k=0;k<TILE_WIDTH_SCAN;k++)
	{
		Temp_val_hor += Image [Hor_Row + Hor_Col + k];
		Temp_val_ver += Image [Ver_Row + Ver_Col + k*WIDTH];
	
	}
	
	hor_scan[by*WIDTH + bx + ty*BLOCKS_X_SCAN]		= Temp_val_hor;
	ver_scan[by*WIDTH + bx*TILE_WIDTH_SCAN + tx]	= Temp_val_ver;
//---------------------------------------------------------------------------------------------

//------------------------ Scanning Major and Minor Axes of an object -------------------------
	for(k=0;k<TILE_WIDTH_SCAN;k++)
	{
		if(hor_scan[by*WIDTH + bx + k*BLOCKS_X_SCAN] > hor_max_obj[by*BLOCKS_Y_SCAN + bx])
			hor_max_obj[by*BLOCKS_Y_SCAN + bx] = hor_scan[by*WIDTH + bx + k*BLOCKS_X_SCAN];

		if(ver_scan[by*WIDTH + bx*TILE_WIDTH_SCAN + k] > ver_max_obj[by*BLOCKS_Y_SCAN + bx])
			ver_max_obj[by*BLOCKS_Y_SCAN + bx] = ver_scan[by*WIDTH + bx*TILE_WIDTH_SCAN + k];

		else
		{
			hor_max_obj[by*BLOCKS_Y_SCAN + bx] = hor_max_obj[by*BLOCKS_Y_SCAN + bx];
			ver_max_obj[by*BLOCKS_Y_SCAN + bx] = ver_max_obj[by*BLOCKS_Y_SCAN + bx];
		}

	}

//---------------------------------------------------------------------------------------------
}
//**********************************************************************




//************* GPU Kernel for Eccentricity Calculations ***************
__global__ void e_calc_G 
(
	int		*hor_max_obj,
	int		*ver_max_obj, 
	float	*e_obj
)
{
	float a,b,c,d;
	
	int tx = threadIdx.x;
	int ty = threadIdx.y;

	int Row = ty*2;
	int Col = tx;

	if(hor_max_obj[Row + Col] == ver_max_obj[Row + Col] )
		e_obj[Row + Col]  = 0.0;
	
	else if (hor_max_obj[Row + Col]  > ver_max_obj[Row + Col] )
		{
			a = hor_max_obj[Row + Col] /2.0;
			b = ver_max_obj[Row + Col] /2.0;
			d = pow(a,2) - pow(b,2);
			c = sqrt(d);
			e_obj[Row + Col]  = c/a;
		}

	else if (hor_max_obj[Row + Col]  < ver_max_obj[Row + Col] )
		{
			b = hor_max_obj[Row + Col] /2.0;
			a = ver_max_obj[Row + Col] /2.0;
			d = pow(a,2) - pow(b,2);
			c = sqrt(d);
			e_obj[Row + Col]  = c/a;
		}
}
//**********************************************************************
=======
//*********************** GPU Kernel for Noise Elimination *************
__global__ void im_open_G
(
	int *Image_In, 
	int *Image_Out, 
	int *Image_Erode, 
	int *Temp
)
{

	int bx = blockIdx.x;
	int by = blockIdx.y;

	int tx = threadIdx.x;

	int Row = by*WIDTH;
	int Col = bx*TILE_WIDTH_MORPH;

	// Perform Erosion

		if(Image_In[Row + Col + tx] == 1)
		{
			if(Image_In[Row + Col + (tx-1)] == 1)
			{
				if(Image_In[Row + Col + (tx+1)] == 1)
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
//**********************************************************************




//********** GPU Kernel for Major/Minor Axes Lengths Scanning **********
__global__ void Image_Scan
(
	int *Image, 
	int *A, 
	int *B
)
{
	int bx = blockIdx.x;
	int by = blockIdx.y;

	int tx = threadIdx.x;
	int ty = threadIdx.y;

	int Hor_Row = (by * TILE_WIDTH_SCAN + ty) * WIDTH;
	int Hor_Col = bx * TILE_WIDTH_SCAN;

	//Vertical Scanning
	int Ver_Row = bx * TILE_WIDTH_SCAN + tx;
	int Ver_Col = (by * TILE_WIDTH_SCAN)*WIDTH; 
	
	int k;
		
	int	A_val = 0;
	int B_val = 0;

	for(k=0;k<TILE_WIDTH_SCAN;k++)
		{
			A_val += Image [Hor_Row + Hor_Col + k];
			B_val += Image [Ver_Row + Ver_Col + k*WIDTH];
		
		}
			
		A[by*WIDTH + bx + ty*BLOCKS_X_SCAN]		= A_val;
		B[by*WIDTH + bx*TILE_WIDTH_SCAN + tx]	= B_val;	

		__syncthreads();
			
}
//**********************************************************************




//******** GPU Kernel for Major/Minor Axes Length calculations *********
__global__ void maj_min_scan_G
(
	int *Image, 
	int *hor_scan, 
	int *ver_scan,
	int *hor_max_obj,
	int *ver_max_obj	
)
{
	int bx = blockIdx.x;
	int by = blockIdx.y;

	int tx = threadIdx.x;
	int ty = threadIdx.y;

	//Horizontal Scanning
	int Hor_Row = (by * TILE_WIDTH_SCAN + ty) * WIDTH;
	int Hor_Col = bx * TILE_WIDTH_SCAN;

	//Vertical Scanning
	int Ver_Row = bx * TILE_WIDTH_SCAN + tx;
	int Ver_Col = (by * TILE_WIDTH_SCAN)*WIDTH; 

	int Temp_val_hor = 0;
	int Temp_val_ver = 0;

	int k;

//---------------- Calculating the lengths of each row and column of an object ----------------
	for(k=0;k<TILE_WIDTH_SCAN;k++)
	{
		Temp_val_hor += Image [Hor_Row + Hor_Col + k];
		Temp_val_ver += Image [Ver_Row + Ver_Col + k*WIDTH];
	
	}
	
	hor_scan[by*WIDTH + bx + ty*BLOCKS_X_SCAN]		= Temp_val_hor;
	ver_scan[by*WIDTH + bx*TILE_WIDTH_SCAN + tx]	= Temp_val_ver;
//---------------------------------------------------------------------------------------------

//------------------------ Scanning Major and Minor Axes of an object -------------------------
	for(k=0;k<TILE_WIDTH_SCAN;k++)
	{
		if(hor_scan[by*WIDTH + bx + k*BLOCKS_X_SCAN] > hor_max_obj[by*BLOCKS_Y_SCAN + bx])
			hor_max_obj[by*BLOCKS_Y_SCAN + bx] = hor_scan[by*WIDTH + bx + k*BLOCKS_X_SCAN];

		if(ver_scan[by*WIDTH + bx*TILE_WIDTH_SCAN + k] > ver_max_obj[by*BLOCKS_Y_SCAN + bx])
			ver_max_obj[by*BLOCKS_Y_SCAN + bx] = ver_scan[by*WIDTH + bx*TILE_WIDTH_SCAN + k];

		else
		{
			hor_max_obj[by*BLOCKS_Y_SCAN + bx] = hor_max_obj[by*BLOCKS_Y_SCAN + bx];
			ver_max_obj[by*BLOCKS_Y_SCAN + bx] = ver_max_obj[by*BLOCKS_Y_SCAN + bx];
		}

	}

//---------------------------------------------------------------------------------------------
}
//**********************************************************************




//************* GPU Kernel for Eccentricity Calculations ***************
__global__ void e_calc_G 
(
	int		*hor_max_obj,
	int		*ver_max_obj, 
	float	*e_obj
)
{
	float a,b,c,d;
	
	int tx = threadIdx.x;
	int ty = threadIdx.y;

	int Row = ty*2;
	int Col = tx;

	if(hor_max_obj[Row + Col] == ver_max_obj[Row + Col] )
		e_obj[Row + Col]  = 0.0;
	
	else if (hor_max_obj[Row + Col]  > ver_max_obj[Row + Col] )
		{
			a = hor_max_obj[Row + Col] /2.0;
			b = ver_max_obj[Row + Col] /2.0;
			d = pow(a,2) - pow(b,2);
			c = sqrt(d);
			e_obj[Row + Col]  = c/a;
		}

	else if (hor_max_obj[Row + Col]  < ver_max_obj[Row + Col] )
		{
			b = hor_max_obj[Row + Col] /2.0;
			a = ver_max_obj[Row + Col] /2.0;
			d = pow(a,2) - pow(b,2);
			c = sqrt(d);
			e_obj[Row + Col]  = c/a;
		}
}
//**********************************************************************
