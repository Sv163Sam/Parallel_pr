#include <ctime>
#include "add.hpp"
#include "omp.h"

struct mtrx
{
    int** matrix;
    int row_size = 0;
    int column_size = 0;
};

void _print_matrix_(mtrx obj)
{
    std::cout << "Matrix from file" << std::endl;
    std::cout << "Amount of rows - " << obj.row_size << std::endl;
    std::cout << "Amount of columns - " << obj.column_size << std::endl;
    std::cout << "Matrix: " << std::endl;
    for (size_t i = 0; i < obj.row_size; i++)
    {
        std::cout << std::endl;
        for(size_t j = 0; j < obj.column_size; j++)
        {
            std::cout << obj.matrix[i][j] << "\t" << std::endl;
        }
    }
    return;
}

mtrx _initialize_matrix_(const char* path, const char* mode)
{
    mtrx obj;
    int row = 0, column = 0;
    FILE *in = fopen(path, mode);
    
    fscanf(in, "%d", &row);
    fscanf(in, "%d", &column);
    
    int** first_m = new int* [row];
    for(size_t i = 0; i < row; i++)
    {
        first_m[i] = new int [column];
        for(size_t j = 0; j < column; j++)
        {
            fscanf(in, "%d", &first_m[i][j]);
        }
    }
    obj.matrix = first_m;
    obj.row_size = row;
    obj.column_size = column;
    fclose(in);
    return obj;
}

mtrx _multi_matrix_(mtrx matrix_f, mtrx matrix_s)
{
    mtrx obj;
    if (matrix_f.column_size != matrix_s.row_size) throw std::length_error("Multiplication is impossible");
    int** res_matrix = new int* [matrix_f.row_size];
    
    int threads;
        omp_set_num_threads(8);
    #pragma omp parallel shared(threads)
    {
        threads = omp_get_num_threads();
#pragma omp for
        for (size_t i = 0; i < matrix_f.row_size; i++)
        {
            res_matrix[i] = new int[matrix_s.column_size];
            for (size_t j = 0; j < matrix_s.column_size; j++)
            {
                res_matrix[i][j] = 0;
                for (size_t k = 0; k < matrix_f.column_size; k++)
                {
                    res_matrix[i][j] += matrix_f.matrix[i][k] * matrix_s.matrix[k][j];
                }
            }
        }
    }
    obj.matrix = res_matrix;
    obj.row_size = matrix_f.row_size;
    obj.column_size = matrix_s.column_size;
    return obj;
}

void _write_file_matrix_(mtrx obj, const char* path, const char* mode, clock_t _time)
{
    FILE* out = fopen(path, mode);
    for (size_t i = 0; i < obj.row_size; i++)
    {
        for(size_t j = 0; j < obj.column_size; j++)
        {
            fprintf(out, "%d", obj.matrix[i][j]);
            fprintf(out, "\t");
        }
        fprintf(out, "\n");
    }
    fprintf(out, "\nRuntime: %lf ms\n", (float)_time/1000);
    fprintf(out, "Amount of rows: %d\n", obj.row_size);
    fprintf(out, "Amount of columns: %d", obj.column_size);
    fclose(out);
    return;
}

int main()
{
    const char* path_f = "/Users/vladimirskobcov/Desktop/lab/lab_parallel_prog/lab_2/lab_2_pp/lab_2_pp/input_f.txt";
    const char* path_s = "/Users/vladimirskobcov/Desktop/lab/lab_parallel_prog/lab_2/lab_2_pp/lab_2_pp/input_s.txt";
    const char* r_m = "rt";
    
    const char* e_path = "/Users/vladimirskobcov/Desktop/lab/lab_parallel_prog/lab_2/lab_2_pp/lab_2_pp/output.txt";
    const char* w_m = "wt";

    mtrx first_mtrx;
    mtrx second_mtrx;
    mtrx res_matrix;
    
    _create_file();
    
    first_mtrx = _initialize_matrix_(path_f, r_m);
    second_mtrx = _initialize_matrix_(path_s, r_m);
    
    try
    {
    clock_t start_time = clock();
    res_matrix = _multi_matrix_(first_mtrx, second_mtrx);
    std::cout << "C++ calculations: " << std::endl;
    for (size_t i = 0; i < res_matrix.row_size; i++)
    {
        std::cout << std::endl;
        for (size_t j = 0; j < res_matrix.column_size; j++)
        {
            std::cout << res_matrix.matrix[i][j] << "\t";
        }
    }
    std::cout << std::endl;
    clock_t end_time = clock();
    clock_t time = end_time - start_time;
    std::cout << time << std::endl;
    _write_file_matrix_(res_matrix, e_path, w_m, time);
    }
    catch (const std::length_error &obj)
    {
        std::cout << "Emergency program crash: " << obj.what() << "\nPlease change the data in the file" << std::endl;
        exit(1);
    }
    std::cout << "\nPython check script\n" << std::endl;

    system("python3 /Users/vladimirskobcov/PycharmProjects/pythonProject5/main.py");
    return 0;
}
