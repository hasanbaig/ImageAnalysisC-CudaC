
//******************************* Noise Elimination Function ************************************
//Author: Hasan Baig
//Email: hasan.baig@hotmail.com
//Web: http://www.hasanbaig.com

void im_open_C 
(
	int Image_In[][Cols], 
	int SE[], 
	int Image_Out[][Rows]
)
{
		int temp[Rows][Cols];
		int img_erode[Rows][Cols];

	
		//Filling Noisy image in erroded image array
		for (i=0;i<Rows;i++)
		{
			for (j=0;j<Cols;j++)
			{
				temp[i][j] = Image_In[i][j];	
			}
		}

		//Performing Erosion
		for (i=0;i<Rows;i++)
		{
			for (j=0;j<Cols;j++)
			{
				if(temp[i][j] == SE[1])
				{
						if(temp[i][j-1] == SE[0])
						{
							if(temp[i][j+1] == SE[2])
								img_erode[i][j] = 1;
							else
								img_erode[i][j] = 0;
						}
						else
								img_erode[i][j] = 0;
				}
				else
					img_erode[i][j] = 0;
			}
		}


		//Filling dilate image array with eroded data
		for (i=0;i<Rows;i++)
		{
			for (j=0;j<Cols;j++)
			{
				temp[i][j] = img_erode[i][j];	
				Image_Out[i][j] = img_erode[i][j];
			}
		}

		//Performing Dilation Process
		for (i=0;i<Rows;i++)
		{
			for (j=0;j<Cols;j++)
			{
				if(temp[i][j] == 1)
				{
					Image_Out[i][j+1] = SE[2];
					Image_Out[i][j] = SE[1];
					Image_Out[i][j-1] = SE[0];
				}
				else 
				{
					if(Image_Out[i][j-1]==1)
						Image_Out[i][j] = Image_Out[i][j];
					else
						Image_Out[i][j] = 0;
				}
			}
		}

}
//***********************************************************************************************





// Image Scanning for Major/Minor Axes Detection
void maj_min_scan_C
(
	int Image[][TILE_WIDTH_SCAN*BLOCKS_X_SCAN], 
	int hor_scan[][BLOCKS_X_SCAN], 
	int ver_scan[][WIDTH],
	int hor_max_obj[][BLOCKS_X_SCAN],
	int ver_max_obj[][BLOCKS_X_SCAN]
)
{
	int Temp_val_hor;
	int Temp_val_ver;

	//---------------- Calculating the lengths of each row and column of an object ----------------

	for(i=0;i<BLOCKS_X_SCAN;i++)					//Scanning Horizontal Tiles
	{
		for(j=0;j<BLOCKS_Y_SCAN;j++)				//Scanning Vertical Tiles
		{
			for(k=0;k<TILE_WIDTH_SCAN;k++)			//Scanning Rows of Tile
			{
				Temp_val_hor = 0;
				Temp_val_ver = 0;
				for(l=0;l<TILE_WIDTH_SCAN;l++)		//Scanning Columns of Tile
				{
					Temp_val_hor += Image[i*TILE_WIDTH_SCAN + k][j*TILE_WIDTH_SCAN + l];
					Temp_val_ver += Image[i*TILE_WIDTH_SCAN + l][j*TILE_WIDTH_SCAN + k];
				}
				hor_scan[i*TILE_WIDTH_SCAN + k][j] = Temp_val_hor;
				ver_scan[i][j*TILE_WIDTH_SCAN + k] = Temp_val_ver;
			}
		}
	}
	//---------------------------------------------------------------------------------------------


	//------------------------ Scanning Major and Minor Axes of an object -------------------------

	// Horizontal Axes Lengths
	for(i=0;i<BLOCKS_X_SCAN;i++)
	{
		for(j=0;j<BLOCKS_Y_SCAN;j++)
		{
			for(k=0;k<TILE_WIDTH_SCAN;k++)
			{
				if(hor_scan[i*TILE_WIDTH_SCAN + k][j] > hor_max_obj[i][j])
					hor_max_obj[i][j] = hor_scan[i*TILE_WIDTH_SCAN + k][j];

				else if (hor_scan[i*TILE_WIDTH_SCAN + k][j] < hor_max_obj[i][j])
				{
					if(k >= (TILE_WIDTH_SCAN / 2))
						hor_max_obj[i][j] = hor_max_obj[i][j]; 

					else 
						//printf("\n\nObject [%d][%d] is Defective\n\n",i,j)
						;
				}
				else
					hor_max_obj[i][j] = hor_max_obj[i][j]; 
			}
		}
	}

	// Vertical Axes Lengths
	for(i=0;i<BLOCKS_X_SCAN;i++)
	{
		for(j=0;j<BLOCKS_Y_SCAN;j++)
		{
			for(l=0;l<TILE_WIDTH_SCAN;l++)
			{
				if(ver_scan[i][j*TILE_WIDTH_SCAN + l] > ver_max_obj[i][j])
					ver_max_obj[i][j] = ver_scan[i][j*TILE_WIDTH_SCAN + l];

				else if (ver_scan[i][j*TILE_WIDTH_SCAN + l] < ver_max_obj[i][j])
				{
					if(l >= (TILE_WIDTH_SCAN / 2))
						ver_max_obj[i][j] = ver_max_obj[i][j]; 

					else 
						//printf("\n\nObject [%d][%d] is Defective\n\n",i,j)
						;
				}
				else
					ver_max_obj[i][j] = ver_max_obj[i][j]; 
			}
		}
	}
	//---------------------------------------------------------------------------------------------


}
//****************************************************************************************************




