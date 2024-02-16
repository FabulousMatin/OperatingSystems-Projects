#include "Filter.hpp"

using namespace std;


void Filter::convult(array<array<float, 3>, 3> kernel, Photo& pic) {

    auto outside = [&pic](int row, int col) -> bool {
        return row < 0 || row >= pic.rows || col < 0 || col >= pic.cols;
    };

    for(int row = 0; row < pic.rows; row++) {
        for (int col = 0; col < pic.cols; col++) {
            float red = 0, green = 0, blue = 0;
            for(int rowoff = -1; rowoff <= 1; rowoff++) {
                for(int coloff = -1; coloff <= 1; coloff++){
                    int i = row;
                    int j = col;
                    if(!outside(i + rowoff, j + coloff)) {
                        i = row + rowoff;
                        j = col + coloff;
                    }
                    red += kernel[rowoff + 1][coloff + 1] * pic.pixels[i][j].red;
                    green += kernel[rowoff + 1][coloff + 1] * pic.pixels[i][j].green;
                    blue += kernel[rowoff + 1][coloff + 1] * pic.pixels[i][j].blue;   
                }
            }
            pic.pixels[row][col].red = min<int>(255, max<int>(0, red));
            pic.pixels[row][col].green = min<int>(255, max<int>(0, green));
            pic.pixels[row][col].blue = min<int>(255, max<int>(0, blue));
        }
    }
}


void
Filter::draw_line(int x1, int y1, int x2, int y2, Photo &pic) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float step = max(abs(dx), abs(dy));

    dx = dx / step;
    dy = dy / step;

    for (int i = 0; i <= step; i++) {
        change_color(pic.pixels[int(x1)][int(y1)], WHITE);

        x1 += dx;
        y1 += dy;
    }
}

void
Filter::reverse(Photo &pic) {
    for (int j = 0; j < pic.cols; j++) {
        for (int i = 0; i < pic.rows / 2; i++) {
            swap(pic.pixels[i][j], pic.pixels[pic.rows - i - 1][j]);
        }
    }
}

void
Filter::purple(Photo &pic) {
    Photo temp(pic.rows, pic.cols);
    for (auto &row : pic.pixels)
        for (auto &pixel : row) {
            auto temp = pixel;
            pixel.red = min<int>(255, 0.5 * temp.red + 0.3 * temp.green +
                                          0.5 * temp.blue);
            pixel.green = min<int>(255, 0.16 * temp.red + 0.5 * temp.green +
                                            0.16 * temp.blue);
            pixel.blue = min<int>(255, 0.6 * temp.red + 0.2 * temp.green +
                                           0.8 * temp.blue);
        }
}

void
Filter::hatch(Photo &pic) {
    draw_line(pic.rows / 2, 0, 0, pic.cols / 2, pic);
    draw_line(pic.rows - 1, 0, 0, pic.cols - 1, pic);
    draw_line(pic.rows - 1, pic.cols / 2, pic.rows / 2, pic.cols - 1, pic);
}

void Filter::blur(Photo &pic) {
    convult(kernels[kerneltype::GAUSSIAN_BLUR], pic);
}

Filter::Filter() {
    kernels[kerneltype::GAUSSIAN_BLUR] = {{
        {1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f},
        {2.0f / 16.0f, 4.0f / 16.0f, 2.0f / 16.0f},
        {1.0f / 16.0f, 2.0f / 16.0f, 1.0f / 16.0f},
    }};

    kernels[kerneltype::BLUR] = {{
        {1.0f / 16.0f, 1.0f / 16.0f, 1.0f / 16.0f},
        {1.0f / 16.0f, 1.0f / 16.0f, 1.0f / 16.0f},
        {1.0f / 16.0f, 1.0f / 16.0f, 1.0f / 16.0f},
    }};
}