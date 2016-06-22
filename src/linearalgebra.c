#ifndef LINEARALGEBRA_C_INCLUDED
#define LINEARALGEBRA_C_INCLUDED
#include "linearalgebra.h"
#include <math.h>

Vector VectorFromComponents(float x, float y, float z)
{
    Vector a;
    a.X = x;
    a.Y = y;
    a.Z = z;
    return a;
}

Vector VectorNormalize(Vector A)
{
    float magnitude = sqrt(A.X * A.X + A.Y * A.Y + A.Z * A.Z);
    A.X /= magnitude;
    A.Y /= magnitude;
    A.Z /= magnitude;
    return A;
}

float VectorDotProduct(Vector A, Vector B)
{
    float result = 0.0;
    result += A.X * B.X;
    result += A.Y * B.Y;
    result += A.Z * B.Z;
    return result;
}

Vector VectorCrossProduct(Vector A, Vector B)
{
    Vector Result;
    Result.X = (A.Y * B.Z) - (A.Z * B.Y);
    Result.Y = (A.Z * B.X) - (A.X * B.Z);
    Result.Z = (A.X * B.Y) - (A.Y * B.X);
    return Result;
}

Vector VectorMultiply(Vector vector, float scalar)
{
    vector.X *= scalar;
    vector.Y *= scalar;
    vector.Z *= scalar;
    return vector;
}

Vector VectorAdd(Vector A, Vector B)
{
    Vector Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    Result.Z = A.Z + B.Z;
    return Result;
}

Vector VectorSubtract(Vector A, Vector B)
{
    Vector Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    Result.Z = A.Z - B.Z;
    return Result;
}

Matrix MatrixIdentity()
{
    Matrix matrix;

    matrix.Data[0] = 1;
    matrix.Data[4] = 0;
    matrix.Data[8] = 0;
    matrix.Data[12] = 0;

    matrix.Data[1] = 0;
    matrix.Data[5] = 1;
    matrix.Data[9] = 0;
    matrix.Data[13] = 0;

    matrix.Data[2] = 0;
    matrix.Data[6] = 0;
    matrix.Data[10] = 1;
    matrix.Data[14] = 0;

    matrix.Data[3] = 0;
    matrix.Data[7] = 0;
    matrix.Data[11] = 0;
    matrix.Data[15] = 1;

    return matrix;
}

Matrix MatrixTranspose(Matrix A)
{
    Matrix transpose;
    int row, col;
    for (row = 0; row < 4; row++)
        for (col = 0; col < 4; col++) {
            transpose.Data[4 * row + col] = A.Data[4 * col + row];
        }
    return transpose;
}

Matrix MatrixInverse(Matrix A)
{
    Matrix B = MatrixIdentity();

    // Doing this allows easy row swapping
    float* rows[4] = { A.Data, A.Data + 4, A.Data + 8, A.Data + 12 };
    float* inverseRows[4] = { B.Data, B.Data + 4, B.Data + 8, B.Data + 12 };

    // Perform Gaussian elimination
    int pivotRow, pivotCol;
    for (pivotRow = 0, pivotCol = 0; pivotRow < 3; pivotRow++, pivotCol++) {
        // Swap the row with the largest value in the pivot column into the pivot
        // row
        int maxRow = pivotRow;
        float maxValue = rows[pivotRow][pivotCol];
        int i;
        for (i = pivotRow + 1; i < 4; i++) {
            if (rows[i][pivotCol] > maxValue) {
                maxRow = i;
                maxValue = rows[i][pivotCol];
            }
        };
        float* temp = rows[pivotRow];
        rows[pivotRow] = rows[maxRow];
        rows[maxRow] = temp;
        temp = inverseRows[pivotRow];
        inverseRows[pivotRow] = inverseRows[maxRow];
        inverseRows[maxRow] = temp;
        // Loop over all rows below the pivot and eliminate the variable in the
        // pivot column
        int elimRow;
        for (elimRow = pivotRow + 1; elimRow < 4; elimRow++) {
            // The multiple of the pivot row to add to the elimination row in order to
            // eliminate the variable
            float multiplier = -rows[elimRow][pivotCol] / rows[pivotRow][pivotCol];
            // Loop over columns and perform elimination
            int elimCol;
            for (elimCol = 0; elimCol < 4; elimCol++) {
                rows[elimRow][elimCol] += multiplier * rows[pivotRow][elimCol];
                inverseRows[elimRow][elimCol] += multiplier * inverseRows[pivotRow][elimCol];
            }
        }
    }

    // Perform back substitution
    for (pivotRow = 3, pivotCol = 3; pivotRow >= 0; pivotRow--, pivotCol--) {
        // Make entry in pivot column 1
        float multiplier = 1.0 / rows[pivotRow][pivotCol];
        int col;
        for (col = 0; col < 4; col++) {
            rows[pivotRow][col] *= multiplier;
            inverseRows[pivotRow][col] *= multiplier;
        }
        // Substitute into rows above
        int elimRow;
        for (elimRow = pivotRow - 1; elimRow >= 0; elimRow--) {
            multiplier = -rows[elimRow][pivotCol];
            rows[elimRow][pivotCol] = 0.0;
            for (col = 0; col < 4; col++) {
                inverseRows[elimRow][col] += multiplier * inverseRows[pivotRow][col];
            }
        }
    }

    Matrix Inverse;
    int row, col, index;
    for (row = 0, index = 0; row < 4; row++)
        for (col = 0; col < 4; col++, index++) {
            Inverse.Data[index] = inverseRows[row][col];
        }
    return Inverse;
}

