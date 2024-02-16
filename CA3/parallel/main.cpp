#include "ParallelFilter.hpp"
#include <chrono>
#include <pthread.h>

using namespace std;
#define NOW chrono::high_resolution_clock::now()
#define DURATION(x) fixed << setprecision(2) << (chrono::duration_cast<chrono::nanoseconds>(NOW - x).count() / 1000000.0f)


int main(int argc, char* argv[]) { 
    BMP bmp(argv[1]);
    if (bmp.alloc() < 0) {
        cout << "File read error." << endl;
        return 1;
    }

    int rows, cols;
    rows = get<0>(bmp.get_size());
    cols = get<1>(bmp.get_size());

    ParallelFilter filter;
    Photo pic(rows, cols);
    int nthreads = (argc == 3) ? atoi(argv[2]) : 8;
    auto ticks = NOW;
    auto start = ticks;

    bmp.parallel(pic, &bmp.partial_read, nthreads);
    cout << "read: " << DURATION(ticks) << " milliseconds\n";
    ticks = NOW;

    filter.parallel(pic, &ParallelFilter::partial_purple, nthreads);
    cout << "purple: " << DURATION(ticks) << " milliseconds\n";
    ticks = NOW;

    filter.parallel(pic, &ParallelFilter::partial_blur, nthreads);
    cout << "blur: " << DURATION(ticks) << " milliseconds\n";
    ticks = NOW;

    filter.parallel(pic, &ParallelFilter::partial_reverse, nthreads);
    cout << "flip: " << DURATION(ticks) << " milliseconds\n";
    ticks = NOW;

    filter.parallel_hatch(pic);
    cout << "hatch: " << DURATION(ticks) << " milliseconds\n";
    ticks = NOW;

    bmp.parallel(pic, &bmp.partial_write, nthreads, (char*)"output.bmp");
    cout << "write: " << DURATION(ticks) << " milliseconds\n";
    ticks = NOW;

    cout << "total execution time: " << DURATION(start) << " milliseconds\n";

    return 0;
}