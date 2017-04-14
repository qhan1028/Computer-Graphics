#include "Application.h"
#include "qt_opengl_framework.h"
#include <vector>
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Application::Application()
{

}
Application::~Application()
{

}
//****************************************************************************
//
// * 初始畫面，並顯示Ntust.png圖檔
// 
//============================================================================
void Application::createScene( void )
{
	
	ui_instance = Qt_Opengl_Framework::getInstance();
	
}

//****************************************************************************
//
// * 打開指定圖檔
// 
//============================================================================
void Application::openImage( QString filePath )
{
	mImageSrc.load(filePath);
	mImageDst.load(filePath);

	renew();

	img_data = mImageSrc.bits();
	img_width = mImageSrc.width();
	img_height = mImageSrc.height();

	ui_instance->ui.label->setFixedHeight(img_height);
	ui_instance->ui.label->setFixedWidth(img_width);
}
//****************************************************************************
//
// * 刷新畫面
// 
//============================================================================
void Application::renew()
{
	ui_instance = Qt_Opengl_Framework::getInstance();

	ui_instance->ui.label->clear();
	ui_instance->ui.label->setPixmap(QPixmap::fromImage(mImageDst));

	std::cout << "Renew" << std::endl;
}

//****************************************************************************
//
// * 畫面初始化
// 
//============================================================================
void Application::reload()
{
	ui_instance = Qt_Opengl_Framework::getInstance();

	ui_instance->ui.label->clear();
	ui_instance->ui.label->setPixmap(QPixmap::fromImage(mImageSrc));
}

//****************************************************************************
//
// * 儲存圖檔
// 
//============================================================================
void Application::saveImage(QString filePath )
{
	
	mImageDst.save(filePath);
}

//****************************************************************************
//
// * 將圖檔資料轉換為RGB色彩資料
// 
//============================================================================
unsigned char* Application::To_RGB( void )
{
	unsigned char *rgb = new unsigned char[img_width * img_height * 3];
	int i, j;

	if (! img_data )
		return NULL;

	// Divide out the alpha
	for (i = 0; i < img_height; i++)
	{
		int in_offset = i * img_width * 4;
		int out_offset = i * img_width * 3;

		for (j = 0 ; j < img_width ; j++)
		{
			RGBA_To_RGB(img_data + (in_offset + j*4), rgb + (out_offset + j*3));
		}
	}

	return rgb;
}

void Application::RGBA_To_RGB( unsigned char *rgba, unsigned char *rgb )
{
	const unsigned char	BACKGROUND[3] = { 0, 0, 0 };

	unsigned char  alpha = rgba[3];

	if (alpha == 0)
	{
		rgb[0] = BACKGROUND[0];
		rgb[1] = BACKGROUND[1];
		rgb[2] = BACKGROUND[2];
	}
	else
	{
		float	alpha_scale = (float)255 / (float)alpha;
		int	val;
		int	i;

		for (i = 0 ; i < 3 ; i++)
		{
			val = (int)floor(rgba[i] * alpha_scale);
			if (val < 0)
				rgb[i] = 0;
			else if (val > 255)
				rgb[i] = 255;
			else
				rgb[i] = val;
		}
	}
}
//------------------------Color------------------------

