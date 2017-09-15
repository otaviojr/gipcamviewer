#include <gtk/gtk.h>
#include <json-glib/json-glib.h>

#include "gtk_ipcam_camera_obj.h"

struct _GtkIpcamCameraObj
{
  GObject parent;

  GValue model;
  GValue name;
  GValue remote_https;
  GValue remote_url;
  GValue local_https;
  GValue local_url;
  GValue remote_port;
  GValue remote_media_port;
  GValue local_port;
  GValue local_media_port;
  GValue username;
  GValue password;
  GValue subchannel;

  GtkIpcamCameraDriverObj* camera_driver;
};

struct _GtkIpcamCameraObjClass
{
  GObjectClass parent_class;
};

enum
{
  GTK_FOSCAM_CAMERA_OBJ_PROP_0,
  GTK_FOSCAM_CAMERA_OBJ_PROP_CAMERA_MODEL,
  GTK_FOSCAM_CAMERA_OBJ_PROP_CAMERA_NAME,
  GTK_FOSCAM_CAMERA_OBJ_PROP_REMOTE_HTTPS,
  GTK_FOSCAM_CAMERA_OBJ_PROP_REMOTE_URL,
  GTK_FOSCAM_CAMERA_OBJ_PROP_LOCAL_HTTPS,
  GTK_FOSCAM_CAMERA_OBJ_PROP_LOCAL_URL,
  GTK_FOSCAM_CAMERA_OBJ_PROP_REMOTE_PORT,
  GTK_FOSCAM_CAMERA_OBJ_PROP_REMOTE_MEDIA_PORT,
  GTK_FOSCAM_CAMERA_OBJ_PROP_LOCAL_PORT,
  GTK_FOSCAM_CAMERA_OBJ_PROP_LOCAL_MEDIA_PORT,
  GTK_FOSCAM_CAMERA_OBJ_PROP_USERNAME,
  GTK_FOSCAM_CAMERA_OBJ_PROP_PASSWORD,
  GTK_FOSCAM_CAMERA_OBJ_PROP_USE_SUB_CHANNEL,
  GTK_FOSCAM_CAMERA_OBJ_PROP_LAST
};

G_DEFINE_TYPE (GtkIpcamCameraObj, gtk_ipcam_camera_obj, G_TYPE_OBJECT);

static GParamSpec
* gtk_ipcam_camera_obj_param_specs[GTK_FOSCAM_CAMERA_OBJ_PROP_LAST] = { NULL, };

