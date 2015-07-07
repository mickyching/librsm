
#ifndef  quadric_h_INC
#define  quadric_h_INC
#include <ostream>                              // os <<
#include "vector.h"                             // add_plane
#include "vertex.h"

class Quadric
{
    friend Quadric operator+(const Quadric& lhs, const Quadric& rhs);
    friend Quadric operator-(const Quadric& lhs, const Quadric& rhs);
    friend std::ostream& operator<<(std::ostream& out, const Quadric& q);
public:
    Quadric(gdouble aa, gdouble ab, gdouble ac, gdouble ad,
            gdouble bb, gdouble bc, gdouble bd, gdouble cc,
            gdouble cd, gdouble dd);
    Quadric();
    Quadric(const Quadric& q);
    ~Quadric(){};
    Quadric& operator=(const Quadric& q);
    Quadric& operator+=(const Quadric& q);
    Quadric& operator-=(const Quadric& q);
    Quadric& operator*=(gdouble m);
    Quadric  operator*(gdouble m) const;

    Quadric& add_plane(const Vector& abc, gdouble d);

    bool get_min_vector(Vector& vec) const;
    gdouble value(gdouble x, gdouble y, gdouble z) const;
    gdouble value(const Vector& v) const;

private:
    gdouble _aa, _ab, _ac, _ad, _bb, _bc, _bd, _cc, _cd, _dd;
}; // -----  end of class Quadric  -----


inline Quadric operator+(const Quadric& lhs, const Quadric& rhs)
{
    return Quadric(
            lhs._aa + rhs._aa, lhs._ab + rhs._ab,
            lhs._ac + rhs._ac, lhs._ad + rhs._ad,
            lhs._bb + rhs._bb, lhs._bc + rhs._bc,
            lhs._bd + rhs._bd, lhs._cc + rhs._cc,
            lhs._cd + rhs._cd, lhs._dd + rhs._dd);
}


inline Quadric operator-(const Quadric& lhs, const Quadric& rhs)
{
    return Quadric(
            lhs._aa - rhs._aa, lhs._ab - rhs._ab,
            lhs._ac - rhs._ac, lhs._ad - rhs._ad,
            lhs._bb - rhs._bb, lhs._bc - rhs._bc,
            lhs._bd - rhs._bd, lhs._cc - rhs._cc,
            lhs._cd - rhs._cd, lhs._dd - rhs._dd);
}


inline Quadric operator*(gdouble m, const Quadric& q)
{
    return q * m;
}


inline std::ostream& operator<<(std::ostream& out, const Quadric& q)
{
    return out << "[[" << q._aa << ", " << q._ab << ", " << q._ac
        << ", " << q._ad << "]\n"
        << " [" << q._ab << ", " << q._bb << ", " << q._bc << ", " << q._bd
        << "]\n"
        << " [" << q._ac << ", " << q._bc << ", " << q._cc << ", " << q._cd
        << "]\n"
        << " [" << q._ad << ", " << q._bd << ", " << q._cd << ", " << q._dd
        << "]]\n";
}


#endif   // ----- #ifndef quadric_h_INC  -----
