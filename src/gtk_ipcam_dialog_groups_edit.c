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

#include <libintl.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "config.h"

#include "gtk_ipcam_dialog_cameras_edit.h"
#include "gtk_ipcam_dialog_groups_edit.h"
#include "gtk_ipcam_dialog_groups_add.h"

static GtkIpcamCameraGroupObj*
get_current_group(GtkIpcamDialogGroupEditInfo* info)
{
  GtkIpcamCameraGroupObj* group = NULL;
  GtkListBoxRow* row = gtk_list_box_get_selected_row(GTK_LIST_BOX(info->groups_list_widget));
  if(row)
  {
    gint group_index = gtk_list_box_row_get_index(row);
    group = gtk_ipcam_preference_obj_get_camera_group_by_index(info->preference, group_index);
  }
  return group;
}

/* GROUP LIST */
static void
add_group(gpointer data, gpointer user_data)
{
  GtkIpcamCameraGroupObj* group = (GtkIpcamCameraGroupObj*)data;
  GtkIpcamDialogGroupEditInfo* info = (GtkIpcamDialogGroupEditInfo*)user_data;

  GtkWidget* row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  GtkWidget* label = gtk_label_new(gtk_ipcam_camera_group_obj_get_name(group));
  gtk_widget_set_hexpand(label, TRUE);
  gtk_label_set_xalign(GTK_LABEL(label), 0.0);
  gtk_container_add(GTK_CONTAINER(row),label);
  gtk_widget_show_all(row);
  gtk_list_box_insert(GTK_LIST_BOX(info->groups_list_widget),row,-1);
}

static void
empty_groups_list(GtkIpcamDialogGroupEditInfo* info)
{
  GList *children, *iter;

  children = gtk_container_get_children(GTK_CONTAINER(info->groups_list_widget));
  for(iter = children; iter != NULL; iter = g_list_next(iter))
    gtk_container_remove(GTK_CONTAINER(info->groups_list_widget),GTK_WIDGET(iter->data));
  g_list_free(children);
}

static void
rebuild_group_list(GtkIpcamDialogGroupEditInfo* info)
{
  GtkListBoxRow* row;
  empty_groups_list(info);
  gtk_ipcam_preference_obj_foreach_camera_group(info->preference, add_group, info);

  row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(info->groups_list_widget),0);
  gtk_list_box_select_row(GTK_LIST_BOX(info->groups_list_widget),row);

  gint count = gtk_ipcam_preference_obj_get_camera_group_count(info->preference);
  if(count <= 1){
    gtk_widget_set_sensitive(info->del_group_btn_widget, FALSE);
  } else {
    gtk_widget_set_sensitive(info->del_group_btn_widget, TRUE);
  }
}
/* GROUP LIST END */

static void
add_group_btn_callback(GtkButton* button, gpointer user_data)
{
  GValue name = G_VALUE_INIT;
  GtkIpcamDialogGroupEditInfo* info = (GtkIpcamDialogGroupEditInfo*)user_data;

  gtk_ipcam_dialog_groups_add_new(info->groups_dialog, &name);
  if(G_VALUE_TYPE(&name) == G_TYPE_STRING)
  {
    printf("Adding group with name %s\n",g_value_get_string(&name));
    gint pos = gtk_ipcam_preference_obj_add_camera_group(info->preference, g_value_get_string(&name));
    g_value_unset(&name);
    rebuild_group_list(info);
    GtkListBoxRow* row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(info->groups_list_widget),pos);
    if(row != NULL)
    {
      gtk_list_box_select_row(GTK_LIST_BOX(info->groups_list_widget),row);
    }
    printf("add_group_btn_callback\n");
  }
}

