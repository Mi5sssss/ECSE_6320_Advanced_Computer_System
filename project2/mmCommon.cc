#include <iostream>

#include <fstream>

#include <chrono>
 // g++ -o mmCommon mmCommon.cc

// Matrix structure
struct Matrix {
    int rows, cols;
    float * data;
};

// Read matrix from txt file
Matrix readMatrix(const char * filename) {
    std::ifstream file(filename);
    Matrix m;
    file >> m.rows >> m.cols;
    m.data = new float[m.rows * m.cols];
    for (int i = 0; i < m.rows * m.cols; i++) {
        file >> m.data[i];
    }
    return m;
}

// Write matrix to txt file
void writeMatrix(const Matrix & m,
    const char * filename) {
    std::ofstream file(filename);
    file << m.rows << " " << m.cols << std::endl;
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            file << m.data[i * m.cols + j] << " ";
        }
        file << std::endl;
    }
}

// Matrix-matrix multiplication
Matrix mulMatrix(const Matrix & a,
    const Matrix & b) {
    Matrix c;
    c.rows = a.rows;
    c.cols = b.cols;
    c.data = new float[c.rows * c.cols];

    for (int i = 0; i < c.rows; i++) {
        for (int j = 0; j < c.cols; j++) {
            float sum = 0.0f;
            for (int k = 0; k < a.cols; k++) {
                sum += a.data[i * a.cols + k] * b.data[k * b.cols + j];
            }
            c.data[i * c.cols + j] = sum;
        }
    }

    return c;
}

int main() {
    Matrix a = readMatrix("matrixA.txt");
    Matrix b = readMatrix("matrixB.txt");

    auto start = std::chrono::high_resolution_clock::now();
    Matrix c = mulMatrix(a, b);
    auto end = std::chrono::high_resolution_clock::now();

    writeMatrix(c, "matrixC.txt");
    auto duration = std::chrono::duration_cast < std::chrono::milliseconds > (end - start).count();
    std::cout << "Duration: " << duration << " milliseconds" << std::endl;

    return 0;
}