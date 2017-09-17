#ifndef __GTK_IPCAM_UTIL_H__
#define __GTK_IPCAM_UTIL_H__

#include "gtk_ipcam_camera_obj.h"
#include "gtk_ipcam_camera_group_obj.h"
#include "gtk_ipcam_preference_obj.h"

enum {
  GTK_IPCAM_COLUMN_CAMERA_GROUP_ID,
  GTK_IPCAM_COLUMN_CAMERA_GROUP_NAME,
  GTK_IPCAM_COLUMN_CAMERA_GROUP_LAST
};

void gtk_ipcam_camera_groups_refresh(GtkWidget* combo, GtkIpcamPreferenceObj* preference);

gchar* gtk_ipcam_replace_str(const gchar* original, const gchar* search, const gchar* replace);

#endif //__GTK_IPCAM_PLAYER_H__