Matrix MatrixMultiply(Matrix A, Matrix B)
{
    Matrix result;
    int i, row, column;
    for (row = 0; row < 4; row++)
        for (column = 0; column < 4; column++) {
            result.Data[4 * row + column] = 0;
            for (i = 0; i < 4; i++)
                result.Data[4 * row + column] += A.Data[4 * row + i] * B.Data[4 * i + column];
        }
    return result;
}

Matrix TranslationMatrix(Vector displacement)
{
    Matrix matrix = MatrixIdentity();
    matrix.Data[3] -= displacement.X;
    matrix.Data[7] -= displacement.Y;
    matrix.Data[11] -= displacement.Z;
    return matrix;
}

Matrix MatrixFromEulerAngles(Vector rotation)
{
    Matrix rotate_x = { { 1.0, 0.0, 0.0, 0.0, 0.0, cos(rotation.X),
        -sin(rotation.X), 0.0, 0.0, sin(rotation.X),
        cos(rotation.X), 0.0, 0.0, 0.0, 0.0, 1.0 } };
    Matrix rotate_y = { { cos(rotation.Y), 0.0, sin(rotation.Y), 0.0, 0.0, 1.0, 0.0,
        0.0, -sin(rotation.Y), 0.0, cos(rotation.Y), 0.0, 0.0,
        0.0, 0.0, 1.0 } };
    Matrix rotate_z = { { cos(rotation.Z), -sin(rotation.Z), 0.0, 0.0,
        sin(rotation.Z), cos(rotation.Z), 0.0, 0.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 0.0, 0.0, 1.0 } };
    return MatrixMultiply(rotate_y, MatrixMultiply(rotate_x, rotate_z));
}

Matrix ProjectionMatrix(float left,
    float right,
    float bottom,
    float top,
    float near,
    float far)
{
    Matrix matrix;

    matrix.Data[0] = 2 * near / (right - left);
    matrix.Data[4] = 0;
    matrix.Data[8] = (right + left) / (right - left);
    matrix.Data[12] = 0;

    matrix.Data[1] = 0;
    matrix.Data[5] = 2 * near / (top - bottom);
    matrix.Data[9] = 2 * (top + bottom) / (top - bottom);
    matrix.Data[13] = 0;

    matrix.Data[2] = 0;
    matrix.Data[6] = 0;
    matrix.Data[10] = -(far + near) / (far - near);
    matrix.Data[14] = -2 * far * near / (far - near);

    matrix.Data[3] = 0;
    matrix.Data[7] = 0;
    matrix.Data[11] = -1;
    matrix.Data[15] = 0;

    return matrix;
}

#endif // LINEARALGEBRA_C_INCLUDED
