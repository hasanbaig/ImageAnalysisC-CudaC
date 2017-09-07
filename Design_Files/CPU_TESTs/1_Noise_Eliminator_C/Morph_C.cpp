//C implementation of Morphological Opening Operation
//Author: Hasan Baig
//Email: hasan.baig@hotmail.com 
//Web: http://www.hasanbaig.com

#include "iostream"
#include "conio.h" 
#include "time.h"
#include "../defines.h"
#include "../my_C_functions.h"



int main ()
{
	using std::cout;
	
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
		printf("\n\nTime Before running Noise-Function\n");
		printf (ctime(&st1));

		printf("\n\n");
		int c1 = clock();
		for(m=0;m<Loop_C;m++)
		im_open_C(Noisy_Image, SE, Image_Out);
	
		int c2 = clock();
		printf(" C1 = %d , C2 = %d , Diff = %d \n", c1, c2, c2-c1);

		end1 = time (NULL);
		printf("\nTime after running Noise-Function\n");
		printf (ctime(&end1));
		
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
		system ("PAUSE");
		//_getch();
	
	return 0;
}

