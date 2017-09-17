#ifndef __GTK_IPCAM_DIALOG_CAMERAS_EDIT_H__
#define __GTK_IPCAM_DIALOG_CAMERAS_EDIT_H__

#include "gtk_ipcam_util.h"
#include "gtk_ipcam_preference_obj.h"
#include "gtk_ipcam_dialog_groups_edit.h"


enum {
  GTK_IPCAM_COLUMN_CAMERA_DRIVER_ID,
  GTK_IPCAM_COLUMN_CAMERA_DRIVER_NAME,
  GTK_IPCAM_COLUMN_CAMERA_DRIVER_LAST
};

typedef struct _GtkIpcamDialogCameraEditInfo {
  GtkIpcamPreferenceObj* preference;
  GtkBuilder* builder;

  GtkWidget* cameras_edit_dialog;

  GtkWidget* cameras_edit_widget;
  GtkWidget* group_list_widget;
  GtkWidget* camera_list_widget;

  /* form fields */
  GtkWidget* cameras_form_stack;

  GtkWidget* cameras_edit_model;
  GtkWidget* cameras_edit_name;
  GtkWidget* cameras_edit_username;
  GtkWidget* cameras_edit_password;
  GtkWidget* cameras_edit_remote_protocol;
  GtkWidget* cameras_edit_remote_http;
  GtkWidget* cameras_edit_remote_https;
  GtkWidget* cameras_edit_remote_url;
  GtkWidget* cameras_edit_remote_port;
  GtkWidget* cameras_edit_remote_media_port;
  GtkWidget* cameras_edit_local_protocol;
  GtkWidget* cameras_edit_local_http;
  GtkWidget* cameras_edit_local_https;
  GtkWidget* cameras_edit_local_url;
  GtkWidget* cameras_edit_local_port;
  GtkWidget* cameras_edit_local_media_port;
  GtkWidget* cameras_edit_use_subchannel;

  GtkWidget* cameras_edit_remote_media;
  GtkWidget* cameras_edit_local_media;
  /* form fields */

  GtkWidget* cameras_edit_down_button;
  GtkWidget* cameras_edit_up_button;

  GtkIpcamCameraObj* current_camera;
} GtkIpcamDialogCameraEditInfo;

void gtk_ipcam_dialog_cameras_edit_new();

#endif //__GTK_IPCAM_DIALOG_CAMERAS_EDIT_H__
