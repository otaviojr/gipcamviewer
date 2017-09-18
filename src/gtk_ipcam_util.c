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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <glib.h>

#include "gtk_ipcam_util.h"

static gint group_count = 0;

/* GROUP COMBO BOX FUNCTIONS*/
static void
add_group(gpointer data, gpointer user_data)
{
  GtkTreeIter iter;
  GtkListStore* groups = GTK_LIST_STORE(user_data);
  GtkIpcamCameraGroupObj* camera_group = GTK_IPCAM_CAMERA_GROUP_OBJ(data);

  gtk_list_store_append (groups, &iter);

  printf("Adding group %s\n", gtk_ipcam_camera_group_obj_get_name(camera_group));

  gtk_list_store_set (groups, &iter,
                          GTK_IPCAM_COLUMN_CAMERA_GROUP_ID, group_count++,
                          GTK_IPCAM_COLUMN_CAMERA_GROUP_NAME, gtk_ipcam_camera_group_obj_get_name(camera_group),
                          -1);
}

static GtkListStore*
create_camera_group_list(GtkIpcamPreferenceObj* preference)
{
  GtkListStore* tmp_groups = NULL;

  if(gtk_ipcam_preference_obj_get_camera_group_count(preference) > 0)
  {
    tmp_groups = gtk_list_store_new(GTK_IPCAM_COLUMN_CAMERA_GROUP_LAST,G_TYPE_INT, G_TYPE_STRING);
    group_count = 0;
    gtk_ipcam_preference_obj_foreach_camera_group(preference, add_group, tmp_groups);
  }

  return tmp_groups;
}

void
gtk_ipcam_camera_groups_refresh(GtkWidget* combo, GtkIpcamPreferenceObj* preference)
{
  GtkListStore* old_model = GTK_LIST_STORE(gtk_combo_box_get_model(GTK_COMBO_BOX(combo)));
  gtk_combo_box_set_model(GTK_COMBO_BOX(combo),NULL);
  if(old_model != NULL)
  {
    g_object_unref(old_model);
  }

  GtkListStore* groups = create_camera_group_list(preference);
  if(groups != NULL)
  {
    gtk_combo_box_set_model(GTK_COMBO_BOX(combo),GTK_TREE_MODEL(groups));
    gtk_combo_box_set_entry_text_column(GTK_COMBO_BOX(combo),GTK_IPCAM_COLUMN_CAMERA_GROUP_NAME);
    gtk_combo_box_set_id_column(GTK_COMBO_BOX(combo),GTK_IPCAM_COLUMN_CAMERA_GROUP_NAME);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo),0);
  }
}
/* GROUP COMBO BOX FUNCTIONS END*/

gchar*
gtk_ipcam_replace_str(const gchar* original, const gchar* search, const gchar* replace)
{
  const gchar* pointer = original;
  const gchar* last_pointer = original;
  gchar* buffer = NULL;
  gchar* tmp_buffer = NULL;
  guint buffer_size = 0, last_buffer_size = 0;

  while(pointer != NULL)
  {
    pointer = g_strstr_len(last_pointer, -1, search);
    if(pointer != NULL){
      last_buffer_size = buffer_size;
      buffer_size += (pointer-last_pointer) + strlen(replace);
      if(buffer == NULL){
        buffer = g_malloc(buffer_size+sizeof(gchar));
        tmp_buffer = buffer;
      } else {
        buffer = g_realloc(buffer, buffer_size+sizeof(gchar));
        tmp_buffer = buffer + last_buffer_size;
      }
      memcpy(tmp_buffer, last_pointer, pointer - last_pointer);
      tmp_buffer += (pointer - last_pointer);
      memcpy(tmp_buffer, replace, strlen(replace));
      tmp_buffer += strlen(replace);
      last_pointer = pointer + strlen(search);
    }
  }

  //printf("last_pointer - original = %d\noriginal length: %d\n", last_pointer - original, strlen(original));
  if(last_pointer - original < strlen(original)){
    guint remaining_size = strlen(original) - (last_pointer - original);
    if(tmp_buffer == NULL)
    {
      buffer = g_malloc(remaining_size+sizeof(gchar));
      tmp_buffer = buffer;
    } else {
      buffer = g_realloc(buffer, buffer_size+remaining_size+sizeof(gchar));
      tmp_buffer = buffer + buffer_size;
    }
    memcpy(tmp_buffer, last_pointer, remaining_size);
    tmp_buffer+=strlen(original) - (last_pointer - original);
    *tmp_buffer = 0;
  }
  //printf("Substitution = %s\n", buffer);
  return buffer;
}
