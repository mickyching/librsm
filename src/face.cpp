#include "face.h"

FaceClass * face_class (void)
{
    static FaceClass * fclass = NULL;

    if (fclass == NULL) {
        GtsObjectClassInfo face_info = {
            "Face",
            sizeof (Face),
            sizeof (FaceClass),
            (GtsObjectClassInitFunc) NULL,
            (GtsObjectInitFunc) NULL,
            (GtsArgSetFunc) NULL,
            (GtsArgGetFunc) NULL
        };
        gpointer oclass = gts_object_class_new (
                GTS_OBJECT_CLASS (gts_face_class ()), // must use father class
                &face_info);
        fclass = FACE_CLASS(oclass);
    }

  return fclass;
}

Face& Face::reverse()
{
    GtsEdge* tmp = GTS_TRIANGLE(this)->e1;
    GTS_TRIANGLE(this)->e1 = GTS_TRIANGLE(this)->e2;
    GTS_TRIANGLE(this)->e2 = tmp;
    return *this;
}
Face& Face::move(const Vector& mv)
{
    v1()->move(mv);
    v2()->move(mv);
    v3()->move(mv);
}

Face& Face::scale(const Vector& sv)
{
    v1()->scale(sv);
    v2()->scale(sv);
    v3()->scale(sv);
}

Vertex* Face::vertex_closest_to(const Vertex& v)
{
    Vertex* fv1 = e1()->v1(), * fv2 = e1()->v2(), * fv3 = e2()->opposite(fv2);
    Vertex* tmp = fv1->len2(v) < fv2->len2(v) ? fv1 : fv2;
    return tmp->len2(v) < fv3->len2(v) ? tmp : fv3;
}

Vector Face::normal_area() const
{
    const GtsPoint* tp1 = p1(), * tp2 = p2(), * tp3 = p3();
    gdouble x1 = tp2->x - tp1->x, y1 = tp2->y - tp1->y, z1 = tp2->z - tp1->z;
    gdouble x2 = tp3->x - tp1->x, y2 = tp3->y - tp1->y, z2 = tp3->z - tp1->z;
    return Vector(y1*z2 - z1*y2, z1*x2 - x1*z2, x1*y2 - y1*x2);
}

Edge* Face::edge_share_with(const Face* f)
{
    if (e1()->is_belong(f)) return e1();
    if (e2()->is_belong(f)) return e2();
    if (e3()->is_belong(f)) return e3();
    return NULL;
}


