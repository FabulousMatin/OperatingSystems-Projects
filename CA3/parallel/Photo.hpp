#ifndef _PHOTO_
#define _PHOTO_

#include <fstream>
#include <iostream>
#include <string.h>
#include <vector>
#include <tuple>

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

struct Pixel {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
};

#define BLACK Pixel{0,0,0}
#define WHITE Pixel{255,255,255}

void change_color(Pixel& pixel, Pixel rgb);

class Photo {
	public:
		int rows;
		int cols;
		std::vector<std::vector<Pixel>> pixels;
		Photo(int, int);
};



class BMP {
	private:
		#pragma pack(push, 1)
		typedef struct tagBITMAPFILEHEADER {
			WORD bfType;
			DWORD bfSize;
			WORD bfReserved1;
			WORD bfReserved2;
			DWORD bfOffBits;
		} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

		typedef struct tagBITMAPINFOHEADER {
			DWORD biSize;
			LONG biWidth;
			LONG biHeight;
			WORD biPlanes;
			WORD biBitCount;
			DWORD biCompression;
			DWORD biSizeImage;
			LONG biXPelsPerMeter;
			LONG biYPelsPerMeter;
			DWORD biClrUsed;
			DWORD biClrImportant;
		} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
		#pragma pack(pop)
		
		class Thread {
			public:
				Thread(int row_begin, int _row_end, char* _file_dest, Photo& _pic, BMP* _source)
				 : pic(_pic), source(_source) {
					this->row_begin = row_begin;
					this->row_end = _row_end;
					this->file_dest = _file_dest;
				
				}
				char* file_dest;
				int row_begin;
				int row_end;
				Photo& pic;
				BMP* source;
		};
		
		int rows;
		int cols;
		char* fileBuffer;
		int bufferSize;
		char* fileName;


	public:
		BMP(char*);

		void parallel(Photo& pic, void* filter(void*), int n_threads, char* file_dest = NULL);
		int alloc();
		static void* partial_read(void*);
		static void* partial_write(void*);
		void read(Photo& pic);
		void write(Photo pic, char* destFile);

		std::tuple<int, int> get_size();
};



#endif