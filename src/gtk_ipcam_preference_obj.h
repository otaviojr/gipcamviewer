#ifndef __GTK_FOSCAM_PREFERENCE_OBJ_H__
#define __GTK_FOSCAM_PREFERENCE_OBJ_H__

#include <gtk/gtk.h>

#include "gtk_ipcam_camera_group_obj.h"
#include "gtk_ipcam_camera_obj.h"

G_BEGIN_DECLS

typedef struct _GtkIpcamPreferenceObj
    GtkIpcamPreferenceObj;
typedef struct _GtkIpcamPreferenceObjClass
    GtkIpcamPreferenceObjClass;

#define GTK_TYPE_FOSCAM_PREFERENCE_OBJ                     (gtk_ipcam_preference_obj_get_type ())
#define GTK_IS_FOSCAM_PREFERENCE_OBJ(obj)                  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_FOSCAM_PREFERENCE_OBJ))
#define GTK_IS_FOSCAM_PREFERENCE_OBJ_CLASS(klass)          (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_FOSCAM_PREFERENCE_OBJ))
#define GTK_FOSCAM_PREFERENCE_OBJ_GET_CLASS(obj)           (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_FOSCAM_PREFERENCE_OBJ, GtkIpcamPreferenceObjClass))
#define GTK_FOSCAM_PREFERENCE_OBJ(obj)                     (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_FOSCAM_PREFERENCE_OBJ, GtkIpcamPreferenceObj))
#define GTK_FOSCAM_PREFERENCE_OBJ_CLASS(klass)             (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_FOSCAM_PREFERENCE_OBJ, GtkIpcamPreferenceObjClass))
#define GTK_FOSCAM_PREFERENCE_OBJ_CAST(obj)                ((GtkIpcamPreferenceObj*)(obj))

GType gtk_ipcam_preference_obj_get_type(void);

GtkIpcamPreferenceObj* gtk_ipcam_preference_obj_new(void);
gboolean gtk_ipcam_preference_obj_save(GtkIpcamPreferenceObj* self, gboolean notify);

gboolean gtk_ipcam_preference_obj_add_camera_group(GtkIpcamPreferenceObj * self, const gchar* group_name);
gboolean gtk_ipcam_preference_obj_del_group(GtkIpcamPreferenceObj * self, GtkIpcamCameraGroupObj* group);
guint gtk_ipcam_preference_obj_get_camera_group_count(GtkIpcamPreferenceObj* self);
void  gtk_ipcam_preference_obj_foreach_camera_group(GtkIpcamPreferenceObj* self, GFunc func, gpointer user_data);
GtkIpcamCameraGroupObj* gtk_ipcam_preference_obj_get_camera_group_by_index(GtkIpcamPreferenceObj* self, guint index);

/* Preferences properties */
guint gtk_ipcam_preference_obj_get_width(GtkIpcamPreferenceObj * self);
gboolean gtk_ipcam_preference_obj_set_width(GtkIpcamPreferenceObj * self, const guint val);
guint gtk_ipcam_preference_obj_get_height(GtkIpcamPreferenceObj * self);
gboolean gtk_ipcam_preference_obj_set_height(GtkIpcamPreferenceObj * self, const guint val);
guint gtk_ipcam_preference_obj_get_left(GtkIpcamPreferenceObj * self);
gboolean gtk_ipcam_preference_obj_set_left(GtkIpcamPreferenceObj * self, const guint val);
guint gtk_ipcam_preference_obj_get_top(GtkIpcamPreferenceObj * self);
gboolean gtk_ipcam_preference_obj_set_top(GtkIpcamPreferenceObj * self, const guint val);

G_END_DECLS

#endif //__GTK_FOSCAM_PREFERENCE_OBJ_H__
