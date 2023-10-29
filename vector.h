#pragma once

#include <qstring.h>

class FVector
{
    public:
    float X, Y;

    FVector(float x = 0, float y = 0);

    // Sum two vectors
    FVector operator+(FVector a) const;
    // Dot product of two vectors
    float operator*(FVector a) const;
    // Appends to vector another vector
    void operator+=(const FVector& a);
    // Multiply vector on num
    FVector operator*(float a) const;
    // Get size of vector
    float Size() const;
    // Get information about vector in readable style
    QString GetString() const;
};