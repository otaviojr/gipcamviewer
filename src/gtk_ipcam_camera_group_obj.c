#include <gtk/gtk.h>
#include <json-glib/json-glib.h>

#include "gtk_ipcam_camera_group_obj.h"

struct _GtkIpcamCameraGroupObj
{
  GObject parent;

  GValue group_name;
  GValue cameras;
};

struct _GtkIpcamCameraGroupObjClass
{
  GObjectClass parent_class;
};

enum
{
  GTK_FOSCAM_CAMERA_GROUP_OBJ_PROP_0,
  GTK_FOSCAM_CAMERA_GROUP_OBJ_PROP_GROUP_NAME,
  GTK_FOSCAM_CAMERA_GROUP_OBJ_PROP_CAMERAS,
  GTK_FOSCAM_CAMERA_GROUP_OBJ_PROP_LAST
};

G_DEFINE_TYPE (GtkIpcamCameraGroupObj, gtk_ipcam_camera_group_obj, G_TYPE_OBJECT);

static GParamSpec
* gtk_ipcam_camera_group_obj_param_specs[GTK_FOSCAM_CAMERA_GROUP_OBJ_PROP_LAST] = { NULL, };

static void
gtk_ipcam_camera_group_obj_free_cameras(gpointer camera)
{
  printf("Destroying camera object\n");
  g_object_unref(camera);
}