static void
del_group_btn_callback(GtkButton* button, gpointer user_data)
{
  GtkStyleContext *context;
  GtkIpcamDialogGroupEditInfo* info = (GtkIpcamDialogGroupEditInfo*)user_data;

  GtkIpcamCameraGroupObj* group = get_current_group(info);;

  GtkWidget* dialog;
  dialog = gtk_message_dialog_new(GTK_WINDOW(info->groups_dialog),
                                    GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_USE_HEADER_BAR | GTK_DIALOG_MODAL,
                                    GTK_MESSAGE_QUESTION,
                                    GTK_BUTTONS_YES_NO,
                                    gettext("Do you really want to delete the %s group?"),
                                    gtk_ipcam_camera_group_obj_get_name(group));

  context = gtk_widget_get_style_context(GTK_WIDGET(dialog));
  gtk_style_context_add_class(context,"message-dialog");

  gint result = gtk_dialog_run(GTK_DIALOG(dialog));
  switch (result)
  {
    case GTK_RESPONSE_YES:
      gtk_ipcam_preference_obj_del_group(info->preference, group);
      rebuild_group_list(info);
      break;

    default:
      break;
  }
  gtk_widget_destroy(dialog);
}

void
gtk_ipcam_dialog_groups_edit_new(GtkIpcamDialogCameraEditInfo* info)
{
  GtkIpcamDialogGroupEditInfo group_edit_info;
  group_edit_info.preference = info->preference;

  GtkStyleContext *context;
  group_edit_info.builder = gtk_builder_new();
  gchar* filename = g_build_filename(PREFIX,DATADIR,"gipcamviewer","resources","ui","groups_edit.ui",NULL);
  gtk_builder_add_from_file(group_edit_info.builder,filename,NULL);
  group_edit_info.groups_edit_widget = GTK_WIDGET(gtk_builder_get_object(group_edit_info.builder,"groups_edit_widget"));
  group_edit_info.groups_list_widget = GTK_WIDGET(gtk_builder_get_object(group_edit_info.builder,"groups_list_widget"));

  /* Action buttons */
  group_edit_info.add_group_btn_widget = GTK_WIDGET(gtk_builder_get_object(group_edit_info.builder,"add_group_btn_widget"));
  group_edit_info.del_group_btn_widget = GTK_WIDGET(gtk_builder_get_object(group_edit_info.builder,"del_group_btn_widget"));
  g_signal_connect(group_edit_info.add_group_btn_widget, "clicked", G_CALLBACK(add_group_btn_callback), &group_edit_info);
  g_signal_connect(group_edit_info.del_group_btn_widget, "clicked", G_CALLBACK(del_group_btn_callback), &group_edit_info);
  /* Action buttons End*/

  g_free(filename);

  gtk_builder_connect_signals(group_edit_info.builder, NULL);

  group_edit_info.groups_dialog = gtk_dialog_new_with_buttons(gettext("Groups Manager"),
                                    GTK_WINDOW(info->cameras_edit_dialog),
                                    GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL | GTK_DIALOG_USE_HEADER_BAR,
                                    "\uE876",
                                    GTK_RESPONSE_OK,
                                    NULL);

  GtkHeaderBar* header_bar = GTK_HEADER_BAR(gtk_dialog_get_header_bar(GTK_DIALOG(group_edit_info.groups_dialog)));
  context = gtk_widget_get_style_context(GTK_WIDGET(header_bar));
  gtk_style_context_add_class(context,"dialog-header");
  gtk_header_bar_set_show_close_button(header_bar, FALSE);

  GtkWidget* ok_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(group_edit_info.groups_dialog),GTK_RESPONSE_OK);
  context = gtk_widget_get_style_context(GTK_WIDGET(ok_button));
  gtk_style_context_add_class(context,"icon-button");

  context = gtk_widget_get_style_context(GTK_WIDGET(group_edit_info.groups_dialog));
  gtk_style_context_add_class(context,"dialog-content");

  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(group_edit_info.groups_dialog))), group_edit_info.groups_edit_widget, TRUE, TRUE, 0);
  gtk_dialog_set_default_response(GTK_DIALOG(group_edit_info.groups_dialog),GTK_RESPONSE_OK);

  gtk_window_set_resizable(GTK_WINDOW(group_edit_info.groups_dialog), FALSE);

  gtk_widget_show_all(group_edit_info.groups_edit_widget);

  rebuild_group_list(&group_edit_info);

  gint result = gtk_dialog_run(GTK_DIALOG(group_edit_info.groups_dialog));
  switch (result)
  {
    case GTK_RESPONSE_OK:
       break;
    default:
       break;
  }

  gtk_widget_destroy(GTK_WIDGET(group_edit_info.groups_dialog));
  g_object_unref(group_edit_info.builder);
}
