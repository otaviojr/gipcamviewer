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

#ifndef __GTK_IPCAM_DIALOG_GROUPS_EDIT_H__
#define __GTK_IPCAM_DIALOG_GROUPS_EDIT_H__

#include "gtk_ipcam_util.h"
#include "gtk_ipcam_preference_obj.h"

struct _GtkIpcamDialogCameraEditInfo;
typedef struct _GtkIpcamDialogCameraEditInfo GtkIpcamDialogCameraEditInfo;

typedef struct {
  GtkIpcamPreferenceObj* preference;
  GtkBuilder* builder;

  GtkWidget* groups_dialog;
  GtkWidget* groups_edit_widget;
  GtkWidget* groups_list_widget;

  GtkWidget* add_group_btn_widget;
  GtkWidget* del_group_btn_widget;

} GtkIpcamDialogGroupEditInfo;

void gtk_ipcam_dialog_groups_edit_new(GtkIpcamDialogCameraEditInfo* info);

#endif //__GTK_IPCAM_DIALOG_GROUPS_EDIT_H__
