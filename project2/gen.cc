#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

struct Matrix {
  int rows, cols;
  float *data;
};

// Generates a random matrix with specified dimensions and precision
Matrix generateMatrix(int rows, int cols, bool isFloat) {
  Matrix m;
  m.rows = rows;
  m.cols = cols;
  m.data = new float[rows * cols];
  srand(time(NULL));
  for (int i = 0; i < rows * cols; i++) {
    if (isFloat) {
      m.data[i] = static_cast<float>(rand()) / RAND_MAX;
    } else {
      m.data[i] = rand() % 65536 / 65536.0f;
    }
  }
  return m;
}

// Writes a matrix to a txt file
void writeMatrix(const Matrix &m, const char *filename) {
  std::ofstream file(filename);
  file << m.rows << " " << m.cols << std::endl;
  for (int i = 0; i < m.rows * m.cols; i++) {
    file << m.data[i] << " ";
    if ((i + 1) % m.cols == 0) {
      file << std::endl;
    }
  }
}

// Read matrix from txt file
Matrix readMatrix(const char *filename) {
  std::ifstream file(filename);
  Matrix m;
  file >> m.rows >> m.cols;
  m.data = new float[m.rows * m.cols];
  for (int i = 0; i < m.rows * m.cols; i++) {
    file >> m.data[i];
  }
  return m;
}

int main() {
  int rows, cols;
  bool isFloat;
  std::cout << "Enter number of rows: ";
  std::cin >> rows;
  std::cout << "Enter number of columns: ";
  std::cin >> cols;
  std::cout << "Enter precision (0 for float, 1 for fixed-point): ";
  std::cin >> isFloat;
  Matrix a = generateMatrix(rows, cols, isFloat == 0);
  Matrix b = generateMatrix(rows, cols, isFloat == 0);
  writeMatrix(a, "matrixA.txt");
  writeMatrix(b, "matrixB.txt");
//   Matrix c = readMatrix("matrixC.txt");
  return 0;
}
