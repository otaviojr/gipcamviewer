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

#ifndef __GTK_IPCAM_CAMERA_OBJ_H__
#define __GTK_IPCAM_CAMERA_OBJ_H__

#include <gtk/gtk.h>

#include "gtk_ipcam_camera_driver_obj.h"

G_BEGIN_DECLS

typedef struct _GtkIpcamCameraObj
    GtkIpcamCameraObj;
typedef struct _GtkIpcamCameraObjClass
    GtkIpcamCameraObjClass;

#define GTK_TYPE_IPCAM_CAMERA_OBJ                     (gtk_ipcam_camera_obj_get_type ())
#define GTK_IS_IPCAM_CAMERA_OBJ(obj)                  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_IPCAM_CAMERA_OBJ))
#define GTK_IS_IPCAM_CAMERA_OBJ_CLASS(klass)          (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_IPCAM_CAMERA_OBJ))
#define GTK_IPCAM_CAMERA_OBJ_GET_CLASS(obj)           (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_IPCAM_CAMERA_OBJ, GtkIpcamCameraObjClass))
#define GTK_IPCAM_CAMERA_OBJ(obj)                     (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IPCAM_CAMERA_OBJ, GtkIpcamCameraObj))
#define GTK_IPCAM_CAMERA_OBJ_CLASS(klass)             (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_IPCAM_CAMERA_OBJ, GtkIpcamCameraObjClass))
#define GTK_IPCAM_CAMERA_OBJ_CAST(obj)                ((GtkIpcamCameraObj*)(obj))


GType gtk_ipcam_camera_obj_get_type(void);
GType gtk_ipcam_camera_obj_get_boxed_type (void);

GtkIpcamCameraObj* gtk_ipcam_camera_obj_new(void);

GtkIpcamCameraDriverObj* gtk_ipcam_camera_obj_get_camera_driver(GtkIpcamCameraObj* self);
gboolean gtk_ipcam_camera_obj_is_valid(GtkIpcamCameraObj* self);

/* CAMERA PROPERTIES */
gchar* gtk_ipcam_camera_obj_get_model(GtkIpcamCameraObj * self);
gboolean gtk_ipcam_camera_obj_set_model(GtkIpcamCameraObj * self, const gchar* val);
gchar* gtk_ipcam_camera_obj_get_name(GtkIpcamCameraObj * self);
gboolean gtk_ipcam_camera_obj_set_name(GtkIpcamCameraObj * self, const gchar* val);
gboolean gtk_ipcam_camera_obj_get_remote_https(GtkIpcamCameraObj * self);
gboolean gtk_ipcam_camera_obj_set_remote_https(GtkIpcamCameraObj * self, const gboolean val);
gchar * gtk_ipcam_camera_obj_get_remote_url(GtkIpcamCameraObj * self);
gboolean gtk_ipcam_camera_obj_set_remote_url(GtkIpcamCameraObj * self, const gchar* val);
gboolean gtk_ipcam_camera_obj_get_local_https(GtkIpcamCameraObj * self);
gboolean gtk_ipcam_camera_obj_set_local_https(GtkIpcamCameraObj * self, const gboolean val);
gchar * gtk_ipcam_camera_obj_get_local_url(GtkIpcamCameraObj * self);
gboolean gtk_ipcam_camera_obj_set_local_url(GtkIpcamCameraObj * self, const gchar* val);
gchar* gtk_ipcam_camera_obj_get_remote_port(GtkIpcamCameraObj * self);
gboolean gtk_ipcam_camera_obj_set_remote_port(GtkIpcamCameraObj * self, const gchar* val);
gchar* gtk_ipcam_camera_obj_get_remote_media_port(GtkIpcamCameraObj * self);
gboolean gtk_ipcam_camera_obj_set_remote_media_port(GtkIpcamCameraObj * self, const gchar* val);
gchar* gtk_ipcam_camera_obj_get_local_port(GtkIpcamCameraObj * self);
gboolean gtk_ipcam_camera_obj_set_local_port(GtkIpcamCameraObj * self, const gchar* val);
gchar* gtk_ipcam_camera_obj_get_local_media_port(GtkIpcamCameraObj * self);
gboolean gtk_ipcam_camera_obj_set_local_media_port(GtkIpcamCameraObj * self, const gchar* val);
gchar * gtk_ipcam_camera_obj_get_username(GtkIpcamCameraObj * self);
gboolean gtk_ipcam_camera_obj_set_username(GtkIpcamCameraObj * self, const gchar* val);
gchar * gtk_ipcam_camera_obj_get_password(GtkIpcamCameraObj * self);
gboolean gtk_ipcam_camera_obj_set_password(GtkIpcamCameraObj * self, const gchar* val);
gboolean gtk_ipcam_camera_obj_get_subchannel(GtkIpcamCameraObj * self);
gboolean gtk_ipcam_camera_obj_set_subchannel(GtkIpcamCameraObj * self, const gboolean val);
gboolean gtk_ipcam_camera_obj_get_flip_controls(GtkIpcamCameraObj * self);
gboolean gtk_ipcam_camera_obj_set_flip_controls(GtkIpcamCameraObj * self, const gboolean val);
gboolean gtk_ipcam_camera_obj_get_mirror_controls(GtkIpcamCameraObj * self);
gboolean gtk_ipcam_camera_obj_set_mirror_controls(GtkIpcamCameraObj * self, const gboolean val);
/* CAMERA PROPERTIES END */

/* CAMERA METHODS */
gboolean  gtk_ipcam_camera_obj_init_driver(GtkIpcamCameraObj* camera);
gchar* gtk_ipcam_camera_obj_get_stream_url(GtkIpcamCameraObj* camera);
gboolean gtk_ipcam_camera_obj_move_up(GtkIpcamCameraObj* camera);
gboolean gtk_ipcam_camera_obj_move_down(GtkIpcamCameraObj* camera);
gboolean gtk_ipcam_camera_obj_move_left(GtkIpcamCameraObj* camera);
gboolean gtk_ipcam_camera_obj_move_right(GtkIpcamCameraObj* camera);
gboolean gtk_ipcam_camera_obj_is_flipped(GtkIpcamCameraObj* camera);
gboolean gtk_ipcam_camera_obj_is_mirrored(GtkIpcamCameraObj* camera);
gboolean gtk_ipcam_camera_obj_can_pan(GtkIpcamCameraObj* camera);
gboolean gtk_ipcam_camera_obj_can_tilt(GtkIpcamCameraObj* camera);
/* END CAMERA METHODS */

G_END_DECLS

#endif //__GTK_IPCAM_CAMERA_OBJ_H__