/*
//------------- Calculation of Eccentricity ---------------
float calc_ecc (int major_axis, int minor_axis)
{
	float e,c;
	float d;
	float a = major_axis/2.0;
	float b = minor_axis/2.0;
	
	printf("\n a = %f\n",a);
	printf("\n b = %f\n",b);
	
	if(a == b)
		e = 0;
	else 
	{
		d = a*a - b*b;
		printf("\n d = %f\n",d);
		
		c = sqrt(d);
		printf("\n c = %f\n",c);
		
		e = c/a;
		printf("\n e = %f\n",e);
	
	}

	return (e);
}
//---------------------------------------------------------
*/
//------------ Calculate Dynamic Eccentricity -------------
//by value
// 
//////////////////Calling from Main///////////////////////
// Passing value by value in a loop
//
//		for(i=0;i<Num_of_Objects-1;i++)
//		{
//			for(j=0;j<BLOCKS_X;j++)
//			{
//				e[i][j] = calc_ecc(hor_max[i][j], ver_max[i][j]);
//			}
//		}
//////////////////////////////////////////////////////////


// Function Prototype : float calc_ecc(int , int );
/* Function Definition
float calc_ecc(int hor_max, int ver_max)
{
	float a,b,c,d;
	float e;
			if(hor_max == ver_max)
				e  = 0;

			else if (hor_max > ver_max)
			{
				a = hor_max/2.0;
				b = ver_max/2.0;
				d = pow(a,2) - pow(b,2);
				c = sqrt(d);
				e  = c/a;
			}

			else if (hor_max < ver_max)
			{
				a		= ver_max/2.0;
				b		= hor_max/2.0;
				d		= pow(a,2) - pow(b,2);
				c		= sqrt(d);
				e = c/a;
			}


	
	return (e);
}

*/
//****************************************************************************************************






//***************************** Dynamic Ecccentricity Calculator *******************************
// Passing an entire array in a loop
// Function Prototype: void e_calc_C(int [][BLOCKS_X], int [][BLOCKS_X], float [][BLOCKS_X]);
// Function Definition
void e_calc_C 
(
	int		hor_max_obj [][BLOCKS_X_SCAN], 
	int		ver_max_obj [][BLOCKS_X_SCAN],
	float	e_obj		[][BLOCKS_X_SCAN]
)
{
	float a,b,c,d;
	i = 0;
	j = 0;

	for(i=0;i<2;i++)
	{
		for(j=0;j<2;j++)
		{
			if(hor_max_obj[i][j] == ver_max_obj[i][j])
				e_obj[i][j]  = 0.0;

			else if (hor_max_obj[i][j] > ver_max_obj[i][j])
			{
				a = hor_max_obj[i][j]/2.0;
				b = ver_max_obj[i][j]/2.0;
				d = pow(a,2) - pow(b,2);
				c = sqrt(d);
				e_obj[i][j] = c/a;
			}

			else if (hor_max_obj[i][j] < ver_max_obj[i][j])
			{
				b = hor_max_obj[i][j]/2.0;
				a = ver_max_obj[i][j]/2.0;
				d = pow(a,2) - pow(b,2);
				c = sqrt(d);
				e_obj[i][j] = c/a;
			}
		}
	}
}
//***********************************************************************************************






