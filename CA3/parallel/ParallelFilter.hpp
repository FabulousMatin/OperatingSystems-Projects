#ifndef _FILTER_
#define _FILTER_

#include <bits/stdc++.h>
#include <algorithm>
#include <pthread.h>
#include "Photo.hpp"
// #include "MyThread.hpp"



class ParallelFilter {

private:
    static void* partial_drawline(void *args);
    static void partial_convult(std::array<std::array<float, 3>, 3> kernel, void *args);
    enum class kerneltype {
        GAUSSIAN_BLUR,
        BLUR
    };
    std::map<kerneltype, std::array<std::array<float, 3>, 3>> kernels;
    
    struct Line {
        Line(int _x1, int _x2, int _y1, int _y2, Photo& _pic) : pic(_pic) {
            x1 = _x1;
            x2 = _x2;
            y1 = _y1;
            y2 = _y2;
        } 
        int x1;
        int x2;
        int y1;
        int y2;
        Photo& pic;
    };

    class Thread {
        public:
            Thread(int col_begin, int _col_end, Photo& _pic) : pic(_pic) {
                this->col_begin = col_begin;
                this->col_end = _col_end;
            }
            int col_begin;
            int col_end;
            Photo& pic;
    };


public:
    ParallelFilter();
    
    void parallel(Photo& pic, void* filter(void*), int n_threads);
    static void* partial_reverse(void*);
    static void* partial_purple(void*);
    static void* partial_blur(void*);

    void parallel_hatch(Photo &pic);
};



#endif