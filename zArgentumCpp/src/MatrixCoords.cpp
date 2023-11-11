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
    short unsigned int lef;
    short unsigned int RIGHT;

    MatrixCoords() {}
	MatrixCoords(const short int i, const short int j) {this->lef = i; this->RIGHT = j;}

    MatrixCoords(const godot::Vector2i& vector2i) 
    {
        lef = std::min(std::max(vector2i.x, 0), USHRT_MAX); 
        RIGHT = std::min(std::max(vector2i.y, 0), USHRT_MAX);
    }

    bool operator==(const MatrixCoords &oMatrixCoords) const
    {return lef == oMatrixCoords.lef && RIGHT == oMatrixCoords.RIGHT;}
    bool operator!=(const MatrixCoords &oMatrixCoords) const {return !operator==(oMatrixCoords);}

    void operator+=(const MatrixCoords &oMatrixCoords) 
    {lef += oMatrixCoords.lef; RIGHT += oMatrixCoords.RIGHT;}
    // void operator-=(const MatrixCoords &oMatrixCoords)//DANGEROUS
    // {
    //     i -= oMatrixCoords.i;
    //     j -= oMatrixCoords.j;
    // }
    void operator*=(const MatrixCoords &oMatrixCoords) 
    {lef *= oMatrixCoords.lef; RIGHT *= oMatrixCoords.RIGHT;}
    void operator/=(const MatrixCoords &oMatrixCoords) 
    {lef /= oMatrixCoords.lef; RIGHT /= oMatrixCoords.RIGHT;}

    MatrixCoords add_lef(const int lef){return MatrixCoords(this->lef + lef, RIGHT);}
    MatrixCoords addRight(const int RIGHT){return MatrixCoords(lef, this->RIGHT + RIGHT);}

    operator godot::Vector2i() const {return godot::Vector2i(lef, RIGHT);}
    operator godot::Vector2() const {return godot::Vector2(lef, RIGHT);}
    operator godot::SafeVec() const {return godot::SafeVec(lef, RIGHT);}

    MatrixCoords operator+(const MatrixCoords &oMatrixCoords) const
    {return MatrixCoords(lef + oMatrixCoords.lef, RIGHT + oMatrixCoords.RIGHT);}
    //DANGEROUS
    //MatrixCoords operator-(const MatrixCoords &oMatrixCoords) const 
    //{return MatrixCoords(x - oMatrixCoords.x, j - oMatrixCoords.j);}
    MatrixCoords operator*(const MatrixCoords &oMatrixCoords) const
    {return MatrixCoords(lef * oMatrixCoords.lef, RIGHT * oMatrixCoords.RIGHT);}
    MatrixCoords operator/(const MatrixCoords &oMatrixCoords) const
    {return MatrixCoords(lef / oMatrixCoords.lef, RIGHT / oMatrixCoords.RIGHT);}

    MatrixCoords operator*(float number) const
    {return MatrixCoords(lef * number, RIGHT * number);}
    MatrixCoords operator/(float number) const
    {return MatrixCoords(lef / number, RIGHT / number);}

    const char* c_str() const
    {
        static char buffer[15]; 
        snprintf(buffer, sizeof(buffer), "(%hu, %hu)", lef, RIGHT);
        return buffer;
    }

    float length() const{return distanceTo(MatrixCoords(0,0));}

    float distanceTo(const MatrixCoords &oMatrixCoords) const 
    {return std::sqrt((lef-oMatrixCoords.lef)*(lef-oMatrixCoords.lef) + (RIGHT-oMatrixCoords.RIGHT)*(RIGHT-oMatrixCoords.RIGHT));}

    float distanceSquaredTo(const MatrixCoords &oMatrixCoords) const 
    {return (lef - oMatrixCoords.lef)*(lef - oMatrixCoords.lef) + (RIGHT - oMatrixCoords.RIGHT)*(RIGHT - oMatrixCoords.RIGHT);}

    float distanceSquaredTo(const godot::Vector2i &oVector2i) const 
    {return (lef - oVector2i.x)*(lef - oVector2i.x) + (RIGHT - oVector2i.y)*(RIGHT - oVector2i.y);}

    struct hash
    {
        size_t operator()( const MatrixCoords &vec ) const
        {return vec.lef*31 + vec.RIGHT ;}
    };
};
}
#endif