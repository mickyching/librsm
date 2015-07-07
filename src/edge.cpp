#include "edge.h"
#include "face.h"

namespace
{
    void edge_clone(GtsObject* clone, GtsObject* object)
    {
        GtsObjectClass* oclass = GTS_OBJECT_CLASS(edge_class());
        (*oclass->parent_class->clone) (clone, object);

        EDGE(clone)->reserved = NULL;
    }
    void edge_destroy(GtsObject* object)
    {
        Edge* edge = EDGE(object);
        g_assert(edge->reserved == NULL);

        GtsObjectClass* oclass = GTS_OBJECT_CLASS(edge_class());
        (*oclass->parent_class->destroy) (object);
    }
    void edge_class_init(GtsObjectClass* klass)
    {
        klass->clone = edge_clone;
        klass->destroy = edge_destroy;
    }
    void edge_init(Edge* edge)
    {
        edge->reserved = NULL;
    }
}

EdgeClass * edge_class (void)
{
    static EdgeClass *eclass = NULL;

    if (eclass == NULL) {
        GtsObjectClassInfo edge_info = {
            "Edge",
            sizeof (Edge),
            sizeof (EdgeClass),
            (GtsObjectClassInitFunc) edge_class_init,
            (GtsObjectInitFunc) edge_init,
            (GtsArgSetFunc) NULL,
            (GtsArgGetFunc) NULL
        };
        gpointer oclass = gts_object_class_new(
                GTS_OBJECT_CLASS(gts_edge_class()), // must use father class
                &edge_info);
        eclass = EDGE_CLASS(oclass);
    }
    return eclass;
}

gboolean Edge::is_belong(const Face* f) const
{
    GSList* i = faces();
    while (i) {
        if (f == FACE(i->data)) return TRUE;
        i = i->next;
    }
    return false;
}

void Edge::faces_prepend(Face* face)
{
    GTS_EDGE(this)->triangles = g_slist_prepend(GTS_EDGE(this)->triangles,
            GTS_TRIANGLE(face));
}

void Edge::faces_remove(Face* face)
{
    GTS_EDGE(this)->triangles = g_slist_remove(GTS_EDGE(this)->triangles,
            GTS_TRIANGLE(face));
}

void Edge::faces_free()
{
    g_slist_free(GTS_EDGE(this)->triangles);
    GTS_EDGE(this)->triangles = NULL;
}

guint Edge::faces_number() const
{
    guint nf = 0;
    GSList* i = faces();
    while (i && GTS_IS_FACE(i->data)) {
        ++nf;
        i = i->next;
    }
    return nf;
}

void Edge::faces_copy(GSList* fslist)
{
    GTS_EDGE(this)->triangles = g_slist_copy(fslist);
}

void Edge::vertex_swap()
{
    GtsVertex* tmp = GTS_SEGMENT(this)->v1;
    GTS_SEGMENT(this)->v1 = GTS_SEGMENT(this)->v2;
    GTS_SEGMENT(this)->v2 = tmp;
}
const Vertex* Edge::opposite(const Vertex* v) const
{
    if (v == v1()) return v2();
    if (v == v2()) return v1();
    return NULL;
}

Vertex* Edge::opposite(const Vertex* v)
{
    if (v == v1()) return v2();
    if (v == v2()) return v1();
    return NULL;
}

const Edge* Edge::prev(const Face* f) const
{
    g_return_val_if_fail(f != NULL, NULL);
    const Edge* e1 = f->e1(), * e2 = f->e2(), * e3 = f->e3();
    if (this == e1) return e3;
    if (this == e2) return e1;
    if (this == e3) return e2;
    return NULL;
}
Edge* Edge::prev(Face* f)
{
    return EDGE(prev(const_cast<const Face*>(f)));
}

const Edge* Edge::next(const Face* f) const
{
    g_return_val_if_fail(f != NULL, NULL);
    const Edge* e1 = f->e1(), * e2 = f->e2(), * e3 = f->e3();
    if (this == e1) return e2;
    if (this == e2) return e3;
    if (this == e3) return e1;
    return NULL;
}

