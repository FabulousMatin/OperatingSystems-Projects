
#include "Photo.hpp"

using namespace std;

void change_color(Pixel& pixel, Pixel rgb) {
    pixel.red = rgb.red;
    pixel.green = rgb.green;
    pixel.blue = rgb.blue;
}


BMP::BMP(char* _fileName) {
    fileName = _fileName;
}

Photo::Photo(int _rows, int _cols) {
    rows = _rows;
    cols = _cols;

    pixels.resize(rows);

    for(int i = 0; i < rows; i++) {
        pixels[i].resize(cols);
    }
}


int BMP::alloc() {
    ifstream file(fileName);
    if (!file) {
        cout << "File" << fileName << " doesn't exist!" << endl;
        return -1;
    }

    file.seekg(0, ios::end);
    streampos length = file.tellg();
    file.seekg(0, ios::beg);

    fileBuffer = new char[length];
    file.read(&fileBuffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&fileBuffer[0]);
    info_header = (PBITMAPINFOHEADER)(&fileBuffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    bufferSize = file_header->bfSize;

    return 0;
}

void BMP::read(Photo& pic) {
    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                case 0:
                    pic.pixels[i][j].red = fileBuffer[bufferSize - count]; 
                    break;
                case 1:
                    pic.pixels[i][j].green = fileBuffer[bufferSize - count]; 
                    break;
                case 2:
                    pic.pixels[i][j].blue = fileBuffer[bufferSize - count]; 
                    break;
                }
                count++;
            }
        }
    }
}

void BMP::write(Photo pic, char* destFile) {
    ofstream write(destFile);
    if (!write) {
        cout << "Failed to write " << destFile << endl;
        return;
    }

    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                case 0:
                    fileBuffer[bufferSize - count] = pic.pixels[i][j].red;
                    break;
                case 1:
                    fileBuffer[bufferSize - count] = pic.pixels[i][j].green;
                    break;
                case 2:
                    fileBuffer[bufferSize - count] = pic.pixels[i][j].blue;
                    break;
                }
                count++;
            }
        }
    }
    write.write(fileBuffer, bufferSize);
}

tuple<int, int> BMP::get_size() {
    return make_tuple(this->rows, this->cols);
}

