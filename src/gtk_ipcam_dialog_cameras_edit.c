#include <libintl.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

#include "config.h"

#include "gtk_ipcam_dialog_cameras_edit.h"
#include "gtk_ipcam_dialog_camera_add.h"

static gboolean changed;
static gint refresh_form(gpointer data);
static void connect_signals(GtkIpcamDialogCameraEditInfo* info);
static void block_signals(GtkIpcamDialogCameraEditInfo* info);
static void unblock_signals(GtkIpcamDialogCameraEditInfo* info);
static void rebuild_camera_list(GtkIpcamDialogCameraEditInfo* info);

static GtkIpcamCameraGroupObj*
get_current_camera_group(GtkIpcamDialogCameraEditInfo* info)
{
  gint active_group = gtk_combo_box_get_active(GTK_COMBO_BOX(info->group_list_widget));
  return GTK_IPCAM_CAMERA_GROUP_OBJ(gtk_ipcam_preference_obj_get_camera_group_by_index(info->preference,active_group));
}

static GtkIpcamCameraObj*
get_current_camera(GtkIpcamDialogCameraEditInfo* info)
{
  GtkIpcamCameraObj* camera = NULL;
  GtkIpcamCameraGroupObj* camera_group = get_current_camera_group(info);
  if(camera_group != NULL)
  {
    GtkListBoxRow* row = gtk_list_box_get_selected_row(GTK_LIST_BOX(info->camera_list_widget));
    if(row)
    {
      gint camera_index = gtk_list_box_row_get_index(row);
      camera = gtk_ipcam_camera_group_obj_get_camera_by_index(camera_group, camera_index);
    }
  }
  return camera;
}

static void
edit_group_btn_callback(GtkButton* button, gpointer user_data)
{
  GtkIpcamDialogCameraEditInfo* info = (GtkIpcamDialogCameraEditInfo*)user_data;
  gtk_ipcam_dialog_groups_edit_new(info);
}

static void
add_camera_btn_callback(GtkButton* button, gpointer user_data)
{
  GValue name = G_VALUE_INIT;
  GtkIpcamDialogCameraEditInfo* info = (GtkIpcamDialogCameraEditInfo*)user_data;
  GtkIpcamCameraGroupObj* camera_group = get_current_camera_group(info);

  g_assert(camera_group != NULL);

  gtk_ipcam_dialog_cameras_add_new(info->cameras_edit_dialog, &name);
  if(G_VALUE_TYPE(&name) == G_TYPE_STRING)
  {
    printf("Adding camera with name %s\n",g_value_get_string(&name));
    GtkIpcamCameraObj* camera = gtk_ipcam_camera_obj_new();
    gtk_ipcam_camera_obj_set_name(camera, g_value_get_string(&name));
    gint pos = gtk_ipcam_camera_group_obj_add_camera(camera_group, camera);
    g_value_unset(&name);
    rebuild_camera_list(info);
    GtkListBoxRow* row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(info->camera_list_widget),pos);
    if(row != NULL)
    {
      gtk_list_box_select_row(GTK_LIST_BOX(info->camera_list_widget),row);
    }
    printf("add_camera_btn_callback\n");
    changed = TRUE;
  }
}

static void
del_camera_btn_callback(GtkButton* button, gpointer user_data)
{
  GtkStyleContext *context;
  GtkIpcamDialogCameraEditInfo* info = (GtkIpcamDialogCameraEditInfo*)user_data;
  GtkIpcamCameraGroupObj* group;
  GtkIpcamCameraObj* camera = get_current_camera(info);

  GtkWidget* dialog;
  dialog = gtk_message_dialog_new(GTK_WINDOW(info->cameras_edit_dialog),
                                    GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_USE_HEADER_BAR | GTK_DIALOG_MODAL,
                                    GTK_MESSAGE_QUESTION,
                                    GTK_BUTTONS_YES_NO,
                                    gettext("Do you really want to delete the %s camera?"),
                                    gtk_ipcam_camera_obj_get_name(camera));

  context = gtk_widget_get_style_context(GTK_WIDGET(dialog));
  gtk_style_context_add_class(context,"message-dialog");

  gint result = gtk_dialog_run(GTK_DIALOG(dialog));
  switch (result)
  {
    case GTK_RESPONSE_YES:
      group = get_current_camera_group(info);
      gtk_ipcam_camera_group_obj_del_camera(group, camera);
      rebuild_camera_list(info);
      changed = TRUE;
      break;

    default:
      break;
  }
  gtk_widget_destroy(dialog);
}

