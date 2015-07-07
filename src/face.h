
#ifndef  face_INC
#define  face_INC

#include <gts.h>
#include "vector.h"
#include "vertex.h"
#include "edge.h"

typedef GtsFaceClass FaceClass;
class Face
{
public:
    const GtsTriangle* t() const {return GTS_TRIANGLE(this);}
    GtsTriangle* t() {return GTS_TRIANGLE(this);}
    const Edge* e1() const {return EDGE(GTS_TRIANGLE(this)->e1);}
    const Edge* e2() const {return EDGE(GTS_TRIANGLE(this)->e2);}
    const Edge* e3() const {return EDGE(GTS_TRIANGLE(this)->e3);}
    Edge* e1() {return EDGE(GTS_TRIANGLE(this)->e1);}
    Edge* e2() {return EDGE(GTS_TRIANGLE(this)->e2);}
    Edge* e3() {return EDGE(GTS_TRIANGLE(this)->e3);}
    const GtsSegment* s1() const {return GTS_SEGMENT(e1());}
    const GtsSegment* s2() const {return GTS_SEGMENT(e2());}
    const GtsSegment* s3() const {return GTS_SEGMENT(e3());}
    GtsSegment* s1() {return GTS_SEGMENT(e1());}
    GtsSegment* s2() {return GTS_SEGMENT(e2());}
    GtsSegment* s3() {return GTS_SEGMENT(e3());}
    const Vertex* v1() const {return const_cast<Face*>(this)->v1();}
    const Vertex* v2() const {return const_cast<Face*>(this)->v2();}
    const Vertex* v3() const {return const_cast<Face*>(this)->v3();}
    Vertex* v1() {return e2()->is_has(e1()->v2()) ? e1()->v1() : e1()->v2();}
    Vertex* v2() {return e1()->opposite(v1());}
    Vertex* v3() {return e2()->opposite(v2());}
    const GtsPoint* p1() const {return GTS_POINT(v1());}
    const GtsPoint* p2() const {return GTS_POINT(v2());}
    const GtsPoint* p3() const {return GTS_POINT(v3());}
    GtsPoint* p1() {return GTS_POINT(v1());}
    GtsPoint* p2() {return GTS_POINT(v2());}
    GtsPoint* p3() {return GTS_POINT(v3());}

    Face& reverse();
    Face& move(const Vector& mv);
    Face& scale(const Vector& sv);
    Face& rotate(const Vector& rv, gdouble angle);
    Face& transform(const GtsMatrix* m);

    Vector normal_area() const;
    Vector normal_uniform() const {return normal_area().uniform();}

    Edge* edge_share_with(const Face* f);
    Vertex* vertex_closest_to(const Vertex& v);

    GtsFace parent;
};
FaceClass* face_class();
#define IS_FACE(obj) (gts_object_is_from_class(obj, face_class()))
#define FACE(obj) GTS_OBJECT_CAST (obj, Face, face_class ())
#define FACE_CLASS(cls) GTS_OBJECT_CLASS_CAST(cls, FaceClass, face_class())
inline Face* face_new(FaceClass* fc, Edge* e1, Edge* e2, Edge* e3)
{
    return FACE(gts_face_new(GTS_FACE_CLASS(fc), GTS_EDGE(e1), GTS_EDGE(e2), GTS_EDGE(e3)));
}

inline std::ostream& operator<<(std::ostream& os, const Face& f)
{
    return os << "{" << *f.e1() << "\n" << *f.e2() << "\n" << *f.e3() << "}";
}

inline void aface_offset(Face* f, gdouble* d)
{
    f->move(f->normal_uniform().scale(*d));
}

inline void aface_reverse(Face* f, gpointer)
{
    f->reverse();
}

#endif   // ----- #ifndef face_INC  -----