static void
gtk_ipcam_camera_obj_get_property(GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec)
{
  GtkIpcamCameraObj *self = GTK_FOSCAM_CAMERA_OBJ(object);

  switch (prop_id) {
    case GTK_FOSCAM_CAMERA_OBJ_PROP_CAMERA_MODEL:
      g_value_set_string(value, g_value_get_string(&self->model));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_CAMERA_NAME:
      g_value_set_string(value, g_value_get_string(&self->name));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_REMOTE_HTTPS:
      g_value_set_boolean(value, g_value_get_boolean(&self->remote_https));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_REMOTE_URL:
      g_value_set_string(value, g_value_get_string(&self->remote_url));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_LOCAL_HTTPS:
      g_value_set_boolean(value, g_value_get_boolean(&self->local_https));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_LOCAL_URL:
      g_value_set_string(value, g_value_get_string(&self->local_url));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_REMOTE_PORT:
      g_value_set_string(value, g_value_get_string(&self->remote_port));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_REMOTE_MEDIA_PORT:
      g_value_set_string(value, g_value_get_string(&self->remote_media_port));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_LOCAL_PORT:
      g_value_set_string(value, g_value_get_string(&self->local_port));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_LOCAL_MEDIA_PORT:
      g_value_set_string(value, g_value_get_string(&self->local_media_port));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_USERNAME:
      g_value_set_string(value, g_value_get_string(&self->username));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_PASSWORD:
      g_value_set_string(value, g_value_get_string(&self->password));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_USE_SUB_CHANNEL:
      g_value_set_boolean(value, g_value_get_boolean(&self->subchannel));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void
gtk_ipcam_camera_obj_set_property(GObject * object, guint prop_id, const GValue * value,
    GParamSpec * pspec)
{
  GtkIpcamCameraObj *self = (GtkIpcamCameraObj *) object;

  switch (prop_id) {
    case GTK_FOSCAM_CAMERA_OBJ_PROP_CAMERA_MODEL:
      if(self->camera_driver != NULL){
        g_object_unref(self->camera_driver);
      }
      g_value_set_string(&self->model, g_value_dup_string(value));
      self->camera_driver = gtk_ipcam_camera_driver_obj_new(g_value_get_string(&self->model));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_CAMERA_NAME:
      g_value_set_string(&self->name, g_value_dup_string(value));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_REMOTE_HTTPS:
      g_value_set_boolean(&self->remote_https, g_value_get_boolean(value));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_REMOTE_URL:
      g_value_set_string(&self->remote_url, g_value_dup_string(value));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_LOCAL_HTTPS:
      g_value_set_boolean(&self->local_https, g_value_get_boolean(value));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_LOCAL_URL:
      g_value_set_string(&self->local_url, g_value_dup_string(value));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_REMOTE_PORT:
      g_value_set_string(&self->remote_port, g_value_dup_string(value));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_REMOTE_MEDIA_PORT:
      g_value_set_string(&self->remote_media_port, g_value_dup_string(value));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_LOCAL_PORT:
      g_value_set_string(&self->local_port, g_value_dup_string(value));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_LOCAL_MEDIA_PORT:
      g_value_set_string(&self->local_media_port, g_value_dup_string(value));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_USERNAME:
      g_value_set_string(&self->username, g_value_dup_string(value));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_PASSWORD:
      g_value_set_string(&self->password, g_value_dup_string(value));
      break;
    case GTK_FOSCAM_CAMERA_OBJ_PROP_USE_SUB_CHANNEL:
      g_value_set_boolean(&self->subchannel, g_value_get_boolean(value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gtk_ipcam_camera_obj_finalize(GObject * object)
{
  GtkIpcamCameraObj *self = GTK_FOSCAM_CAMERA_OBJ(object);

  printf("gtk_ipcam_camera_obj_finalize\n");

  g_value_unset(&self->model);
  g_value_unset(&self->name);
  g_value_unset(&self->remote_https);
  g_value_unset(&self->remote_url);
  g_value_unset(&self->local_https);
  g_value_unset(&self->local_url);
  g_value_unset(&self->remote_port);
  g_value_unset(&self->remote_media_port);
  g_value_unset(&self->local_port);
  g_value_unset(&self->local_media_port);
  g_value_unset(&self->username);
  g_value_unset(&self->password);
  g_value_unset(&self->subchannel);

  if(self->camera_driver != NULL){
    g_object_unref(self->camera_driver);
    self->camera_driver = NULL;
  }

  G_OBJECT_CLASS(gtk_ipcam_camera_obj_parent_class)->finalize(object);
}

static void
gtk_ipcam_camera_obj_class_init(GtkIpcamCameraObjClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = gtk_ipcam_camera_obj_set_property;
  gobject_class->get_property = gtk_ipcam_camera_obj_get_property;
  gobject_class->finalize = gtk_ipcam_camera_obj_finalize;

  gtk_ipcam_camera_obj_param_specs
      [GTK_FOSCAM_CAMERA_OBJ_PROP_CAMERA_MODEL] =
      g_param_spec_string("model", "Camera Model",
      "Camera Model. We will load specifications, from specification files, using this information.", NULL,
      G_PARAM_READWRITE);

    gtk_ipcam_camera_obj_param_specs
        [GTK_FOSCAM_CAMERA_OBJ_PROP_CAMERA_NAME] =
        g_param_spec_string("name", "Camera Name",
        "Camera Name", NULL,
        G_PARAM_READWRITE);

  gtk_ipcam_camera_obj_param_specs
      [GTK_FOSCAM_CAMERA_OBJ_PROP_REMOTE_HTTPS] =
      g_param_spec_boolean("remote_https", "Remote HTTPS", "Whether to use or not https to remote connection",
      TRUE,
      G_PARAM_READWRITE);

  gtk_ipcam_camera_obj_param_specs
      [GTK_FOSCAM_CAMERA_OBJ_PROP_REMOTE_URL] =
      g_param_spec_string("remote_url", "Remote URL",
      "Camera external address.", NULL,
      G_PARAM_READWRITE);

  gtk_ipcam_camera_obj_param_specs
      [GTK_FOSCAM_CAMERA_OBJ_PROP_LOCAL_HTTPS] =
      g_param_spec_boolean("local_https", "Local HTTPS", "Whether to use or not https to local connection",
      TRUE,
      G_PARAM_READWRITE);

  gtk_ipcam_camera_obj_param_specs
      [GTK_FOSCAM_CAMERA_OBJ_PROP_LOCAL_URL] =
      g_param_spec_string("local_url", "Local URL", "Camera local address. To access inside camera's network.",
      NULL,
      G_PARAM_READWRITE);

  gtk_ipcam_camera_obj_param_specs
      [GTK_FOSCAM_CAMERA_OBJ_PROP_REMOTE_PORT] =
      g_param_spec_string("remote_port", "Remote Port", "Remote port to connect to.",
      NULL,
      G_PARAM_READWRITE);

  gtk_ipcam_camera_obj_param_specs
      [GTK_FOSCAM_CAMERA_OBJ_PROP_REMOTE_MEDIA_PORT] =
      g_param_spec_string("remote_media_port", "Remote Media Port", "Remote RTSP port to connect to.",
      NULL,
      G_PARAM_READWRITE);

  gtk_ipcam_camera_obj_param_specs
      [GTK_FOSCAM_CAMERA_OBJ_PROP_LOCAL_PORT] =
      g_param_spec_string("local_port", "Local Port", "Local port to connect to.",
      NULL,
      G_PARAM_READWRITE);

  gtk_ipcam_camera_obj_param_specs
      [GTK_FOSCAM_CAMERA_OBJ_PROP_LOCAL_MEDIA_PORT] =
      g_param_spec_string("local_media_port", "Local Media Port", "Local RTSP port to connect to.",
      NULL,
      G_PARAM_READWRITE);

  gtk_ipcam_camera_obj_param_specs
      [GTK_FOSCAM_CAMERA_OBJ_PROP_USERNAME] =
      g_param_spec_string("username", "User Name", "User that should be used",
      NULL,
      G_PARAM_READWRITE);

  gtk_ipcam_camera_obj_param_specs
      [GTK_FOSCAM_CAMERA_OBJ_PROP_PASSWORD] =
      g_param_spec_string("password", "Password", "Password that should be used",
      NULL,
      G_PARAM_READWRITE);

  gtk_ipcam_camera_obj_param_specs
      [GTK_FOSCAM_CAMERA_OBJ_PROP_USE_SUB_CHANNEL] =
      g_param_spec_boolean("subchannel", "Sub Channel", "Whether to use or not the subchannel on multiview",
      FALSE,
      G_PARAM_READWRITE);

  g_object_class_install_properties (gobject_class,
      GTK_FOSCAM_CAMERA_OBJ_PROP_LAST, gtk_ipcam_camera_obj_param_specs);
}

static void
gtk_ipcam_camera_obj_init(GtkIpcamCameraObj * self)
{
  printf("Init Camera Object\n");

  g_value_init(&self->model, G_TYPE_STRING);
  g_value_init(&self->name, G_TYPE_STRING);
  g_value_init(&self->remote_https, G_TYPE_BOOLEAN);
  g_value_init(&self->remote_url, G_TYPE_STRING);
  g_value_init(&self->local_https, G_TYPE_BOOLEAN);
  g_value_init(&self->local_url, G_TYPE_STRING);
  g_value_init(&self->remote_port, G_TYPE_STRING);
  g_value_init(&self->remote_media_port, G_TYPE_STRING);
  g_value_init(&self->local_port, G_TYPE_STRING);
  g_value_init(&self->local_media_port, G_TYPE_STRING);
  g_value_init(&self->username, G_TYPE_STRING);
  g_value_init(&self->password, G_TYPE_STRING);
  g_value_init(&self->subchannel, G_TYPE_BOOLEAN);

  g_value_set_boolean(&self->remote_https, TRUE);
  g_value_set_boolean(&self->local_https, TRUE);
  g_value_set_boolean(&self->subchannel, FALSE);
  self->camera_driver = NULL;
}

GtkIpcamCameraObj *
gtk_ipcam_camera_obj_new(void)
{
  return g_object_new(GTK_TYPE_FOSCAM_CAMERA_OBJ, NULL);
}

GtkIpcamCameraDriverObj*
gtk_ipcam_camera_obj_get_camera_driver(GtkIpcamCameraObj* self)
{
  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), NULL);
  return self->camera_driver;
}

gboolean
gtk_ipcam_camera_obj_is_valid(GtkIpcamCameraObj* self)
{
  if( gtk_ipcam_camera_obj_get_camera_driver(self) == NULL)
  {
    return FALSE;
  }

  if(gtk_ipcam_camera_obj_get_remote_url(self) == NULL && gtk_ipcam_camera_obj_get_local_url(self) == NULL){
    return FALSE;
  }
  return TRUE;
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
gtk_ipcam_camera_obj_copy(gpointer src_boxed)
{
  printf("Trying to copy camera object\n");
  return src_boxed;
}

void
gtk_ipcam_camera_obj_free(gpointer boxed)
{
  printf("Trying to free camera object\n");
}
/******************************************************************************/

static void
gtk_ipcam_camera_obj_serialize_item(gpointer data, gpointer user_data)
{
  JsonArray* array = user_data;
  JsonNode* object_node = json_gobject_serialize(data);
  json_array_add_element(array, object_node);
}

static JsonNode *
gtk_ipcam_camera_obj_serialize(gconstpointer boxed)
{
  printf("Serializing camera object\n");
  if(boxed == NULL){
    return json_node_new(JSON_NODE_NULL);
  }

  JsonArray* array = json_array_new();
  g_ptr_array_foreach((GPtrArray*)boxed, gtk_ipcam_camera_obj_serialize_item, array);

  JsonNode* node = json_node_new(JSON_NODE_ARRAY);
  json_node_take_array(node,array);

  return node;
}

static void
gtk_ipcam_camera_obj_deserialize_item(JsonArray * json_array, guint index, JsonNode* element_node, gpointer user_data)
{
    GPtrArray* array = user_data;
    GObject* camera = json_gobject_deserialize(GTK_TYPE_FOSCAM_CAMERA_OBJ,element_node);
    g_ptr_array_add(array,camera);
}

static gpointer
gtk_ipcam_camera_obj_deserialize(JsonNode* node)
{
  printf("Deserializing camera object\n");

  GPtrArray* array = g_ptr_array_new();;
  g_assert(array != NULL);

  json_array_foreach_element(json_node_get_array(node),gtk_ipcam_camera_obj_deserialize_item, array);

  return array;
}

GType
gtk_ipcam_camera_obj_get_boxed_type(void)
{
  static GType camera_obj_boxed_type = 0;

  if (camera_obj_boxed_type == 0)
    {
      camera_obj_boxed_type =
        g_boxed_type_register_static (g_intern_static_string ("GtkIpcamCameraObjBoxedType"),
                                      (GBoxedCopyFunc) gtk_ipcam_camera_obj_copy,
                                      (GBoxedFreeFunc) gtk_ipcam_camera_obj_free);

      json_boxed_register_serialize_func (camera_obj_boxed_type, JSON_NODE_ARRAY,
                                          gtk_ipcam_camera_obj_serialize);
      json_boxed_register_deserialize_func (camera_obj_boxed_type, JSON_NODE_ARRAY,
                                            gtk_ipcam_camera_obj_deserialize);
    }

  return camera_obj_boxed_type;
}
/******************************************************************************
 * JSON SERIALIZER ENDS
 ******************************************************************************/

gchar *
gtk_ipcam_camera_obj_get_model(GtkIpcamCameraObj * self)
{
  gchar *ret;

  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), NULL);

  g_object_get(self, "model", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_camera_obj_set_model(GtkIpcamCameraObj * self, const gchar* val)
{
  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), FALSE);

  g_object_set(self, "model", val, NULL);

  return TRUE;
}

gchar *
gtk_ipcam_camera_obj_get_name(GtkIpcamCameraObj * self)
{
  gchar *ret;

  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), NULL);

  g_object_get(self, "name", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_camera_obj_set_name(GtkIpcamCameraObj * self, const gchar* val)
{
  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), FALSE);

  g_object_set(self, "name", val, NULL);

  return TRUE;
}

gboolean
gtk_ipcam_camera_obj_get_remote_https(GtkIpcamCameraObj * self)
{
  gboolean ret;

  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), TRUE);

  g_object_get(self, "remote_https", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_camera_obj_set_remote_https(GtkIpcamCameraObj * self, const gboolean val)
{
  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), FALSE);

  g_object_set(self, "remote_https", val, NULL);

  return TRUE;
}