static void
cameras_down_btn_callback(GtkButton* button, gpointer user_data)
{
  GtkIpcamDialogCameraEditInfo* info = (GtkIpcamDialogCameraEditInfo*)user_data;
  GtkIpcamCameraGroupObj* group = get_current_camera_group(info);
  GtkListBoxRow* row = gtk_list_box_get_selected_row(GTK_LIST_BOX(info->camera_list_widget));

  if(row)
  {
    gint camera_index = gtk_list_box_row_get_index(row);
    GtkIpcamCameraObj* camera = gtk_ipcam_camera_group_obj_get_camera_by_index(group, camera_index);

    if(camera_index < gtk_ipcam_camera_group_obj_get_cameras_count(group)-1)
    {
      g_object_ref(row);
      gtk_list_box_unselect_row(GTK_LIST_BOX(info->camera_list_widget),row);
      gtk_container_remove(GTK_CONTAINER(info->camera_list_widget),GTK_WIDGET(row));
      gtk_ipcam_camera_group_obj_change_camera_pos(group,camera,camera_index+1);
      gtk_list_box_insert(GTK_LIST_BOX(info->camera_list_widget), GTK_WIDGET(row), camera_index+1);
      gtk_list_box_select_row(GTK_LIST_BOX(info->camera_list_widget),row);
      refresh_form(info);
      g_object_unref(row);
      changed = TRUE;
    }
  }
}

static void
cameras_up_btn_callback(GtkButton* button, gpointer user_data)
{
  GtkIpcamDialogCameraEditInfo* info = (GtkIpcamDialogCameraEditInfo*)user_data;
  GtkIpcamCameraGroupObj* group = get_current_camera_group(info);
  GtkListBoxRow* row = gtk_list_box_get_selected_row(GTK_LIST_BOX(info->camera_list_widget));

  if(row)
  {
    gint camera_index = gtk_list_box_row_get_index(row);
    GtkIpcamCameraObj* camera = gtk_ipcam_camera_group_obj_get_camera_by_index(group, camera_index);

    if(camera_index > 0)
    {
      g_object_ref(row);
      gtk_list_box_unselect_row(GTK_LIST_BOX(info->camera_list_widget),row);
      gtk_container_remove(GTK_CONTAINER(info->camera_list_widget),GTK_WIDGET(row));
      gtk_ipcam_camera_group_obj_change_camera_pos(group,camera,camera_index-1);
      gtk_list_box_insert(GTK_LIST_BOX(info->camera_list_widget), GTK_WIDGET(row), camera_index-1);
      gtk_list_box_select_row(GTK_LIST_BOX(info->camera_list_widget),row);
      refresh_form(info);
      g_object_unref(row);
      changed = TRUE;
    }
  }
}

/* CAMERA LIST */
static void
add_camera(gpointer data, gpointer user_data)
{
  GtkIpcamCameraObj* camera = GTK_IPCAM_CAMERA_OBJ(data);
  GtkIpcamDialogCameraEditInfo* info = (GtkIpcamDialogCameraEditInfo*)user_data;

  GtkWidget* row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  GtkWidget* label = gtk_label_new(gtk_ipcam_camera_obj_get_name(camera));
  gtk_widget_set_hexpand (label, TRUE);
  gtk_label_set_xalign(GTK_LABEL(label), 0.0);
  gtk_container_add(GTK_CONTAINER(row),label);
  gtk_widget_show_all(row);
  gtk_list_box_insert(GTK_LIST_BOX(info->camera_list_widget),row,-1);
}

static void
empty_camera_list(GtkIpcamDialogCameraEditInfo* info) {
  GList *children, *iter;

  children = gtk_container_get_children(GTK_CONTAINER(info->camera_list_widget));
  for(iter = children; iter != NULL; iter = g_list_next(iter))
    gtk_container_remove(GTK_CONTAINER(info->camera_list_widget),GTK_WIDGET(iter->data));
  g_list_free(children);
}

static void
rebuild_camera_list(GtkIpcamDialogCameraEditInfo* info)
{
  GtkIpcamCameraGroupObj* camera_group = get_current_camera_group(info);

  GtkListBoxRow* row = gtk_list_box_get_selected_row(GTK_LIST_BOX(info->camera_list_widget));
  gint camera_index = (row != NULL ? gtk_list_box_row_get_index(row) : 0);

  empty_camera_list(info);
  gtk_ipcam_camera_group_obj_foreach_camera(camera_group, add_camera, info);

  row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(info->camera_list_widget),camera_index);
  if(row == NULL){
    row = gtk_list_box_get_row_at_index(GTK_LIST_BOX(info->camera_list_widget),0);
  }
  if(row != NULL)
  {
    gtk_list_box_select_row(GTK_LIST_BOX(info->camera_list_widget),row);
  }
}

