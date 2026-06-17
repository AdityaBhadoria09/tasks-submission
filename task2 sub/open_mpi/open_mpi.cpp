#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <string>
#include <mpi.h>
#include <fftw3.h>

const double PI = 3.14159265358979323846;

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

    // Take the dimension directly from the command line!
    int dim = 10; 
    if (argc > 1) {
        dim = std::stoi(argv[1]);
    }
    
    int N_total = dim * dim;
    
    if (dim % size != 0) {
        if (rank == 0) std::cerr << "Error: Dimension " << dim << " must be divisible by " << size << " cores." << std::endl;
        MPI_Finalize();
        return 1;
    }

    int rows_per_proc = dim / size;
    int elements_per_proc = rows_per_proc * dim;

    fftw_complex *global_in = nullptr;
    fftw_complex *global_out = nullptr;
    
    if (rank == 0) {
        global_in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N_total);
        global_out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N_total);
        
        double fs = 1000.0; 
        int M = 7;          
        double fin = (static_cast<double>(M) / N_total) * fs;

        for (int i = 0; i < N_total; ++i) {
            double t = static_cast<double>(i) / fs;
            global_in[i][0] = std::sin(2.0 * PI * fin * t); 
            global_in[i][1] = 0.0;                                
        }
    }

    fftw_complex *local_in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * elements_per_proc);
    fftw_complex *local_out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * elements_per_proc);

    MPI_Scatter(global_in, elements_per_proc * 2, MPI_DOUBLE, 
                local_in, elements_per_proc * 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    fftw_plan plan_fwd = fftw_plan_dft_1d(dim, local_in, local_out, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_plan plan_bwd = fftw_plan_dft_1d(dim, local_out, local_in, FFTW_BACKWARD, FFTW_ESTIMATE);

    MPI_Barrier(MPI_COMM_WORLD); 
    auto start_time = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < rows_per_proc; i++) {
        fftw_execute_dft(plan_fwd, &local_in[i * dim], &local_out[i * dim]); 
        fftw_execute_dft(plan_bwd, &local_out[i * dim], &local_in[i * dim]); 
    }

    MPI_Barrier(MPI_COMM_WORLD); 
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> comp_time = end_time - start_time;

    MPI_Gather(local_in, elements_per_proc * 2, MPI_DOUBLE, 
               global_out, elements_per_proc * 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        std::cout << "MPI FFT + IFFT (" << dim << "x" << dim << "): " << comp_time.count() << " us" << std::endl;
        fftw_free(global_in);
        fftw_free(global_out);
    }

    fftw_destroy_plan(plan_fwd);
    fftw_destroy_plan(plan_bwd);
    fftw_free(local_in);
    fftw_free(local_out);

    MPI_Finalize();
    return 0;
}