gchar *
gtk_ipcam_camera_obj_get_remote_url(GtkIpcamCameraObj * self)
{
  gchar *ret;

  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), NULL);

  g_object_get(self, "remote_url", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_camera_obj_set_remote_url(GtkIpcamCameraObj * self, const gchar* val)
{
  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), FALSE);

  g_object_set(self, "remote_url", val, NULL);

  return TRUE;
}

gboolean
gtk_ipcam_camera_obj_get_local_https(GtkIpcamCameraObj * self)
{
  gboolean ret;

  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), TRUE);

  g_object_get(self, "local_https", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_camera_obj_set_local_https(GtkIpcamCameraObj * self, const gboolean val)
{
  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), FALSE);

  g_object_set(self, "local_https", val, NULL);

  return TRUE;
}

gchar *
gtk_ipcam_camera_obj_get_local_url(GtkIpcamCameraObj * self)
{
  gchar *ret;

  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), NULL);

  g_object_get(self, "local_url", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_camera_obj_set_local_url(GtkIpcamCameraObj * self, const gchar* val)
{
  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), FALSE);

  g_object_set(self, "local_url", val, NULL);

  return TRUE;
}

gchar*
gtk_ipcam_camera_obj_get_remote_port(GtkIpcamCameraObj * self)
{
  gchar* ret;

  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), 0);

  g_object_get(self, "remote_port", &ret, NULL);

  return ret;
}

