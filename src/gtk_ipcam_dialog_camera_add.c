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

struct _GtkIpcamDialogCameraAdd
{
  GObject parent;

  GtkBuilder* builder;
  GtkWidget* cameras_add_widget;
  GtkWidget* cameras_add_input_widget;
};

struct _GtkIpcamDialogCameraAddClass
{
  GObjectClass parent_class;

  /**
	 * Callback function to invoke when emitting the "new-camera"
	 * signal.
	 *
	 * @param self      \e GtkIpcamDialogCameraAdd widget that emitted the signal
	 * @param new_time  New (current) position of playback in milliseconds
	 */
	void (*new_camera)	(GtkIpcamDialogCameraAdd *self, GValue* name);
};

enum
{
  GTK_IPCAM_DIALOG_CAMERA_ADD_PROP_LAST
};

enum
{
  GTK_IPCAM_DIALOG_CAMERA_ADD_SIGNAL_0,
  GTK_IPCAM_DIALOG_CAMERA_ADD_SIGNAL_ADD_NEW,
  GTK_IPCAM_DIALOG_CAMERA_ADD_SIGNAL_LAST
};

G_DEFINE_TYPE (GtkIpcamDialogCameraAdd, gtk_ipcam_dialog_camera_add, G_TYPE_OBJECT);

static GParamSpec*
gtk_ipcam_dialog_camera_add_param_specs[GTK_IPCAM_DIALOG_CAMERA_ADD_PROP_LAST] = { NULL };

static guint
gtk_ipcam_dialog_camera_add_signals[GTK_IPCAM_DIALOG_CAMERA_ADD_SIGNAL_LAST] = {0, };


static void
gtk_ipcam_dialog_camera_add_get_property(GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec)
{
  GtkIpcamDialogCameraAdd *self = GTK_IPCAM_DIALOG_CAMERA_ADD(object);

  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void
gtk_ipcam_dialog_camera_add_set_property(GObject * object, guint prop_id, const GValue * value,
    GParamSpec * pspec)
{
  GtkIpcamDialogCameraAdd *self = GTK_IPCAM_DIALOG_CAMERA_ADD(object);

  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gtk_ipcam_dialog_camera_add_finalize(GObject * object)
{
  GtkIpcamDialogCameraAdd *self = GTK_IPCAM_DIALOG_CAMERA_ADD(object);
  G_OBJECT_CLASS(gtk_ipcam_dialog_camera_add_parent_class)->finalize(object);
}

static void
gtk_ipcam_dialog_camera_add_class_init(GtkIpcamDialogCameraAddClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = gtk_ipcam_dialog_camera_add_set_property;
  gobject_class->get_property = gtk_ipcam_dialog_camera_add_get_property;
  gobject_class->finalize = gtk_ipcam_dialog_camera_add_finalize;

  gtk_ipcam_dialog_camera_add_signals[GTK_IPCAM_DIALOG_CAMERA_ADD_SIGNAL_ADD_NEW] =
      g_signal_new("new-camera",
                    G_TYPE_FROM_CLASS(gobject_class),
                    G_SIGNAL_RUN_FIRST,
                    G_STRUCT_OFFSET(GtkIpcamDialogCameraAddClass, new_camera),
                    NULL,
                    NULL,
                    g_cclosure_marshal_VOID__STRING,
                    G_TYPE_NONE,
                    1,
                    G_TYPE_STRING);

  g_object_class_install_properties (gobject_class,
      GTK_IPCAM_DIALOG_CAMERA_ADD_PROP_LAST, gtk_ipcam_dialog_camera_add_param_specs);
}

static void
gtk_ipcam_dialog_camera_add_init(GtkIpcamDialogCameraAdd * self)
{
}

GtkIpcamDialogCameraAdd *
gtk_ipcam_dialog_camera_add_new(void)
{
  return g_object_new(GTK_TYPE_IPCAM_DIALOG_CAMERA_ADD, NULL);
}

static void
gtk_ipcam_dialog_cameras_add_response(GtkDialog *dialog,
                    int        response,
                    gpointer   user_data)
{
  GtkIpcamDialogCameraAdd* self = GTK_IPCAM_DIALOG_CAMERA_ADD(user_data);
  GValue name = G_VALUE_INIT;

  switch (response)
  {
    case GTK_RESPONSE_OK:
      if(gtk_entry_get_text_length(GTK_ENTRY(self->cameras_add_input_widget)) > 0)
      {
        g_value_init(&name,G_TYPE_STRING);
        g_value_set_string(&name,gtk_editable_get_text(GTK_EDITABLE(self->cameras_add_input_widget)));
        g_signal_emit(self, gtk_ipcam_dialog_camera_add_signals[GTK_IPCAM_DIALOG_CAMERA_ADD_SIGNAL_ADD_NEW], 0, &name);
        g_value_unset(&name);
      }
      break;

    default:
      break;
  }

  gtk_window_destroy (GTK_WINDOW (dialog));
}

void
gtk_ipcam_dialog_cameras_add_show(GtkIpcamDialogCameraAdd* self, GtkWidget* parent)
{
  GtkStyleContext *context;

  self->builder = gtk_builder_new();
  gchar* filename = g_build_filename(PREFIX,DATADIR,"gipcamviewer","resources","ui","cameras_add.ui",NULL);
  gtk_builder_add_from_file(self->builder,filename,NULL);
  self->cameras_add_widget = GTK_WIDGET(gtk_builder_get_object(self->builder,"cameras_add"));
  self->cameras_add_input_widget = GTK_WIDGET(gtk_builder_get_object(self->builder,"cameras_add_input"));
  g_free(filename);

  GtkWidget* cameras_add_dialog = gtk_dialog_new_with_buttons(gettext("Add Camera"),
                                    GTK_WINDOW(parent),
                                    GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL | GTK_DIALOG_USE_HEADER_BAR,
                                    "\uE876",
                                    GTK_RESPONSE_OK,
                                    NULL);

  GtkHeaderBar* header_bar = GTK_HEADER_BAR(gtk_dialog_get_header_bar(GTK_DIALOG(cameras_add_dialog)));
  context = gtk_widget_get_style_context(GTK_WIDGET(header_bar));
  gtk_style_context_add_class(context,"dialog-header");
  gtk_header_bar_set_show_title_buttons(header_bar, TRUE);

  GtkWidget* ok_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(cameras_add_dialog),GTK_RESPONSE_OK);
  context = gtk_widget_get_style_context(GTK_WIDGET(ok_button));
  gtk_style_context_add_class(context,"icon-button");

  context = gtk_widget_get_style_context(GTK_WIDGET(self->cameras_add_widget));
  gtk_style_context_add_class(context,"dialog-content");

  gtk_box_append(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(cameras_add_dialog))), GTK_WIDGET(self->cameras_add_widget));
  gtk_dialog_set_default_response(GTK_DIALOG(cameras_add_dialog),GTK_RESPONSE_OK);
  gtk_window_set_resizable(GTK_WINDOW(cameras_add_dialog), FALSE);
  gtk_entry_set_activates_default (GTK_ENTRY(self->cameras_add_input_widget), TRUE);

  g_signal_connect(GTK_DIALOG(cameras_add_dialog), "response", G_CALLBACK (gtk_ipcam_dialog_cameras_add_response), self);
  gtk_widget_show(GTK_WIDGET (cameras_add_dialog));
}
