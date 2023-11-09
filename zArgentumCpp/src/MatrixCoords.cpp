#ifndef __MATRIX_COORDS_H__
#define __MATRIX_COORDS_H__
#include <godot_cpp/variant/vector2i.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <cstdio>
#include <algorithm>
#include <climits>
#include "SafeVector.cpp"

namespace godot{
// Small Unsigned Vector (cannot be negative)
//TODO EXPORTAR A GODOT
struct MatrixCoords
{
    short unsigned int i;
    short unsigned int j;

    MatrixCoords() {}
	MatrixCoords(const short int i, const short int j) {this->i = i; this->j = j;}

    MatrixCoords(const godot::Vector2i& vector2i) 
    {
        i = std::min(std::max(vector2i.x, 0), USHRT_MAX); 
        j = std::min(std::max(vector2i.y, 0), USHRT_MAX);
    }

    bool operator==(const MatrixCoords &oMatrixCoords) const
    {return i == oMatrixCoords.i && j == oMatrixCoords.j;}
    bool operator!=(const MatrixCoords &oMatrixCoords) const {return !operator==(oMatrixCoords);}

    void operator+=(const MatrixCoords &oMatrixCoords) 
    {i += oMatrixCoords.i; j += oMatrixCoords.j;}
    // void operator-=(const MatrixCoords &oMatrixCoords)//DANGEROUS
    // {
    //     i -= oMatrixCoords.i;
    //     j -= oMatrixCoords.j;
    // }
    void operator*=(const MatrixCoords &oMatrixCoords) 
    {i *= oMatrixCoords.i; j *= oMatrixCoords.j;}
    void operator/=(const MatrixCoords &oMatrixCoords) 
    {i /= oMatrixCoords.i; j /= oMatrixCoords.j;}

    operator godot::Vector2i() const {return godot::Vector2i(i, j);}
    operator godot::Vector2() const {return godot::Vector2(i, j);}
    operator godot::SafeVec() const {return godot::SafeVec(i, j);}

    MatrixCoords operator+(const MatrixCoords &oMatrixCoords) const
    {return MatrixCoords(i + oMatrixCoords.i, j + oMatrixCoords.j);}
    //DANGEROUS
    //MatrixCoords operator-(const MatrixCoords &oMatrixCoords) const 
    //{return MatrixCoords(x - oMatrixCoords.x, j - oMatrixCoords.j);}
    MatrixCoords operator*(const MatrixCoords &oMatrixCoords) const
    {return MatrixCoords(i * oMatrixCoords.i, j * oMatrixCoords.j);}
    MatrixCoords operator/(const MatrixCoords &oMatrixCoords) const
    {return MatrixCoords(i / oMatrixCoords.i, j / oMatrixCoords.j);}

    MatrixCoords operator*(float number) const
    {return MatrixCoords(i * number, j * number);}
    MatrixCoords operator/(float number) const
    {return MatrixCoords(i / number, j / number);}

    const char* c_str() const
    {
        static char buffer[15]; 
        snprintf(buffer, sizeof(buffer), "(%hu, %hu)", i, j);
        return buffer;
    }

    float length() const{return distanceTo(MatrixCoords(0,0));}

    float distanceTo(const MatrixCoords &oMatrixCoords) const 
    {return std::sqrt((i-oMatrixCoords.i)*(i-oMatrixCoords.i) + (j-oMatrixCoords.j)*(j-oMatrixCoords.j));}

    float distanceSquaredTo(const MatrixCoords &oMatrixCoords) const 
    {return (i - oMatrixCoords.i)*(i - oMatrixCoords.i) + (j - oMatrixCoords.j)*(j - oMatrixCoords.j);}

    float distanceSquaredTo(const godot::Vector2i &oVector2i) const 
    {return (i - oVector2i.x)*(i - oVector2i.x) + (j - oVector2i.y)*(j - oVector2i.y);}

    struct hash
    {
        size_t operator()( const MatrixCoords &vec ) const
        {return vec.i*31 + vec.j ;}
    };
};
}
#endif