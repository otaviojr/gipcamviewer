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
