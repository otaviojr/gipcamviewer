#include <libintl.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "config.h"

#include "gtk_ipcam_dialog_groups_add.h"

void
gtk_ipcam_dialog_groups_add_new(GtkWidget* parent, GValue* ret)
{
  GtkStyleContext *context;

  GtkBuilder* builder = gtk_builder_new();
  gchar* filename = g_build_filename(PREFIX,DATADIR,"gipcamviewer","resources","ui","groups_add.ui",NULL);
  gtk_builder_add_from_file(builder,filename,NULL);
  GtkWidget* groups_add_widget = GTK_WIDGET(gtk_builder_get_object(builder,"groups_add"));
  GtkWidget* groups_add_input_widget = GTK_WIDGET(gtk_builder_get_object(builder,"groups_add_input"));
  g_free(filename);
  gtk_builder_connect_signals(builder, NULL);

  GtkWidget* groups_add_dialog = gtk_dialog_new_with_buttons(gettext("Add Group"),
                                    GTK_WINDOW(parent),
                                    GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL | GTK_DIALOG_USE_HEADER_BAR,
                                    "\uE876",
                                    GTK_RESPONSE_OK,
                                    NULL);

  GtkHeaderBar* header_bar = GTK_HEADER_BAR(gtk_dialog_get_header_bar(GTK_DIALOG(groups_add_dialog)));
  context = gtk_widget_get_style_context(GTK_WIDGET(header_bar));
  gtk_style_context_add_class(context,"dialog-header");
  gtk_header_bar_set_show_close_button(header_bar, TRUE);

  GtkWidget* ok_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(groups_add_dialog),GTK_RESPONSE_OK);
  context = gtk_widget_get_style_context(GTK_WIDGET(ok_button));
  gtk_style_context_add_class(context,"icon-button");

  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(groups_add_dialog))), GTK_WIDGET(groups_add_widget), TRUE, TRUE, 0);

  gtk_dialog_set_default_response(GTK_DIALOG(groups_add_dialog),GTK_RESPONSE_OK);
  gtk_entry_set_activates_default (GTK_ENTRY(groups_add_input_widget), TRUE);
  gint result = gtk_dialog_run(GTK_DIALOG (groups_add_dialog));
  switch (result)
  {
    case GTK_RESPONSE_OK:
      g_value_init(ret,G_TYPE_STRING);
      g_value_set_string(ret,gtk_entry_get_text(GTK_ENTRY(groups_add_input_widget)));
      break;

    default:
      break;
  }
  gtk_widget_destroy(groups_add_dialog);
}
