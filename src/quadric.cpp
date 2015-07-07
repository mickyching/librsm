// =====================================================================================
//
//       Filename:  quadric.cpp
//
//    Description:
//
//        Version:  1.0
//        Created:  11/27/2012 04:26:52 PM
//       Revision:  none
//       Compiler:  gcc
//
//         Author:  Micky Ching (MC), mickyching@gmail.com
//   Organization:
//
// =====================================================================================

#include <ostream>
#include "vector.h"
#include "quadric.h"

Quadric::Quadric(gdouble aa, gdouble ab, gdouble ac, gdouble ad,
        gdouble bb, gdouble bc, gdouble bd, gdouble cc,
        gdouble cd, gdouble dd)
:   _aa(aa), _ab(ab), _ac(ac), _ad(ad),
    _bb(bb), _bc(bc), _bd(bd), _cc(cc), _cd(cd), _dd(dd)
{}


Quadric::Quadric()
:   _aa(0.), _ab(0.), _ac(0.), _ad(0.),
    _bb(0.), _bc(0.), _bd(0.), _cc(0.), _cd(0.), _dd(0.)
{}


Quadric::Quadric(const Quadric& q)
:   _aa(q._aa), _ab(q._ab), _ac(q._ac), _ad(q._ad),
    _bb(q._bb), _bc(q._bc), _bd(q._bd), _cc(q._cc), _cd(q._cd), _dd(q._dd)
{}


Quadric& Quadric::operator=(const Quadric& q)
{
    _aa = q._aa; _ab = q._ab; _ac = q._ac; _ad = q._ad;
    _bb = q._bb; _bc = q._bc; _bd = q._bd; _cc = q._cc;
    _cd = q._cd; _dd = q._dd;
    return *this;
}


bool Quadric::get_min_vector(Vector& vec) const
{
    gdouble det = _aa*_bb*_cc + 2.0*_ab*_bc*_ac;
    det -= _ac*_bb*_ac + _ab*_ab*_cc + _aa*_bc*_bc;
    if (det == 0.0) return false;
    Vector xvec(_bb*_cc-_bc*_bc, _bc*_ac-_ab*_cc, _ab*_bc-_bb*_ac);
    Vector yvec(_bc*_ac-_ab*_cc, _cc*_aa-_ac*_ac, _ac*_ab-_aa*_bc);
    Vector zvec(_ab*_bc-_ac*_bb, _ac*_ab-_aa*_bc, _aa*_bb-_ab*_ab);
    Vector dvec(-_ad / det, -_bd / det, -_cd / det);
    vec.x = xvec*dvec;
    vec.y = yvec*dvec;
    vec.z = zvec*dvec;
    return true;
}


Quadric& Quadric::operator+=(const Quadric& q)
{
    _aa += q._aa; _ab += q._ab; _ac += q._ac; _ad += q._ad;
    _bb += q._bb; _bc += q._bc; _bd += q._bd;
    _cc += q._cc; _cd += q._cd;
    _dd += q._dd;
    return *this;
}


Quadric& Quadric::operator-=(const Quadric& q)
{
    _aa -= q._aa; _ab -= q._ab; _ac -= q._ac; _ad -= q._ad;
    _bb -= q._bb; _bc -= q._bc; _bd -= q._bd;
    _cc -= q._cc; _cd -= q._cd;
    _dd -= q._dd;
    return *this;
}


Quadric& Quadric::operator*=(gdouble m)
{
    _aa *= m; _ab *= m; _ac *= m; _ad *= m;
    _bb *= m; _bc *= m; _bd *= m;
    _cc *= m; _cd *= m;
    _dd *= m;
    return *this;
}


Quadric Quadric::operator*(gdouble m) const
{
    return Quadric(_aa * m, _ab * m, _ac * m, _ad * m,
            _bb * m, _bc * m, _bd * m, _cc * m, _cd * m, _dd * m);
}


Quadric& Quadric::add_plane(const Vector& nv, gdouble nd)
{
    _aa += nv.x*nv.x; _ab += nv.x*nv.y; _ac += nv.x*nv.z; _ad += nv.x*nd;
    _bb += nv.y*nv.y; _bc += nv.y*nv.z; _bd += nv.y*nd;
    _cc += nv.z*nv.z; _cd += nv.z*nd;
    _dd += nd*nd;
    return *this;
}


gdouble Quadric::value(const Vector& v) const
{
    return value(v.x, v.y, v.z);
}


gdouble Quadric::value(gdouble x, gdouble y, gdouble z) const
{
    return (x * (_aa*x + _ab*y + _ac*z + _ad)
          + y * (_ab*x + _bb*y + _bc*z + _bd)
          + z * (_ac*x + _bc*y + _cc*z + _cd)
          + _ad*x + _bd*y + _cd*z + _dd);
}



