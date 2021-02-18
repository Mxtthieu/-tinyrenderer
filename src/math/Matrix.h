#ifndef OPENCM_MATRIX_H
#define OPENCM_MATRIX_H

#include <vector>
#include <cassert>

class Matrix{
private:
    std::vector<std::vector<float>> matrix;
    int rows;
    int cols;

public:

    Matrix(int dim) : matrix(std::vector<std::vector<float> >(dim, std::vector<float>(dim, 0.f))), rows(dim), cols(dim) {
        for (int i=0; i<dim; i++) {
            for (int j=0; j<dim; j++) {
                matrix[i][j] = (i==j ? 1.f : 0.f);
            }
        }
    }

    Matrix(int r, int c) : matrix(std::vector<std::vector<float> >(r, std::vector<float>(c, 0.f))), rows(r), cols(c) { }

    std::vector<float>& operator[](const int i) {
        assert(i>=0 && i<rows);
        return matrix[i];
    }

    void set_col(size_t idx, Vec2f v) {
        for (size_t i=rows; i--; matrix[i][idx]=v[i]);
    }

    Matrix operator*(const Matrix& a) {
        assert(cols == a.rows);
        Matrix result(rows, a.cols);
        for (int i=0; i<rows; i++) {
            for (int j=0; j<a.cols; j++) {
                result.matrix[i][j] = 0.f;
                for (int k=0; k<cols; k++) {
                    result.matrix[i][j] += matrix[i][k]*a.matrix[k][j];
                }
            }
        }
        return result;
    }
};

#endif
