#include "vertex.h"
#include "edge.h"

const gdouble Vertex::EPSILON = 1.e-10;


namespace
{
    void vertex_clone(GtsObject* clone, GtsObject* object)
    {
        GtsObjectClass* oclass = GTS_OBJECT_CLASS(vertex_class());
        (*oclass->parent_class->clone) (clone, object);

        VERTEX(clone)->reserved = NULL;
    }

    void vertex_destroy(GtsObject* object)
    {
        Vertex* vertex = VERTEX(object);
        g_assert(vertex->reserved == NULL);

        GtsObjectClass* oclass = GTS_OBJECT_CLASS(vertex_class());
        (*oclass->parent_class->destroy) (object);
    }

    void vertex_class_init(VertexClass* klass)
    {
        GTS_OBJECT_CLASS (klass)->clone = vertex_clone;
        GTS_OBJECT_CLASS (klass)->destroy = vertex_destroy;
    }

    void vertex_init(Vertex* vertex)
    {
        vertex->reserved = NULL;
    }
}


VertexClass* vertex_class()
{
    static VertexClass* klass = NULL;
    if (klass == NULL) {
        GtsObjectClassInfo vertex_info = {
            "Vertex",
            sizeof(Vertex),
            sizeof(VertexClass),
            (GtsObjectClassInitFunc) vertex_class_init,
            (GtsObjectInitFunc) vertex_init,
            (GtsArgSetFunc) NULL,
            (GtsArgGetFunc) NULL
        };
        gpointer vclass = gts_object_class_new(
                GTS_OBJECT_CLASS(gts_vertex_class()), // must use father class
                &vertex_info);
        klass = VERTEX_CLASS(vclass);
    }
    return klass;
}


Vertex& Vertex::transform(const GtsMatrix* tm)
{
    GtsPoint* p1 = GTS_POINT(this);
    gdouble tx = p1->x, ty = p1->y, tz = p1->z;
    p1->x = (tm[0][0]*tx + tm[0][1]*ty + tm[0][2]*tz + tm[0][3]);
    p1->y = (tm[1][0]*tx + tm[1][1]*ty + tm[1][2]*tz + tm[1][3]);
    p1->z = (tm[2][0]*tx + tm[2][1]*ty + tm[2][2]*tz + tm[2][3]);
}


bool Vertex::is_belong(const Edge& edge) const
{
    GSList* i = edges();
    while (i) {
        if (EDGE(i->data) == &edge) return true;
        i = i->next;
    }
    return false;
}

void Vertex::edges_prepend(Edge* edge)
{
    GTS_VERTEX(this)->segments = g_slist_prepend(GTS_VERTEX(this)->segments, edge);
}
