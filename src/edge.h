
#ifndef  edge_INC
#define  edge_INC

#include <gts.h>
#include <cmath>                                // len:sqrt
#include "vertex.h"

typedef GtsEdgeClass EdgeClass;
class Face;
class Edge
{
public:
    const GtsSegment* s() const {return GTS_SEGMENT(this);}
    GtsSegment* s() {return GTS_SEGMENT(this);}
    const Vertex* v1() const {return VERTEX(GTS_SEGMENT(this)->v1);}
    const Vertex* v2() const {return VERTEX(GTS_SEGMENT(this)->v2);}
    Vertex* v1() {return VERTEX(GTS_SEGMENT(this)->v1);}
    Vertex* v2() {return VERTEX(GTS_SEGMENT(this)->v2);}
    const GtsPoint* p1() const {return GTS_POINT(v1());}
    const GtsPoint* p2() const {return GTS_POINT(v2());}
    GtsPoint* p1() {return GTS_POINT(v1());}
    GtsPoint* p2() {return GTS_POINT(v2());}

    gdouble len() const {return sqrt(len2());}
    gdouble len2() const {v1()->len2(*v2());}
    const Vertex* beg(const Face* f) const;
    const Vertex* end(const Face* f) const;
    Vertex* beg(Face* f);
    Vertex* end(Face* f);
    const Edge* prev(const Face* f) const;
    const Edge* next(const Face* f) const;
    Edge* prev(Face* f);
    Edge* next(Face* f);

    GSList* faces() const {return GTS_EDGE(this)->triangles;}
    void faces_prepend(Face* face);
    void faces_remove(Face* face);
    void faces_addseq(GSList* fslist);
    void faces_copy(GSList* fslist);
    void faces_free();
    guint faces_number() const;

    void vertex_swap();
    const Vertex* opposite(const Vertex* v) const;
    Vertex* opposite(const Vertex* v);

    void replace(Edge& e) {gts_edge_replace(GTS_EDGE(&e), GTS_EDGE(this));}
    Vertex* mid_vertex(VertexClass* vclass = vertex_class()) const;
    Edge* duplicate() const;

    gboolean is_boundary() const {return !faces()->next;}
    gboolean is_belong(const Face* f) const;
    gboolean is_has(const Vertex* v) const {return v1() == v || v2() == v;}
    gboolean is_has(const GtsPoint* p) const {return p1() == p || p2() == p;}

    GtsEdge parent;
    gpointer reserved;
};
EdgeClass* edge_class();
#define IS_EDGE(obj) (gts_object_is_from_class(obj, edge_class()))
#define EDGE(obj) GTS_OBJECT_CAST(obj, Edge, edge_class())
#define EDGE_CLASS(cls) GTS_OBJECT_CLASS_CAST(cls, EdgeClass, edge_class())
inline Edge* edge_new(EdgeClass* ec, Vertex* v1, Vertex* v2)
{
    return EDGE(gts_edge_new(GTS_EDGE_CLASS(ec), GTS_VERTEX(v1), GTS_VERTEX(v2)));
}
inline Edge* get_edge(EdgeClass* eclass, Vertex* v1, Vertex* v2)
{
    GSList* i = v1->edges();
    while (i) {
        Edge* edge = EDGE(i->data);
        if (edge->is_has(v2)) return edge;
        i = i->next;
    }
    return edge_new(eclass, v1, v2);
}

inline std::ostream& operator<<(std::ostream& os, const Edge& e)
{
    return os << "[ " << *e.v1() << " = " << *e.v2() << " ]";
}

gboolean edge_number_stop(gdouble cost, guint nedge, guint* min);
gboolean edge_number_stop_verbose(gdouble cost, guint nedge, guint* min);

#endif   // ----- #ifndef edge_INC  -----
