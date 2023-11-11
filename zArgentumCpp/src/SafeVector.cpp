#ifndef __SAFE_VEC_H__
#define __SAFE_VEC_H__
#include <godot_cpp/variant/vector2i.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <cstdio>
#include <algorithm>

namespace godot{

// poner 'sv' para escribirlo rápido
// A vector for error-prone humans
struct SafeVec
{
    int lef;
    int RIGHT;

    SafeVec() {}
	SafeVec(const short int i, const short int j) {this->lef = i; this->RIGHT = j;}

    SafeVec(const godot::Vector2i& vector2i) 
    {
        lef = vector2i.x;
        RIGHT = vector2i.y;
    }

    bool operator==(const SafeVec &oSafeVec) const
    {return lef == oSafeVec.lef && RIGHT == oSafeVec.RIGHT;}
    bool operator!=(const SafeVec &oSafeVec) const {return !operator==(oSafeVec);}

    void operator+=(const SafeVec &oSafeVec) 
    {lef += oSafeVec.lef; RIGHT += oSafeVec.RIGHT;}
    void operator-=(const SafeVec &oSafeVec) 
    {lef -= oSafeVec.lef; RIGHT -= oSafeVec.RIGHT;}
    void operator*=(const SafeVec &oSafeVec) 
    {lef *= oSafeVec.lef; RIGHT *= oSafeVec.RIGHT;}
    void operator/=(const SafeVec &oSafeVec) 
    {lef /= oSafeVec.lef; RIGHT /= oSafeVec.RIGHT;}


    void addAssign_lef(const SafeVec &oSafeVec){lef += oSafeVec.lef;}
    void addAssignRIGHT(const SafeVec &oSafeVec){RIGHT += oSafeVec.RIGHT;}

    SafeVec add_lef(const int i){return SafeVec(this->lef + i, RIGHT);}
    SafeVec addRight(const int j){return SafeVec(lef, this->RIGHT + j);}

    SafeVec add_lef(const SafeVec &oSafeVec){return SafeVec(lef + oSafeVec.lef, RIGHT);}
    SafeVec addRIGHT(const SafeVec &oSafeVec){return SafeVec(lef, RIGHT + oSafeVec.RIGHT);}

    char compare_lef(const SafeVec &oSafeVec)
    {
        if (lef == oSafeVec.lef) 
            return 0;
        else if (lef < oSafeVec.lef)
            return -1;
        else 
            return 1;
    }
    char compareRIGHT(const SafeVec &oSafeVec)
    {
        if (RIGHT == oSafeVec.RIGHT) 
            return 0;
        else if (RIGHT < oSafeVec.RIGHT)
            return -1;
        else 
            return 1;
    }

    operator godot::Vector2i() const {return godot::Vector2i(lef, RIGHT);}
    operator godot::Vector2() const {return godot::Vector2(lef, RIGHT);}

    SafeVec operator+(const SafeVec &oSafeVec) const
    {return SafeVec(lef + oSafeVec.lef, RIGHT + oSafeVec.RIGHT);}
    SafeVec operator-(const SafeVec &oSafeVec) const
    {return SafeVec(lef - oSafeVec.lef, RIGHT - oSafeVec.RIGHT);}

    SafeVec operator-()const
    {return SafeVec(-lef, -RIGHT);}

    SafeVec operator*(const SafeVec &oSafeVec) const
    {return SafeVec(lef * oSafeVec.lef, RIGHT * oSafeVec.RIGHT);}
    SafeVec operator/(const SafeVec &oSafeVec) const
    {return SafeVec(lef / oSafeVec.lef, RIGHT / oSafeVec.RIGHT);}

    bool operator<(const SafeVec &oSafeVec) const { return (lef == oSafeVec.lef) ? (RIGHT < oSafeVec.RIGHT) : (lef < oSafeVec.lef); }
	bool operator>(const SafeVec &oSafeVec) const { return (lef == oSafeVec.lef) ? (RIGHT > oSafeVec.RIGHT) : (lef > oSafeVec.lef); }

	bool operator<=(const SafeVec &oSafeVec) const { return lef == oSafeVec.lef ? (RIGHT <= oSafeVec.RIGHT) : (lef < oSafeVec.lef); }
	bool operator>=(const SafeVec &oSafeVec) const { return lef == oSafeVec.lef ? (RIGHT >= oSafeVec.RIGHT) : (lef > oSafeVec.lef); }

    SafeVec operator*(float number) const
    {return SafeVec(lef * number, RIGHT * number);}
    SafeVec operator/(float number) const
    {return SafeVec(lef / number, RIGHT / number);}

    const char* c_str() const
    {
        static char buffer[28]; 
        snprintf(buffer, sizeof(buffer), "(%hu, %hu)", lef, RIGHT);
        return buffer;
    }

    float length() const{return distanceTo(SafeVec(0,0));}

    float distanceTo(const SafeVec &oSafeVec) const 
    {return std::sqrt((lef-oSafeVec.lef)*(lef-oSafeVec.lef) + (RIGHT-oSafeVec.RIGHT)*(RIGHT-oSafeVec.RIGHT));}

    float distanceSquaredTo(const SafeVec &oSafeVec) const 
    {return (lef - oSafeVec.lef)*(lef - oSafeVec.lef) + (RIGHT - oSafeVec.RIGHT)*(RIGHT - oSafeVec.RIGHT);}

    float distanceSquaredTo(const godot::Vector2i &oVector2i) const 
    {return (lef - oVector2i.x)*(lef - oVector2i.x) + (RIGHT - oVector2i.y)*(RIGHT - oVector2i.y);}

    struct hash
    {
        size_t operator()( const SafeVec &vec ) const
        {return vec.lef*31 + vec.RIGHT ;}
    };
};
}
#endif