static void
refresh_camera_list(GtkComboBox* widget, gpointer user_data)
{
  printf("Refreshing camera list\n");
  GtkIpcamDialogCameraEditInfo* info = (GtkIpcamDialogCameraEditInfo*)user_data;
  rebuild_camera_list(info);
}
/* CAMERA LIST END */

/* DRIVERS/MODEL */
static void
add_driver(gpointer data, gpointer user_data)
{
  GtkTreeIter iter;
  GtkListStore* drivers = GTK_LIST_STORE(user_data);
  GtkIpcamCameraDriverObj* driver = GTK_IPCAM_CAMERA_DRIVER_OBJ(data);

  gtk_list_store_append (drivers, &iter);

  printf("Adding driver %s to list store\n", gtk_ipcam_camera_driver_obj_get_model(driver));

  gtk_list_store_set (drivers, &iter,
                          GTK_IPCAM_COLUMN_CAMERA_DRIVER_ID, gtk_ipcam_camera_driver_obj_get_driver_name(driver),
                          GTK_IPCAM_COLUMN_CAMERA_DRIVER_NAME, gtk_ipcam_camera_driver_obj_get_model(driver),
                          -1);
}

static GtkListStore*
create_drivers_list(GPtrArray* drivers)
{
  GtkTreeIter iter;

  GtkListStore* tmp_drivers = gtk_list_store_new(GTK_IPCAM_COLUMN_CAMERA_DRIVER_LAST,G_TYPE_STRING, G_TYPE_STRING);

  gtk_list_store_append (tmp_drivers, &iter);
  gtk_list_store_set (tmp_drivers, &iter,
                          GTK_IPCAM_COLUMN_CAMERA_DRIVER_ID, "",
                          GTK_IPCAM_COLUMN_CAMERA_DRIVER_NAME, "",
                          -1);

  g_ptr_array_foreach(drivers, add_driver, tmp_drivers);
  return tmp_drivers;
}

static void
refresh_drivers(GtkIpcamDialogCameraEditInfo* info, GPtrArray* drivers) {
  GtkListStore* old_model = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(info->cameras_edit_model)));
  gtk_combo_box_set_model(GTK_COMBO_BOX(info->cameras_edit_model),NULL);
  if(old_model != NULL)
  {
    g_object_unref(old_model);
  }

  GtkListStore* groups = create_drivers_list(drivers);
  if(groups != NULL)
  {
    gtk_combo_box_set_model(GTK_COMBO_BOX(info->cameras_edit_model),GTK_TREE_MODEL(groups));
    gtk_combo_box_set_entry_text_column(GTK_COMBO_BOX(info->cameras_edit_model),GTK_IPCAM_COLUMN_CAMERA_DRIVER_NAME);
    gtk_combo_box_set_id_column(GTK_COMBO_BOX(info->cameras_edit_model),GTK_IPCAM_COLUMN_CAMERA_DRIVER_ID);
    gtk_combo_box_set_active(GTK_COMBO_BOX(info->cameras_edit_model),0);
  }
}

static void
model_changed(GtkComboBox* widget, gpointer user_data)
{
  GtkIpcamDialogCameraEditInfo* info = (GtkIpcamDialogCameraEditInfo*)user_data;

  GtkIpcamCameraObj* camera = info->current_camera;
  gtk_ipcam_camera_obj_set_model(camera, gtk_combo_box_get_active_id(GTK_COMBO_BOX(widget)));
  refresh_form(info);
  printf("model_changed\n");
  changed = TRUE;
}
/* DRIVERS/MODEL END*/