=======
//******************************* Noise Elimination Function ************************************

void im_open_C 
(
	int Image_In[][Cols], 
	int SE[], 
	int Image_Out[][Rows]
)
{
		int temp[Rows][Cols];
		int img_erode[Rows][Cols];

	
		//Filling Noisy image in erroded image array
		for (i=0;i<Rows;i++)
		{
			for (j=0;j<Cols;j++)
			{
				temp[i][j] = Image_In[i][j];	
			}
		}

		//Performing Erosion
		for (i=0;i<Rows;i++)
		{
			for (j=0;j<Cols;j++)
			{
				if(temp[i][j] == SE[1])
				{
						if(temp[i][j-1] == SE[0])
						{
							if(temp[i][j+1] == SE[2])
								img_erode[i][j] = 1;
							else
								img_erode[i][j] = 0;
						}
						else
								img_erode[i][j] = 0;
				}
				else
					img_erode[i][j] = 0;
			}
		}


		//Filling dilate image array with eroded data
		for (i=0;i<Rows;i++)
		{
			for (j=0;j<Cols;j++)
			{
				temp[i][j] = img_erode[i][j];	
				Image_Out[i][j] = img_erode[i][j];
			}
		}

		//Performing Dilation Process
		for (i=0;i<Rows;i++)
		{
			for (j=0;j<Cols;j++)
			{
				if(temp[i][j] == 1)
				{
					Image_Out[i][j+1] = SE[2];
					Image_Out[i][j] = SE[1];
					Image_Out[i][j-1] = SE[0];
				}
				else 
				{
					if(Image_Out[i][j-1]==1)
						Image_Out[i][j] = Image_Out[i][j];
					else
						Image_Out[i][j] = 0;
				}
			}
		}

}
//***********************************************************************************************





// Image Scanning for Major/Minor Axes Detection
void maj_min_scan_C
(
	int Image[][TILE_WIDTH_SCAN*BLOCKS_X_SCAN], 
	int hor_scan[][BLOCKS_X_SCAN], 
	int ver_scan[][WIDTH],
	int hor_max_obj[][BLOCKS_X_SCAN],
	int ver_max_obj[][BLOCKS_X_SCAN]
)
{
	int Temp_val_hor;
	int Temp_val_ver;

	//---------------- Calculating the lengths of each row and column of an object ----------------

	for(i=0;i<BLOCKS_X_SCAN;i++)					//Scanning Horizontal Tiles
	{
		for(j=0;j<BLOCKS_Y_SCAN;j++)				//Scanning Vertical Tiles
		{
			for(k=0;k<TILE_WIDTH_SCAN;k++)			//Scanning Rows of Tile
			{
				Temp_val_hor = 0;
				Temp_val_ver = 0;
				for(l=0;l<TILE_WIDTH_SCAN;l++)		//Scanning Columns of Tile
				{
					Temp_val_hor += Image[i*TILE_WIDTH_SCAN + k][j*TILE_WIDTH_SCAN + l];
					Temp_val_ver += Image[i*TILE_WIDTH_SCAN + l][j*TILE_WIDTH_SCAN + k];
				}
				hor_scan[i*TILE_WIDTH_SCAN + k][j] = Temp_val_hor;
				ver_scan[i][j*TILE_WIDTH_SCAN + k] = Temp_val_ver;
			}
		}
	}
	//---------------------------------------------------------------------------------------------


	//------------------------ Scanning Major and Minor Axes of an object -------------------------

	// Horizontal Axes Lengths
	for(i=0;i<BLOCKS_X_SCAN;i++)
	{
		for(j=0;j<BLOCKS_Y_SCAN;j++)
		{
			for(k=0;k<TILE_WIDTH_SCAN;k++)
			{
				if(hor_scan[i*TILE_WIDTH_SCAN + k][j] > hor_max_obj[i][j])
					hor_max_obj[i][j] = hor_scan[i*TILE_WIDTH_SCAN + k][j];

				else if (hor_scan[i*TILE_WIDTH_SCAN + k][j] < hor_max_obj[i][j])
				{
					if(k >= (TILE_WIDTH_SCAN / 2))
						hor_max_obj[i][j] = hor_max_obj[i][j]; 

					else 
						//printf("\n\nObject [%d][%d] is Defective\n\n",i,j)
						;
				}
				else
					hor_max_obj[i][j] = hor_max_obj[i][j]; 
			}
		}
	}

	// Vertical Axes Lengths
	for(i=0;i<BLOCKS_X_SCAN;i++)
	{
		for(j=0;j<BLOCKS_Y_SCAN;j++)
		{
			for(l=0;l<TILE_WIDTH_SCAN;l++)
			{
				if(ver_scan[i][j*TILE_WIDTH_SCAN + l] > ver_max_obj[i][j])
					ver_max_obj[i][j] = ver_scan[i][j*TILE_WIDTH_SCAN + l];

				else if (ver_scan[i][j*TILE_WIDTH_SCAN + l] < ver_max_obj[i][j])
				{
					if(l >= (TILE_WIDTH_SCAN / 2))
						ver_max_obj[i][j] = ver_max_obj[i][j]; 

					else 
						//printf("\n\nObject [%d][%d] is Defective\n\n",i,j)
						;
				}
				else
					ver_max_obj[i][j] = ver_max_obj[i][j]; 
			}
		}
	}
	//---------------------------------------------------------------------------------------------


}
//****************************************************************************************************




