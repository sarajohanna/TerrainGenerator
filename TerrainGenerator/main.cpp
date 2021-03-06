#pragma once

#include "pch.h"
//#include <iostream>

#include <cstdint>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iostream>

//1) generera ett landskap(med diamond square)
//2) färglägg landskapet
//3) skriv ut det som en bild

//world_size = 512; // must be power of two
//total_number_of_heights = (world_size + 1) * (world_size + 1);
//image_width = world_size + 1;
//image_height = world_size + 1;
//
//min_square_size = 1;
//max_square_size = world_size;

	//for (int square_size = ...)
	//{
	//	for (int ypos = ...)
	//	{
	//		for (int xpos = ...)
	//		{
	//			...

//int index_for( int x, int y )
//{
//
//}


//du behöver en funktion som kan ta 
//step_size och en mittpunkt och sen 
//läsa höjderna på dom 4 närliggande punkterna, 
//ta ett medelvärde, 
//slumpa ett tal mellan 0 och 1, 
//skala med step_size och 
//addera ihop allting

namespace
{
	struct BMP_magicnumber
	{
		uint8_t magicnumber[2];
	};
	struct BMP_fileheader
	{
		uint32_t filesize;    //
		uint16_t reserved[2]; // ???
		uint32_t offset;      // starting address of bitmap data
	};
	struct BMP_information
	{
		uint32_t informationsize; //
		int32_t width;           //
		int32_t height;          //
		uint16_t nplanes;         //
		uint16_t bitspp;          //
		uint32_t compression;     //
		uint32_t imagesize;       //
		int32_t hres;            //
		int32_t vres;            //
		uint32_t ncolors;         //
		uint32_t nimpcolors;      //
	};

	struct Pixel
	{
		uint8_t red;
		uint8_t green;
		uint8_t blue;
	};
}

class Grid
{
public:
	Grid( int squares_in_width )
	{
		m_squares_in_width = squares_in_width;
	}

	int CoordinatesToIndex(int x, int y )
	{
		return y * (m_squares_in_width + 1) + x;
	}

	int m_squares_in_width = 0;
};


double SquareStepHeightAtIndex(int x, int y, int step_size, std::vector<double>& heights, Grid grid)
{
	double height = 0;
	int divider = 0;

	if (y + step_size / 2 <= grid.m_squares_in_width) // point above
	{
		height += heights[grid.CoordinatesToIndex(x, y + step_size / 2)];
		++divider;
	}

	if (y - step_size / 2 > 0) //point below
	{
		height += heights[grid.CoordinatesToIndex(x, y - step_size / 2)];
		++divider;
	}

	if (x + step_size <= grid.m_squares_in_width) //point to the right
	{
		height += heights[grid.CoordinatesToIndex(x + step_size / 2, y)];
		++divider;
	}

	if (x - step_size > 0) //point to the left
	{
		height += heights[grid.CoordinatesToIndex(x - step_size / 2, y)];
		++divider;
	}

	return (height / divider) + (rand() / (float)RAND_MAX) *grid.m_squares_in_width; //rand() % 10; // +;//rand() % 10;
}

void GenerateLandscape(std::vector<double>& heights, const int image_width)
{
	Grid grid(image_width - 1);

	// Initialize corner values
	heights[grid.CoordinatesToIndex(0, 0)] = 22;
	heights[grid.CoordinatesToIndex(image_width - 1, 0)] = 12;
	heights[grid.CoordinatesToIndex(0, image_width - 1)] = 31;
	heights[grid.CoordinatesToIndex(image_width - 1, image_width - 1)] = 4;

	for ( int step_size = grid.m_squares_in_width; step_size > 1; step_size = step_size/2 )
	{
		for (int x = step_size / 2; x < grid.m_squares_in_width; x += step_size ) // compute center, diamond step
		{
			for (int y = step_size / 2; y < grid.m_squares_in_width; y += step_size)
			{
				double height =
					((heights[grid.CoordinatesToIndex(x - step_size / 2, y + step_size / 2)] +
						heights[grid.CoordinatesToIndex(x - step_size / 2, y - step_size / 2)] +
						heights[grid.CoordinatesToIndex(x + step_size / 2, y + step_size / 2)] +
						heights[grid.CoordinatesToIndex(x + step_size / 2, y - step_size / 2)]) / 4) + (rand() / (float)RAND_MAX) *grid.m_squares_in_width;//rand() % 10; // +step_size;//

				float hej = rand() / (float)RAND_MAX;

				heights[grid.CoordinatesToIndex(x, y)] = height; //number;//(heights.size() - 1) % image_width;
			}
		}

		for (int x = step_size / 2; x < grid.m_squares_in_width; x += step_size) // compute heights to the left and right, above and below center, square step
		{
			for (int y = step_size / 2; y < grid.m_squares_in_width; y += step_size)
			{
				int y_up = y + step_size / 2;
				int y_down = y - step_size / 2;
				int x_right = x + step_size / 2;
				int x_left = x - step_size / 2;

				heights[grid.CoordinatesToIndex(x, y_up)] = SquareStepHeightAtIndex(x, y_up, step_size, heights, grid);
				heights[grid.CoordinatesToIndex(x, y_down)] = SquareStepHeightAtIndex(x, y_down, step_size, heights, grid);
				heights[grid.CoordinatesToIndex(x_right, y)] = SquareStepHeightAtIndex(x_right, y, step_size, heights, grid);
				heights[grid.CoordinatesToIndex(x_left, y)] = SquareStepHeightAtIndex(x_left, y, step_size, heights, grid);
			}
		}
	}
}


