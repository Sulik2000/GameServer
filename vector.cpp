#include "vector.h"
#include <cmath>

FVector::FVector(int x, int y)
{
    this->X = x;
    this->Y = y;
}

FVector FVector::operator+(FVector a) const
{
    return FVector(this->X + a.X, this->Y + a.Y);
}

float FVector::operator*(FVector a) const
{
    // a * b = ax*bx + ay*by = |a| * |b| * cos a
    return this->X * a.X + this->Y * a.Y;
}

void FVector::operator+=(const FVector& a)
{
    this->X += a.X;
    this->Y += a.Y;
}

FVector FVector::operator*(float a) const
{
    return FVector(this->X * a, this->Y * a);
}

float FVector::Size() const
{
    return sqrt(this->X^2 + this->Y^2);
}

QString FVector::GetString() const
{
    return QString("X: " + QString::number(X) + "; Y: " + QString::number(Y));
}