/*
//------------- Calculation of Eccentricity ---------------
float calc_ecc (int major_axis, int minor_axis)
{
	float e,c;
	float d;
	float a = major_axis/2.0;
	float b = minor_axis/2.0;
	
	printf("\n a = %f\n",a);
	printf("\n b = %f\n",b);
	
	if(a == b)
		e = 0;
	else 
	{
		d = a*a - b*b;
		printf("\n d = %f\n",d);
		
		c = sqrt(d);
		printf("\n c = %f\n",c);
		
		e = c/a;
		printf("\n e = %f\n",e);
	
	}

	return (e);
}
//---------------------------------------------------------
*/
//------------ Calculate Dynamic Eccentricity -------------
//by value
// 
//////////////////Calling from Main///////////////////////
// Passing value by value in a loop
//
//		for(i=0;i<Num_of_Objects-1;i++)
//		{
//			for(j=0;j<BLOCKS_X;j++)
//			{
//				e[i][j] = calc_ecc(hor_max[i][j], ver_max[i][j]);
//			}
//		}
//////////////////////////////////////////////////////////


// Function Prototype : float calc_ecc(int , int );
/* Function Definition
float calc_ecc(int hor_max, int ver_max)
{
	float a,b,c,d;
	float e;
			if(hor_max == ver_max)
				e  = 0;

			else if (hor_max > ver_max)
			{
				a = hor_max/2.0;
				b = ver_max/2.0;
				d = pow(a,2) - pow(b,2);
				c = sqrt(d);
				e  = c/a;
			}

			else if (hor_max < ver_max)
			{
				a		= ver_max/2.0;
				b		= hor_max/2.0;
				d		= pow(a,2) - pow(b,2);
				c		= sqrt(d);
				e = c/a;
			}


	
	return (e);
}

*/
//****************************************************************************************************






//***************************** Dynamic Ecccentricity Calculator *******************************
// Passing an entire array in a loop
// Function Prototype: void e_calc_C(int [][BLOCKS_X], int [][BLOCKS_X], float [][BLOCKS_X]);
// Function Definition
void e_calc_C 
(
	int		hor_max_obj [][BLOCKS_X_SCAN], 
	int		ver_max_obj [][BLOCKS_X_SCAN],
	float	e_obj		[][BLOCKS_X_SCAN]
)
{
	float a,b,c,d;
	i = 0;
	j = 0;

	for(i=0;i<2;i++)
	{
		for(j=0;j<2;j++)
		{
			if(hor_max_obj[i][j] == ver_max_obj[i][j])
				e_obj[i][j]  = 0.0;

			else if (hor_max_obj[i][j] > ver_max_obj[i][j])
			{
				a = hor_max_obj[i][j]/2.0;
				b = ver_max_obj[i][j]/2.0;
				d = pow(a,2) - pow(b,2);
				c = sqrt(d);
				e_obj[i][j] = c/a;
			}

			else if (hor_max_obj[i][j] < ver_max_obj[i][j])
			{
				b = hor_max_obj[i][j]/2.0;
				a = ver_max_obj[i][j]/2.0;
				d = pow(a,2) - pow(b,2);
				c = sqrt(d);
				e_obj[i][j] = c/a;
			}
		}
	}
}
//***********************************************************************************************

