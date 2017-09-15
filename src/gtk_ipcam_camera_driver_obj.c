#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include <gtk/gtk.h>
#include <json-glib/json-glib.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "gtk_ipcam_camera_driver_obj.h"
#include "gtk_ipcam_camera_obj.h"

#include "config.h"

struct _GtkIpcamCameraDriverObj
{
  GObject parent;

  GValue model;
  GValue protocol;

  GValue driver_name;
};

struct _GtkIpcamCameraDriverObjClass
{
  GObjectClass parent_class;
};

enum
{
  GTK_FOSCAM_CAMERA_DRIVER_OBJ_PROP_0,
  GTK_FOSCAM_CAMERA_DRIVER_OBJ_PROP_DRIVER_NAME,
  GTK_FOSCAM_CAMERA_DRIVER_OBJ_PROP_PROTOCOL,
  GTK_FOSCAM_CAMERA_DRIVER_OBJ_PROP_MODEL,
  GTK_FOSCAM_CAMERA_DRIVER_OBJ_PROP_LAST
};

G_DEFINE_TYPE (GtkIpcamCameraDriverObj, gtk_ipcam_camera_driver_obj, G_TYPE_OBJECT);

static GParamSpec
* gtk_ipcam_camera_driver_obj_param_specs[GTK_FOSCAM_CAMERA_DRIVER_OBJ_PROP_LAST] = { NULL, };

