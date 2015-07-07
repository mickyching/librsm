
#ifndef  vertex_INC
#define  vertex_INC

#include <gts.h>
#include <ostream>                              // os <<
#include <cmath>                                // sqrt
#include "vector.h"

typedef GtsVertexClass VertexClass;
class Edge;
class Vertex
{
public:
    static const gdouble EPSILON;

public:
    const GtsPoint* p() const {return GTS_POINT(this);}
    GtsPoint* p() {return GTS_POINT(this);}

    Vertex& move(const Vector& mv);
    Vertex& scale(const Vector& sv);
    Vertex& rotate(const Vector& rv, gdouble angle);
    Vertex& transform(const GtsMatrix* tm);

    bool is_touch(const Vertex& v) const;
    bool is_touchx(const Vertex& v) const;
    bool is_touchy(const Vertex& v) const;
    bool is_touchz(const Vertex& v) const;
    bool is_belong(const Edge& e) const;

    gdouble len2(const Vertex& v) const;
    gdouble len(const Vertex& v) const {return sqrt(len2(v));}

    void replace(Vertex& v);
    void enlarge_bbox(GtsBBox& bbox);

    GSList* edges() const {return GTS_VERTEX(this)->segments;}
    void edges_prepend(Edge* edge);
    void edges_free()
    {
        g_slist_free(GTS_VERTEX(this)->segments);
        GTS_VERTEX(this)->segments = NULL;
    }

    GSList* faces() const;

    GtsVertex parent;
    gpointer reserved;
};
VertexClass*  vertex_class();
#define IS_VERTEX(obj) (gts_object_is_from_class(obj, vertex_class()))
#define VERTEX(obj) GTS_OBJECT_CAST(obj, Vertex, vertex_class())
#define VERTEX_CLASS(obj) GTS_OBJECT_CLASS_CAST(obj, \
        VertexClass, vertex_class())


inline Vertex* vertex_new(VertexClass* vc, gdouble x, gdouble y, gdouble z)
{
    return VERTEX(gts_vertex_new(vc, x, y, z));
}


inline std::ostream& operator<<(std::ostream& os, const Vertex& v)
{
    GtsPoint* p1 = GTS_POINT(&v);
    return os << p1->x << " " << p1->y << " " << p1->z;
}


inline std::istream& operator>>(std::istream& is, Vertex& v)
{
    GtsPoint* p1 = GTS_POINT(&v);
    return is >> p1->x >> p1->y >> p1->z;
}


inline Vertex& Vertex::move(const Vector& mv)
{
    GtsPoint* p1 = GTS_POINT(this);
    p1->x += mv.x; p1->y += mv.y; p1->z += mv.z;
    return *this;
}


inline Vertex& Vertex::scale(const Vector& sv)
{
    GtsPoint* p1 = GTS_POINT(this);
    p1->x *= sv.x; p1->y *= sv.y; p1->z *= sv.z;
    return *this;
}


inline bool Vertex::is_touch(const Vertex& v) const
{
    GtsPoint* p1 = GTS_POINT(this);
    GtsPoint* p2 = GTS_POINT(&v);
    return ABS(p1->x - p2->x) + ABS(p1->y - p2->y) +
        ABS(p1->z - p2->z) < EPSILON;
}


inline bool Vertex::is_touchx(const Vertex& v) const
{
    GtsPoint* p1 = GTS_POINT(this);
    GtsPoint* p2 = GTS_POINT(&v);
    return ABS(p1->x - p2->x) < EPSILON;
}


inline bool Vertex::is_touchy(const Vertex& v) const
{
    GtsPoint* p1 = GTS_POINT(this);
    GtsPoint* p2 = GTS_POINT(&v);
    return ABS(p1->y - p2->y) < EPSILON;
}


inline bool Vertex::is_touchz(const Vertex& v) const
{
    GtsPoint* p1 = GTS_POINT(this);
    GtsPoint* p2 = GTS_POINT(&v);
    return ABS(p1->z - p2->z) < EPSILON;
}


inline gdouble Vertex::len2(const Vertex& v) const
{
    GtsPoint* p1 = GTS_POINT(this);
    GtsPoint* p2 = GTS_POINT(&v);
    gdouble lx = p1->x - p2->x;
    gdouble ly = p1->y - p2->y;
    gdouble lz = p1->z - p2->z;
    return lx*lx + ly*ly + lz*lz;
}


inline void Vertex::replace(Vertex& v)
{
    gts_vertex_replace(GTS_VERTEX(&v), GTS_VERTEX(this));
}


inline void Vertex::enlarge_bbox(GtsBBox& bbox)
{
    GtsPoint* p1 = GTS_POINT(this);
    if (p1->x < bbox.x1) bbox.x1 = p1->x;
    if (p1->x > bbox.x2) bbox.x2 = p1->x;
    if (p1->y < bbox.y1) bbox.y1 = p1->y;
    if (p1->y > bbox.y2) bbox.y2 = p1->y;
    if (p1->z < bbox.z1) bbox.z1 = p1->z;
    if (p1->z > bbox.z2) bbox.z2 = p1->z;
}

#endif   // ----- #ifndef vertex_INC  -----
