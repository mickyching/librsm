
#ifndef  vector_INC
#define  vector_INC
#include <iostream>                             // os
#include <cmath>                                // sqrt
#include <gts.h>

class Vector
{
public:
    Vector(gdouble vx, gdouble vy, gdouble vz): x(vx), y(vy), z(vz) {}
    Vector(): x(0.0), y(0.0), z(0.0) {}
    Vector(const Vector& v): x(v.x), y(v.y), z(v.z) {}
    ~Vector(){}

    Vector& operator =(const Vector& v);
    Vector& operator+=(const Vector& v);
    Vector& operator-=(const Vector& v);
    Vector& operator*=(gdouble d);
    Vector& operator/=(gdouble d);

    Vector operator*(gdouble m) const {return Vector(m*x, m*y, m*z);}
    Vector operator/(gdouble m) const;

    Vector& set(gdouble sx, gdouble sy, gdouble sz);

    gdouble len() const {return sqrt(len2());}
    gdouble len2() const {return x*x + y*y + z*z;}

    Vector cross(const Vector& v) const;

    Vector& uniform();
    Vector& scale(const gdouble s);

    gdouble x, y, z;
};


inline Vector& Vector::operator=(const Vector& v)
{
    x = v.x; y = v.y; z = v.z;
    return *this;
}


inline Vector& Vector::operator+=(const Vector& v)
{
    x += v.x; y += v.y; z += v.z;
    return *this;
}


inline Vector& Vector::operator-=(const Vector& v)
{
    x -= v.x; y -= v.y; z -= v.z;
    return *this;
}


inline Vector& Vector::operator*=(gdouble m)
{
    x *= m; y *= m; z *= m;
    return *this;
}


inline Vector& Vector::operator/=(gdouble m)
{
    x /= m; y /= m; z /= m;
    return *this;
}


inline Vector operator+(const Vector& lhs, const Vector& rhs)
{
    return Vector(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}


inline Vector operator-(const Vector& lhs, const Vector& rhs)
{
    return Vector(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}


inline Vector operator*(gdouble m, const Vector& vec)
{
    return vec * m;
}


inline gdouble operator*(const Vector& lhs, const Vector& rhs)
{
    return lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
}


inline std::ostream& operator<<(std::ostream& os, const Vector& v)
{
    return os << v.x << " " << v.y << " " << v.z;
}


inline std::istream& operator>>(std::istream& is, Vector& v)
{
    return is >> v.x >> v.y >> v.z;
}


inline Vector Vector::operator/(gdouble m) const
{
    return Vector(x/m, y/m, z/m);
}


inline Vector& Vector::set(gdouble sx, gdouble sy, gdouble sz)
{
    x = sx; y = sy; z = sz;
}

inline Vector Vector::cross(const Vector& v) const
{
    return Vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
}


inline Vector& Vector::uniform()
{
    gdouble u = len();
    if (u == 0.0) return *this;
    x /= u; y /= u; z /= u;
    return *this;
}


inline Vector& Vector::scale(const gdouble s)
{
    x *= s; y *= s; z *= s;
    return *this;
}


#endif   // ----- #ifndef vector_INC  -----
