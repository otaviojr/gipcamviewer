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

#include "gtk_ipcam_dialog_camera_add.h"

void
gtk_ipcam_dialog_cameras_add_new(GtkWidget* parent, GValue* ret)
{
  GtkStyleContext *context;

  GtkBuilder* builder = gtk_builder_new();
  gchar* filename = g_build_filename(PREFIX,DATADIR,"gipcamviewer","resources","ui","cameras_add.ui",NULL);
  gtk_builder_add_from_file(builder,filename,NULL);
  GtkWidget* cameras_add_widget = GTK_WIDGET(gtk_builder_get_object(builder,"cameras_add"));
  GtkWidget* cameras_add_input_widget = GTK_WIDGET(gtk_builder_get_object(builder,"cameras_add_input"));
  g_free(filename);
  gtk_builder_connect_signals(builder, NULL);


  GtkWidget* cameras_add_dialog = gtk_dialog_new_with_buttons(gettext("Add Camera"),
                                    GTK_WINDOW(parent),
                                    GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL | GTK_DIALOG_USE_HEADER_BAR,
                                    "\uE876",
                                    GTK_RESPONSE_OK,
                                    NULL);

  GtkHeaderBar* header_bar = GTK_HEADER_BAR(gtk_dialog_get_header_bar(GTK_DIALOG(cameras_add_dialog)));
  context = gtk_widget_get_style_context(GTK_WIDGET(header_bar));
  gtk_style_context_add_class(context,"dialog-header");
  gtk_header_bar_set_show_close_button(header_bar, TRUE);

  GtkWidget* ok_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(cameras_add_dialog),GTK_RESPONSE_OK);
  context = gtk_widget_get_style_context(GTK_WIDGET(ok_button));
  gtk_style_context_add_class(context,"icon-button");

  context = gtk_widget_get_style_context(GTK_WIDGET(cameras_add_widget));
  gtk_style_context_add_class(context,"dialog-content");

  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(cameras_add_dialog))), GTK_WIDGET(cameras_add_widget), TRUE, TRUE, 0);
  gtk_dialog_set_default_response(GTK_DIALOG(cameras_add_dialog),GTK_RESPONSE_OK);
  gtk_window_set_resizable(GTK_WINDOW(cameras_add_dialog), FALSE);
  gtk_entry_set_activates_default (GTK_ENTRY(cameras_add_input_widget), TRUE);

  gint result = gtk_dialog_run(GTK_DIALOG (cameras_add_dialog));
  switch (result)
  {
    case GTK_RESPONSE_OK:
      if(gtk_entry_get_text_length(GTK_ENTRY(cameras_add_input_widget)) > 0)
      {
        g_value_init(ret,G_TYPE_STRING);
        g_value_set_string(ret,gtk_entry_get_text(GTK_ENTRY(cameras_add_input_widget)));
      }
      break;

    default:
      break;
  }
  gtk_widget_destroy(cameras_add_dialog);
}