static gint
refresh_form(gpointer data)
{
  GtkIpcamDialogCameraEditInfo* info = (GtkIpcamDialogCameraEditInfo*)data;
  GtkIpcamCameraObj* camera = get_current_camera(info);
  if(camera != NULL)
  {
    printf("Setting camera info\n");

    info->current_camera = camera;

    /*
     * To not mark as changed when populating with data
     */
    block_signals(info);

    gchar* value = (gtk_ipcam_camera_obj_get_model(camera) ? gtk_ipcam_camera_obj_get_model(camera) : "");
    if(gtk_combo_box_set_active_id(GTK_COMBO_BOX(info->cameras_edit_model), value) != TRUE)
    {
      gtk_combo_box_set_active(GTK_COMBO_BOX(info->cameras_edit_model), 0);
    }

    value = (gtk_ipcam_camera_obj_get_name(camera) ? gtk_ipcam_camera_obj_get_name(camera) : "");
    gtk_entry_set_text(GTK_ENTRY(info->cameras_edit_name), value);

    value = (gtk_ipcam_camera_obj_get_username(camera) ? gtk_ipcam_camera_obj_get_username(camera) : "");
    gtk_entry_set_text(GTK_ENTRY(info->cameras_edit_username), value);

    value = (gtk_ipcam_camera_obj_get_password(camera) ? gtk_ipcam_camera_obj_get_password(camera) : "");
    gtk_entry_set_text(GTK_ENTRY(info->cameras_edit_password), value);

    value = (gtk_ipcam_camera_obj_get_remote_url(camera) ? gtk_ipcam_camera_obj_get_remote_url(camera) : "");
    gtk_entry_set_text(GTK_ENTRY(info->cameras_edit_remote_url), value);

    value = (gtk_ipcam_camera_obj_get_remote_port(camera) ? gtk_ipcam_camera_obj_get_remote_port(camera) : "");
    gtk_entry_set_text(GTK_ENTRY(info->cameras_edit_remote_port), value);

    value = (gtk_ipcam_camera_obj_get_remote_media_port(camera) ? gtk_ipcam_camera_obj_get_remote_media_port(camera) : "");
    gtk_entry_set_text(GTK_ENTRY(info->cameras_edit_remote_media_port), value);

    value = (gtk_ipcam_camera_obj_get_local_url(camera) ? gtk_ipcam_camera_obj_get_local_url(camera) : "");
    gtk_entry_set_text(GTK_ENTRY(info->cameras_edit_local_url), value);

    value = (gtk_ipcam_camera_obj_get_local_port(camera) ? gtk_ipcam_camera_obj_get_local_port(camera) : "");
    gtk_entry_set_text(GTK_ENTRY(info->cameras_edit_local_port), value);

    value = (gtk_ipcam_camera_obj_get_local_media_port(camera) ? gtk_ipcam_camera_obj_get_local_media_port(camera) : "");
    gtk_entry_set_text(GTK_ENTRY(info->cameras_edit_local_media_port), value);

    if(gtk_ipcam_camera_obj_get_remote_https(camera) == TRUE)
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(info->cameras_edit_remote_https), TRUE);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(info->cameras_edit_remote_http), FALSE);
    }
    else
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(info->cameras_edit_remote_http), TRUE);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(info->cameras_edit_remote_https), FALSE);
    }

    if(gtk_ipcam_camera_obj_get_local_https(camera) == TRUE)
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(info->cameras_edit_local_https), TRUE);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(info->cameras_edit_local_http), FALSE);
    }
    else
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(info->cameras_edit_local_http), TRUE);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(info->cameras_edit_local_https), FALSE);
    }

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(info->cameras_edit_use_subchannel), gtk_ipcam_camera_obj_get_subchannel(camera));

    GtkIpcamCameraDriverObj* driver = gtk_ipcam_camera_obj_get_camera_driver(camera);
    if(driver != NULL)
    {
      if(g_strrstr(gtk_ipcam_camera_driver_obj_get_protocol(driver),"http") != NULL)
      {
        gtk_widget_show(info->cameras_edit_remote_protocol);
        gtk_widget_show(info->cameras_edit_local_protocol);
      }
      else
      {
        gtk_widget_hide(info->cameras_edit_remote_protocol);
        gtk_widget_hide(info->cameras_edit_local_protocol);
      }

      if(g_strrstr(gtk_ipcam_camera_driver_obj_get_protocol(driver),"rtsp") != NULL)
      {
        gtk_widget_show(info->cameras_edit_remote_media);
        gtk_widget_show(info->cameras_edit_local_media);
      }
      else
      {
        gtk_widget_hide(info->cameras_edit_remote_media);
        gtk_widget_hide(info->cameras_edit_local_media);
      }
    } else {
      gtk_widget_hide(info->cameras_edit_remote_protocol);
      gtk_widget_hide(info->cameras_edit_local_protocol);
      gtk_widget_hide(info->cameras_edit_remote_media);
      gtk_widget_hide(info->cameras_edit_local_media);
    }

    unblock_signals(info);

    gtk_stack_set_visible_child_name(GTK_STACK(info->cameras_form_stack),"form_layer");
  } else {
    gtk_stack_set_visible_child_name(GTK_STACK(info->cameras_form_stack),"null_layer");
  }
  return FALSE;
}

static void
refresh_camera_form(GtkListBox* widget, GtkListBoxRow *row, gpointer user_data)
{
  printf("Camera Form Refresh\n");
  GtkIpcamDialogCameraEditInfo* info = (GtkIpcamDialogCameraEditInfo*)user_data;
  g_timeout_add(100,refresh_form,info);
}

