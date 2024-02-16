parallel:
- default number of threads is 8.
- ./ImageFilters.out <input> <n_threads>

serial:
- ./ImageFilters.out <input>

output:
- will be saved in "output.bmp"

report:
- includes:
-- _testing_main.cpp -> replace with main.cpp in parallel, to save results
-- execution_times -> results of exec time for different number of threads
-- report.ipynb -> plot results, speed-ups and etc.
-- .png -> results in image!