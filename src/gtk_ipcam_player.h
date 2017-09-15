#ifndef __GTK_FOSCAM_PLAYER_H__
#define __GTK_FOSCAM_PLAYER_H__

#include <gtk/gtk.h>

#include "gtk_ipcam_camera_obj.h"
#include "gtk_ipcam_camera_driver_obj.h"

G_BEGIN_DECLS

enum
{
  GTK_FOSCAM_PLAYER_STATE_PLAYING,
  GTK_FOSCAM_PLAYER_STATE_STOPPED,
};

typedef struct _GtkIpcamPlayer
    GtkIpcamPlayer;
typedef struct _GtkIpcamPlayerClass
    GtkIpcamPlayerClass;

#define GTK_TYPE_FOSCAM_PLAYER                         (gtk_ipcam_player_get_type ())
#define GTK_IS_FOSCAM_PLAYER(obj)                      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_FOSCAM_PLAYER))
#define GTK_IS_FOSCAM_PLAYER_CLASS(klass)              (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_FOSCAM_PLAYER))
#define GTK_FOSCAM_PLAYER_GET_CLASS(obj)               (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_FOSCAM_PLAYER, GtkIpcamPlayerClass))
#define GTK_FOSCAM_PLAYER(obj)                         (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_FOSCAM_PLAYER, GtkIpcamPlayer))
#define GTK_FOSCAM_PLAYER_CLASS(klass)                 (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_FOSCAM_PLAYER, GtkIpcamPlayerClass))
#define GTK_FOSCAM_PLAYER_CAST(obj)                    ((GtkIpcamPlayer*)(obj))


GType gtk_ipcam_player_get_type(void);

GtkIpcamPlayer* gtk_ipcam_player_new(GtkIpcamCameraObj* camera);

void gtk_ipcam_player_load(GtkIpcamPlayer *self);

//GtkWidget * gtk_ipcam_player_get_widget(GtkIpcamPlayer* self);
GtkIpcamCameraObj * gtk_ipcam_player_get_camera(GtkIpcamPlayer * self);

//GstPlayer * gtk_ipcam_player_get_player(GtkIpcamPlayer* self);

gboolean gtk_ipcam_player_start_video(GtkIpcamPlayer* self);
gboolean gtk_ipcam_player_stop_video(GtkIpcamPlayer* self);
gboolean gtk_ipcam_player_start_audio(GtkIpcamPlayer* self);
gboolean gtk_ipcam_player_stop_audio(GtkIpcamPlayer* self);

G_END_DECLS

#endif //__GTK_FOSCAM_PLAYER_H__
