#ifndef __GTK_FOSCAM_DIALOG_GROUPS_EDIT_H__
#define __GTK_FOSCAM_DIALOG_GROUPS_EDIT_H__

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

#endif //__GTK_FOSCAM_DIALOG_GROUPS_EDIT_H__