/* TOGGLE BUTTONS FUNCTIONS*/
static void
remote_http_toggled(GtkToggleButton* button, gpointer user_data)
{
  GtkIpcamDialogCameraEditInfo* info = (GtkIpcamDialogCameraEditInfo*)user_data;
  if(gtk_toggle_button_get_active(button) == TRUE)
  {
    GtkIpcamCameraObj* camera = info->current_camera;
    if(button == GTK_TOGGLE_BUTTON(info->cameras_edit_remote_http)){
      printf("disabling https\n");
      gtk_ipcam_camera_obj_set_remote_https(camera, FALSE);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(info->cameras_edit_remote_https), FALSE);
    } else {
      printf("disabling http\n");
      gtk_ipcam_camera_obj_set_remote_https(camera, TRUE);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(info->cameras_edit_remote_http), FALSE);
    }
    printf("remote_http_toggled\n");
    changed = TRUE;
  }
}

static void
local_http_toggled(GtkToggleButton* button, gpointer user_data)
{
  GtkIpcamDialogCameraEditInfo* info = (GtkIpcamDialogCameraEditInfo*)user_data;
  if(gtk_toggle_button_get_active(button) == TRUE)
  {
    GtkIpcamCameraObj* camera = info->current_camera;
    if(button == GTK_TOGGLE_BUTTON(info->cameras_edit_local_http)){
      printf("disabling https\n");
      gtk_ipcam_camera_obj_set_local_https(camera, FALSE);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(info->cameras_edit_local_https), FALSE);
    } else {
      printf("disabling http\n");
      gtk_ipcam_camera_obj_set_local_https(camera, TRUE);
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(info->cameras_edit_local_http), FALSE);
    }
    printf("local_http_toggled\n");
    changed = TRUE;
  }
}

static void
subchannel_toggled(GtkToggleButton* button, gpointer user_data)
{
  GtkIpcamDialogCameraEditInfo* info = (GtkIpcamDialogCameraEditInfo*)user_data;
  GtkIpcamCameraObj* camera = info->current_camera;

  printf("subchannel_toggled\n");
  changed = TRUE;
  gtk_ipcam_camera_obj_set_subchannel(camera, gtk_toggle_button_get_active(button));
}
/* TOGGLE BUTTONS FUNCTIONS END */

static void
entry_changed(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
  GtkIpcamDialogCameraEditInfo* info = (GtkIpcamDialogCameraEditInfo*)user_data;
  GtkIpcamCameraObj* camera = info->current_camera;

  if(GTK_IS_ENTRY(widget))
  {
    if(widget == info->cameras_edit_name)
    {
      gtk_ipcam_camera_obj_set_name(camera, gtk_entry_get_text(GTK_ENTRY(widget)));
      rebuild_camera_list(info);
    }
    else if(widget == info->cameras_edit_username)
    {
      gtk_ipcam_camera_obj_set_username(camera, gtk_entry_get_text(GTK_ENTRY(widget)));
    }
    else if(widget == info->cameras_edit_password)
    {
      gtk_ipcam_camera_obj_set_password(camera, gtk_entry_get_text(GTK_ENTRY(widget)));
    }
    else if(widget == info->cameras_edit_remote_url)
    {
      gtk_ipcam_camera_obj_set_remote_url(camera, gtk_entry_get_text(GTK_ENTRY(widget)));
    }
    else if(widget == info->cameras_edit_remote_port)
    {
      gtk_ipcam_camera_obj_set_remote_port(camera, gtk_entry_get_text(GTK_ENTRY(widget)));
    }
    else if(widget == info->cameras_edit_remote_media_port)
    {
      gtk_ipcam_camera_obj_set_remote_media_port(camera, gtk_entry_get_text(GTK_ENTRY(widget)));
    }
    else if(widget == info->cameras_edit_local_url)
    {
      gtk_ipcam_camera_obj_set_local_url(camera, gtk_entry_get_text(GTK_ENTRY(widget)));
    }
    else if(widget == info->cameras_edit_local_port)
    {
      gtk_ipcam_camera_obj_set_local_port(camera, gtk_entry_get_text(GTK_ENTRY(widget)));
    }
    else if(widget == info->cameras_edit_local_media_port)
    {
      gtk_ipcam_camera_obj_set_local_media_port(camera, gtk_entry_get_text(GTK_ENTRY(widget)));
    }
    printf("entry_changed\n");
    changed = TRUE;
  }
}