///////////////////////////////////////////////////////////////////////////////
//
//  Convert image to grayscale.  Red, green, and blue channels should all 
//  contain grayscale value.  Alpha channel shoould be left unchanged.  Return
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Gray() //ok
{
	unsigned char *rgb = To_RGB();

	for (int i=0; i<img_height; i++)
	{
		for (int j=0; j<img_width; j++)
		{
			int offset_rgb = i*img_width*3+j*3;
			int offset_rgba = i*img_width*4+j*4;
			unsigned char gray = 0.299 * rgb[offset_rgb + rr] + 0.587 * rgb[offset_rgb + gg] + 0.114 * rgb[offset_rgb + bb];

			for (int k=0; k<3; k++)
				img_data[offset_rgba+k] = gray;
			img_data[offset_rgba + aa] = WHITE;
		}
	}
	
	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using uniform quantization.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Quant_Uniform() // ok
{
	unsigned char *rgb = this->To_RGB();

	for (int i=0; i<img_height; i++)
	{
		for (int j=0; j<img_width; j++)
		{
			int offset_rgb = i*img_width*3+j*3;
			int offset_rgba = i*img_width*4+j*4;

			img_data[offset_rgba + rr] = (rgb[offset_rgb + rr] / 32) * 32;
			img_data[offset_rgba + gg] = (rgb[offset_rgb + gg] / 32) * 32;
			img_data[offset_rgba + bb] = (rgb[offset_rgb + bb] / 64) * 64;

			img_data[offset_rgba + aa] = WHITE;
		}
	}

	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using populosity quantization.  
//  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
typedef struct {
	int r;
	int g;
	int b;
	int count;
} RGB_count;

bool RGB_Count_Compare(const RGB_count c1, const RGB_count c2) {return c1.count > c2.count;} 

void Application::Quant_Populosity() // ok
{
	unsigned char *rgb = this->To_RGB();
	
	RGB_count *rgb_count = new RGB_count[32768];
	memset(rgb_count, 0, sizeof(RGB_count) * 32768);

	for (int i=0; i<img_height; i++)
	{
		for (int j=0; j<img_width; j++)
		{
			int offset_rgb = i*img_width*3+j*3;
			
			int r = rgb[offset_rgb + rr] / 8;	// offset of interval
			rgb[offset_rgb + rr] = r * 8;		// value
			int g = rgb[offset_rgb + gg] / 8;
			rgb[offset_rgb + gg] = g * 8;
			int b = rgb[offset_rgb + bb] / 8;
			rgb[offset_rgb + bb] = b * 8;	

			int offset = r * 1024 + g * 32 + b;
			rgb_count[offset].count++;
			if (rgb_count[offset].count > 0)
			{
				rgb_count[offset].r = r;
				rgb_count[offset].g = g;
				rgb_count[offset].b = b;
			}
		}
	}

	RGB_count *rgb_rank = new RGB_count[32768];
	memcpy(rgb_rank, rgb_count, sizeof(RGB_count) * 32768);
	std::sort(rgb_rank, rgb_rank + 32768, RGB_Count_Compare);
	int last_color_count = rgb_rank[255].count;

	for (int i=0; i<img_height; i++)
	{
		for (int j=0; j<img_width; j++)
		{
			int offset_rgb = i*img_width*3+j*3;
			int offset_rgba = i*img_width*4+j*4;

			int r = rgb[offset_rgb + rr] / 8;
			int g = rgb[offset_rgb + gg] / 8;
			int b = rgb[offset_rgb + bb] / 8;
			int offset_cnt = r * 1024 + g * 32 + b;
			if (rgb_count[offset_cnt].count >= last_color_count)
			{
				img_data[offset_rgba + rr] = r * 8;
				img_data[offset_rgba + gg] = g * 8;
				img_data[offset_rgba + bb] = b * 8;
				img_data[offset_rgba + aa] = WHITE;
			}
			else
			{
				int distance = 1024 * 3;
				int choose = 0;
				for (int k = 0; k < 256; k++)
				{
					int tmp_dis = pow(r - rgb_rank[k].r, 2);
					tmp_dis += pow(g - rgb_rank[k].g, 2);
					tmp_dis += pow(b - rgb_rank[k].b, 2);
					if (tmp_dis < distance)
					{
						distance = tmp_dis;
						choose = k;
					}
				}
				img_data[offset_rgba + rr] = rgb_rank[choose].r * 8;
				img_data[offset_rgba + gg] = rgb_rank[choose].g * 8;
				img_data[offset_rgba + bb] = rgb_rank[choose].b * 8;
				img_data[offset_rgba + aa] = WHITE;
			}
		}
	}

	delete[] rgb_count;
	delete[] rgb_rank;
	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}

//------------------------Dithering------------------------

///////////////////////////////////////////////////////////////////////////////
//
//  Dither the image using a threshold of 1/2.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Dither_Threshold() // ok
{
	Gray();
	unsigned char *rgb = To_RGB();

	for (int i=0; i<img_height; i++)
	{
		for (int j=0; j<img_width; j++)
		{
			int offset_rgb = i*img_width*3+j*3;
			int offset_rgba = i*img_width*4+j*4;
			unsigned char result = (rgb[offset_rgb] >= 128) ? 255 : 0;
			
			for (int k=0; k<3; k++)
			{
				img_data[offset_rgba + k] = result;
			}
			img_data[offset_rgba + aa] = WHITE;
		}
	}	

	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Dither image using random dithering.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Dither_Random() // ok
{
	Gray();
	unsigned char *rgb = To_RGB();
	srand(time(NULL));

	for (int i=0; i<img_height; i++)
	{
		for (int j=0; j<img_width; j++)
		{
			int offset_rgb = i*img_width*3+j*3;
			int offset_rgba = i*img_width*4+j*4;

			int tmp = rgb[offset_rgb] + (rand() % 103) - 51;
			unsigned char result = (tmp >= 128) ? 255 : 0;
			
			for (int k=0; k<3; k++)
			{
				img_data[offset_rgba + k] = result;
			}
		}
	}	

	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Perform Floyd-Steinberg dithering on the image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Dither_FS() // ok
{
	Gray();
	unsigned char *rgb = this->To_RGB();

	float **float_rgb = new float *[img_height];
	for (int i = 0; i < img_height; i++)
	{
		float_rgb[i] = new float[img_width];
		for (int j = 0; j < img_width; j++)
		{
			float_rgb[i][j] = float(rgb[(i * img_width + j) * 3]);
		}
	}

	for (int i = 0; i < img_height; i++)
	{
		int offset_i = i * img_width;
		if (i % 2 == 0) // from left to right
		{
			for (int j = 0; j < img_width; j++)
			{
				float result = (float_rgb[i][j] >= 128)? 255 : 0;
				float error = float_rgb[i][j] - result;

				int offset_rgba = (offset_i + j) * 4;
				for (int k = 0; k < 3; k++) img_data[offset_rgba + k] = result;

				if (i == img_height - 1) // bottom edge
				{
					if (j == img_width - 1) continue; // bottom right edge
					else float_rgb[i  ][j+1] += error * ((float)7/16);
				}
				else if (j == 0) // left edge
				{
					float_rgb[i+1][j  ] += error * ((float)5/16);
					float_rgb[i+1][j+1] += error * ((float)1/16);
					float_rgb[i  ][j+1] += error * ((float)7/16);
				}
				else if (j == img_width - 1) // right edge
				{
					float_rgb[i+1][j  ] += error * ((float)5/16);
					float_rgb[i+1][j-1] += error * ((float)3/16);
				}
				else // not at edge
				{
					float_rgb[i+1][j-1] += error * ((float)3/16);
					float_rgb[i+1][j  ] += error * ((float)5/16);
					float_rgb[i+1][j+1] += error * ((float)1/16);
					float_rgb[i  ][j+1] += error * ((float)7/16);
				}
			}
		}
		else if (i % 2 == 1) // from right to left
		{
			for (int j = img_width - 1; j >= 0; j--)
			{
				float result = (float_rgb[i][j] >= 128)? 255 : 0;
				float error = float_rgb[i][j] - result;

				int offset_rgba = (offset_i + j) * 4;
				for (int k = 0; k < 3; k++) img_data[offset_rgba + k] = result;

				if (i == img_height - 1) // bottom edge
				{
					if (j == 0) continue; // bottom left edge
					else float_rgb[i  ][j-1] += error * ((float)7/16);
				}
				else if (j == img_width - 1) // right edge
				{
					float_rgb[i+1][j  ] += error * ((float)5/16);
					float_rgb[i+1][j-1] += error * ((float)1/16);
					float_rgb[i  ][j-1] += error * ((float)7/16);
				}
				else if (j == 0) // left edge
				{
					float_rgb[i+1][j  ] += error * ((float)5/16);
					float_rgb[i+1][j+1] += error * ((float)3/16);
				}
				else // not at edge
				{
					float_rgb[i+1][j+1] += error * ((float)3/16);
					float_rgb[i+1][j  ] += error * ((float)5/16);
					float_rgb[i+1][j-1] += error * ((float)1/16);
					float_rgb[i  ][j-1] += error * ((float)7/16);
				}
			}
		}
	}

	for (int i = 0; i < img_height; i++) delete[] float_rgb[i];
	delete[] float_rgb;
	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Dither the image while conserving the average brightness.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool Bright_Compare(const int a, const int b)
{
	return a < b;
}

void Application::Dither_Bright() // ok
{
	Gray();
	unsigned char *rgb = To_RGB();
	unsigned char *brightness = new unsigned char[img_width * img_width];
	float sum_bright = 0;
	
	for (int i=0; i<img_height; i++)
	{
		int i_offset = i * img_width;
		for (int j=0; j<img_width; j++)
		{
			int offset_rgb = i*img_width*3+j*3;

			brightness[i_offset + j] = rgb[offset_rgb];
			sum_bright += (float)rgb[offset_rgb];
		}
	}

	std::sort(brightness, brightness + img_width * img_height, Bright_Compare);
	float average_bright = (float)sum_bright / (float)(img_width * img_height);
	int threshold = brightness[(int)((1-(average_bright / 255)) * (img_width * img_height - 1))];


	for (int i=0; i<img_height; i++)
	{
		for (int j=0; j<img_width; j++)
		{
			int offset_rgb = i*img_width*3+j*3;
			int offset_rgba = i*img_width*4+j*4;
			unsigned char result = (rgb[offset_rgb] >= threshold) ? 255 : 0;
			
			for (int k=0; k<3; k++)
			{
				img_data[offset_rgba + k] = result;
			}
			img_data[offset_rgba + aa] = WHITE;
		}
	}

	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Perform clustered differing of the image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Dither_Cluster() // ok
{
	Gray();
	unsigned char *rgb = this->To_RGB();

	float mask[4][4] = {{0.7059, 0.0588, 0.4706, 0.1765},
						{0.3529, 0.9412, 0.7647, 0.5294},
					  	{0.5882, 0.8235, 0.8824, 0.2941},
					  	{0.2353, 0.4118, 0.1176, 0.6471}};
	for (int i = 0; i < img_height; i++)
	{
		int offset_i = i * img_width;
		for (int j = 0; j < img_width; j++)
		{
			int offset_rgb = (offset_i + j) * 3;
			int offset_rgba = (offset_i + j) * 4;

			float tmp = (float)rgb[offset_rgb] / 255;
			if (tmp >= mask[j%4][i%4])
			{
				for (int k = 0; k < 3; k++)
				{
					img_data[offset_rgba + k] = 255;
				}
			}
		}
	}

	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using Floyd-Steinberg dithering over
//  a uniform quantization - the same quantization as in Quant_Uniform.
//  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
struct Float_RGB{
	float r;
	float g;
	float b;

	Float_RGB() : r(0), g(0), b(0) {}
	Float_RGB(float r, float g, float b) : r(r), g(g), b(b) {}
	Float_RGB& operator=(const Float_RGB& a)
	{
		r = a.r;
		g = a.g;
		b = a.b;
		return *this;
	}
	Float_RGB operator+(const Float_RGB& a) const
	{
		return Float_RGB(r+a.r, g+a.g, b+a.b);
	}
	Float_RGB operator-(const Float_RGB& a) const
	{
		return Float_RGB(r-a.r, g-a.g, b-a.b);
	}
	Float_RGB operator*(const float a) const
	{
		return Float_RGB(r*a, g*a, b*a);
	}
	Float_RGB& operator+=(const Float_RGB& a)
	{
		r = r + a.r;
		g = g + a.g;
		b = b + a.b;
		return *this;
	}
};
Float_RGB Find_Result(Float_RGB float_rgb)
{
	unsigned char rg_table[8] = {0, 36, 73, 109, 146, 182, 219, 255};
	unsigned char b_table[4] = {0, 85, 170, 255};
	Float_RGB result = Float_RGB(255, 255, 255);
	// R
	for (int k = 0; k < 8; k++) {
		if (float_rgb.r <= rg_table[k]) {
			result.r = rg_table[k];
			break;
		}
	}
	// G
	for (int k = 0; k < 8; k++) {
		if (float_rgb.g <= rg_table[k]) {
			result.g = rg_table[k];
			break;
		}
	}
	// B
	for (int k = 0; k < 4; k++) {
		if (float_rgb.b <= b_table[k]) {
			result.b = b_table[k];
			break;
		}
	}
	return result;
}
void Application::Dither_Color() // ok
{
	unsigned char *rgb = this->To_RGB();

	Float_RGB **float_rgb = new Float_RGB *[img_height];
	for (int i = 0; i < img_height; i++)
	{
		int offset_i = i * img_width;
		float_rgb[i] = new Float_RGB[img_width];
		for (int j = 0; j < img_width; j++)
		{
			int offset = (offset_i + j) * 3;
			float_rgb[i][j] = Float_RGB((float)rgb[offset + rr], (float)rgb[offset + gg], (float)rgb[offset + bb]);
		}
	}

	for (int i = 0; i < img_height; i++)
	{
		int offset_i = i * img_width;
		if (i % 2 == 0) // from left to right
		{
			for (int j = 0; j < img_width; j++)
			{	
				Float_RGB result = Find_Result(float_rgb[i][j]);

				int offset_rgba = (offset_i + j) * 4;
				img_data[offset_rgba + rr] = (unsigned char)result.r;
				img_data[offset_rgba + gg] = (unsigned char)result.g;
				img_data[offset_rgba + bb] = (unsigned char)result.b;
				img_data[offset_rgba + aa] = WHITE;

				Float_RGB error = (float_rgb[i][j] - result);
				if (i == img_height - 1) // bottom edge
				{
					if (j == img_width - 1) continue; // bottom right edge
					else float_rgb[i  ][j+1] += error * ((float)7/16);
				}
				else if (j == 0) // left edge
				{
					float_rgb[i+1][j  ] += error * ((float)5/16);
					float_rgb[i+1][j+1] += error * ((float)1/16);
					float_rgb[i  ][j+1] += error * ((float)7/16);
				}
				else if (j == img_width - 1) // right edge
				{
					float_rgb[i+1][j  ] += error * ((float)5/16);
					float_rgb[i+1][j-1] += error * ((float)3/16);
				}
				else // not at edge
				{
					float_rgb[i+1][j-1] += error * ((float)3/16);
					float_rgb[i+1][j  ] += error * ((float)5/16);
					float_rgb[i+1][j+1] += error * ((float)1/16);
					float_rgb[i  ][j+1] += error * ((float)7/16);
				}
			}
		}
		else if (i % 2 == 1) // from right to left
		{
			for (int j = img_width - 1; j >= 0; j--)
			{
				Float_RGB result = Find_Result(float_rgb[i][j]);

				int offset_rgba = (offset_i + j) * 4;
				img_data[offset_rgba + rr] = result.r;
				img_data[offset_rgba + gg] = result.g;
				img_data[offset_rgba + bb] = result.b;
				img_data[offset_rgba + aa] = WHITE;

				Float_RGB error = float_rgb[i][j] - result;
				if (i == img_height - 1) // bottom edge
				{
					if (j == 0) continue; // bottom left edge
					else float_rgb[i  ][j-1] += error * ((float)7/16);
				}
				else if (j == img_width - 1) // right edge
				{
					float_rgb[i+1][j  ] += error * ((float)5/16);
					float_rgb[i+1][j-1] += error * ((float)1/16);
					float_rgb[i  ][j-1] += error * ((float)7/16);
				}
				else if (j == 0) // left edge
				{
					float_rgb[i+1][j  ] += error * ((float)5/16);
					float_rgb[i+1][j+1] += error * ((float)3/16);
				}
				else // not at edge
				{
					float_rgb[i+1][j+1] += error * ((float)3/16);
					float_rgb[i+1][j  ] += error * ((float)5/16);
					float_rgb[i+1][j-1] += error * ((float)1/16);
					float_rgb[i  ][j-1] += error * ((float)7/16);
				}
			}
		}
	}

	for (int i = 0; i < img_height; i++) delete[] float_rgb[i];
	delete[] float_rgb;
	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}

//------------------------Filter------------------------

///////////////////////////////////////////////////////////////////////////////
//
//     Filtering the img_data array by the filter from the parameters
//
///////////////////////////////////////////////////////////////////////////////
void Application::filtering( double filter[][5] )
{
	unsigned char *rgb = this->To_RGB();



	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}

void Application::filtering( double **filter, int n )
{
	unsigned char *rgb = this->To_RGB();



	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Perform 5x5 box filter on this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
struct Int_RGB{
	int r;
	int g;
	int b;

	Int_RGB() : r(0), g(0), b(0) {}
	Int_RGB(int r, int g, int b) : r(r), g(g), b(b) {}
	Int_RGB& operator=(const Int_RGB& a)
	{
		r = a.r;
		g = a.g;
		b = a.b;
		return *this;
	}
	Int_RGB operator+(const Int_RGB& a) const
	{
		return Int_RGB(r+a.r, g+a.g, b+a.b);
	}
	Int_RGB operator-(const Int_RGB& a) const
	{
		return Int_RGB(r-a.r, g-a.g, b-a.b);
	}
	Int_RGB operator*(const int a) const
	{
		return Int_RGB(r*a, g*a, b*a);
	}
	Int_RGB& operator+=(const Int_RGB& a)
	{
		r = r + a.r;
		g = g + a.g;
		b = b + a.b;
		return *this;
	}
};
void Application::Filter_Box() // ok
{
	unsigned char *rgb = this->To_RGB();
	int N = 5, NS = 25;
	int EDGE = N / 2;
	int WIDTH = 2*EDGE + img_width, HEIGHT = 2*EDGE + img_height;
	int START = EDGE;
	int W_END = WIDTH - EDGE - 1, H_END = HEIGHT - EDGE - 1;
	Int_RGB **int_rgb = new Int_RGB *[HEIGHT];
	for (int i = 0; i < HEIGHT; i++)
	{
		int_rgb[i] = new Int_RGB[WIDTH];
		for (int j = 0; j < WIDTH; j++)
		{
			int ri = i, rj = j;
			if (i < START) ri = START + (START - i);
			if (i > H_END) ri = H_END - (i - H_END);
			if (j < START) rj = START + (START - j);
			if (j > W_END) rj = W_END - (j - W_END);
			int offset = ((ri-EDGE) * img_width + (rj-EDGE)) * 3; // original picture has no edge
			int_rgb[i][j] = Int_RGB(rgb[offset + rr], rgb[offset + gg], rgb[offset + bb]);
		}
	}

	for (int i = START; i <= H_END; i++)
	{
		for (int j = START; j <= W_END; j++)
		{
			Int_RGB result;
			for (int ti = i-EDGE; ti <= i+EDGE; ti++) {
				for (int tj = j-EDGE; tj <= j+EDGE; tj++) {
					result += int_rgb[ti][tj];
				}
			}
			int offset_rgba = ((i-START) * img_width + (j-START)) * 4;
			img_data[offset_rgba + rr] = (unsigned char)(result.r / NS);
			img_data[offset_rgba + gg] = (unsigned char)(result.g / NS);
			img_data[offset_rgba + bb] = (unsigned char)(result.b / NS);
		}
	}

	for (int i = 0; i < H_END; i++) delete[] int_rgb[i];
	delete[] int_rgb;
	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Perform 5x5 Bartlett filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Filter_Bartlett() // ok
{
	unsigned char *rgb = this->To_RGB();
	int N = 5, NS = 81; // full weight is 81
	int EDGE = N / 2;
	int WIDTH = 2*EDGE + img_width, HEIGHT = 2*EDGE + img_height;
	int START = EDGE;
	int W_END = WIDTH - EDGE - 1, H_END = HEIGHT - EDGE - 1;
	Int_RGB **int_rgb = new Int_RGB *[HEIGHT];
	for (int i = 0; i < HEIGHT; i++)
	{
		int_rgb[i] = new Int_RGB[WIDTH];
		for (int j = 0; j < WIDTH; j++)
		{
			int ri = i, rj = j;
			if (i < START) ri = START + (START - i);
			if (i > H_END) ri = H_END - (i - H_END);
			if (j < START) rj = START + (START - j);
			if (j > W_END) rj = W_END - (j - W_END);
			int offset = ((ri-START) * img_width + (rj-START)) * 3; // original picture has no edge
			int_rgb[i][j] = Int_RGB(rgb[offset + rr], rgb[offset + gg], rgb[offset + bb]);
		}
	}

	int mask[5][5] = {{1, 2, 3, 2, 1},
					  {2, 4, 6, 4, 2},
					  {3, 6, 9, 6, 3},
					  {2, 4, 6, 4, 2},
					  {1, 2, 3, 2, 1}};
	for (int i = START; i <= H_END; i++)
	{
		for (int j = START; j <= W_END; j++)
		{
			Int_RGB result;
			for (int mi = 0; mi < 5; mi++) {
				for (int mj = 0; mj < 5; mj++) {
					result += int_rgb[(i-EDGE) + mi][(j-EDGE) + mj] * mask[mi][mj];
				}
			}
			int offset_rgba = ((i-START) * img_width + (j-START)) * 4; // original picture has no edge
			img_data[offset_rgba + rr] = (unsigned char)(result.r / NS);
			img_data[offset_rgba + gg] = (unsigned char)(result.g / NS);
			img_data[offset_rgba + bb] = (unsigned char)(result.b / NS);
		}
	}

	for (int i = 0; i < H_END; i++) delete[] int_rgb[i];
	delete[] int_rgb;
	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Perform 5x5 Gaussian filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Filter_Gaussian() // ok
{
	unsigned char *rgb = this->To_RGB();
	int N = 5, NS = 256; // full weight is 256
	int EDGE = N / 2;
	int WIDTH = 2*EDGE + img_width, HEIGHT = 2*EDGE + img_height;
	int START = EDGE;
	int W_END = WIDTH - EDGE - 1, H_END = HEIGHT - EDGE - 1;
	Int_RGB **int_rgb = new Int_RGB *[HEIGHT];
	for (int i = 0; i < HEIGHT; i++)
	{
		int_rgb[i] = new Int_RGB[WIDTH];
		for (int j = 0; j < WIDTH; j++)
		{
			int ri = i, rj = j;
			if (i < START) ri = START + (START - i);
			if (i > H_END) ri = H_END - (i - H_END);
			if (j < START) rj = START + (START - j);
			if (j > W_END) rj = W_END - (j - W_END);
			int offset = ((ri-START) * img_width + (rj-START)) * 3; // original picture has no edge
			int_rgb[i][j] = Int_RGB(rgb[offset + rr], rgb[offset + gg], rgb[offset + bb]);
		}
	}

	int mask[5][5] = {{1, 4, 6, 4, 1},
					  {4,16,24,16, 4},
					  {6,24,36,24, 6},
					  {4,16,24,16, 4},
					  {1, 4, 6, 4, 1}};
	for (int i = START; i <= H_END; i++)
	{
		for (int j = START; j <= W_END; j++)
		{
			Int_RGB result;
			for (int mi = 0; mi < 5; mi++) {
				for (int mj = 0; mj < 5; mj++) {
					result += int_rgb[(i-EDGE) + mi][(j-EDGE) + mj] * mask[mi][mj];
				}
			}
			int offset_rgba = ((i-START) * img_width + (j-START)) * 4; // original picture has no edge
			img_data[offset_rgba + rr] = (unsigned char)(result.r / NS);
			img_data[offset_rgba + gg] = (unsigned char)(result.g / NS);
			img_data[offset_rgba + bb] = (unsigned char)(result.b / NS);
		}
	}

	for (int i = 0; i < H_END; i++) delete[] int_rgb[i];
	delete[] int_rgb;
	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Perform NxN Gaussian filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Filter_Gaussian_N( unsigned int N )
{

}
///////////////////////////////////////////////////////////////////////////////
//
//  Perform 5x5 edge detect (high pass) filter on this image.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Filter_Edge()
{

}
///////////////////////////////////////////////////////////////////////////////
//
//  Perform a 5x5 enhancement filter to this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Filter_Enhance()
{
	unsigned char *rgb = this->To_RGB();



	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}

//------------------------Size------------------------

///////////////////////////////////////////////////////////////////////////////
//
//  Halve the dimensions of this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Half_Size()
{
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Double the dimensions of this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Double_Size()
{
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  resample_src for resize and rotate
//
///////////////////////////////////////////////////////////////////////////////
void Application::resample_src(int u, int v, float ww, unsigned char* rgba)
{

}

///////////////////////////////////////////////////////////////////////////////
//
//  Scale the image dimensions by the given factor.  The given factor is 
//	assumed to be greater than one.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
Float_RGB Insert(Float_RGB A, Float_RGB B, float a, float b)
{
	return (a == 0.0)? A : (A * b + B * a) * (1 / (a + b));
}
void Application::Resize( float scale ) // ok
{
	unsigned char *rgb = this->To_RGB();

	int new_width = img_width * scale;
	int new_height = img_height * scale;
	unsigned char *new_data = new unsigned char[new_width * new_height * 4];

	Float_RGB **old_rgb = new Float_RGB *[img_height];
	for (int i = 0; i < img_height; i++) 
	{
		old_rgb[i] = new Float_RGB[img_width];
		int offset_i = i * img_width;
		for (int j = 0; j < img_width; j++)
		{
			int offset = (offset_i + j) * 3;
			old_rgb[i][j] = Float_RGB(rgb[offset + rr], rgb[offset + gg], rgb[offset + bb]);
		}
	}

	for (int i = 0; i < new_height; i++)
	{
		int offset_i = i * new_width;
		for (int j = 0; j < new_width; j++)
		{
			float oi = ((float)i/scale); // old i
			float oj = ((float)j/scale); // old j

			float i1 = oi, i2 = oi, di1 = 0, di2 = 0;
			i1 = floor(oi), i2 = (oi <= float(img_height-1))? ceil(oi) : i1;
			di1 = oi - i1, di2 = i2 - oi;

			float j1 = oj, j2 = oj, dj1 = 0, dj2 = 0;
			j1 = floor(oj), j2 = (oj <= float(img_width-1))? ceil(oj) : j1;
			dj1 = oj - j1, dj2 = j2 - oj;

			Float_RGB left = Insert(old_rgb[(int)i1][(int)j1], old_rgb[(int)i2][(int)j1], di1, di2);
			Float_RGB right = Insert(old_rgb[(int)i1][(int)j2], old_rgb[(int)i2][(int)j2], di1, di2);
			Float_RGB result = Insert(left, right, dj1, dj2);

			int offset = (offset_i + j) * 4;
			new_data[offset + rr] = (unsigned char)result.r;
			new_data[offset + gg] = (unsigned char)result.g;
			new_data[offset + bb] = (unsigned char)result.b;
			new_data[offset + aa] = WHITE;
		}
	}

	for (int i = 0; i < img_height; i++) delete[] old_rgb[i];
	delete[] old_rgb;
	delete[] rgb;
	img_height = new_height;
	img_width = new_width;
	img_data = new_data;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}

//////////////////////////////////////////////////////////////////////////////
//
//  Rotate the image clockwise by the given angle.  Do not resize the 
//  image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
#define PI 3.14159265
void Application::Rotate( float angleDegrees )
{
	unsigned char *rgb = this->To_RGB();

	Float_RGB **old_rgb = new Float_RGB *[img_height];
	for (int i = 0; i < img_height; i++) 
	{
		old_rgb[i] = new Float_RGB[img_width];
		int offset_i = i * img_width;
		for (int j = 0; j < img_width; j++)
		{
			int offset = (offset_i + j) * 3;
			old_rgb[i][j] = Float_RGB(rgb[offset + rr], rgb[offset + gg], rgb[offset + bb]);
		}
	}

	float sine = sin(angleDegrees * PI / 180);
	float cosine = cos(angleDegrees * PI / 180);

	for (int i = 0; i < img_height; i++)
	{
		int offset_i = i * img_width;
		for (int j = 0; j < img_width; j++)
		{
			float x = (float)j - ((float)img_width/2); // convert to x
			float y = (float)(img_height-i) - ((float)img_height/2); // convert to y
			float ox = ((float)x * cosine - (float)y * sine); // old x
			float oy = ((float)x * sine + (float)y * cosine); // old y
			float oi = (float)(img_height/2) - oy; // old i
			float oj = ox + ((float)img_width/2); // old j

			float i1 = oi, i2 = oi, di1 = 0, di2 = 0;
			i1 = floor(oi), i2 = (oi <= float(img_height-1))? ceil(oi) : i1;
			di1 = oi - i1, di2 = i2 - oi;

			float j1 = oj, j2 = oj, dj1 = 0, dj2 = 0;
			j1 = floor(oj), j2 = (oj <= float(img_width-1))? ceil(oj) : j1;
			dj1 = oj - j1, dj2 = j2 - oj;

			Float_RGB result = Float_RGB(0, 0, 0);
			if (oi < float(img_height-1) && oj < float(img_width-1) && oi >= 0 && oj >= 0) {
				Float_RGB left = Insert(old_rgb[(int)i1][(int)j1], old_rgb[(int)i2][(int)j1], di1, di2);
				Float_RGB right = Insert(old_rgb[(int)i1][(int)j2], old_rgb[(int)i2][(int)j2], di1, di2);
				result = Insert(left, right, dj1, dj2);
			}

			int offset = (offset_i + j) * 4;
			img_data[offset + rr] = (unsigned char)result.r;
			img_data[offset + gg] = (unsigned char)result.g;
			img_data[offset + bb] = (unsigned char)result.b;
			img_data[offset + aa] = WHITE;
		}
	}

	for (int i = 0; i < img_height; i++) delete[] old_rgb[i];
	delete[] old_rgb;
	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}

//------------------------Composing------------------------


void Application::loadSecondaryImge( QString filePath )
{
	mImageSrcSecond.load(filePath);

	renew();

	img_data2 = mImageSrcSecond.bits();
	img_width2 = mImageSrcSecond.width();
	img_height2 = mImageSrcSecond.height();
}

//////////////////////////////////////////////////////////////////////////
//
//	Composite the image A and image B by Over, In, Out, Xor and Atom. 
//
//////////////////////////////////////////////////////////////////////////
void Application::Comp_image( int tMethod )
{
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}

///////////////////////////////////////////////////////////////////////////////
//
//      Composite the current image over the given image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Comp_Over()
{
	if (img_height == img_height2 && img_width == img_width2)
	{

	}
	else
	{
		std::cout << "Images not the same size" << std::endl;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image "in" the given image.  See lecture notes for 
//  details.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Comp_In()
{
	if (img_height == img_height2 && img_width == img_width2)
	{

	}
	else
	{
		std::cout << "Images not the same size" << std::endl;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image "out" the given image.  See lecture notes for 
//  details.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Comp_Out()
{
	if (img_height == img_height2 && img_width == img_width2)
	{

	}
	else
	{
		std::cout << "Images not the same size" << std::endl;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//      Composite current image "atop" given image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Comp_Atop()
{
	if (img_height == img_height2 && img_width == img_width2)
	{

	}
	else
	{
		std::cout << "Images not the same size" << std::endl;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image with given image using exclusive or (XOR).  Return
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Comp_Xor()
{
	if (img_height == img_height2 && img_width == img_width2)
	{

	}
	else
	{
		std::cout << "Images not the same size" << std::endl;
	}
}

//------------------------NPR------------------------

///////////////////////////////////////////////////////////////////////////////
//
//      Run simplified version of Hertzmann's painterly image filter.
//      You probably will want to use the Draw_Stroke funciton and the
//      Stroke class to help.
// Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::NPR_Paint()
{
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}

void Application::NPR_Paint_Layer( unsigned char *tCanvas, unsigned char *tReferenceImage, int tBrushSize )
{

}

///////////////////////////////////////////////////////////////////////////////
//
//      Helper function for the painterly filter; paint a stroke at
// the given location
//
///////////////////////////////////////////////////////////////////////////////
void Application::Paint_Stroke( const Stroke& s )
{
	int radius_squared = (int)s.radius * (int)s.radius;
	for (int x_off = -((int)s.radius); x_off <= (int)s.radius; x_off++) 
	{
		for (int y_off = -((int)s.radius); y_off <= (int)s.radius; y_off++) 
		{
			int x_loc = (int)s.x + x_off;
			int y_loc = (int)s.y + y_off;

			// are we inside the circle, and inside the image?
			if ((x_loc >= 0 && x_loc < img_width && y_loc >= 0 && y_loc < img_height)) 
			{
				int dist_squared = x_off * x_off + y_off * y_off;
				int offset_rgba = (y_loc * img_width + x_loc) * 4;

				if (dist_squared <= radius_squared) 
				{
					img_data[offset_rgba + rr] = s.r;
					img_data[offset_rgba + gg] = s.g;
					img_data[offset_rgba + bb] = s.b;
					img_data[offset_rgba + aa] = s.a;
				} 
				else if (dist_squared == radius_squared + 1) 
				{
					img_data[offset_rgba + rr] = (img_data[offset_rgba + rr] + s.r) / 2;
					img_data[offset_rgba + gg] = (img_data[offset_rgba + gg] + s.g) / 2;
					img_data[offset_rgba + bb] = (img_data[offset_rgba + bb] + s.b) / 2;
					img_data[offset_rgba + aa] = (img_data[offset_rgba + aa] + s.a) / 2;
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//      Build a Stroke
//
///////////////////////////////////////////////////////////////////////////////
Stroke::Stroke() {}

///////////////////////////////////////////////////////////////////////////////
//
//      Build a Stroke
//
///////////////////////////////////////////////////////////////////////////////
Stroke::Stroke(unsigned int iradius, unsigned int ix, unsigned int iy,
	unsigned char ir, unsigned char ig, unsigned char ib, unsigned char ia) :
radius(iradius),x(ix),y(iy),r(ir),g(ig),b(ib),a(ia)
{
}
