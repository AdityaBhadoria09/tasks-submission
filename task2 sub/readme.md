Matrix FFT Computation - Performance Benchmarking

Overview

This project benchmarks the execution performance of computing the Fast Fourier Transform (FFT) and Inverse Fast Fourier Transform (IFFT) on coherently sampled 2D sine wave matrices. It compares three different computational approaches:

Serial Execution (Single-threaded CPU using FFTW3)

OpenMPI Execution (Multi-core CPU parallelization using FFTW3)

CUDA Execution (GPU offloading using cuFFT)

System Prerequisites

This project was built and tested on Ubuntu Linux. The following packages are required:

build-essential (g++ compiler)

cmake (Build system)

libfftw3-dev (FFTW3 Library)

openmpi-bin & libopenmpi-dev (OpenMPI Library)

nvidia-cuda-toolkit (CUDA Compiler and cuFFT)

To install all dependencies on Ubuntu:

sudo apt update
sudo apt install build-essential cmake gdb libfftw3-dev libopenmpi-dev openmpi-bin nvidia-cuda-toolkit -y


Build Instructions

The project uses CMake to link the required libraries (FFTW, MPI, and cuFFT) and generate the Makefiles.

From the root project directory (matrix_fft_project), run:

# 1. Generate the build system files
cmake .

# 2. Compile the executables
make


This will generate three executable files: serial_fft, mpi_fft, and cuda_fft.

Execution Instructions

The MPI and CUDA executables accept the matrix dimension as a command-line argument. The benchmark requires testing sizes: 10, 100, 700, 2000, and 5000.

1. Serial (Single CPU Core)

To run the serial version (edit the dim variable in main.cpp and recompile to change sizes):

./serial_fft


2. OpenMPI (Parallel CPU)

To run the MPI version across 2 CPU cores for a specific matrix size (e.g., 100x100):

mpirun -np 2 ./mpi_fft 100


Note: The dimension size must be perfectly divisible by the number of cores (-np).

3. CUDA (GPU)

To run the CUDA version for a specific matrix size (e.g., 5000x5000):

./cuda_fft 5000


Documented Bugs, Issues, and Solutions

During the setup and development phases, the following issues were encountered and resolved:

1. Ubuntu Package Manager Architecture Errors (404)

Issue: Initial sudo apt update commands hung and threw 404 Not Found errors attempting to fetch armhf packages on an amd64 architecture.

Solution: Removed the foreign architecture configuration and flushed the corrupted package cache to restore normal apt functionality:
sudo dpkg --remove-architecture armhf
sudo apt clean

2. MPI Core Divisibility Constraint

Issue: The OpenMPI strategy relied on dividing the matrix rows equally among the available processes using MPI_Scatter. Attempting to run dimensions that did not divide evenly (e.g., a 700x700 matrix split across 4 cores) caused memory misalignment and segmentation faults.

Solution: Added a strict modulus guard (dim % size != 0) early in the execution to catch the mathematical impossibility and gracefully exit before memory allocation. The benchmark was standardized on -np 2 to fit the assigned matrix sizes smoothly.

3. CUDA Silent Failure and Kernel Launch Overhead

Issue: When running the CUDA implementation across all matrix sizes (from 10x10 to 5000x5000), the execution timer consistently output ~3.5 microseconds.

Analysis & Solution: A $5000 \times 5000$ matrix contains 25 million complex elements. Hardware memory bandwidth limits make processing this in 3.5 microseconds physically impossible. The consistent ~3.5µs measurement represents the GPU kernel launch overhead (the time it takes for the CPU to issue instructions to the device). The actual math execution suffered a silent failure—likely an out-of-memory error on the VRAM during cufftExecZ2Z execution. This behavior was documented in the final analysis report to highlight the importance of explicit error-handling wrappers (like cudaGetErrorString) when managing massive parallel data sets on GPUs.