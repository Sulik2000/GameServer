#include "vector.h"
#include <cmath>

FVector::FVector(float x, float y)
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
    float sq = sqrt((int)this->X * (int)this->X + (int)(this->Y) * (int)this->Y);
    return sq;
}

QString FVector::GetString() const
{
    return QString("X: " + QString::number(X) + "; Y: " + QString::number(Y));
}