gchar*
gtk_ipcam_camera_obj_get_remote_media_port(GtkIpcamCameraObj * self)
{
  gchar* ret;

  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), 0);

  g_object_get(self, "remote_media_port", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_camera_obj_set_remote_port(GtkIpcamCameraObj * self, const gchar* val)
{
  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), FALSE);

  g_object_set(self, "remote_port", val, NULL);

  return TRUE;
}

gboolean
gtk_ipcam_camera_obj_set_remote_media_port(GtkIpcamCameraObj * self, const gchar* val)
{
  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), FALSE);

  g_object_set(self, "remote_media_port", val, NULL);

  return TRUE;
}

gchar*
gtk_ipcam_camera_obj_get_local_port(GtkIpcamCameraObj * self)
{
  gchar* ret;

  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), 0);

  g_object_get(self, "local_port", &ret, NULL);

  return ret;
}

gchar*
gtk_ipcam_camera_obj_get_local_media_port(GtkIpcamCameraObj * self)
{
  gchar* ret;

  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), 0);

  g_object_get(self, "local_media_port", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_camera_obj_set_local_port(GtkIpcamCameraObj * self, const gchar* val)
{
  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), FALSE);

  g_object_set(self, "local_port", val, NULL);

  return TRUE;
}

gboolean
gtk_ipcam_camera_obj_set_local_media_port(GtkIpcamCameraObj * self, const gchar* val)
{
  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), FALSE);

  g_object_set(self, "local_media_port", val, NULL);

  return TRUE;
}

