#include <iostream>
#include <math.h>
#include <stdlib.h>
#include<string.h>
//#include<bits/stdc++.h>
#include<msclr\marshal_cppstd.h>
#include <ctime>// include this header 
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
	int* Red = new int[BM.Height * BM.Width];
	int* Green = new int[BM.Height * BM.Width];
	int* Blue = new int[BM.Height * BM.Width];
	input = new int[BM.Height * BM.Width];
	for (int i = 0; i < BM.Height; i++)
	{
		for (int j = 0; j < BM.Width; j++)
		{
			System::Drawing::Color c = BM.GetPixel(j, i);

			Red[i * BM.Width + j] = c.R;
			Blue[i * BM.Width + j] = c.B;
			Green[i * BM.Width + j] = c.G;

			input[i * BM.Width + j] = ((c.R + c.B + c.G) / 3); //gray scale value equals the average of RGB values

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
			if (image[i * width + j] < 0)
			{
				image[i * width + j] = 0;
			}
			if (image[i * width + j] > 255)
			{
				image[i * width + j] = 255;
			}
			System::Drawing::Color c = System::Drawing::Color::FromArgb(image[i * MyNewImage.Width + j], image[i * MyNewImage.Width + j], image[i * MyNewImage.Width + j]);
			MyNewImage.SetPixel(j, i, c);
		}
	}
	MyNewImage.Save("..//Data//Output//outputRes" + index + ".png");
	cout << "result Image Saved " << index << endl;
}

int calc_l(int n) {
	return int(sqrt(n) / 2);
}


int find_median(int* a, int size) {
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

	//cout << arr[size / 2]<<endl;

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

	// new array size 

	int p_hight = ImageHeight + 2 * levels;
	int p_width = ImageWidth + 2 * levels;
	int p_size = p_hight * p_width;

	int* pad_image = new int[p_size] {0};

	int* filter = new int[filter_size];



	for (int i = levels; i < p_hight - levels; i++)
	{
		for (int j = levels; j < p_width - levels; j++)
		{
			pad_image[p_width * i + j] = imageData[ImageWidth * (i - levels) + (j - levels)];
		}
	}



	for (int i = levels; i < p_hight - levels; i++)
	{

		for (int j = levels; j < p_width - levels; j++)
		{

			int counter = 0;
			for (int h = i - levels; h <= i + levels; h++)
			{
				for (int w = j - levels; w <= j + levels; w++)
				{
					filter[counter] = pad_image[p_width * h + w];
					counter++;
				}
			}
			imageData[ImageWidth * (i - levels) + (j - levels)] = find_median(filter, filter_size);

		}

	}

	createImage(imageData, ImageWidth, ImageHeight, 0);
	stop_s = clock();

	TotalTime += (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000;
	cout << "time: " << TotalTime << endl;
	system("pause");

	free(imageData);
	free(pad_image);
	free(filter);
	return 0;

}