static void
connect_signals(GtkIpcamDialogCameraEditInfo* info)
{
  printf("connect_signals\n");
  /* HTTP/HTTPS toggled button */
  g_signal_connect(info->cameras_edit_remote_http, "toggled", G_CALLBACK(remote_http_toggled), info);
  g_signal_connect(info->cameras_edit_remote_https, "toggled", G_CALLBACK(remote_http_toggled), info);
  g_signal_connect(info->cameras_edit_local_http, "toggled", G_CALLBACK(local_http_toggled), info);
  g_signal_connect(info->cameras_edit_local_https, "toggled", G_CALLBACK(local_http_toggled), info);
  g_signal_connect(info->cameras_edit_use_subchannel, "toggled", G_CALLBACK(subchannel_toggled), info);
  /* HTTP/HTTPS toggled button END*/

  /* Entry Signal */
  g_signal_connect(info->cameras_edit_name, "focus-out-event", G_CALLBACK(entry_changed), info);
  g_signal_connect(info->cameras_edit_username, "focus-out-event", G_CALLBACK(entry_changed), info);
  g_signal_connect(info->cameras_edit_password, "focus-out-event", G_CALLBACK(entry_changed), info);
  g_signal_connect(info->cameras_edit_remote_url, "focus-out-event", G_CALLBACK(entry_changed), info);
  g_signal_connect(info->cameras_edit_remote_port, "focus-out-event", G_CALLBACK(entry_changed), info);
  g_signal_connect(info->cameras_edit_remote_media_port, "focus-out-event", G_CALLBACK(entry_changed), info);
  g_signal_connect(info->cameras_edit_local_url, "focus-out-event", G_CALLBACK(entry_changed), info);
  g_signal_connect(info->cameras_edit_local_port, "focus-out-event", G_CALLBACK(entry_changed), info);
  g_signal_connect(info->cameras_edit_local_media_port, "focus-out-event", G_CALLBACK(entry_changed), info);
  /* Entry Signal End */

  /* Driver List Signal */
  g_signal_connect(info->cameras_edit_model, "changed", G_CALLBACK(model_changed), info);
  /* Driver List Signal End */
}

static void
block_signals(GtkIpcamDialogCameraEditInfo* info)
{
  printf("block_signals\n");
  /* HTTP/HTTPS toggled button */
  g_signal_handlers_block_by_func(info->cameras_edit_remote_http, remote_http_toggled, info);
  g_signal_handlers_block_by_func(info->cameras_edit_remote_https, remote_http_toggled, info);
  g_signal_handlers_block_by_func(info->cameras_edit_local_http, local_http_toggled, info);
  g_signal_handlers_block_by_func(info->cameras_edit_local_https, local_http_toggled, info);
  g_signal_handlers_block_by_func(info->cameras_edit_use_subchannel, subchannel_toggled, info);
  /* HTTP/HTTPS toggled button END*/

  /* Entry Signal */
  g_signal_handlers_block_by_func(info->cameras_edit_name, entry_changed, info);
  g_signal_handlers_block_by_func(info->cameras_edit_username, entry_changed, info);
  g_signal_handlers_block_by_func(info->cameras_edit_password, entry_changed, info);
  g_signal_handlers_block_by_func(info->cameras_edit_remote_url, entry_changed, info);
  g_signal_handlers_block_by_func(info->cameras_edit_remote_port, entry_changed, info);
  g_signal_handlers_block_by_func(info->cameras_edit_remote_media_port, entry_changed, info);
  g_signal_handlers_block_by_func(info->cameras_edit_local_url, entry_changed, info);
  g_signal_handlers_block_by_func(info->cameras_edit_local_port, entry_changed, info);
  g_signal_handlers_block_by_func(info->cameras_edit_local_media_port, entry_changed, info);
  /* Entry Signal End */

  /* Driver List Signal */
  g_signal_handlers_block_by_func(info->cameras_edit_model, model_changed, info);
  /* Driver List Signal End */
}

static void
unblock_signals(GtkIpcamDialogCameraEditInfo* info)
{
  printf("unblock_signals\n");
  /* HTTP/HTTPS toggled button */
  g_signal_handlers_unblock_by_func(info->cameras_edit_remote_http, remote_http_toggled, info);
  g_signal_handlers_unblock_by_func(info->cameras_edit_remote_https, remote_http_toggled, info);
  g_signal_handlers_unblock_by_func(info->cameras_edit_local_http, local_http_toggled, info);
  g_signal_handlers_unblock_by_func(info->cameras_edit_local_https, local_http_toggled, info);
  g_signal_handlers_unblock_by_func(info->cameras_edit_use_subchannel, subchannel_toggled, info);
  /* HTTP/HTTPS toggled button END*/

  /* Entry Signal */
  g_signal_handlers_unblock_by_func(info->cameras_edit_name, entry_changed, info);
  g_signal_handlers_unblock_by_func(info->cameras_edit_username, entry_changed, info);
  g_signal_handlers_unblock_by_func(info->cameras_edit_password, entry_changed, info);
  g_signal_handlers_unblock_by_func(info->cameras_edit_remote_url, entry_changed, info);
  g_signal_handlers_unblock_by_func(info->cameras_edit_remote_port, entry_changed, info);
  g_signal_handlers_unblock_by_func(info->cameras_edit_remote_media_port, entry_changed, info);
  g_signal_handlers_unblock_by_func(info->cameras_edit_local_url, entry_changed, info);
  g_signal_handlers_unblock_by_func(info->cameras_edit_local_port, entry_changed, info);
  g_signal_handlers_unblock_by_func(info->cameras_edit_local_media_port, entry_changed, info);
  /* Entry Signal End */

  /* Driver List Signal */
  g_signal_handlers_unblock_by_func(info->cameras_edit_model, model_changed, info);
  /* Driver List Signal End */
}