gchar *
gtk_ipcam_camera_obj_get_username(GtkIpcamCameraObj * self)
{
  gchar *ret;

  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), NULL);

  g_object_get(self, "username", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_camera_obj_set_username(GtkIpcamCameraObj * self, const gchar* val)
{
  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), FALSE);

  g_object_set(self, "username", val, NULL);

  return TRUE;
}

gchar *
gtk_ipcam_camera_obj_get_password(GtkIpcamCameraObj * self)
{
  gchar *ret;

  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), NULL);

  g_object_get(self, "password", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_camera_obj_set_password(GtkIpcamCameraObj * self, const gchar* val)
{
  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), FALSE);

  g_object_set(self, "password", val, NULL);

  return TRUE;
}

gboolean
gtk_ipcam_camera_obj_get_subchannel(GtkIpcamCameraObj * self)
{
  gboolean ret;

  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), TRUE);

  g_object_get(self, "subchannel", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_camera_obj_set_subchannel(GtkIpcamCameraObj * self, const gboolean val)
{
  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_OBJ(self), FALSE);

  g_object_set(self, "subchannel", val, NULL);

  return TRUE;
}

gchar*
gtk_ipcam_camera_obj_get_stream_url(GtkIpcamCameraObj* camera)
{
  return gtk_ipcam_camera_driver_obj_get_stream_url(camera->camera_driver, camera);
}
