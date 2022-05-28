#include <iostream>
#include <math.h>
#include <stdlib.h>
#include<string.h>
//#include<bits/stdc++.h>
#include<msclr\marshal_cppstd.h>
#include <ctime>// include this header 
#include<mpi.h>
#pragma once

#using <mscorlib.dll>
#using <System.dll>
#using <System.Drawing.dll>
#using <System.Windows.Forms.dll>
using namespace std;
using namespace msclr::interop;

int* inputImage(int* w, int* h, System::String^ imagePath) //put the size of image in w & h
{
	int* input;


	int OriginalImageWidth, OriginalImageHeight;

	//*********************************************************Read Image and save it to local arrayss*************************	
	//Read Image and save it to local arrayss

	System::Drawing::Bitmap BM(imagePath);

	OriginalImageWidth = BM.Width;
	OriginalImageHeight = BM.Height;
	*w = BM.Width;
	*h = BM.Height;
	int *Red = new int[BM.Height * BM.Width];
	int *Green = new int[BM.Height * BM.Width];
	int *Blue = new int[BM.Height * BM.Width];
	input = new int[BM.Height*BM.Width];
	for (int i = 0; i < BM.Height; i++)
	{
		for (int j = 0; j < BM.Width; j++)
		{
			System::Drawing::Color c = BM.GetPixel(j, i);

			Red[i * BM.Width + j] = c.R;
			Blue[i * BM.Width + j] = c.B;
			Green[i * BM.Width + j] = c.G;

			input[i*BM.Width + j] = ((c.R + c.B + c.G) / 3); //gray scale value equals the average of RGB values

		}

	}
	return input;
}


void createImage(int* image, int width, int height, int index)
{
	System::Drawing::Bitmap MyNewImage(width, height);


	for (int i = 0; i < MyNewImage.Height; i++)
	{
		for (int j = 0; j < MyNewImage.Width; j++)
		{
			//i * OriginalImageWidth + j
			if (image[i*width + j] < 0)
			{
				image[i*width + j] = 0;
			}
			if (image[i*width + j] > 255)
			{
				image[i*width + j] = 255;
			}
			System::Drawing::Color c = System::Drawing::Color::FromArgb(image[i*MyNewImage.Width + j], image[i*MyNewImage.Width + j], image[i*MyNewImage.Width + j]);
			MyNewImage.SetPixel(j, i, c);
		}
	}
	MyNewImage.Save("..//Data//Output//outputRes" + index + ".png");
	cout << "result Image Saved " << index << endl;
}

int calc_l(int n) {
	return int(sqrt(n) / 2);
}


int find_median(int* a ,int size) {
	//sort(arr, arr + size);
	int i, j, temp;

	for (int i = 0; i < size; i++) {
		for (int j = i + 1; j < size; j++)
		{
			if (a[j] < a[i]) {
				temp = a[i];
				a[i] = a[j];
				a[j] = temp;
			}
		}
		
	}

	

	return a[size / 2];

}





int main()
{
	int ImageWidth = 4, ImageHeight = 4;

	int start_s, stop_s, TotalTime = 0;

	System::String^ imagePath;
	std::string img;
	img = "..//Data//Input//NoiseImage//N_N_Salt_Pepper.PNG";

	imagePath = marshal_as<System::String^>(img);
	int* imageData = inputImage(&ImageWidth, &ImageHeight, imagePath);


	start_s = clock();


	int filter_size = 9;
	int levels = calc_l(filter_size);

	int p_hight = ImageHeight + 2 * levels;
	int p_width = ImageWidth + 2 * levels;
	int p_size = p_hight * p_width;

	int* pad_image = new int[p_size] {0};

	int* filter = new int[filter_size] ;

	
	// add pading to image
	for  (int i = levels; i < p_hight-levels; i++)
	{
		for (int  j =levels; j < p_width-levels; j++)
		{
        	pad_image[p_width * i + j] = imageData[ImageWidth * (i - levels) + (j - levels)];
        }
	}

	// mpi init
	MPI_Init(NULL, NULL);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	int partial_size = ImageHeight * ImageWidth / world_size;
	cout << "world size : " << world_size << " partial : " << partial_size<<endl;
	int start = partial_size * world_rank;   //foreach process
	int end = start + partial_size - 1;      //foreach process
	int* partial_image = new int[partial_size] ;


	int s_col_index = start % ImageWidth;
	int s_row_index = (start / ImageWidth);

	if (s_row_index == -1) { s_row_index = 0; }

	int e_col_index = end % ImageWidth;
	int e_row_index = (end / ImageWidth);

	// filter 

//	if (s_row_index <levels) { s_row_index = levels; }
	//if (s_col_index <levels) { s_col_index = levels; }

//	if (e_row_index > p_width-levels) { e_row_index = p_width - levels; }
//	if (e_col_index > p_hight-levels) { e_col_index = p_hight - levels; }


	cout << "rank : " << world_rank << " s_row : " << s_row_index << " s_col : " << s_col_index << " end : " << end<<" start : " << start<< endl;
	cout << "rank : " << world_rank << " e_row : " << e_row_index << " e_col : " << e_col_index << endl;


	if (world_rank == 0) {
		for (int i = 0; i < ImageHeight*ImageWidth; i++)
		{
			imageData[i] = 255;
		}
	}
	MPI_Bcast(imageData, ImageHeight * ImageWidth, MPI_INT, 0, MPI_COMM_WORLD);
	int j = s_col_index;
	int counn = 0;
	for (int i = s_row_index; i < ImageHeight; i++)
	{
		for (j = s_col_index; j < ImageWidth; j++)
		{
			imageData[ImageWidth * i + j] = 0;
			counn++;
			if (i == e_row_index && j == e_col_index) {
				break;
			}
		}
		if (i == e_row_index && j == e_col_index) {
			break;
		}
	}
	createImage(imageData, ImageWidth, ImageHeight, world_rank);






	for (int i = s_row_index+ levels; i < e_row_index; i++)
	{

		for (int j = s_col_index + levels; j < e_col_index; j++)
		{
			
			int counter = 0;

			// loop for filter 
			for (int h = i-levels; h <= i+levels ; h++)
			{
				for (int w =j-levels; w <= j+levels ; w++)
				{
					filter[counter] = pad_image[p_width * h + w];
					counter++;
				}
			}
			partial_image[ImageWidth * i-s_row_index + j- s_col_index]=find_median(filter,filter_size);

		}
	
	}


	MPI_Gather(partial_image, partial_size, MPI_INT, imageData, partial_size, MPI_INT, 0, MPI_COMM_WORLD);



	MPI_Finalize();
	
	stop_s = clock();

	//createImage(imageData, ImageWidth, ImageHeight, 0);

	

	TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
	cout << "time: " << TotalTime << endl;
	system("pause");

	free(imageData);
	delete[] pad_image;
	delete[] filter;
	return 0;

}





