#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <libintl.h>
#include <X11/Xlib.h>
#include <gtk/gtk.h>

#include <gst/gst.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include <gdk/gdk.h>
#include<pango/pangocairo.h>

#include <gst/player/player.h>
#include "gtk_vlc_player.h"
#include "gtk_ipcam_player.h"

#include "gtk_ipcam_preference_obj.h"

#include "gtk_ipcam_dialog_cameras_edit.h"

#include "config.h"

#define APP_NAME "GIpcamViewer"

typedef GtkApplication GtkIpcamViewerApp;
typedef GtkApplicationClass GtkIpcamViewerAppClass;

GType gtk_ipcam_viewer_app_get_type (void);
G_DEFINE_TYPE (GtkIpcamViewerApp, gtk_ipcam_viewer_app, GTK_TYPE_APPLICATION);

typedef struct
{
  GtkApplicationWindow parent;

  GPtrArray* g_players;

  GtkBox* main_area;
  GtkGrid* cameras_area;
  GtkWidget* null_layer;
  GtkWidget* group_list_title_widget;
  GtkWidget* main_popover_mennu_widget;
  guint cameras_count;

  GtkIpcamPreferenceObj* preference;

} GtkIpcamViewerWindow;

typedef GtkApplicationWindowClass GtkIpcamViewerWindowClass;

G_DEFINE_TYPE (GtkIpcamViewerWindow, gtk_ipcam_viewer_window, GTK_TYPE_APPLICATION_WINDOW);

enum
{
  PROP_0,
//  PROP_URIS,
  LAST_PROP
};

static GParamSpec *gtk_ipcam_viewer_properties[LAST_PROP] = { NULL, };

