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

#ifndef __GTK_IPCAM_CAMERA_GROUP_OBJ_H__
#define __GTK_IPCAM_CAMERA_GROUP_OBJ_H__

#include <gtk/gtk.h>
#include "gtk_ipcam_camera_obj.h"

G_BEGIN_DECLS

typedef struct _GtkIpcamCameraGroupObj
    GtkIpcamCameraGroupObj;
typedef struct _GtkIpcamCameraGroupObjClass
    GtkIpcamCameraGroupObjClass;

#define GTK_TYPE_IPCAM_CAMERA_GROUP_OBJ                     (gtk_ipcam_camera_group_obj_get_type ())
#define GTK_IS_IPCAM_CAMERA_GROUP_OBJ(obj)                  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_IPCAM_CAMERA_GROUP_OBJ))
#define GTK_IS_IPCAM_CAMERA_GROUP_OBJ_CLASS(klass)          (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_IPCAM_CAMERA_GROUP_OBJ))
#define GTK_IPCAM_CAMERA_GROUP_OBJ_GET_CLASS(obj)           (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_IPCAM_CAMERA_GROUP_OBJ, GtkIpcamCameraGroupObjClass))
#define GTK_IPCAM_CAMERA_GROUP_OBJ(obj)                     (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IPCAM_CAMERA_GROUP_OBJ, GtkIpcamCameraGroupObj))
#define GTK_IPCAM_CAMERA_GROUP_OBJ_CLASS(klass)             (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_IPCAM_CAMERA_GROUP_OBJ, GtkIpcamCameraGroupObjClass))
#define GTK_IPCAM_CAMERA_GROUP_OBJ_CAST(obj)                ((GtkIpcamCameraGroupObj*)(obj))


GType gtk_ipcam_camera_group_obj_get_type(void);

GtkIpcamCameraGroupObj* gtk_ipcam_camera_group_obj_new(void);
GType gtk_ipcam_camera_group_obj_get_boxed_type (void);
void gtk_ipcam_camera_group_obj_foreach_camera(GtkIpcamCameraGroupObj* self, GFunc func, gpointer user_data);
GtkIpcamCameraObj* gtk_ipcam_camera_group_obj_get_camera_by_index(GtkIpcamCameraGroupObj* self, guint index);
guint gtk_ipcam_camera_group_obj_get_cameras_count(GtkIpcamCameraGroupObj* self);
gboolean gtk_ipcam_camera_group_obj_change_camera_pos(GtkIpcamCameraGroupObj* self, GtkIpcamCameraObj* camera, gint new_pos);


gchar * gtk_ipcam_camera_group_obj_get_name(GtkIpcamCameraGroupObj * self);
gboolean gtk_ipcam_camera_group_obj_set_name(GtkIpcamCameraGroupObj * self, const gchar* val);
gint gtk_ipcam_camera_group_obj_add_camera(GtkIpcamCameraGroupObj * self, GtkIpcamCameraObj* camera);
gboolean gtk_ipcam_camera_group_obj_del_camera(GtkIpcamCameraGroupObj * self, GtkIpcamCameraObj* camera);

G_END_DECLS

#endif //__GTK_IPCAM_CAMERA_GROUP_OBJ_H__
