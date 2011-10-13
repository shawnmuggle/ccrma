// STTransform3.cpp
#include "STTransform3.h"
#include "STVector3.h"

Matrix::Matrix(int aWidth, int aHeight)
{
    width = aWidth;
    height = aHeight;
    for (int y = 0; y < height; y++) {
        std::vector<float> row(width);
        elements.push_back(row);
    }
}

void Matrix::clear()
{
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            elements[y][x] = 0.0f;
        }
    }
}

void MatrixMultiply(const Matrix &left, const Matrix &right, Matrix &output)
{
    for (int y = 0; y < left.height; y++)  // for each column
    {
        printf("y: %d\n", y);
        for (int x = 0; x < right.width; x++)  // for each row
        {
            printf("x: %d\n", x);
            for (int k = 0; k < left.width; k++)
            {
                printf("k: %d\n", k);
                printf("Left element at [%d][%d]: %f\n", y, k, left.elements[y][k]);
                printf("Right element at [%d][%d]: %f\n", k, x, right.elements[k][x]);
                // Sum of the pairwise combinations of the same row from the left matrix and the same column from the right matrix
                output.elements[y][x] += left.elements[y][k] * right.elements[k][x];
            }
            printf("Output element at %d, %d: %f\n", y, x, output.elements[y][x]);
        }
    }    
}

STTransform3::STTransform3() : matrix(3, 3)
{ 
    loadIdentity();
}

STTransform3::STTransform3(Matrix m) : matrix(m)
{
    
}

void STTransform3::loadIdentity()
{
    matrix.clear();
    matrix.elements[0][0] = 1.0f;
    matrix.elements[1][1] = 1.0f;
    matrix.elements[2][2] = 1.0f;
}

STTransform3 operator*(const STTransform3& left, const STTransform3 &right)
{
    STTransform3 newTransform;
    MatrixMultiply(left.matrix, right.matrix, newTransform.matrix);
    return newTransform;
}

STVector3 operator*(const STTransform3 &left, const STVector3 &right)
{
    Matrix inVector(1, 3);
    inVector.elements[0][0] = right.x;
    inVector.elements[1][0] = right.y;
    inVector.elements[2][0] = right.z;
    Matrix outVector(1, 3);
    MatrixMultiply(left.matrix, inVector, outVector);
    return STVector3(outVector.elements[0][0], outVector.elements[1][0], outVector.elements[2][0]);
}