static void
gtk_ipcam_viewer_window_set_property (GObject * object, guint prop_id, const GValue * value,
    GParamSpec * pspec)
{
  //GtkIpcamViewerWindow *self = (GtkIpcamViewerWindow *) object;

  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
set_title (GtkIpcamViewerWindow * win, const gchar * title)
{
  if (title == NULL) {
    gtk_window_set_title (GTK_WINDOW (win), APP_NAME);
  } else {
    gtk_window_set_title (GTK_WINDOW (win), title);
  }
}

static gboolean
draw_titlebar(GtkWidget *widget, cairo_t *cr, gpointer data)
{
  //Paint the event box header bar.
  gint width=gtk_widget_get_allocated_width(widget);
  gint height=gtk_widget_get_allocated_height(widget);

  GtkStyleContext *context=gtk_widget_get_style_context(widget);
  gtk_render_background(context, cr, 0, 0, width, height);

  return FALSE;
}

static void
create_camera(gpointer data, gpointer user_data)
{
  GtkIpcamViewerWindow* win = (GtkIpcamViewerWindow*)user_data;
  g_assert(GTK_IS_FOSCAM_CAMERA_OBJ(data));
  GtkIpcamCameraObj* camera = GTK_FOSCAM_CAMERA_OBJ(data);

  if(gtk_ipcam_camera_obj_is_valid(camera))
  {
    GtkIpcamPlayer* player = gtk_ipcam_player_new(camera);
    g_assert(GTK_IS_FOSCAM_PLAYER(player));
    g_ptr_array_add(win->g_players, player);
  }
}

static void
destroy_player(gpointer data)
{
  g_assert(GTK_IS_FOSCAM_PLAYER(data));
  GtkIpcamPlayer * player = GTK_FOSCAM_PLAYER(data);
  gtk_widget_destroy(GTK_WIDGET(player));
  //g_object_unref(camera);
}

static void
create_cameras(GtkIpcamViewerWindow * win)
{
  gint active_group = gtk_combo_box_get_active(GTK_COMBO_BOX(win->group_list_title_widget));
  GtkIpcamCameraGroupObj* group = GTK_FOSCAM_CAMERA_GROUP_OBJ(gtk_ipcam_preference_obj_get_camera_group_by_index(win->preference,active_group));

  g_assert(GTK_IS_FOSCAM_CAMERA_GROUP_OBJ(group));

  win->g_players = g_ptr_array_new();
  g_ptr_array_set_free_func(win->g_players,destroy_player);

  gtk_ipcam_camera_group_obj_foreach_camera(group, create_camera, win);
}

static void
destroy_cameras(GtkIpcamViewerWindow* self)
{
  if(self->g_players != NULL){
    g_ptr_array_unref(self->g_players);
    self->g_players = NULL;
  }
}

static void
layout_remove_camera(gpointer data, gpointer user_data)
{
  printf("removing cameras\n");
  GtkIpcamViewerWindow* win = (GtkIpcamViewerWindow*)user_data;
  g_assert(GTK_IS_FOSCAM_PLAYER(data));
  GtkIpcamPlayer * player = GTK_FOSCAM_PLAYER(data);

  gtk_ipcam_player_stop_video(player);
  gtk_ipcam_player_stop_audio(player);

  if(GTK_IS_GRID(win->cameras_area))
  {
    g_object_ref(player);
    gtk_container_remove(GTK_CONTAINER(win->cameras_area), GTK_WIDGET(player));
    printf("camera removed\n");
  }
}

static void
layout_camera(gpointer data, gpointer user_data)
{
  GtkIpcamViewerWindow* win = (GtkIpcamViewerWindow*)user_data;
  g_assert(GTK_IS_FOSCAM_PLAYER(data));
  GtkIpcamPlayer * player = GTK_FOSCAM_PLAYER(data);

  printf("layout_camera(%d)\n",win->cameras_count);
  /*
   * TODO: improve layout with not even configurations, maybe, pagination, etc..
   */
  guint grid_width=2;
  guint row = 0, col = 0, width = 1, height = 1;

  row = floor(win->cameras_count/grid_width);
  col = abs((row*grid_width) - win->cameras_count);

  if(GTK_IS_GRID(win->cameras_area))
  {
    gtk_grid_attach(win->cameras_area, GTK_WIDGET(player), col, row, width, height);
    gtk_widget_show_all(GTK_WIDGET(player));
    gtk_ipcam_player_load(player);
    g_object_unref(player);
  }

  win->cameras_count ++;
}

static void
layout_cameras(GtkIpcamViewerWindow * win)
{
  gint active_group = gtk_combo_box_get_active(GTK_COMBO_BOX(win->group_list_title_widget));
  GtkIpcamCameraGroupObj* group = GTK_FOSCAM_CAMERA_GROUP_OBJ(gtk_ipcam_preference_obj_get_camera_group_by_index(win->preference,active_group));

  if(win->g_players == NULL) return;

  g_ptr_array_foreach(win->g_players, layout_remove_camera, win);

  if(gtk_ipcam_camera_group_obj_get_cameras_count(group) > 0)
  {
    gtk_container_remove(GTK_CONTAINER(win->cameras_area), win->null_layer);
    win->cameras_count = 0;
    g_ptr_array_foreach(win->g_players, layout_camera, win);
  } else {
    if(GTK_IS_GRID(win->cameras_area))
    {
      gtk_grid_attach(win->cameras_area, win->null_layer, 1, 1, 1, 1);
      gtk_widget_show_all(win->null_layer);
    }
  }
}

static void
gtk_ipcam_camera_edit_callback(GtkWidget* widget, gpointer data)
{
  GtkIpcamViewerWindow* win = (GtkIpcamViewerWindow*)data;
  gtk_ipcam_dialog_cameras_edit_new(win->preference);
}

static void
gtk_ipcam_preferences_callback(GtkWidget* widget, gpointer data)
{
  GtkIpcamViewerWindow* win = (GtkIpcamViewerWindow*)data;
  gtk_popover_popup(GTK_POPOVER(win->main_popover_mennu_widget));
}

static void
gtk_ipcam_reload_callback(GtkWidget* widget, gpointer user_data)
{
  GtkIpcamViewerWindow* win = (GtkIpcamViewerWindow*)user_data;
  destroy_cameras(win);
  create_cameras(win);
  layout_cameras(win);
}

static void
refresh_cameras_area(GtkComboBox* combo, gpointer user_data)
{
  GtkIpcamViewerWindow* win = (GtkIpcamViewerWindow*)user_data;
  destroy_cameras(win);
  create_cameras(win);
  layout_cameras(win);
}

static void
create_ui(GtkIpcamViewerWindow * win)
{
  GdkMonitor* monitor = gdk_display_get_monitor_at_window(gdk_display_get_default(), gtk_widget_get_window(GTK_WIDGET(win)));

  gint width = gtk_ipcam_preference_obj_get_width(win->preference);
  gint height = gtk_ipcam_preference_obj_get_height(win->preference);

  GdkRectangle area;
  gdk_monitor_get_workarea(monitor,&area);

  if(width <= 0) {
    width = 800;
  }

  if(width > area.width){
    width = area.width;
  }

  if(height > area.height){
    height = area.height;
  }

  if(height <= 0) {
    height = 600;
  }

  gtk_window_set_position(GTK_WINDOW (win), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size (GTK_WINDOW (win), width, height);

  //g_signal_connect (G_OBJECT (play), "delete-event",
  //    G_CALLBACK (delete_event_cb), play);

  //gtk_widget_set_events (GTK_WIDGET (play),
  //    GDK_KEY_RELEASE_MASK | GDK_KEY_PRESS_MASK);
  //g_signal_connect (G_OBJECT (play), "key-press-event",
  //    G_CALLBACK (key_press_event_cb), NULL);

  GtkBuilder* builder = gtk_builder_new();
  gchar* filename = g_build_filename(PREFIX,DATADIR,"gipcamviewer","resources","ui","main_header.ui",NULL);
  gtk_builder_add_from_file(builder,filename,NULL);
  GtkWidget* header = GTK_WIDGET(gtk_builder_get_object(builder,"main_header"));
  GtkWidget* add_new_camera_button = GTK_WIDGET(gtk_builder_get_object(builder,"add_new_camera_button"));
  GtkWidget* preferences_button = GTK_WIDGET(gtk_builder_get_object(builder,"preferences_button"));
  win->group_list_title_widget = GTK_WIDGET(gtk_builder_get_object(builder,"group_list_title"));
  g_free(filename);

  g_signal_connect(header, "draw", G_CALLBACK(draw_titlebar), NULL);
  g_signal_connect(add_new_camera_button, "clicked", G_CALLBACK(gtk_ipcam_camera_edit_callback), win);
  g_signal_connect(preferences_button, "clicked", G_CALLBACK(gtk_ipcam_preferences_callback), win);
  gtk_window_set_titlebar(GTK_WINDOW(win), header);

  /* Title Group Combo */
  GtkCellRenderer *group_list_cell = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start( GTK_CELL_LAYOUT(win->group_list_title_widget), group_list_cell, TRUE );
  gtk_cell_layout_set_attributes( GTK_CELL_LAYOUT(win->group_list_title_widget), group_list_cell, "text", GTK_FOSCAM_COLUMN_CAMERA_GROUP_NAME, NULL );

  g_signal_connect(win->group_list_title_widget, "changed", G_CALLBACK(refresh_cameras_area),win);

  gtk_ipcam_camera_groups_refresh(win->group_list_title_widget, win->preference);
  /* Title Group Combo End */

  set_title (win, APP_NAME);

  gtk_application_add_window (GTK_APPLICATION (g_application_get_default ()),
      GTK_WINDOW (win));

  filename = g_build_filename(PREFIX,DATADIR,"gipcamviewer","resources","ui","main_area.ui",NULL);
  gtk_builder_add_from_file(builder,filename,NULL);
  win->main_area = GTK_BOX(gtk_builder_get_object(builder,"main_area"));
  win->cameras_area = GTK_GRID(gtk_builder_get_object(builder,"cameras_area"));
  GtkWidget* properties = GTK_WIDGET(gtk_builder_get_object(builder,"cameras_properties"));
  g_free(filename);

  g_assert(win->main_area != NULL);
  g_assert(win->cameras_area != NULL);

  gtk_container_add(GTK_CONTAINER(win),GTK_WIDGET(win->main_area));
  /*
   * I will not handle this to 1.0v. But, let it here.
   * Could be used in a later version
   */
  gtk_container_remove(GTK_CONTAINER(win->main_area),GTK_WIDGET(properties));

  gtk_builder_connect_signals(builder, NULL);
  g_object_unref(builder);

  builder = gtk_builder_new();
  filename = g_build_filename(PREFIX,DATADIR,"gipcamviewer","resources","ui","null_layer.ui",NULL);
  gtk_builder_add_from_file(builder,filename,NULL);
  win->null_layer = GTK_WIDGET(gtk_builder_get_object(builder,"null_layer"));
  g_object_ref(win->null_layer);
  g_free(filename);
  gtk_builder_connect_signals(builder, NULL);
  g_object_unref(builder);


  builder = gtk_builder_new();
  filename = g_build_filename(PREFIX,DATADIR,"gipcamviewer","resources","ui","main_popover_menu.ui",NULL);
  gtk_builder_add_from_file(builder,filename,NULL);
  win->main_popover_mennu_widget = GTK_WIDGET(gtk_builder_get_object(builder,"main_popover_menu_widget"));
  GtkWidget* refresh_button = GTK_WIDGET(gtk_builder_get_object(builder,"main_popover_menu_reload_button"));
  g_object_ref(win->main_popover_mennu_widget);
  g_free(filename);
  gtk_builder_connect_signals(builder, NULL);
  g_object_unref(builder);

  gtk_popover_set_relative_to(GTK_POPOVER(win->main_popover_mennu_widget),preferences_button);
  g_signal_connect(refresh_button, "clicked", G_CALLBACK(gtk_ipcam_reload_callback), win);

  layout_cameras(win);

  //GstPlayer* player;

  //player = gtk_ipcam_player_get_player(win->g_player);
  //gst_player_set_uri (player, "http://otacasa.getmyip.com:3002/videostream.cgi?user=admin&pwd=ota1808&rate=25");
  //gst_player_play (player);
  //gst_player_set_rate(player,25);

  //gtk_vlc_player_load_uri(GTK_VLC_PLAYER(gtk_ipcam_player_get_widget(GTK_FOSCAM_PLAYER(win->g_player))),"http://otacasa.getmyip.com:3002/videostream.cgi?user=admin&pwd=ota1808&rate=25");

  //player = gtk_ipcam_player_get_player(win->g_player1);
  //gst_player_set_uri (player, "http://otacasa.getmyip.com:3003/videostream.cgi?user=admin&pwd=ota1808&rate=25");
  //gst_player_play (player);
  //gst_player_set_rate(player,25);
  //gtk_vlc_player_load_uri(GTK_VLC_PLAYER(gtk_ipcam_player_get_widget(GTK_FOSCAM_PLAYER(win->g_player1))),"http://otacasa.getmyip.com:3003/videostream.cgi?user=admin&pwd=ota1808&rate=25");

  //player = gtk_ipcam_player_get_player(win->g_player2);
  //gst_player_set_uri (player, "http://otacasa.getmyip.com:3004/videostream.cgi?user=admin&pwd=ota1808&rate=25");
  //gst_player_play (player);
  //gst_player_set_rate(player,25);
  //gtk_vlc_player_load_uri(GTK_VLC_PLAYER(gtk_ipcam_player_get_widget(GTK_FOSCAM_PLAYER(win->g_player2))),"http://otacasa.getmyip.com:3004/videostream.cgi?user=admin&pwd=ota1808&rate=25");

  //player = gtk_ipcam_player_get_player(win->g_player3);
  //gst_player_set_uri (player, "rtsp://otaviojr:ota180879@otacasa.getmyip.com:3011/videoMain");
  //gst_player_play (player);
  //gst_player_set_rate(player,25);
  //gtk_vlc_player_load_uri(GTK_VLC_PLAYER(gtk_ipcam_player_get_widget(GTK_FOSCAM_PLAYER(win->g_player3))),"rtsp://otaviojr:ota180879@otacasa.getmyip.com:3011/videoMain");

  //gtk_vlc_player_play(GTK_VLC_PLAYER(gtk_ipcam_player_get_widget(GTK_FOSCAM_PLAYER(win->g_player))));
  //gtk_vlc_player_play(GTK_VLC_PLAYER(gtk_ipcam_player_get_widget(GTK_FOSCAM_PLAYER(win->g_player1))));
  //gtk_vlc_player_play(GTK_VLC_PLAYER(gtk_ipcam_player_get_widget(GTK_FOSCAM_PLAYER(win->g_player2))));
  //gtk_vlc_player_play(GTK_VLC_PLAYER(gtk_ipcam_player_get_widget(GTK_FOSCAM_PLAYER(win->g_player3))));
}

static void
attach_style(GtkIpcamViewerWindow * win)
{
  GtkCssProvider *cssProvider = gtk_css_provider_new();
  gchar* filename = g_build_filename(PREFIX,DATADIR,"gipcamviewer","resources","css","main.css",NULL);
  gtk_css_provider_load_from_path(cssProvider, filename, NULL);
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(cssProvider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  g_free(filename);
  g_object_unref (cssProvider);
}

static GObject *
gtk_ipcam_viewer_window_constructor (GType type, guint n_construct_params,
    GObjectConstructParam * construct_params)
{
  GtkIpcamViewerWindow *self;

  self =
      (GtkIpcamViewerWindow *) G_OBJECT_CLASS (gtk_ipcam_viewer_window_parent_class)->constructor (type,
      n_construct_params, construct_params);

  create_cameras(self);
  create_ui(self);
  attach_style(self);

  return G_OBJECT (self);
}

static void
gtk_ipcam_viewer_window_dispose (GObject * object)
{
  GtkIpcamViewerWindow *self = (GtkIpcamViewerWindow *) object;

  destroy_cameras(self);

  if(self->preference){
    gint width, height, left, top;
    gtk_window_get_size(GTK_WINDOW(self),&width,&height);
    gtk_window_get_position(GTK_WINDOW(self),&left,&top);
    gtk_ipcam_preference_obj_set_width(self->preference, width);
    gtk_ipcam_preference_obj_set_height(self->preference, height);
    gtk_ipcam_preference_obj_set_left(self->preference, left);
    gtk_ipcam_preference_obj_set_top(self->preference, top);
    gtk_ipcam_preference_obj_save(self->preference, FALSE);
    g_object_unref(self->preference);
  }
  self->preference = NULL;

  G_OBJECT_CLASS (gtk_ipcam_viewer_window_parent_class)->dispose(object);
}

static void
gtk_ipcam_viewer_window_class_init (GtkIpcamViewerWindowClass * klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->constructor = gtk_ipcam_viewer_window_constructor;
  object_class->dispose = gtk_ipcam_viewer_window_dispose;
  object_class->set_property = gtk_ipcam_viewer_window_set_property;

  g_object_class_install_properties (object_class, LAST_PROP,
      gtk_ipcam_viewer_properties);
}

static void
preference_change_callback(GtkIpcamPreferenceObj* preference, gpointer user_data)
{
  GtkIpcamViewerWindow* win = (GtkIpcamViewerWindow*)user_data;
  destroy_cameras(win);
  create_cameras(win);
  layout_cameras(win);
}

static void
refresh_camera_group_callback(GtkIpcamPreferenceObj* preference, gpointer user_data)
{
  GtkIpcamViewerWindow* win = (GtkIpcamViewerWindow*)user_data;
  gtk_ipcam_camera_groups_refresh(win->group_list_title_widget, win->preference);
}

static void
gtk_ipcam_viewer_window_init(GtkIpcamViewerWindow * win)
{
  win->preference = gtk_ipcam_preference_obj_new();
  if(gtk_ipcam_preference_obj_get_camera_group_count(win->preference)<= 0){
    gtk_ipcam_preference_obj_add_camera_group(win->preference,gettext("Default"));
  }

  g_signal_connect(win->preference, "changed", G_CALLBACK(preference_change_callback), win);
  g_signal_connect(win->preference, "group-changed", G_CALLBACK(refresh_camera_group_callback),win);
}

static gint
gtk_ipcam_viewer_app_command_line (GApplication * application,
    GApplicationCommandLine * command_line)
{
  //GVariantDict *options;
  GtkIpcamViewerWindow *win;

  //options = g_application_command_line_get_options_dict(command_line);

  win =
      g_object_new (gtk_ipcam_viewer_window_get_type (), NULL);
  gtk_widget_show_all(GTK_WIDGET (win));

  return
      G_APPLICATION_CLASS (gtk_ipcam_viewer_app_parent_class)->command_line
      (application, command_line);
}

static void
gtk_ipcam_viewer_app_class_init (GtkIpcamViewerAppClass * klass)
{
  GApplicationClass *application_class = G_APPLICATION_CLASS (klass);
  application_class->command_line = gtk_ipcam_viewer_app_command_line;
}

static GtkIpcamViewerApp*
gtk_ipcam_viewer_app_new(void)
{
  GtkIpcamViewerApp* self;

  self = g_object_new (gtk_ipcam_viewer_app_get_type(),
      "application-id", "org.gtk.foscamviewer",
      "flags", G_APPLICATION_HANDLES_COMMAND_LINE,
      "register-session", TRUE, NULL);

  g_application_set_default (G_APPLICATION (self));
  //g_application_add_option_group (G_APPLICATION (self),
  //    gst_init_get_option_group ());

  return self;
}

static void
gtk_ipcam_viewer_app_init (GtkIpcamViewerApp * self)
{
}

int main(int argc, char *argv[])
{
  GtkIpcamViewerApp *app;
  gint status;

  XInitThreads();

  app = gtk_ipcam_viewer_app_new ();
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  //gst_deinit ();
  return status;
}
