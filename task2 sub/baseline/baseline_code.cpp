#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <fftw3.h>

const double PI = 3.14159265358979323846;

int main() {
    int dim = 10; // Remember to scale this to 100, 700, 2000, 5000
    int N = dim * dim;
    
    double fs = 1000.0; 
    int M = 7;          
    double fin = (static_cast<double>(M) / N) * fs;
    
    fftw_complex *in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    
    for (int i = 0; i < N; ++i) {
        double t = static_cast<double>(i) / fs;
        in[i][0] = std::sin(2.0 * PI * fin * t); 
        in[i][1] = 0.0;                                
    }
    
    // Create BOTH plans (Forward and Backward)
    fftw_plan plan_fwd = fftw_plan_dft_2d(dim, dim, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_plan plan_bwd = fftw_plan_dft_2d(dim, dim, out, in, FFTW_BACKWARD, FFTW_ESTIMATE);
    
    // Measure time for the full cycle
    auto start_time = std::chrono::high_resolution_clock::now();
    
    fftw_execute(plan_fwd); // Compute FFT
    fftw_execute(plan_bwd); // Compute IFFT
    
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> comp_time = end_time - start_time;
    
    std::cout << "Serial FFT + IFFT Time: " << comp_time.count() << " microseconds." << std::endl;
    
    // Cleanup
    fftw_destroy_plan(plan_fwd);
    fftw_destroy_plan(plan_bwd);
    fftw_free(in);
    fftw_free(out);
    
    return 0;
}