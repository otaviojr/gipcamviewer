#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <gtk/gtk.h>
#include <json-glib/json-glib.h>

#include "gtk_ipcam_preference_obj.h"

struct _GtkIpcamPreferenceObj
{
  GObject parent;

  GValue width;
  GValue height;
  GValue top;
  GValue left;

  GValue camera_groups;
};

struct _GtkIpcamPreferenceObjClass
{
  GObjectClass parent_class;
};

enum
{
  GTK_IPCAM_PREFERENCE_OBJ_PROP_0,
  GTK_IPCAM_PREFERENCE_OBJ_PROP_WIDTH,
  GTK_IPCAM_PREFERENCE_OBJ_PROP_HEIGHT,
  GTK_IPCAM_PREFERENCE_OBJ_PROP_TOP,
  GTK_IPCAM_PREFERENCE_OBJ_PROP_LEFT,
  GTK_IPCAM_PREFERENCE_OBJ_PROP_CAMERA_GROUPS,
  GTK_IPCAM_PREFERENCE_OBJ_PROP_LAST
};

enum
{
  GTK_IPCAM_PREFERENCE_OBJ_SIGNAL_0,
  GTK_IPCAM_PREFERENCE_OBJ_SIGNAL_CHANGED,
  GTK_IPCAM_PREFERENCE_OBJ_SIGNAL_GROUP_CHANGED,
  GTK_IPCAM_PREFERENCE_OBJ_SIGNAL_LAST
};

G_DEFINE_TYPE (GtkIpcamPreferenceObj, gtk_ipcam_preference_obj, G_TYPE_OBJECT);

static GParamSpec*
gtk_ipcam_preference_obj_param_specs[GTK_IPCAM_PREFERENCE_OBJ_PROP_LAST] = { NULL, };

static guint
gtk_ipcam_preference_obj_signals[GTK_IPCAM_PREFERENCE_OBJ_SIGNAL_LAST] = {0, };

static void
gtk_ipcam_preference_obj_free_camera_groups(gpointer camera_group)
{
  printf("Destroying camera group\n");
  g_object_unref(camera_group);
}

