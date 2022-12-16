# benchmarking
This repository contains the benchmarking scripts used to benchmark ALEX and a B-tree for CS598 AWG.

# Datasets
- We used the longitudes dataset which is part of OpenStreetMap (https://aws.amazon.com/about-aws/whats-new/2017/06/openstreetmap-public-data-set-now-available-on-aws/). A download of this dataset can be found at https://github.com/microsoft/ALEX.

- We also used the Uniform 64-bit integer dataset from SOSD. The dataset can be found here: https://github.com/learnedsystems/SOSD/blob/master/gen_uniform.py

# Running the benchmarks
Ensure you have downloaded the datasets from above.
 - To run the ALEX benchmark we need to clone the repository from https://github.com/microsoft/ALEX. We can then replace main.cpp with our benchmark in the benchmark folder in the ALEX repository. You can then follow the instructions on the ALEX repository to build and run the benchmark. Ensure you change the file path ```std::string keys_file_path = "resources/uniform_dense_200M_uint64";```

# Citations
- https://github.com/microsoft/ALEX
- https://github.com/abseil/abseil-cpp
- https://github.com/learnedsystems/SOSD