static void
gtk_ipcam_camera_group_obj_get_property(GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec)
{
  GtkIpcamCameraGroupObj *self = GTK_FOSCAM_CAMERA_GROUP_OBJ(object);

  switch (prop_id) {
    case GTK_FOSCAM_CAMERA_GROUP_OBJ_PROP_GROUP_NAME:
      g_value_set_string(value, g_value_get_string(&self->group_name));
      break;
    case GTK_FOSCAM_CAMERA_GROUP_OBJ_PROP_CAMERAS:
      g_value_set_boxed(value, g_value_get_boxed(&self->cameras));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void
gtk_ipcam_camera_group_obj_set_property(GObject * object, guint prop_id, const GValue * value,
    GParamSpec * pspec)
{
  GtkIpcamCameraGroupObj *self = GTK_FOSCAM_CAMERA_GROUP_OBJ(object);

  switch (prop_id) {
    case GTK_FOSCAM_CAMERA_GROUP_OBJ_PROP_GROUP_NAME:
      g_value_set_string(&self->group_name, g_value_dup_string(value));
      break;
    case GTK_FOSCAM_CAMERA_GROUP_OBJ_PROP_CAMERAS:
      g_value_set_boxed(&self->cameras, g_value_get_boxed(value));
      g_ptr_array_set_free_func(g_value_get_boxed(value),gtk_ipcam_camera_group_obj_free_cameras);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gtk_ipcam_camera_group_obj_finalize(GObject * object)
{
  GtkIpcamCameraGroupObj *self = GTK_FOSCAM_CAMERA_GROUP_OBJ(object);

  printf("gtk_ipcam_camera_group_obj_finalize\n");

  g_value_unset(&self->group_name);

  GPtrArray* array = g_value_get_boxed(&self->cameras);
  if(array != NULL){
    g_ptr_array_unref(array);
  }
  g_value_unset(&self->cameras);

  G_OBJECT_CLASS(gtk_ipcam_camera_group_obj_parent_class)->finalize(object);
}

static void
gtk_ipcam_camera_group_obj_class_init(GtkIpcamCameraGroupObjClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = gtk_ipcam_camera_group_obj_set_property;
  gobject_class->get_property = gtk_ipcam_camera_group_obj_get_property;
  gobject_class->finalize = gtk_ipcam_camera_group_obj_finalize;

  gtk_ipcam_camera_group_obj_param_specs
      [GTK_FOSCAM_CAMERA_GROUP_OBJ_PROP_GROUP_NAME] =
      g_param_spec_string("group_name", "Group Name",
      "Name of this group.", NULL,
      G_PARAM_READWRITE);

  gtk_ipcam_camera_group_obj_param_specs
      [GTK_FOSCAM_CAMERA_GROUP_OBJ_PROP_CAMERAS] =
      g_param_spec_boxed("cameras", "Cameras", "Cameras who belongs to this group",
      gtk_ipcam_camera_obj_get_boxed_type(),
      G_PARAM_READWRITE | G_PARAM_CONSTRUCT);

  g_object_class_install_properties (gobject_class,
      GTK_FOSCAM_CAMERA_GROUP_OBJ_PROP_LAST, gtk_ipcam_camera_group_obj_param_specs);
}

static void
gtk_ipcam_camera_group_obj_init(GtkIpcamCameraGroupObj * self)
{
  g_value_init(&self->group_name, G_TYPE_STRING);
  g_value_init(&self->cameras, gtk_ipcam_camera_obj_get_boxed_type());
}

GtkIpcamCameraGroupObj *
gtk_ipcam_camera_group_obj_new(void)
{
  return g_object_new(GTK_TYPE_FOSCAM_CAMERA_GROUP_OBJ, NULL);
}

void
gtk_ipcam_camera_group_obj_foreach_camera(GtkIpcamCameraGroupObj* self, GFunc func, gpointer user_data)
{
  GPtrArray* array = g_value_get_boxed(&self->cameras);
  if(array == NULL) return;
  g_ptr_array_foreach(array, func, user_data);
}

GtkIpcamCameraObj*
gtk_ipcam_camera_group_obj_get_camera_by_index(GtkIpcamCameraGroupObj* self, guint index)
{
  GPtrArray* array = g_value_get_boxed(&self->cameras);
  if(array == NULL) return NULL;
  return g_ptr_array_index(array, index);
}


/******************************************************************************
 * JSON SERIALIZER START
 ******************************************************************************/
 /******************************************************************************
 * Since our preference object will live as long as our application is
 * running we don't need to copy and free objects.
 *
 * We will just send the pointer to the real object and not free it. It will
 * be free when application ends
 *
 * This will save me a lot of work :-)
 ******************************************************************************/
gpointer
gtk_ipcam_camera_group_obj_copy(gpointer src_boxed)
{
  printf("Trying to copy camera group object\n");
  return src_boxed;
}

void
gtk_ipcam_camera_group_obj_free(gpointer boxed)
{
  printf("Trying to free camera group object\n");
}
/******************************************************************************/

static void
gtk_ipcam_camera_group_obj_serialize_item(gpointer data, gpointer user_data)
{
  JsonArray* array = user_data;
  JsonNode* object_node = json_gobject_serialize(data);
  json_array_add_element(array, object_node);
}

static JsonNode *
gtk_ipcam_camera_group_obj_serialize(gconstpointer boxed)
{
  printf("Serializing camera group\n");
  if(boxed == NULL){
    return json_node_new(JSON_NODE_NULL);
  }

  JsonArray* array = json_array_new();
  g_ptr_array_foreach((GPtrArray*)boxed, gtk_ipcam_camera_group_obj_serialize_item, array);

  JsonNode* node = json_node_new(JSON_NODE_ARRAY);
  json_node_take_array(node,array);

  return node;
}

static void
gtk_ipcam_camera_group_obj_deserialize_item(JsonArray * json_array, guint index, JsonNode* element_node, gpointer user_data)
{
    GPtrArray* array = user_data;
    GObject* camera_group = json_gobject_deserialize(GTK_TYPE_FOSCAM_CAMERA_GROUP_OBJ,element_node);
    g_ptr_array_add(array,camera_group);
}

static gpointer
gtk_ipcam_camera_group_obj_deserialize(JsonNode* node)
{
  printf("Deserializing camera group\n");

  GPtrArray* array = g_ptr_array_new();
  g_assert(array != NULL);

  json_array_foreach_element(json_node_get_array(node),gtk_ipcam_camera_group_obj_deserialize_item, array);

  return array;
}

GType
gtk_ipcam_camera_group_obj_get_boxed_type (void)
{
  static GType camera_group_obj_boxed_type = 0;

  if (camera_group_obj_boxed_type == 0)
    {
      camera_group_obj_boxed_type =
        g_boxed_type_register_static (g_intern_static_string ("GtkIpcamCameraGroupObjBoxedType"),
                                      (GBoxedCopyFunc) gtk_ipcam_camera_group_obj_copy,
                                      (GBoxedFreeFunc) gtk_ipcam_camera_group_obj_free);

      json_boxed_register_serialize_func (camera_group_obj_boxed_type, JSON_NODE_ARRAY,
                                          gtk_ipcam_camera_group_obj_serialize);
      json_boxed_register_deserialize_func (camera_group_obj_boxed_type, JSON_NODE_ARRAY,
                                            gtk_ipcam_camera_group_obj_deserialize);
    }

  return camera_group_obj_boxed_type;
}
/******************************************************************************
 * JSON SERIALIZER ENDS
 ******************************************************************************/

gchar *
gtk_ipcam_camera_group_obj_get_name(GtkIpcamCameraGroupObj * self)
{
  gchar *ret;

  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_GROUP_OBJ(self), NULL);

  g_object_get(self, "group_name", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_camera_group_obj_set_name(GtkIpcamCameraGroupObj * self, const gchar* val)
{
  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_GROUP_OBJ(self), FALSE);

  g_object_set(self, "group_name", val, NULL);

  return TRUE;
}

gint
gtk_ipcam_camera_group_obj_add_camera(GtkIpcamCameraGroupObj * self, GtkIpcamCameraObj* camera)
{
  GPtrArray* array = g_value_get_boxed(&self->cameras);

  if(array == NULL)
  {
    array = g_ptr_array_new();
    g_ptr_array_set_free_func(array,gtk_ipcam_camera_group_obj_free_cameras);
    g_value_set_boxed(&self->cameras, array);
  }

  g_assert(array != NULL);

  g_ptr_array_add(array,camera);
  return array->len-1;
}

gboolean
gtk_ipcam_camera_group_obj_del_camera(GtkIpcamCameraGroupObj * self, GtkIpcamCameraObj* camera)
{
  GPtrArray* array = g_value_get_boxed(&self->cameras);

  if(array == NULL)
  {
    return FALSE;
  }

  g_ptr_array_remove(array, camera);
  g_object_unref(camera);
  return TRUE;
}

guint
gtk_ipcam_camera_group_obj_get_cameras_count(GtkIpcamCameraGroupObj* self)
{
  GPtrArray* array = g_value_get_boxed(&self->cameras);
  if(array == NULL) return 0;
  return array->len;
}

gboolean
gtk_ipcam_camera_group_obj_change_camera_pos(GtkIpcamCameraGroupObj* self, GtkIpcamCameraObj* camera, gint new_pos)
{
  GPtrArray* array = g_value_get_boxed(&self->cameras);

  if(array == NULL)
  {
    return FALSE;
  }

  g_object_ref(camera);
  g_ptr_array_remove(array, camera);
  g_ptr_array_insert(array, new_pos, camera);
  //g_object_unref(camera);
  return TRUE;
}
