#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <string>
#include <cufft.h> 

const double PI = 3.14159265358979323846;

int main(int argc, char** argv) {
    // Take the dimension directly from the command line!
    int dim = 10; 
    if (argc > 1) {
        dim = std::stoi(argv[1]);
    }
    
    int N = dim * dim;

    cufftDoubleComplex *h_data = new cufftDoubleComplex[N];

    double fs = 1000.0;
    int M = 7;
    double fin = (static_cast<double>(M) / N) * fs;

    for (int i = 0; i < N; ++i) {
        double t = static_cast<double>(i) / fs;
        h_data[i].x = std::sin(2.0 * PI * fin * t); 
        h_data[i].y = 0.0;                          
    }

    cufftDoubleComplex *d_data;
    cudaMalloc((void**)&d_data, sizeof(cufftDoubleComplex) * N);
    cudaMemcpy(d_data, h_data, sizeof(cufftDoubleComplex) * N, cudaMemcpyHostToDevice);

    cufftHandle plan;
    cufftPlan2d(&plan, dim, dim, CUFFT_Z2Z);

    cudaDeviceSynchronize(); 
    auto start_time = std::chrono::high_resolution_clock::now();

    cufftExecZ2Z(plan, d_data, d_data, CUFFT_FORWARD);
    cufftExecZ2Z(plan, d_data, d_data, CUFFT_INVERSE);
    
    cudaDeviceSynchronize(); 
    auto end_time = std::chrono::high_resolution_clock::now();
    
    std::chrono::duration<double, std::micro> comp_time = end_time - start_time;
    std::cout << "CUDA FFT + IFFT (" << dim << "x" << dim << "): " << comp_time.count() << " us\n";

    cudaMemcpy(h_data, d_data, sizeof(cufftDoubleComplex) * N, cudaMemcpyDeviceToHost);

    cufftDestroy(plan);
    cudaFree(d_data);
    delete[] h_data;

    return 0;
}