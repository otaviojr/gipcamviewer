#ifndef __GTK_FOSCAM_CAMERA_DRIVER_OBJ_H__
#define __GTK_FOSCAM_CAMERA_DRIVER_OBJ_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _GtkIpcamCameraObj GtkIpcamCameraObj;

typedef struct _GtkIpcamCameraDriverObj
    GtkIpcamCameraDriverObj;
typedef struct _GtkIpcamCameraDriverObjClass
    GtkIpcamCameraDriverObjClass;

#define GTK_TYPE_FOSCAM_CAMERA_DRIVER_OBJ                     (gtk_ipcam_camera_driver_obj_get_type ())
#define GTK_IS_FOSCAM_CAMERA_DRIVER_OBJ(obj)                  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_FOSCAM_CAMERA_DRIVER_OBJ))
#define GTK_IS_FOSCAM_CAMERA_DRIVER_OBJ_CLASS(klass)          (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_FOSCAM_CAMERA_DRIVER_OBJ))
#define GTK_FOSCAM_CAMERA_DRIVER_OBJ_GET_CLASS(obj)           (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_FOSCAM_CAMERA_DRIVER_OBJ, GtkIpcamCameraDriverObjClass))
#define GTK_FOSCAM_CAMERA_DRIVER_OBJ(obj)                     (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_FOSCAM_CAMERA_DRIVER_OBJ, GtkIpcamCameraDriverObj))
#define GTK_FOSCAM_CAMERA_DRIVER_OBJ_CLASS(klass)             (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_FOSCAM_CAMERA_DRIVER_OBJ, GtkIpcamCameraDriverObjClass))
#define GTK_FOSCAM_CAMERA_DRIVER_OBJ_CAST(obj)                ((GtkIpcamCameraDriverObj*)(obj))

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

gchar* gtk_ipcam_camera_driver_obj_get_stream_url(GtkIpcamCameraDriverObj * self, GtkIpcamCameraObj* camera);

G_END_DECLS

#endif //__GTK_FOSCAM_CAMERA_DRIVER_OBJ_H__
