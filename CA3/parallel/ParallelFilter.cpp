#include "ParallelFilter.hpp"

using namespace std;

void *
ParallelFilter::partial_purple(void *args) {
    Thread *params = (Thread *) (args);

    auto &col_begin = params->col_begin;
    auto &col_end = params->col_end;
    auto &pic = params->pic;

    for (auto &row : pic.pixels)
        for (int j = col_begin; j < col_end; j++) {
            auto temp = row[j];
            row[j].red = min<int>(255, 0.5 * temp.red + 0.3 * temp.green +
                                           0.5 * temp.blue);
            row[j].green = min<int>(255, 0.16 * temp.red + 0.5 * temp.green +
                                             0.16 * temp.blue);
            row[j].blue = min<int>(255, 0.6 * temp.red + 0.2 * temp.green +
                                            0.8 * temp.blue);
        }

    return NULL;
}

void *
ParallelFilter::partial_reverse(void *args) {
    Thread *params = (Thread *) (args);

    auto &col_begin = params->col_begin;
    auto &col_end = params->col_end;
    auto &pic = params->pic;

    for (int j = col_begin; j < col_end; j++) {
        for (int i = 0; i < pic.rows / 2; i++) {
            swap(pic.pixels[i][j], pic.pixels[pic.rows - i - 1][j]);
        }
    }
    return NULL;
}

void
ParallelFilter::partial_convult(array<array<float, 3>, 3> kernel, void *args) {

    Thread *params = (Thread *) (args);

    auto &col_begin = params->col_begin;
    auto &col_end = params->col_end;
    auto &pic = params->pic;

    auto is_outside = [&pic](int row, int col) -> bool {
        return row < 0 || row >= pic.rows || col < 0 || col >= pic.cols;
    };

    for (int row = 0; row < pic.rows; row++) {
        for (int col = col_begin; col < col_end; col++) {
            float red = 0, green = 0, blue = 0;
            for (int rowoff = -1; rowoff <= 1; rowoff++) {
                for (int coloff = -1; coloff <= 1; coloff++) {
                    int i = row;
                    int j = col;
                    if (!is_outside(i + rowoff, j + coloff)) {
                        i = row + rowoff;
                        j = col + coloff;
                    }
                    red +=
                        kernel[rowoff + 1][coloff + 1] * pic.pixels[i][j].red;
                    green +=
                        kernel[rowoff + 1][coloff + 1] * pic.pixels[i][j].green;
                    blue +=
                        kernel[rowoff + 1][coloff + 1] * pic.pixels[i][j].blue;
                }
            }
            pic.pixels[row][col].red = min<int>(255, max<int>(0, red));
            pic.pixels[row][col].green = min<int>(255, max<int>(0, green));
            pic.pixels[row][col].blue = min<int>(255, max<int>(0, blue));
        }
    }
}

void *
ParallelFilter::partial_drawline(void *args) {
    Line *params = (Line *) (args);
    int x1 = params->x1;
    int x2 = params->x2;
    int y1 = params->y1;
    int y2 = params->y2;
    Photo &pic = params->pic;

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

    return NULL;
}

void
ParallelFilter::parallel_hatch(Photo &pic) {
    vector<pthread_t> t_ids;

    auto create_thread = [&pic](int x1, int y1, int x2, int y2) {
        Line *thread_args = new Line(x1, y1, x2, y2, pic);
        pthread_t t_id;
        pthread_create(&t_id, NULL, &ParallelFilter::partial_drawline,
                       (void *) thread_args);
        return t_id;
    };

    t_ids.push_back(create_thread(pic.rows / 2, 0, 0, pic.cols / 2));
    t_ids.push_back(create_thread(pic.rows - 1, 0, 0, pic.cols - 1));
    t_ids.push_back(
        create_thread(pic.rows - 1, pic.cols / 2, pic.rows / 2, pic.cols - 1));

    for (auto t_id : t_ids)
        pthread_join(t_id, NULL);
}

void *
ParallelFilter::partial_blur(void *args) {
    ParallelFilter nsmember;
    nsmember.partial_convult(nsmember.kernels[kerneltype::GAUSSIAN_BLUR], args);

    return NULL;
}

ParallelFilter::ParallelFilter() {
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

void
ParallelFilter::parallel(Photo &pic, void *filter(void *), int n_threads = 8) {

    auto create_thread = [&pic, &filter](int col_begin, int col_end) {
        Thread *thread_args = new Thread(col_begin, col_end, pic);
        pthread_t t_id;
        pthread_create(&t_id, NULL, *filter, (void *) thread_args);
        return t_id;
    };

    vector<pthread_t> t_ids;

    int width = pic.cols;
    for (int i = 0; i < n_threads; ++i) {
        int col_begin = i * (width / n_threads);
        int col_end = (i != n_threads - 1) ? (i + 1) * (width / n_threads) : width;
        t_ids.push_back(create_thread(col_begin, col_end));
    }


    for (auto t_id : t_ids)
        pthread_join(t_id, NULL);
}
