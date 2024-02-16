#ifndef _FILTER_
#define _FILTER_

#include <bits/stdc++.h>
#include <algorithm>
#include "Photo.hpp"



class Filter {

private:
    void draw_line(int x1, int y1, int x2, int y2, Photo& pic);
    void convult(std::array<std::array<float, 3>, 3> kernel, Photo& pic);
    enum class kerneltype {
        GAUSSIAN_BLUR,
        BLUR
    };
    std::map<kerneltype, std::array<std::array<float, 3>, 3>> kernels;

public:
    Filter();
    void reverse(Photo& pic);
    void purple(Photo& pic);
    void blur(Photo& pic);
    void hatch(Photo& pic);
};



#endif