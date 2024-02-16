
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

void* BMP::partial_read(void* args) {
    Thread* params = (Thread*)(args);

    auto& row_begin = params->row_begin;
    auto& row_end = params->row_end;
    auto& pic = params->pic;
    auto& bmp = params->source;


    int count = (row_begin) * bmp->cols * 3 + 1;
    int extra = bmp->cols % 4;
    for (int i = row_begin; i < row_end; i++) {
        count += extra;
        for (int j = bmp->cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                case 0:
                    pic.pixels[i][j].red = bmp->fileBuffer[bmp->bufferSize - count]; 
                    break;
                case 1:
                    pic.pixels[i][j].green = bmp->fileBuffer[bmp->bufferSize - count]; 
                    break;
                case 2:
                    pic.pixels[i][j].blue = bmp->fileBuffer[bmp->bufferSize - count]; 
                    break;
                }
                count++;
            }
        }
    }

    return NULL;
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

void* BMP::partial_write(void* args) {
    Thread* params = (Thread*)(args);

    auto& row_begin = params->row_begin;
    auto& row_end = params->row_end;
    auto& pic = params->pic;
    auto& destFile = params->file_dest;
    auto& bmp = params->source;

    ofstream write(destFile);
    if (!write) {
        cout << "Failed to write " << destFile << endl;
        return NULL;
    }

    int count = (row_begin) * bmp->cols * 3 + 1;
    int extra = bmp->cols % 4;
    for (int i = row_begin; i < row_end; i++) {
        count += extra;
        for (int j = bmp->cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                case 0:
                    bmp->fileBuffer[bmp->bufferSize - count] = pic.pixels[i][j].red;
                    break;
                case 1:
                    bmp->fileBuffer[bmp->bufferSize - count] = pic.pixels[i][j].green;
                    break;
                case 2:
                    bmp->fileBuffer[bmp->bufferSize - count] = pic.pixels[i][j].blue;
                    break;
                }
                count++;
            }
        }
    }
    
    write.write(bmp->fileBuffer, bmp->bufferSize);
    return NULL;
}

tuple<int, int> BMP::get_size() {
    return make_tuple(this->rows, this->cols);
}

void
BMP::parallel(Photo &pic, void *filter(void *), int n_threads = 8, char* file_dest) {

    auto create_thread = [&pic, &filter, this](int row_begin, int row_end, char* file_dest) {
        Thread *thread_args = new Thread(row_begin, row_end, file_dest, pic, this);
        pthread_t t_id;
        pthread_create(&t_id, NULL, *filter, (void *) thread_args);
        return t_id;
    };

    vector<pthread_t> t_ids;

    int height = pic.rows;
    for (int i = 0; i < n_threads; ++i) {
        int row_begin = i * (height / n_threads);
        int row_end = (i != n_threads - 1) ? (i + 1) * (height / n_threads) : height;
        t_ids.push_back(create_thread(row_begin, row_end, file_dest));
    }

    for (auto t_id : t_ids)
        pthread_join(t_id, NULL);

}
    // ofstream write("output.bmp");
    // if (!write) {
    //     cout << "Failed to write " << endl;
    //     return;
    // }
    // write.write(fileBuffer, bufferSize);
