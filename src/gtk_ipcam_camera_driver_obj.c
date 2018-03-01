/*
 * Copyright (c) 2017 - Otávio Ribeiro <otavio.ribeiro@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

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
  GValue has_ptz;

  GValue base_url;
  GValue base_media_url;
};

struct _GtkIpcamCameraDriverObjClass
{
  GObjectClass parent_class;
};

enum
{
  GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_0,
  GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_DRIVER_NAME,
  GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_PROTOCOL,
  GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_MODEL,
  GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_HAS_PTZ,
  GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_LAST
};

G_DEFINE_TYPE (GtkIpcamCameraDriverObj, gtk_ipcam_camera_driver_obj, G_TYPE_OBJECT);

static GParamSpec
* gtk_ipcam_camera_driver_obj_param_specs[GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_LAST] = { NULL, };

static void
gtk_ipcam_camera_driver_obj_get_property(GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec)
{
  GtkIpcamCameraDriverObj *self = GTK_IPCAM_CAMERA_DRIVER_OBJ(object);

  switch (prop_id) {
    case GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_DRIVER_NAME:
      g_value_set_string(value, g_value_get_string(&self->driver_name));
      break;
    case GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_PROTOCOL:
      g_value_set_string(value, g_value_get_string(&self->protocol));
      break;
    case GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_MODEL:
      g_value_set_string(value, g_value_get_string(&self->model));
      break;
      case GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_HAS_PTZ:
        g_value_set_boolean(value, g_value_get_boolean(&self->has_ptz));
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
  GtkIpcamCameraDriverObj *self = GTK_IPCAM_CAMERA_DRIVER_OBJ(object);

  switch (prop_id) {
    case GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_MODEL:
      g_value_set_string(&self->model, g_value_dup_string(value));
      break;
    case GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_PROTOCOL:
      g_value_set_string(&self->protocol, g_value_dup_string(value));
      break;
      case GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_HAS_PTZ:
        g_value_set_boolean(&self->has_ptz, g_value_get_boolean(value));
        break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gtk_ipcam_camera_driver_obj_finalize(GObject * object)
{
  GtkIpcamCameraDriverObj *self = GTK_IPCAM_CAMERA_DRIVER_OBJ(object);

  printf("gtk_ipcam_camera_driver_obj_finalize\n");

  g_value_unset(&self->driver_name);
  g_value_unset(&self->protocol);
  g_value_unset(&self->model);
  g_value_unset(&self->has_ptz);
  g_value_unset(&self->base_url);
  g_value_unset(&self->base_media_url);

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
      [GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_DRIVER_NAME] =
      g_param_spec_string("driver_name", "Driver  Name",
      "Internal name of the driver", NULL,
      G_PARAM_READABLE);

  gtk_ipcam_camera_driver_obj_param_specs
      [GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_PROTOCOL] =
      g_param_spec_string("protocol", "Protocol",
      "Driver primary protocol", NULL,
      G_PARAM_READWRITE);

  gtk_ipcam_camera_driver_obj_param_specs
      [GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_MODEL] =
      g_param_spec_string("model", "Driver Model",
      "Driver Model", NULL,
      G_PARAM_READWRITE);

  gtk_ipcam_camera_driver_obj_param_specs
      [GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_HAS_PTZ] =
      g_param_spec_boolean("has_ptz", "Has PTZ",
      "If this cameras support ptz", FALSE,
      G_PARAM_READWRITE);

  g_object_class_install_properties (gobject_class,
      GTK_IPCAM_CAMERA_DRIVER_OBJ_PROP_LAST, gtk_ipcam_camera_driver_obj_param_specs);
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
  g_value_init(&self->has_ptz, G_TYPE_BOOLEAN);
  g_value_init(&self->base_url, G_TYPE_STRING);
  g_value_init(&self->base_media_url, G_TYPE_STRING);

  g_value_set_boolean(&self->has_ptz, FALSE);
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
      ret = GTK_IPCAM_CAMERA_DRIVER_OBJ(json_gobject_deserialize(GTK_TYPE_IPCAM_CAMERA_DRIVER_OBJ, rootNode));
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

static int lua_sleep(lua_State *l)
{
    int m = luaL_checknumber(l,1);
    usleep(m * 1000);
    return 0;
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

  /* exporting functions to lua */
  lua_pushcfunction(l, lua_sleep);
  lua_setglobal(l, "sleep");
  return l;
}


gchar*
gtk_ipcam_camera_driver_obj_get_driver_name(GtkIpcamCameraDriverObj * self)
{
  gchar* ret;

  g_return_val_if_fail (GTK_IS_IPCAM_CAMERA_DRIVER_OBJ(self), 0);

  g_object_get(self, "driver_name", &ret, NULL);

  return ret;
}