Edge* Edge::next(Face* f)
{
    return EDGE(next(const_cast<const Face*>(f)));
}

const Vertex* Edge::beg(const Face* f) const
{
    const Vertex* ev1 = f->v1();
    if (this->is_has(ev1) && prev(f)->is_has(ev1)) return ev1;
    const Vertex* ev2 = f->v2();
    if (this->is_has(ev2) && prev(f)->is_has(ev2)) return ev2;
    const Vertex* ev3 = f->v3();
    if (this->is_has(ev3) && prev(f)->is_has(ev3)) return ev3;
    return NULL;
}

Vertex* Edge::beg(Face* f)
{
    return VERTEX(beg(const_cast<const Face*>(f)));
}

const Vertex* Edge::end(const Face* f) const
{
    const Vertex* ev1 = f->v1();
    if (this->is_has(ev1) && next(f)->is_has(ev1)) return ev1;
    const Vertex* ev2 = f->v2();
    if (this->is_has(ev2) && next(f)->is_has(ev2)) return ev2;
    const Vertex* ev3 = f->v3();
    if (this->is_has(ev3) && next(f)->is_has(ev3)) return ev3;
    return NULL;
}

Vertex* Edge::end(Face* f)
{
    return VERTEX(end(const_cast<const Face*>(f)));
}

Vertex* Edge::mid_vertex(VertexClass* vclass) const
{
    return vertex_new(vclass,
            (p1()->x + p2()->x)/2.0,
            (p1()->y + p2()->y)/2.0,
            (p1()->z + p2()->z)/2.0);
}

Edge* Edge::duplicate() const
{
    return EDGE(gts_edge_is_duplicate(GTS_EDGE(this)));
}

gboolean edge_number_stop(gdouble cost, guint nedge, guint* min_number)
{
    if (nedge < *min_number) return true;
    else return false;
}

gboolean edge_number_stop_verbose(gdouble cost, guint number, guint* min)
{
    static guint nmax = 0, nold = 0;
    static GTimer *timer = NULL, *total_timer = NULL;
    g_return_val_if_fail (min != NULL, TRUE);
    if (timer == NULL) {
        nmax = nold = number;
        timer = g_timer_new ();
        total_timer = g_timer_new ();
        g_timer_start (total_timer);
    }
    if (number != nold && number % 500 == 0 && number < nmax && nmax > *min) {
        gdouble total_elapsed = g_timer_elapsed (total_timer, NULL);

        g_timer_stop (timer);

        gdouble hours = floor (total_elapsed/3600.);
        gdouble mins = floor ((total_elapsed - 3600.*hours)/60.);
        gdouble secs = floor (total_elapsed - 3600.*hours - 60.*mins);
        gdouble remaining = total_elapsed*((nmax - *min)/(gdouble) (nmax - number) - 1.);
        gdouble hours1 = floor (remaining/3600.);
        gdouble mins1 = floor ((remaining - 3600.*hours1)/60.);
        gdouble secs1 = floor (remaining - 3600.*hours1 - 60.*mins1);

        fprintf (stderr,
	        "\rEdges: %10u %3.0f%% %6.0f edges/s "
	        "Elapsed: %02.0f:%02.0f:%02.0f "
	        "Remaining: %02.0f:%02.0f:%02.0f ",
	        number,
	        100.*(nmax - number)/(nmax - *min),
	        (nold - number)/g_timer_elapsed (timer, NULL),
	        hours, mins, secs,
	        hours1, mins1, secs1);
        fflush (stderr);

        nold = number;
        g_timer_start (timer);
    }

    if (number < *min) {
        std::cerr << "\n";
        g_timer_destroy (timer);
        timer = NULL;
        g_timer_destroy (total_timer);
        total_timer = NULL;
        return TRUE;
    }
    return FALSE;
}