void ColorLandscape(const int image_width, const int image_height, std::vector<Pixel>& pixels, std::vector<double>& heights)
{
	pixels.resize(image_width * image_height);

	const int radius = image_width / 2;
	int k = image_width / 2;
	int h = image_height / 2;

	int x = 0;
	int y = 0;

	int a = 0;
	int b = 0;

	int c2 = 0;

	double min_height = heights[0], max_height = heights[0];

	for (int i = 0; i < pixels.size(); ++i)
	{
		if (heights[i] > max_height)
			max_height = heights[i];

		if (heights[i] < min_height)
			min_height = heights[i];
	}
	

	uint8_t low_red = 0, low_green = 255, low_blue = 0;
	uint8_t high_red = 128, high_green = 128, high_blue = 128;

	Pixel low = {low_red, low_green, low_blue };
	Pixel high = { high_red, high_green, high_blue };

	for (int i = 0; i < pixels.size(); ++i)
	{
		x = i % image_width;
		y = i / image_height;

		//a = x - k;
		//b = y - h;

		//c2 = (a * a) + (b * b);
		//if (c2 <= (radius*radius))
		//{
			double t = (heights[i] - min_height) / (max_height - min_height);
			uint8_t red = (int8_t)(low_red * (1.0 - t) + high_red * t);
			uint8_t green = (int8_t)(low_green * (1.0 - t) + high_green * t);
			uint8_t blue = (int8_t)(low_blue * (1.0 - t) + high_blue * t);

			pixels[i] = Pixel{ red, green, blue };
		//}
	}
}


void CreateBMPFile(const int image_width, const int image_height, std::vector<Pixel>& pixels)
{
	const int row_size_without_padding = image_width * sizeof(Pixel);
	int padding = 0;
	int modulus = row_size_without_padding % 4;

	switch (modulus)
	{
	case(1):
		padding = 3;
		break;
	case(2):
		padding = 2;
		break;
	case(3):
		padding = 1;
	}

	const int image_size_with_padding = (row_size_without_padding + padding) * image_height;

	std::ofstream f("images/image.bmp", std::ofstream::binary);

	BMP_magicnumber mn = { {0x42, 0x4D} };
	f.write((char*)&mn, sizeof mn);

	BMP_fileheader fh = {};
	fh.filesize = sizeof(BMP_magicnumber) + sizeof(BMP_fileheader) + sizeof(BMP_information) + image_size_with_padding;
	fh.offset = sizeof(BMP_magicnumber) + sizeof(BMP_fileheader) + sizeof(BMP_information);
	f.write((char*)&fh, sizeof fh);

	BMP_information i = {
	  sizeof i,
	  image_width,
	  image_height,
	  1, // must be one
	  24, // bits per pixel, 24 => 3 * 8
	  0, // no compression
	  image_size_with_padding,
	  2835, // pixels/metre horizontal
	  2835, // pixels/metre vertical
	  0, // no color palette?
	  0 // ???
	};
	f.write((char*)&i, sizeof i);

	char padding_values[4] = {};
	for (int h = 0; h < image_height; h++)
	{
		f.write((char*)(pixels.data() + h * image_width), row_size_without_padding);
		f.write(padding_values, padding);
	}
}


int main()
{
	const int image_width = 257;
	const int image_height = 257;

	std::vector<Pixel> pixels;
	std::vector<double> heights;

	heights.resize(image_width * image_height);

	GenerateLandscape(heights, image_width);
	ColorLandscape(image_width, image_height, pixels, heights);
	CreateBMPFile(image_width, image_height, pixels);

	return 0;
}