static void
gtk_ipcam_preference_obj_get_property(GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec)
{
  GtkIpcamPreferenceObj *self = GTK_IPCAM_PREFERENCE_OBJ(object);

  switch (prop_id) {
    case GTK_IPCAM_PREFERENCE_OBJ_PROP_WIDTH:
      g_value_set_uint(value, g_value_get_uint(&self->width));
      break;
    case GTK_IPCAM_PREFERENCE_OBJ_PROP_HEIGHT:
      g_value_set_uint(value, g_value_get_uint(&self->height));
      break;
    case GTK_IPCAM_PREFERENCE_OBJ_PROP_TOP:
      g_value_set_uint(value, g_value_get_uint(&self->top));
      break;
    case GTK_IPCAM_PREFERENCE_OBJ_PROP_LEFT:
      g_value_set_uint(value, g_value_get_uint(&self->left));
      break;
    case GTK_IPCAM_PREFERENCE_OBJ_PROP_CAMERA_GROUPS:
      g_value_set_boxed(value, g_value_get_boxed(&self->camera_groups));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void
gtk_ipcam_preference_obj_set_property(GObject * object, guint prop_id, const GValue * value,
    GParamSpec * pspec)
{
  printf("setting up preferences object\n");
  GtkIpcamPreferenceObj *self = (GtkIpcamPreferenceObj *) object;

  switch (prop_id) {
    case GTK_IPCAM_PREFERENCE_OBJ_PROP_WIDTH:
      g_value_set_uint(&self->width, g_value_get_uint(value));
      break;
    case GTK_IPCAM_PREFERENCE_OBJ_PROP_HEIGHT:
      g_value_set_uint(&self->height, g_value_get_uint(value));
      break;
    case GTK_IPCAM_PREFERENCE_OBJ_PROP_TOP:
      g_value_set_uint(&self->top, g_value_get_uint(value));
      break;
    case GTK_IPCAM_PREFERENCE_OBJ_PROP_LEFT:
      g_value_set_uint(&self->left, g_value_get_uint(value));
      break;
    case GTK_IPCAM_PREFERENCE_OBJ_PROP_CAMERA_GROUPS:
      /*
       * We don't have to duplicate it as the json parser will not destroy the
       * local object. So, we have to free it when we destroy this object
       */
      g_value_set_boxed(&self->camera_groups, g_value_get_boxed(value));
      g_ptr_array_set_free_func(g_value_get_boxed(value),gtk_ipcam_preference_obj_free_camera_groups);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gtk_ipcam_preference_obj_finalize(GObject * object)
{
  GtkIpcamPreferenceObj *self = GTK_IPCAM_PREFERENCE_OBJ(object);

  printf("gtk_ipcam_preference_obj_finalize\n");

  g_value_unset(&self->width);
  g_value_unset(&self->height);
  g_value_unset(&self->top);
  g_value_unset(&self->left);

  GPtrArray* array = g_value_get_boxed(&self->camera_groups);
  if(array != NULL){
    g_ptr_array_unref(array);
  }

  g_value_unset(&self->camera_groups);

  G_OBJECT_CLASS(gtk_ipcam_preference_obj_parent_class)->finalize(object);
}

static void
gtk_ipcam_preference_obj_class_init(GtkIpcamPreferenceObjClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = gtk_ipcam_preference_obj_set_property;
  gobject_class->get_property = gtk_ipcam_preference_obj_get_property;
  gobject_class->finalize = gtk_ipcam_preference_obj_finalize;

  gtk_ipcam_preference_obj_param_specs
      [GTK_IPCAM_PREFERENCE_OBJ_PROP_WIDTH] =
      g_param_spec_uint("width", "Width",
      "Last window width.", 0,10000,800,
      G_PARAM_READWRITE);

  gtk_ipcam_preference_obj_param_specs
      [GTK_IPCAM_PREFERENCE_OBJ_PROP_HEIGHT] =
      g_param_spec_uint("height", "Height",
      "Last window height.", 0,10000,600,
      G_PARAM_READWRITE);

  gtk_ipcam_preference_obj_param_specs
      [GTK_IPCAM_PREFERENCE_OBJ_PROP_TOP] =
      g_param_spec_uint("top", "Top",
      "Last window top position.", 0,10000,0,
      G_PARAM_READWRITE);

  gtk_ipcam_preference_obj_param_specs
      [GTK_IPCAM_PREFERENCE_OBJ_PROP_LEFT] =
      g_param_spec_uint("left", "Left",
      "Last window left position.", 0,10000,0,
      G_PARAM_READWRITE);

  gtk_ipcam_preference_obj_param_specs
      [GTK_IPCAM_PREFERENCE_OBJ_PROP_CAMERA_GROUPS] =
      g_param_spec_boxed("camera_groups", "Camera groups", "Group of cameras",
      gtk_ipcam_camera_group_obj_get_boxed_type(),
      G_PARAM_READWRITE | G_PARAM_CONSTRUCT);

  gtk_ipcam_preference_obj_signals[GTK_IPCAM_PREFERENCE_OBJ_SIGNAL_CHANGED] =
      g_signal_newv("changed",
                    G_TYPE_FROM_CLASS(gobject_class),
                    G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    G_TYPE_NONE,
                    0,
                    NULL);

  gtk_ipcam_preference_obj_signals[GTK_IPCAM_PREFERENCE_OBJ_SIGNAL_GROUP_CHANGED] =
      g_signal_newv("group-changed",
                    G_TYPE_FROM_CLASS(gobject_class),
                    G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    G_TYPE_NONE,
                    0,
                    NULL);

  g_object_class_install_properties (gobject_class,
      GTK_IPCAM_PREFERENCE_OBJ_PROP_LAST, gtk_ipcam_preference_obj_param_specs);
}

static const gchar*
gtk_ipcam_preference_obj_get_home_dir(void)
{
  const char *homedir;
  if ((homedir = getenv("HOME")) == NULL) {
      homedir = getpwuid(getuid())->pw_dir;
  }

  return homedir;
}

static void
gtk_ipcam_preference_obj_init(GtkIpcamPreferenceObj * self)
{
  g_value_init(&self->width, G_TYPE_UINT);
  g_value_init(&self->height, G_TYPE_UINT);
  g_value_init(&self->left, G_TYPE_UINT);
  g_value_init(&self->top, G_TYPE_UINT);

  g_value_init(&self->camera_groups, gtk_ipcam_camera_group_obj_get_boxed_type());

  gchar* filename = g_build_filename(gtk_ipcam_preference_obj_get_home_dir(),".gipcamviewer",NULL);
  g_mkdir_with_parents(filename,0770);
  g_free(filename);
}

GtkIpcamPreferenceObj *
gtk_ipcam_preference_obj_new(void)
{
  GtkIpcamPreferenceObj * ret;
  JsonParser* parser = json_parser_new();
  GError* error = NULL;

  gchar* filename = g_build_filename(gtk_ipcam_preference_obj_get_home_dir(),".gipcamviewer","preferences.json",NULL);
  if(json_parser_load_from_file(parser, filename, &error) == TRUE){
    JsonNode* rootNode = json_parser_get_root(parser);
    ret = GTK_IPCAM_PREFERENCE_OBJ(json_gobject_deserialize(GTK_TYPE_IPCAM_PREFERENCE_OBJ, rootNode));
  } else {
    ret = g_object_new(GTK_TYPE_IPCAM_PREFERENCE_OBJ, NULL);
  }

  g_free(filename);
  g_object_unref(parser);
  return ret;
}

gboolean
gtk_ipcam_preference_obj_save(GtkIpcamPreferenceObj* self, gboolean notify)
{
  gboolean ret;
  JsonNode* root = json_gobject_serialize(G_OBJECT(self));
  gchar* file_content = json_to_string(root,TRUE);
  GError* error = NULL;
  ret = g_file_set_contents(g_build_filename(gtk_ipcam_preference_obj_get_home_dir(),".gipcamviewer","preferences.json",NULL), file_content, -1, &error);
  if(!ret){
    GtkWidget* dialog;
    dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_application_get_active_window(GTK_APPLICATION (g_application_get_default()))),
                                      GTK_DIALOG_DESTROY_WITH_PARENT,
                                      GTK_MESSAGE_ERROR,
                                      GTK_BUTTONS_CLOSE,
                                      "ERROR saving preference file\n%s",
                                      error->message);
     gtk_dialog_run(GTK_DIALOG(dialog));
     gtk_widget_destroy(dialog);
     g_free(error);
  }

  if(notify)
  {
    g_signal_emit(self, gtk_ipcam_preference_obj_signals[GTK_IPCAM_PREFERENCE_OBJ_SIGNAL_CHANGED], 0);
  }
  return ret;
}

guint
gtk_ipcam_preference_obj_get_width(GtkIpcamPreferenceObj * self)
{
  guint ret;

  g_return_val_if_fail (GTK_IS_IPCAM_PREFERENCE_OBJ(self), 0);

  g_object_get(self, "width", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_preference_obj_set_width(GtkIpcamPreferenceObj * self, const guint val)
{
  g_return_val_if_fail (GTK_IS_IPCAM_PREFERENCE_OBJ(self), FALSE);

  g_object_set(self, "width", val, NULL);

  return TRUE;
}

guint
gtk_ipcam_preference_obj_get_height(GtkIpcamPreferenceObj * self)
{
  guint ret;

  g_return_val_if_fail (GTK_IS_IPCAM_PREFERENCE_OBJ(self), 0);

  g_object_get(self, "height", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_preference_obj_set_height(GtkIpcamPreferenceObj * self, const guint val)
{
  g_return_val_if_fail (GTK_IS_IPCAM_PREFERENCE_OBJ(self), FALSE);

  g_object_set(self, "height", val, NULL);

  return TRUE;
}

guint
gtk_ipcam_preference_obj_get_left(GtkIpcamPreferenceObj * self)
{
  guint ret;

  g_return_val_if_fail (GTK_IS_IPCAM_PREFERENCE_OBJ(self), 0);

  g_object_get(self, "left", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_preference_obj_set_left(GtkIpcamPreferenceObj * self, const guint val)
{
  g_return_val_if_fail (GTK_IS_IPCAM_PREFERENCE_OBJ(self), FALSE);

  g_object_set(self, "left", val, NULL);

  return TRUE;
}

guint
gtk_ipcam_preference_obj_get_top(GtkIpcamPreferenceObj * self)
{
  guint ret;

  g_return_val_if_fail (GTK_IS_IPCAM_PREFERENCE_OBJ(self), 0);

  g_object_get(self, "top", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_preference_obj_set_top(GtkIpcamPreferenceObj * self, const guint val)
{
  g_return_val_if_fail (GTK_IS_IPCAM_PREFERENCE_OBJ(self), FALSE);

  g_object_set(self, "top", val, NULL);

  return TRUE;
}

gboolean
gtk_ipcam_preference_obj_add_camera_group(GtkIpcamPreferenceObj * self, const gchar* group_name)
{
  GPtrArray* array = g_value_get_boxed(&self->camera_groups);

  if(array == NULL){
    array = g_ptr_array_new();
    g_ptr_array_set_free_func(array,gtk_ipcam_preference_obj_free_camera_groups);
    g_value_set_boxed(&self->camera_groups, array);
  }

  g_assert(array != NULL);

  GtkIpcamCameraGroupObj* camera_group = gtk_ipcam_camera_group_obj_new();
  gtk_ipcam_camera_group_obj_set_name(camera_group,group_name);
  g_ptr_array_add(array,camera_group);
  g_signal_emit(self, gtk_ipcam_preference_obj_signals[GTK_IPCAM_PREFERENCE_OBJ_SIGNAL_GROUP_CHANGED], 0);
  return TRUE;
}

guint
gtk_ipcam_preference_obj_get_camera_group_count(GtkIpcamPreferenceObj* self)
{
  GPtrArray* array = g_value_get_boxed(&self->camera_groups);
  if(array == NULL) return 0;
  return array->len;
}

void
gtk_ipcam_preference_obj_foreach_camera_group(GtkIpcamPreferenceObj* self, GFunc func, gpointer user_data)
{
  GPtrArray* array = g_value_get_boxed(&self->camera_groups);
  if(array == NULL) return;
  g_ptr_array_foreach(array, func, user_data);
}

GtkIpcamCameraGroupObj*
gtk_ipcam_preference_obj_get_camera_group_by_index(GtkIpcamPreferenceObj* self, guint index)
{
  GPtrArray* array = g_value_get_boxed(&self->camera_groups);
  if(array == NULL) return NULL;
  return g_ptr_array_index(array, index);
}

gboolean
gtk_ipcam_preference_obj_del_group(GtkIpcamPreferenceObj * self, GtkIpcamCameraGroupObj* group)
{
  GPtrArray* array = g_value_get_boxed(&self->camera_groups);

  if(array == NULL)
  {
    return FALSE;
  }

  g_ptr_array_remove(array, group);
  g_signal_emit(self, gtk_ipcam_preference_obj_signals[GTK_IPCAM_PREFERENCE_OBJ_SIGNAL_GROUP_CHANGED], 0);
  g_object_unref(group);
  return TRUE;
}
