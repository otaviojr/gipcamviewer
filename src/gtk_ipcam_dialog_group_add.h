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

#ifndef __GTK_IPCAM_DIALOG_GROUPS_ADD_H__
#define __GTK_IPCAM_DIALOG_GROUPS_ADD_H__

#include "gtk_ipcam_util.h"

typedef struct _GtkIpcamDialogGroupAdd
    GtkIpcamDialogGroupAdd;
typedef struct _GtkIpcamDialogGroupAddClass
    GtkIpcamDialogGroupAddClass;

#define GTK_TYPE_IPCAM_DIALOG_GROUP_ADD                     (gtk_ipcam_dialog_group_add_get_type ())
#define GTK_IS_IPCAM_DIALOG_GROUP_ADD(obj)                  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_IPCAM_DIALOG_GROUP_ADD))
#define GTK_IS_IPCAM_DIALOG_GROUP_ADD_CLASS(klass)          (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_IPCAM_DIALOG_GROUP_ADD))
#define GTK_IPCAM_DIALOG_GROUP_ADD_GET_CLASS(obj)           (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_IPCAM_DIALOG_GROUP_ADD, GtkIpcamDialogGroupAddClass))
#define GTK_IPCAM_DIALOG_GROUP_ADD(obj)                     (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IPCAM_DIALOG_GROUP_ADD, GtkIpcamDialogGroupAdd))
#define GTK_IPCAM_DIALOG_GROUP_ADD_CLASS(klass)             (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_IPCAM_DIALOG_GROUP_ADD, GtkIpcamDialogGroupAddClass))
#define GTK_IPCAM_DIALOG_GROUP_ADD_CAST(obj)                ((GtkIpcamDialogGroupAdd*)(obj))

GType gtk_ipcam_dialog_group_add_get_type(void);

GtkIpcamDialogGroupAdd* gtk_ipcam_dialog_group_add_new(void);
void  gtk_ipcam_dialog_group_add_show(GtkIpcamDialogGroupAdd* self, GtkWidget* parent);


#endif //__GTK_IPCAM_DIALOG_GROUPS_ADD_H__
