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

#ifndef __GTK_IPCAM_PLAYER_H__
#define __GTK_IPCAM_PLAYER_H__

#include <gtk/gtk.h>

#include "gtk_ipcam_camera_obj.h"
#include "gtk_ipcam_camera_driver_obj.h"

G_BEGIN_DECLS

enum
{
  GTK_IPCAM_PLAYER_STATE_IDLE,
  GTK_IPCAM_PLAYER_STATE_LOADED,
  GTK_IPCAM_PLAYER_STATE_STOPPED,
  GTK_IPCAM_PLAYER_STATE_PLAYING,
};

typedef struct _GtkIpcamPlayer
    GtkIpcamPlayer;
typedef struct _GtkIpcamPlayerClass
    GtkIpcamPlayerClass;

#define GTK_TYPE_IPCAM_PLAYER                         (gtk_ipcam_player_get_type ())
#define GTK_IS_IPCAM_PLAYER(obj)                      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_IPCAM_PLAYER))
#define GTK_IS_IPCAM_PLAYER_CLASS(klass)              (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_IPCAM_PLAYER))
#define GTK_IPCAM_PLAYER_GET_CLASS(obj)               (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_IPCAM_PLAYER, GtkIpcamPlayerClass))
#define GTK_IPCAM_PLAYER(obj)                         (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IPCAM_PLAYER, GtkIpcamPlayer))
#define GTK_IPCAM_PLAYER_CLASS(klass)                 (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_IPCAM_PLAYER, GtkIpcamPlayerClass))
#define GTK_IPCAM_PLAYER_CAST(obj)                    ((GtkIpcamPlayer*)(obj))


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
gboolean gtk_ipcam_player_is_audio_playing(GtkIpcamPlayer* self);

G_END_DECLS

#endif //__GTK_IPCAM_PLAYER_H__
