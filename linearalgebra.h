#ifndef LINEARALGEBRA_H_INCLUDED
#define LINEARALGEBRA_H_INCLUDED

#ifndef MATH_H_INCLUDED
#define MATH_H_INCLUDED


typedef struct
{
float Data[16];
}
Matrix;

typedef struct
{
float X;
float Y;
float Z;
}Vector;

typedef struct
{
float W;
float I;
float J;
float K;
}Quaternion;

Quaternion QuaternionConjugate(Quaternion Quat);
Quaternion QuaternionMultiply(Quaternion A,Quaternion B);
Vector QuaternionTransformVector(Quaternion Quat,Vector Vec);
Matrix MatrixFromQuaternion(Quaternion Qtr);

Vector VectorFromComponents(float x,float y,float z);
Vector VectorMultiply(Vector vector,float scalar);
Vector VectorNormalize(Vector A);
Vector VectorAdd(Vector A,Vector B);
Vector VectorSubtract(Vector A,Vector B);
Vector VectorCrossProduct(Vector A,Vector B);
float VectorDotProduct(Vector A,Vector B);
Matrix MatrixIdentity();
Matrix MatrixTranspose(Matrix A);
Matrix MatrixInverse(Matrix A);
Matrix MatrixMultiply(Matrix A,Matrix B);

Matrix MatrixFromEulerAngles(Vector rotation);
Matrix TranslationMatrix(Vector displacement);
Matrix ProjectionMatrix(float left,float right,float bottom,float top,float near,float far);

#endif // MATH_H_INCLUDED


#endif // LINEARALGEBRA_H_INCLUDED