static void
gtk_ipcam_camera_driver_obj_get_property(GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec)
{
  GtkIpcamCameraDriverObj *self = GTK_FOSCAM_CAMERA_DRIVER_OBJ(object);

  switch (prop_id) {
    case GTK_FOSCAM_CAMERA_DRIVER_OBJ_PROP_DRIVER_NAME:
      g_value_set_string(value, g_value_get_string(&self->driver_name));
      break;
    case GTK_FOSCAM_CAMERA_DRIVER_OBJ_PROP_PROTOCOL:
      g_value_set_string(value, g_value_get_string(&self->protocol));
      break;
    case GTK_FOSCAM_CAMERA_DRIVER_OBJ_PROP_MODEL:
      g_value_set_string(value, g_value_get_string(&self->model));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void
gtk_ipcam_camera_driver_obj_set_property(GObject * object, guint prop_id, const GValue * value,
    GParamSpec * pspec)
{
  GtkIpcamCameraDriverObj *self = GTK_FOSCAM_CAMERA_DRIVER_OBJ(object);

  switch (prop_id) {
    case GTK_FOSCAM_CAMERA_DRIVER_OBJ_PROP_MODEL:
      g_value_set_string(&self->model, g_value_dup_string(value));
      break;
    case GTK_FOSCAM_CAMERA_DRIVER_OBJ_PROP_PROTOCOL:
      g_value_set_string(&self->protocol, g_value_dup_string(value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gtk_ipcam_camera_driver_obj_finalize(GObject * object)
{
  GtkIpcamCameraDriverObj *self = GTK_FOSCAM_CAMERA_DRIVER_OBJ(object);

  printf("gtk_ipcam_camera_driver_obj_finalize\n");

  g_value_unset(&self->driver_name);
  g_value_unset(&self->protocol);
  g_value_unset(&self->model);

  G_OBJECT_CLASS(gtk_ipcam_camera_driver_obj_parent_class)->finalize(object);
}

static void
gtk_ipcam_camera_driver_obj_class_init(GtkIpcamCameraDriverObjClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = gtk_ipcam_camera_driver_obj_set_property;
  gobject_class->get_property = gtk_ipcam_camera_driver_obj_get_property;
  gobject_class->finalize = gtk_ipcam_camera_driver_obj_finalize;

  gtk_ipcam_camera_driver_obj_param_specs
      [GTK_FOSCAM_CAMERA_DRIVER_OBJ_PROP_DRIVER_NAME] =
      g_param_spec_string("driver_name", "Driver  Name",
      "Internal name of the driver", NULL,
      G_PARAM_READABLE);

  gtk_ipcam_camera_driver_obj_param_specs
      [GTK_FOSCAM_CAMERA_DRIVER_OBJ_PROP_PROTOCOL] =
      g_param_spec_string("protocol", "Protocol",
      "Driver primary protocol", NULL,
      G_PARAM_READWRITE);

  gtk_ipcam_camera_driver_obj_param_specs
      [GTK_FOSCAM_CAMERA_DRIVER_OBJ_PROP_MODEL] =
      g_param_spec_string("model", "Driver Model",
      "Driver Model", NULL,
      G_PARAM_READWRITE);

  g_object_class_install_properties (gobject_class,
      GTK_FOSCAM_CAMERA_DRIVER_OBJ_PROP_LAST, gtk_ipcam_camera_driver_obj_param_specs);
}

static const gchar*
gtk_ipcam_camera_driver_obj_get_drivers_dir(void)
{
  return g_build_filename(PREFIX,DATADIR,"gipcamviewer","drivers",NULL);
}

static void
gtk_ipcam_camera_driver_obj_init(GtkIpcamCameraDriverObj * self)
{
  g_value_init(&self->driver_name, G_TYPE_STRING);
  g_value_init(&self->protocol, G_TYPE_STRING);
  g_value_init(&self->model, G_TYPE_STRING);
}

GtkIpcamCameraDriverObj *
gtk_ipcam_camera_driver_obj_new(const gchar* driver)
{
  GtkIpcamCameraDriverObj * ret = NULL;
  GError* error;
  JsonParser* parser = json_parser_new();
  gchar* filename = g_strconcat(driver,".json",NULL);
  gchar* driver_path = g_build_filename(gtk_ipcam_camera_driver_obj_get_drivers_dir(),filename,NULL);

  printf("driver path: %s\n",driver_path);
  if(g_file_test(driver_path, G_FILE_TEST_EXISTS) == TRUE)
  {
    if(json_parser_load_from_file(parser, driver_path, &error) == TRUE){
      JsonNode* rootNode = json_parser_get_root(parser);
      ret = GTK_FOSCAM_CAMERA_DRIVER_OBJ(json_gobject_deserialize(GTK_TYPE_FOSCAM_CAMERA_DRIVER_OBJ, rootNode));
      if(ret != NULL)
      {
        g_value_set_string(&ret->driver_name,driver);
      }
    }
  }
  g_free(driver_path);
  g_free(filename);
  g_object_unref(parser);

  return ret;
}

static lua_State*
gtk_ipcam_camera_driver_obj_init_lua(GtkIpcamCameraDriverObj* self, GtkIpcamCameraObj* camera)
{
  lua_State* l;
  gchar* filename = g_strconcat(g_value_get_string(&self->driver_name),".lua",NULL);
  gchar* driver_path = g_build_filename(gtk_ipcam_camera_driver_obj_get_drivers_dir(),filename,NULL);

  printf("lua driver path: %s\n",driver_path);
  if(g_file_test(driver_path, G_FILE_TEST_EXISTS) == TRUE)
  {
    l = luaL_newstate();
    luaL_openlibs(l);

    if(luaL_dofile(l, driver_path))
    {
      printf("Couldn't load file: %s\n", lua_tostring(l, -1));
      lua_close(l);
      l=NULL;
    }
  }
  else
  {
    printf("Couldn't load lua driver file: %s\n", driver_path);
    l=NULL;
  }
  g_free(driver_path);
  g_free(filename);

  if(l == NULL) return NULL;

  lua_pushstring(l,gtk_ipcam_camera_obj_get_name(camera));
  lua_setglobal(l, "camera_name");
  lua_pushstring(l,gtk_ipcam_camera_obj_get_remote_url(camera));
  lua_setglobal(l, "camera_remote_url");
  lua_pushstring(l,gtk_ipcam_camera_obj_get_remote_port(camera));
  lua_setglobal(l, "camera_remote_port");
  lua_pushstring(l,gtk_ipcam_camera_obj_get_remote_media_port(camera));
  lua_setglobal(l, "camera_remote_media_port");
  lua_pushstring(l,gtk_ipcam_camera_obj_get_local_url(camera));
  lua_setglobal(l, "camera_local_url");
  lua_pushstring(l,gtk_ipcam_camera_obj_get_local_port(camera));
  lua_setglobal(l, "camera_local_port");
  lua_pushstring(l,gtk_ipcam_camera_obj_get_local_media_port(camera));
  lua_setglobal(l, "camera_local_media_port");
  lua_pushstring(l,gtk_ipcam_camera_obj_get_username(camera));
  lua_setglobal(l, "camera_username");
  lua_pushstring(l,gtk_ipcam_camera_obj_get_password(camera));
  lua_setglobal(l, "camera_password");
  lua_pushboolean(l,gtk_ipcam_camera_obj_get_remote_https(camera));
  lua_setglobal(l, "camera_remote_https");
  lua_pushboolean(l,gtk_ipcam_camera_obj_get_local_https(camera));
  lua_setglobal(l, "camera_local_https");
  lua_pushboolean(l,gtk_ipcam_camera_obj_get_subchannel(camera));
  lua_setglobal(l, "camera_subchannel");

  return l;
}


gchar*
gtk_ipcam_camera_driver_obj_get_driver_name(GtkIpcamCameraDriverObj * self)
{
  gchar* ret;

  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_DRIVER_OBJ(self), 0);

  g_object_get(self, "driver_name", &ret, NULL);

  return ret;
}

gchar*
gtk_ipcam_camera_driver_obj_get_protocol(GtkIpcamCameraDriverObj * self)
{
  gchar* ret;

  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_DRIVER_OBJ(self), 0);

  g_object_get(self, "protocol", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_camera_driver_obj_set_protocol(GtkIpcamCameraDriverObj * self, const guint val)
{
  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_DRIVER_OBJ(self), FALSE);

  g_object_set(self, "protocol", val, NULL);

  return TRUE;
}

gchar*
gtk_ipcam_camera_driver_obj_get_model(GtkIpcamCameraDriverObj * self)
{
  gchar* ret;

  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_DRIVER_OBJ(self), 0);

  g_object_get(self, "model", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_camera_driver_obj_set_model(GtkIpcamCameraDriverObj * self, const guint val)
{
  g_return_val_if_fail (GTK_IS_FOSCAM_CAMERA_DRIVER_OBJ(self), FALSE);

  g_object_set(self, "model", val, NULL);

  return TRUE;
}

gchar*
gtk_ipcam_camera_driver_obj_get_stream_url(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera)
{
  lua_State* l = gtk_ipcam_camera_driver_obj_init_lua(self, camera);
  gchar* ret = NULL;

  if(l!= NULL)
  {
    lua_getglobal(l, "driver_get_stream_url");
    if(lua_pcall(l,0,1,0) != 0)
    {
      printf("error running function `driver_get_stream_url': %s\n", lua_tostring(l, -1));
      return NULL;
    }

    if(lua_isstring(l, -1))
    {
      ret = g_strdup(lua_tostring(l,-1));
      printf("`driver_get_stream_url' returned %s\n", ret);
    }
    lua_pop(l,1);
  }
  lua_close(l);
  return ret;
}

GPtrArray*
gtk_ipcam_camera_driver_obj_list()
{
  const gchar* filename;
  GPtrArray* ret;
  GDir* dir;
  GError *error;
  gchar* token;
  GtkIpcamCameraDriverObj* driver;

  dir = g_dir_open(gtk_ipcam_camera_driver_obj_get_drivers_dir(),0,&error);

  g_assert(error != NULL);

  ret = g_ptr_array_new();

  while ((filename = g_dir_read_name(dir))){
    token = g_strrstr(filename,".");
    *token = 0;
    driver = gtk_ipcam_camera_driver_obj_new(filename);
    if(driver != NULL)
    {
      printf("Adding driver: %s\n", filename);
      g_ptr_array_add(ret,driver);
    }
    else
    {
      printf("Error adding driver: %s\n", filename);
    }
  }

  return ret;
}

static void
gtk_ipcam_camera_driver_obj_list_free_item(gpointer driver) {
  g_object_unref(driver);
}

void
gtk_ipcam_camera_driver_obj_list_free(GPtrArray* list)
{
  g_assert(list != NULL);
  g_ptr_array_set_free_func(list,gtk_ipcam_camera_driver_obj_list_free_item);
  g_ptr_array_unref(list);
}
