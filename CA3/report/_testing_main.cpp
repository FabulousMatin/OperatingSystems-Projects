#include "ParallelFilter.hpp"
#include <chrono>
#include <fstream>
#include <pthread.h>

using namespace std;

int main() {
    ofstream outputFile("execution_times.txt");
    if (!outputFile) {
        cout << "Failed to open the output file." << endl;
        return 1;
    }
    
    BMP bmp("input.bmp");
    if (bmp.alloc() < 0) {
        cout << "File read error." << endl;
        return 1;
    }

    int rows, cols;
    rows = get<0>(bmp.get_size());
    cols = get<1>(bmp.get_size());

    ParallelFilter filter;
    Photo pic(rows, cols);

    for (int nthreads = 1; nthreads <= 50; nthreads++) {
        auto startTime = chrono::high_resolution_clock::now();

        bmp.parallel(pic, &bmp.partial_read, nthreads);
        filter.parallel(pic, &ParallelFilter::partial_purple, nthreads);
        filter.parallel(pic, &ParallelFilter::partial_blur, nthreads);
        filter.parallel(pic, &ParallelFilter::partial_reverse, nthreads);
        filter.parallel_hatch(pic);
        bmp.parallel(pic, &bmp.partial_write, nthreads, "output.bmp");

        auto endTime = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();

        outputFile << nthreads << " " << duration << endl;
    }

    outputFile.close();
    cout << "Execution times have been written to the file." << endl;

    return 0;
}