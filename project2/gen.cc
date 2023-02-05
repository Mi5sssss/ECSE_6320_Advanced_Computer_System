#include <fstream>
#include <iostream>
#include <random>
#include <string>

// Generate a random float between -1.0 and 1.0
float randomFloat() {
  static std::mt19937 rng(std::random_device{}());
  static std::uniform_real_distribution<float> dist(-1.0, 1.0);
  return dist(rng);
}

// Generate a random 2-byte fixed point value between -32767 and 32767
int16_t randomFixedPoint() {
  static std::mt19937 rng(std::random_device{}());
  static std::uniform_int_distribution<int16_t> dist(-32767, 32767);
  return dist(rng);
}

void generateMatrix(int rows, int cols, bool isFloat, const std::string &filename) {
  std::ofstream ofs(filename);
  // ofs << rows << " " << cols << " " << (isFloat ? "4" : "2") << " " << fractionalBits << std::endl;
  ofs << rows << " " << cols << " " << isFloat << std::endl;
  // ofs << rows << " " << cols << " " << fractionalBits << std::endl;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      if (isFloat) {
        ofs << randomFloat() << " ";
      } else {
        ofs << randomFixedPoint() << " ";
      }
    }
    ofs << std::endl;
  }
}

int main() {
  int rows, cols;
  bool isFloat;
  int fractionalBits;
  std::string filename;

  std::cout << "Enter the number of rows: ";
  std::cin >> rows;
  std::cout << "Enter the number of columns: ";
  std::cin >> cols;
  std::cout << "Is the matrix floating point (1/0)? ";
  std::cin >> isFloat;
  // std::cout << "Enter the number of fractional bits (for fixed point only): ";
  // std::cin >> fractionalBits;
//   std::cout << "Enter the filename: ";
//   std::cin >> filename;

//   generateMatrix(rows, cols, isFloat, fractionalBits, filename);
  generateMatrix(rows, cols, isFloat, "matrixA.txt");
  generateMatrix(rows, cols, isFloat, "matrixB.txt");
  return 0;
}
