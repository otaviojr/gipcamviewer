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

#ifndef __GTK_IPCAM_CAMERA_DRIVER_OBJ_H__
#define __GTK_IPCAM_CAMERA_DRIVER_OBJ_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _GtkIpcamCameraObj GtkIpcamCameraObj;

typedef struct _GtkIpcamCameraDriverObj
    GtkIpcamCameraDriverObj;
typedef struct _GtkIpcamCameraDriverObjClass
    GtkIpcamCameraDriverObjClass;

#define GTK_TYPE_IPCAM_CAMERA_DRIVER_OBJ                     (gtk_ipcam_camera_driver_obj_get_type ())
#define GTK_IS_IPCAM_CAMERA_DRIVER_OBJ(obj)                  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_IPCAM_CAMERA_DRIVER_OBJ))
#define GTK_IS_IPCAM_CAMERA_DRIVER_OBJ_CLASS(klass)          (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_IPCAM_CAMERA_DRIVER_OBJ))
#define GTK_IPCAM_CAMERA_DRIVER_OBJ_GET_CLASS(obj)           (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_IPCAM_CAMERA_DRIVER_OBJ, GtkIpcamCameraDriverObjClass))
#define GTK_IPCAM_CAMERA_DRIVER_OBJ(obj)                     (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IPCAM_CAMERA_DRIVER_OBJ, GtkIpcamCameraDriverObj))
#define GTK_IPCAM_CAMERA_DRIVER_OBJ_CLASS(klass)             (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_IPCAM_CAMERA_DRIVER_OBJ, GtkIpcamCameraDriverObjClass))
#define GTK_IPCAM_CAMERA_DRIVER_OBJ_CAST(obj)                ((GtkIpcamCameraDriverObj*)(obj))

GType gtk_ipcam_camera_driver_obj_get_type(void);

GtkIpcamCameraDriverObj* gtk_ipcam_camera_driver_obj_new(const gchar* driver);

GPtrArray* gtk_ipcam_camera_driver_obj_list();
void gtk_ipcam_camera_driver_obj_list_free(GPtrArray* list);

/* Camera Driver properties */
gchar* gtk_ipcam_camera_driver_obj_get_driver_name(GtkIpcamCameraDriverObj * self);

gchar* gtk_ipcam_camera_driver_obj_get_model(GtkIpcamCameraDriverObj * self);
gboolean gtk_ipcam_camera_driver_obj_set_model(GtkIpcamCameraDriverObj * self, const guint val);
gchar* gtk_ipcam_camera_driver_obj_get_protocol(GtkIpcamCameraDriverObj * self);
gboolean gtk_ipcam_camera_driver_obj_set_protocol(GtkIpcamCameraDriverObj * self, const guint val);
gboolean gtk_ipcam_camera_driver_obj_get_has_ptz(GtkIpcamCameraDriverObj * self);
gboolean gtk_ipcam_camera_driver_obj_set_has_ptz(GtkIpcamCameraDriverObj * self, const gboolean val);

gboolean gtk_ipcam_camera_driver_obj_init_driver(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera);
gchar* gtk_ipcam_camera_driver_obj_get_stream_url(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera);
gboolean gtk_ipcam_camera_driver_obj_move_up(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera);
gboolean gtk_ipcam_camera_driver_obj_move_down(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera);
gboolean gtk_ipcam_camera_driver_obj_move_left(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera);
gboolean gtk_ipcam_camera_driver_obj_move_right(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera);
gboolean gtk_ipcam_camera_driver_obj_is_flipped(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera);
gboolean gtk_ipcam_camera_driver_obj_is_mirrored(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera);
gboolean gtk_ipcam_camera_driver_obj_can_tilt(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera);
gboolean gtk_ipcam_camera_driver_obj_can_pan(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera);

G_END_DECLS

#endif //__GTK_IPCAM_CAMERA_DRIVER_OBJ_H__