gchar*
gtk_ipcam_camera_driver_obj_get_protocol(GtkIpcamCameraDriverObj * self)
{
  gchar* ret;

  g_return_val_if_fail (GTK_IS_IPCAM_CAMERA_DRIVER_OBJ(self), 0);

  g_object_get(self, "protocol", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_camera_driver_obj_set_protocol(GtkIpcamCameraDriverObj * self, const guint val)
{
  g_return_val_if_fail (GTK_IS_IPCAM_CAMERA_DRIVER_OBJ(self), FALSE);

  g_object_set(self, "protocol", val, NULL);

  return TRUE;
}

gchar*
gtk_ipcam_camera_driver_obj_get_model(GtkIpcamCameraDriverObj * self)
{
  gchar* ret;

  g_return_val_if_fail (GTK_IS_IPCAM_CAMERA_DRIVER_OBJ(self), 0);

  g_object_get(self, "model", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_camera_driver_obj_set_model(GtkIpcamCameraDriverObj * self, const guint val)
{
  g_return_val_if_fail (GTK_IS_IPCAM_CAMERA_DRIVER_OBJ(self), FALSE);

  g_object_set(self, "model", val, NULL);

  return TRUE;
}

gboolean
gtk_ipcam_camera_driver_obj_get_has_ptz(GtkIpcamCameraDriverObj * self)
{
  gboolean ret;

  g_return_val_if_fail (GTK_IS_IPCAM_CAMERA_DRIVER_OBJ(self), 0);

  g_object_get(self, "has_ptz", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_camera_driver_obj_set_has_ptz(GtkIpcamCameraDriverObj * self, const gboolean val)
{
  g_return_val_if_fail (GTK_IS_IPCAM_CAMERA_DRIVER_OBJ(self), FALSE);

  g_object_set(self, "has_ptz", val, NULL);

  return TRUE;
}

gboolean
gtk_ipcam_camera_driver_obj_init_driver(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera)
{
  lua_State* l = gtk_ipcam_camera_driver_obj_init_lua(self, camera);
  gboolean ret = FALSE;

  if(l!= NULL)
  {
    lua_getglobal(l, "driver_get_base_url");
    if(lua_pcall(l,0,2,0) != 0)
    {
      printf("error running function `driver_get_base_url': %s\n", lua_tostring(l, -1));
    }
    else
    {
      if(lua_isstring(l, -1))
      {
        g_value_reset(&self->base_url);
        g_value_set_string(&self->base_url, lua_tostring(l,-1));
        printf("`driver_get_base_url' returned url %s\n", g_value_get_string(&self->base_url));
        ret = TRUE;
      }

      if(lua_isstring(l, -2))
      {
        g_value_reset(&self->base_media_url);
        g_value_set_string(&self->base_media_url, lua_tostring(l,-2));
        printf("`driver_get_base_url' returned media url %s\n", g_value_get_string(&self->base_media_url));
        ret = TRUE;
      }
      lua_pop(l,2);
    }
    lua_close(l);
  }
  return ret;
}

gchar*
gtk_ipcam_camera_driver_obj_get_stream_url(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera)
{
  lua_State* l = gtk_ipcam_camera_driver_obj_init_lua(self, camera);
  gchar* ret = NULL;

  if(l!= NULL)
  {
    lua_getglobal(l, "driver_get_stream_url");
    lua_pushstring(l,g_value_get_string(&self->base_url));
    lua_pushstring(l,g_value_get_string(&self->base_media_url));
    if(lua_pcall(l,2,1,0) != 0)
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
    lua_close(l);
  }
  return ret;
}

gboolean
gtk_ipcam_camera_driver_obj_move_up(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera)
{
  lua_State* l = gtk_ipcam_camera_driver_obj_init_lua(self, camera);
  gboolean ret = FALSE;

  if(l!= NULL)
  {
    lua_getglobal(l, "driver_move_up");
    lua_pushstring(l,g_value_get_string(&self->base_url));
    lua_pushstring(l,g_value_get_string(&self->base_media_url));
    if(lua_pcall(l,2,1,0) != 0)
    {
      printf("error running function `driver_move_up': %s\n", lua_tostring(l, -1));
      return ret;
    }

    if(lua_isboolean(l, -1))
    {
      ret = lua_toboolean(l,-1);
      printf("`driver_move_up' returned %i\n", ret);
    }
    lua_pop(l,1);
    lua_close(l);
  }
  return ret;
}

gboolean
gtk_ipcam_camera_driver_obj_move_down(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera)
{
  lua_State* l = gtk_ipcam_camera_driver_obj_init_lua(self, camera);
  gboolean ret = FALSE;

  if(l!= NULL)
  {
    lua_getglobal(l, "driver_move_down");
    lua_pushstring(l,g_value_get_string(&self->base_url));
    lua_pushstring(l,g_value_get_string(&self->base_media_url));
    if(lua_pcall(l,2,1,0) != 0)
    {
      printf("error running function `driver_move_down': %s\n", lua_tostring(l, -1));
      return ret;
    }

    if(lua_isboolean(l, -1))
    {
      ret = lua_toboolean(l,-1);
      printf("`driver_move_down' returned %i\n", ret);
    }
    lua_pop(l,1);
    lua_close(l);
  }
  return ret;
}

gboolean
gtk_ipcam_camera_driver_obj_move_left(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera)
{
  lua_State* l = gtk_ipcam_camera_driver_obj_init_lua(self, camera);
  gboolean ret = FALSE;

  if(l!= NULL)
  {
    lua_getglobal(l, "driver_move_left");
    lua_pushstring(l,g_value_get_string(&self->base_url));
    lua_pushstring(l,g_value_get_string(&self->base_media_url));
    if(lua_pcall(l,2,1,0) != 0)
    {
      printf("error running function `driver_move_left': %s\n", lua_tostring(l, -1));
      return ret;
    }

    if(lua_isboolean(l, -1))
    {
      ret = lua_toboolean(l,-1);
      printf("`driver_move_left' returned %i\n", ret);
    }
    lua_pop(l,1);
    lua_close(l);
  }
  return ret;
}

gboolean
gtk_ipcam_camera_driver_obj_move_right(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera)
{
  lua_State* l = gtk_ipcam_camera_driver_obj_init_lua(self, camera);
  gboolean ret = FALSE;

  if(l!= NULL)
  {
    lua_getglobal(l, "driver_move_right");
    lua_pushstring(l,g_value_get_string(&self->base_url));
    lua_pushstring(l,g_value_get_string(&self->base_media_url));
    if(lua_pcall(l,2,1,0) != 0)
    {
      printf("error running function `driver_move_right': %s\n", lua_tostring(l, -1));
      return ret;
    }

    if(lua_isboolean(l, -1))
    {
      ret = lua_toboolean(l,-1);
      printf("`driver_move_right' returned %i\n", ret);
    }
    lua_pop(l,1);
    lua_close(l);
  }
  return ret;
}

gboolean
gtk_ipcam_camera_driver_obj_is_flipped(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera)
{
  lua_State* l = gtk_ipcam_camera_driver_obj_init_lua(self, camera);
  gboolean ret = FALSE;

  if(l!= NULL)
  {
    lua_getglobal(l, "driver_is_flipped");
    lua_pushstring(l,g_value_get_string(&self->base_url));
    lua_pushstring(l,g_value_get_string(&self->base_media_url));
    if(lua_pcall(l,2,1,0) != 0)
    {
      printf("error running function `driver_is_flipped': %s\n", lua_tostring(l, -1));
      return ret;
    }

    if(lua_isboolean(l, -1))
    {
      ret = lua_toboolean(l,-1);
      printf("`driver_is_flipped' returned %i\n", ret);
    }
    lua_pop(l,1);
    lua_close(l);
  }
  return ret;
}

gboolean
gtk_ipcam_camera_driver_obj_is_mirrored(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera)
{
  lua_State* l = gtk_ipcam_camera_driver_obj_init_lua(self, camera);
  gboolean ret = FALSE;

  if(l!= NULL)
  {
    lua_getglobal(l, "driver_is_mirrored");
    lua_pushstring(l,g_value_get_string(&self->base_url));
    lua_pushstring(l,g_value_get_string(&self->base_media_url));
    if(lua_pcall(l,2,1,0) != 0)
    {
      printf("error running function `driver_is_mirrored': %s\n", lua_tostring(l, -1));
      return ret;
    }

    if(lua_isboolean(l, -1))
    {
      ret = lua_toboolean(l,-1);
      printf("`driver_is_mirrored' returned %i\n", ret);
    }
    lua_pop(l,1);
    lua_close(l);
  }
  return ret;
}

gboolean
gtk_ipcam_camera_driver_obj_can_tilt(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera)
{
  gboolean ret = FALSE;
  if(g_value_get_boolean(&self->has_ptz))
  {
    lua_State* l = gtk_ipcam_camera_driver_obj_init_lua(self, camera);

    if(l!= NULL)
    {
      lua_getglobal(l, "driver_move_down");
      lua_getglobal(l, "driver_move_up");
      if( lua_isfunction(l,-1) && lua_isfunction(l,-2))
      {
        ret=TRUE;
      }
      lua_close(l);
    }
  }
  return ret;
}

gboolean
gtk_ipcam_camera_driver_obj_can_pan(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera)
{
  gboolean ret = FALSE;
  if(g_value_get_boolean(&self->has_ptz))
  {
    lua_State* l = gtk_ipcam_camera_driver_obj_init_lua(self, camera);

    if(l!= NULL)
    {
      lua_getglobal(l, "driver_move_left");
      lua_getglobal(l, "driver_move_right");
      if( lua_isfunction(l,-1) && lua_isfunction(l,-2))
      {
          ret=TRUE;
      }
      lua_close(l);
    }
  }
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
    if(g_strcmp0(++token,"json") == 0)
    {
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