static void
group_changed_callback(GtkIpcamPreferenceObj* preference, gpointer user_data)
{
  GtkIpcamDialogCameraEditInfo* info = (GtkIpcamDialogCameraEditInfo*)user_data;
  gtk_ipcam_camera_groups_refresh(info->group_list_widget, info->preference);
}

void
gtk_ipcam_dialog_cameras_edit_new(GtkIpcamPreferenceObj* preference)
{
  GtkIpcamDialogCameraEditInfo camera_edit_info;
  camera_edit_info.preference = preference;

  GtkStyleContext *context;
  camera_edit_info.builder = gtk_builder_new();
  gchar* filename = g_build_filename(PREFIX,DATADIR,"gipcamviewer","resources","ui","cameras_edit.ui",NULL);
  gtk_builder_add_from_file(camera_edit_info.builder,filename,NULL);
  camera_edit_info.cameras_edit_widget = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit"));
  camera_edit_info.group_list_widget = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"group_list"));
  camera_edit_info.camera_list_widget = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"camera_list"));

  /* form fields */
  camera_edit_info.cameras_form_stack = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"camera_edit_form_stack_widget"));

  camera_edit_info.cameras_edit_model = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_model"));
  camera_edit_info.cameras_edit_name = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_name"));
  camera_edit_info.cameras_edit_username = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_username"));
  camera_edit_info.cameras_edit_password = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_password"));
  camera_edit_info.cameras_edit_remote_protocol = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_remote_protocol"));
  camera_edit_info.cameras_edit_remote_http = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_remote_http"));
  camera_edit_info.cameras_edit_remote_https = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_remote_https"));
  camera_edit_info.cameras_edit_remote_url = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_remote_url"));
  camera_edit_info.cameras_edit_remote_port = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_remote_port"));
  camera_edit_info.cameras_edit_remote_media_port = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_remote_media_port"));
  camera_edit_info.cameras_edit_local_protocol = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_local_protocol"));
  camera_edit_info.cameras_edit_local_http = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_local_http"));
  camera_edit_info.cameras_edit_local_https = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_local_https"));
  camera_edit_info.cameras_edit_local_url = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_local_url"));
  camera_edit_info.cameras_edit_local_port = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_local_port"));
  camera_edit_info.cameras_edit_local_media_port = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_local_media_port"));
  camera_edit_info.cameras_edit_use_subchannel = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_use_subchannel"));

  camera_edit_info.cameras_edit_remote_media = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_remote_media"));
  camera_edit_info.cameras_edit_local_media = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_local_media"));
  /* form fields */

  /* Action buttons */
  GtkWidget* cameras_edit_add_camera_button = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_add_camera_button"));
  GtkWidget* cameras_edit_del_camera_button = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_del_camera_button"));
  GtkWidget* cameras_edit_group_edit_button = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_group_edit_button"));
  camera_edit_info.cameras_edit_down_button = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_down_button"));
  camera_edit_info.cameras_edit_up_button = GTK_WIDGET(gtk_builder_get_object(camera_edit_info.builder,"cameras_edit_up_button"));

  g_signal_connect(cameras_edit_add_camera_button, "clicked", G_CALLBACK(add_camera_btn_callback), &camera_edit_info);
  g_signal_connect(cameras_edit_del_camera_button, "clicked", G_CALLBACK(del_camera_btn_callback), &camera_edit_info);
  g_signal_connect(cameras_edit_group_edit_button, "clicked", G_CALLBACK(edit_group_btn_callback), &camera_edit_info);
  g_signal_connect(camera_edit_info.cameras_edit_down_button, "clicked", G_CALLBACK(cameras_down_btn_callback), &camera_edit_info);
  g_signal_connect(camera_edit_info.cameras_edit_up_button, "clicked", G_CALLBACK(cameras_up_btn_callback), &camera_edit_info);
  /* Action buttons END */

  g_free(filename);

  gtk_builder_connect_signals(camera_edit_info.builder, NULL);

  camera_edit_info.cameras_edit_dialog = gtk_dialog_new_with_buttons(gettext("Cameras Manager"),
                                    GTK_WINDOW(gtk_application_get_active_window(GTK_APPLICATION (g_application_get_default()))),
                                    GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL | GTK_DIALOG_USE_HEADER_BAR,
                                    "\uE876",
                                    GTK_RESPONSE_OK,
                                    NULL);

  GtkHeaderBar* header_bar = GTK_HEADER_BAR(gtk_dialog_get_header_bar(GTK_DIALOG(camera_edit_info.cameras_edit_dialog)));
  context = gtk_widget_get_style_context(GTK_WIDGET(header_bar));
  gtk_style_context_add_class(context,"dialog-header");
  gtk_header_bar_set_show_close_button(header_bar, FALSE);

  GtkWidget* ok_button = gtk_dialog_get_widget_for_response(GTK_DIALOG(camera_edit_info.cameras_edit_dialog),GTK_RESPONSE_OK);
  context = gtk_widget_get_style_context(GTK_WIDGET(ok_button));
  gtk_style_context_add_class(context,"icon-button");

  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(camera_edit_info.cameras_edit_dialog))), camera_edit_info.cameras_edit_widget, TRUE, TRUE, 0);
  gtk_dialog_set_default_response(GTK_DIALOG(camera_edit_info.cameras_edit_dialog),GTK_RESPONSE_OK);

  gtk_window_set_resizable(GTK_WINDOW(camera_edit_info.cameras_edit_dialog), FALSE);

  gtk_widget_show_all(camera_edit_info.cameras_edit_widget);

  /* DRIVER LIST */
  /****************************************************************************************
   * We must connect the model list "changed" signal before
   * refreshing camera groups. Otherwise, the driver list will
   * not be selected on the first load.
   ****************************************************************************************/
  GtkCellRenderer *driver_list_cell = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start( GTK_CELL_LAYOUT(camera_edit_info.cameras_edit_model), driver_list_cell, TRUE );
  gtk_cell_layout_set_attributes( GTK_CELL_LAYOUT(camera_edit_info.cameras_edit_model), driver_list_cell, "text", GTK_IPCAM_COLUMN_CAMERA_DRIVER_NAME, NULL );

  GPtrArray* drivers = gtk_ipcam_camera_driver_obj_list();
  refresh_drivers(&camera_edit_info, drivers);
  gtk_ipcam_camera_driver_obj_list_free(drivers);
  /* DRIVER LIST END */

  connect_signals(&camera_edit_info);

  /* CAMERA LIST */
  /****************************************************************************************
   * We must connect the camera list "row-selected" signal before
   * refreshing camera groups. Otherwise, the camera form will
   * not be displayed for the first selection.
   ****************************************************************************************/
  g_signal_connect(camera_edit_info.camera_list_widget, "row-selected", G_CALLBACK(refresh_camera_form),&camera_edit_info);
  /* CAMERA LIST END */

  /* GOUP COMBO BOX */
  GtkCellRenderer *group_list_cell = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start( GTK_CELL_LAYOUT(camera_edit_info.group_list_widget), group_list_cell, TRUE );
  gtk_cell_layout_set_attributes( GTK_CELL_LAYOUT(camera_edit_info.group_list_widget), group_list_cell, "text", GTK_IPCAM_COLUMN_CAMERA_GROUP_NAME, NULL );

  g_signal_connect(camera_edit_info.group_list_widget, "changed", G_CALLBACK(refresh_camera_list),&camera_edit_info);

  gtk_ipcam_camera_groups_refresh(camera_edit_info.group_list_widget, preference);

  g_signal_connect(camera_edit_info.preference,"group-changed",G_CALLBACK(group_changed_callback),&camera_edit_info);
  /* GROUP COMBO BOX END */

  changed = FALSE;

  gint result = gtk_dialog_run(GTK_DIALOG (camera_edit_info.cameras_edit_dialog));
  switch (result)
  {
    case GTK_RESPONSE_OK:
       break;
    default:
       break;
  }

  /* DESTROY GROUP COMBO BOX */
  GtkListStore* old_model = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(camera_edit_info.group_list_widget)));
  gtk_combo_box_set_model(GTK_COMBO_BOX(camera_edit_info.group_list_widget),NULL);
  if(old_model != NULL){
    g_object_unref(old_model);
  }
  /* DESTROY GROUP COMBO BOX END*/

  /* DESTROY MODEL COMBO BOX */
  old_model = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(camera_edit_info.cameras_edit_model)));
  gtk_combo_box_set_model(GTK_COMBO_BOX(camera_edit_info.cameras_edit_model),NULL);
  if(old_model != NULL){
    g_object_unref(old_model);
  }
  /* DESTROY MODEL COMBO BOX END*/

  gtk_widget_destroy (camera_edit_info.cameras_edit_dialog);
  camera_edit_info.cameras_edit_dialog = NULL;

  g_object_unref(camera_edit_info.builder);

  if(changed)
  {
    gtk_ipcam_preference_obj_save(preference, TRUE);
  }
}
