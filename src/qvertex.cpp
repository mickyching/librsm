
#include "rsmdefs.h"
#include "qvertex.h"

static void vertex_clone(GtsObject* clone, GtsObject* object)
{
  (*GTS_OBJECT_CLASS(gts_vertex_class())->parent_class->clone)(clone, object);
  Q_VERTEX(clone)->segments = NULL;
  Q_VERTEX(clone)->quadric = new Quadric;
}

static void vertex_destroy(GtsObject* object)
{
    QVertex* vertex = Q_VERTEX(object);

    GSList* i = vertex->segments;
    while (i) {
        GTS_OBJECT_SET_FLAGS(i->data, GTS_DESTROYED);
        i = i->next;
    }
    i = vertex->segments;
    while (i) {
        GSList* next = i->next;
        gts_object_destroy(GTS_OBJECT(i->data));
        i = next;
    }
    g_assert(vertex->segments == NULL);

    if (vertex->quadric != NULL) {                      // 删除二次型
        delete vertex->quadric;
        vertex->quadric = NULL;
    }

    (*GTS_OBJECT_CLASS(gts_vertex_class())->parent_class->destroy)(object);
}

static void vertex_class_init(GtsVertexClass* klass)
{
    klass->intersection_attributes = NULL;
    GTS_OBJECT_CLASS (klass)->clone = vertex_clone;
    GTS_OBJECT_CLASS (klass)->destroy = vertex_destroy;
}

static void vertex_init(QVertex* vertex)
{
    vertex->segments = NULL;
    vertex->quadric = new Quadric;
}

GtsVertexClass* q_vertex_class()
{
    static GtsVertexClass* klass = NULL;

    if (klass == NULL) {
        GtsObjectClassInfo info;
        strcpy(info.name, "QVertex");
        info.object_size = sizeof(QVertex);
        info.class_size  = sizeof(GtsVertexClass);
        info.class_init_func  = (GtsObjectClassInitFunc) vertex_class_init;
        info.object_init_func = (GtsObjectInitFunc) vertex_init;
        info.arg_get_func     = (GtsArgSetFunc) NULL,
        info.arg_set_func     = (GtsArgGetFunc) NULL;

        GtsObjectClass* parent_class = GTS_OBJECT_CLASS(gts_point_class());
        gpointer object_class = gts_object_class_new(parent_class, &info);
        klass = GTS_VERTEX_CLASS(object_class);
    }
    return klass;
}
