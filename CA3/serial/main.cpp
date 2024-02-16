#include "Filter.hpp"
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

    Filter filter;
    Photo pic(rows, cols);
    auto ticks = NOW;
    auto start = ticks;

    bmp.read(pic);
    cout << "read: " << DURATION(ticks) << " milliseconds\n";
    ticks = NOW;

    filter.purple(pic);
    cout << "purple: " << DURATION(ticks) << " milliseconds\n";
    ticks = NOW;

    filter.blur(pic);
    cout << "blur: " << DURATION(ticks) << " milliseconds\n";
    ticks = NOW;

    filter.reverse(pic);
    cout << "flip: " << DURATION(ticks) << " milliseconds\n";
    ticks = NOW;

    filter.hatch(pic);
    cout << "hatch: " << DURATION(ticks) << " milliseconds\n";
    ticks = NOW;

    bmp.write(pic, (char*)"output.bmp");
    cout << "write: " << DURATION(ticks) << " milliseconds\n";
    ticks = NOW;

    cout << "total execution time: " << DURATION(start) << " milliseconds\n";


    return 0;
}