#include <gtk/gtk.h>

#include <gst/gst.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include <gdk/gdk.h>

#include <gst/player/player.h>

#include "gtk_ipcam_player.h"
#include "gtk_vlc_player.h"
#include "gtk_video_renderer.h"
#include "gtk_ipcam_util.h"

struct _GtkIpcamPlayer
{
  GtkEventBox parent;

  //GstPlayer *player;
  //GstPlayerVideoRenderer *renderer;

  GtkWidget* btn_down;
  GtkWidget* btn_up;
  GtkWidget* btn_left;
  GtkWidget* btn_right;

  GtkWidget* video_area;
  GtkIpcamCameraObj* camera;

  gint state;

  GThread* load_thread;

  gboolean is_flipped;
  gboolean is_mirrored;
};

struct _GtkIpcamPlayerClass
{
  GtkEventBoxClass parent_class;
};

enum
{
  GTK_IPCAM_PLAYER_PROP_0,
  GTK_IPCAM_PLAYER_PROP_WIDGET,
  GTK_IPCAM_PLAYER_PROP_CAMERA,
  GTK_IPCAM_PLAYER_PROP_LAST
};

G_DEFINE_TYPE (GtkIpcamPlayer, gtk_ipcam_player, GTK_TYPE_EVENT_BOX);

static GParamSpec
* gtk_ipcam_player_param_specs[GTK_IPCAM_PLAYER_PROP_LAST] = { NULL, };

static void
gtk_ipcam_player_get_property(GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec)
{
  GtkIpcamPlayer *self = GTK_IPCAM_PLAYER(object);

  switch (prop_id) {
    //case GTK_IPCAM_PLAYER_PROP_WIDGET:
    //  g_value_set_object (value, self->frame);
    //  break;
    case GTK_IPCAM_PLAYER_PROP_CAMERA:
      g_value_set_object(value, self->camera);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gtk_ipcam_player_set_property(GObject * object, guint prop_id, const GValue * value,
    GParamSpec * pspec)
{
  GtkIpcamPlayer *self = GTK_IPCAM_PLAYER(object);

  switch (prop_id) {
    case GTK_IPCAM_PLAYER_PROP_CAMERA:
      printf("Setting up player camera\n");
      if(self->camera != NULL){
        g_object_unref(self->camera);
        self->camera = NULL;
      }
      self->camera = g_value_get_object(value);
      g_object_ref(self->camera);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gtk_ipcam_player_finalize(GObject * object)
{
  printf("gtk_ipcam_player_finalize\n");
  GtkIpcamPlayer *self = GTK_IPCAM_PLAYER(object);

  if(self->load_thread)
  {
    g_thread_join(self->load_thread);
    g_thread_unref(self->load_thread);
    self->load_thread = NULL;
  }

  //if(self->state == GTK_IPCAM_PLAYER_STATE_PLAYING)
  //{
  //  gtk_ipcam_player_stop_video(self);
  //  gtk_ipcam_player_stop_audio(self);
  //}

  //if(self->frame)
  //{
  //  gtk_widget_destroy(self->frame);
  //  self->frame = NULL;
  //}

  if(self->camera != NULL)
  {
    g_object_unref(self->camera);
    self->camera = NULL;
  }

  //if (self->player) {
  //  gst_player_stop (self->player);
  //  g_object_unref (self->player);
  //}
  //self->player = NULL;

  printf("gtk_ipcam_player_finalized\n");

  G_OBJECT_CLASS
      (gtk_ipcam_player_parent_class)->finalize(object);
}

static gpointer
gtk_ipcam_player_move_up_background(gpointer user_data)
{
  GtkIpcamPlayer *self = (GtkIpcamPlayer *)user_data;

  gtk_ipcam_camera_obj_move_up(self->camera);
  return NULL;
}

static gpointer
gtk_ipcam_player_move_down_background(gpointer user_data)
{
  GtkIpcamPlayer *self = (GtkIpcamPlayer *)user_data;

  gtk_ipcam_camera_obj_move_down(self->camera);
  return NULL;
}

static gpointer
gtk_ipcam_player_move_left_background(gpointer user_data)
{
  GtkIpcamPlayer *self = (GtkIpcamPlayer *)user_data;

  gtk_ipcam_camera_obj_move_left(self->camera);
  return NULL;
}

static gpointer
gtk_ipcam_player_move_right_background(gpointer user_data)
{
  GtkIpcamPlayer *self = (GtkIpcamPlayer *)user_data;

  gtk_ipcam_camera_obj_move_right(self->camera);
  return NULL;
}

static void
gtk_ipcam_player_run_backgroup(GtkIpcamPlayer *self, GThreadFunc func)
{
  if(self->load_thread)
  {
    g_thread_join(self->load_thread);
    g_thread_unref(self->load_thread);
  }
  self->load_thread = g_thread_new("player_run_background", func, self);
}

static void
gtk_ipcam_player_up_cb(GtkWidget* widget, gpointer user_data)
{
  printf("goind up\n");
  GtkIpcamPlayer* self = GTK_IPCAM_PLAYER(user_data);
  gtk_ipcam_player_run_backgroup(self,gtk_ipcam_player_move_up_background);
}

static void
gtk_ipcam_player_down_cb(GtkWidget* widget, gpointer user_data)
{
  printf("goind down\n");
  GtkIpcamPlayer* self = GTK_IPCAM_PLAYER(user_data);
  gtk_ipcam_player_run_backgroup(self,gtk_ipcam_player_move_down_background);
}

static void
gtk_ipcam_player_left_cb(GtkWidget* widget, gpointer user_data)
{
  printf("goind left\n");
  GtkIpcamPlayer* self = GTK_IPCAM_PLAYER(user_data);
  gtk_ipcam_player_run_backgroup(self,gtk_ipcam_player_move_left_background);
}

static void
gtk_ipcam_player_right_cb(GtkWidget* widget, gpointer user_data)
{
  printf("goind right\n");
  GtkIpcamPlayer* self = GTK_IPCAM_PLAYER(user_data);
  gtk_ipcam_player_run_backgroup(self,gtk_ipcam_player_move_right_background);
}

static void
gtk_ipcam_enable_controls(GtkIpcamPlayer* self)
{
  if(self->is_flipped)
  {
    g_signal_connect(G_OBJECT(self->btn_up), "clicked", G_CALLBACK (gtk_ipcam_player_down_cb), self);
    g_signal_connect(G_OBJECT(self->btn_down), "clicked", G_CALLBACK (gtk_ipcam_player_up_cb), self);
  }
  else
  {
    g_signal_connect(G_OBJECT(self->btn_up), "clicked", G_CALLBACK (gtk_ipcam_player_up_cb), self);
    g_signal_connect(G_OBJECT(self->btn_down), "clicked", G_CALLBACK (gtk_ipcam_player_down_cb), self);
  }

  if(!self->is_mirrored)
  {
    g_signal_connect(G_OBJECT(self->btn_left), "clicked", G_CALLBACK (gtk_ipcam_player_right_cb), self);
    g_signal_connect(G_OBJECT(self->btn_right), "clicked", G_CALLBACK (gtk_ipcam_player_left_cb), self);
  }
  else
  {
    g_signal_connect(G_OBJECT(self->btn_left), "clicked", G_CALLBACK (gtk_ipcam_player_left_cb), self);
    g_signal_connect(G_OBJECT(self->btn_right), "clicked", G_CALLBACK (gtk_ipcam_player_right_cb), self);
  }
}

static gpointer
gtk_ipcam_player_background_load(gpointer user_data)
{
  GtkIpcamPlayer *self = (GtkIpcamPlayer *)user_data;

  if(gtk_ipcam_camera_obj_init_driver(self->camera))
  {
    self->state = GTK_IPCAM_PLAYER_STATE_LOADED;
    self->is_flipped = gtk_ipcam_camera_obj_is_flipped(self->camera);
    self->is_mirrored = gtk_ipcam_camera_obj_is_mirrored(self->camera);

    gchar* camera_url = gtk_ipcam_camera_obj_get_stream_url(self->camera);

    if(camera_url != NULL)
    {
      printf("Playing: %s\n",camera_url);
      gtk_vlc_player_load_uri(GTK_VLC_PLAYER(self->video_area),camera_url);

      g_free(camera_url);

      gtk_ipcam_enable_controls(self);
      gtk_ipcam_player_start_video(self);
    }
  }
  return NULL;
}

void
gtk_ipcam_player_load(GtkIpcamPlayer *self)
{
  gtk_ipcam_player_run_backgroup(self, gtk_ipcam_player_background_load);
}

static gboolean
gtk_ipcam_player_pointer_enter_cb(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
  GtkIpcamPlayer* self = GTK_IPCAM_PLAYER(widget);
  if(self->state == GTK_IPCAM_PLAYER_STATE_PLAYING)
  {
    gtk_widget_show(self->btn_up);
    gtk_widget_show(self->btn_down);
    gtk_widget_show(self->btn_left);
    gtk_widget_show(self->btn_right);
  }
  return FALSE;
}

static gboolean
gtk_ipcam_player_pointer_leave_cb(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
  GtkIpcamPlayer* self = GTK_IPCAM_PLAYER(widget);
  gtk_widget_hide(self->btn_up);
  gtk_widget_hide(self->btn_down);
  gtk_widget_hide(self->btn_left);
  gtk_widget_hide(self->btn_right);
  return FALSE;
}

static void
gtk_ipcam_player_show_cb(GtkWidget* widget, gpointer user_data)
{
  printf("showing player\n");
  GtkIpcamPlayer* self = GTK_IPCAM_PLAYER(widget);
  gtk_widget_hide(self->btn_up);
  gtk_widget_hide(self->btn_down);
  gtk_widget_hide(self->btn_left);
  gtk_widget_hide(self->btn_right);
}

static void
gtk_ipcam_player_constructed(GObject* object)
{
  GtkIpcamPlayer *self = GTK_IPCAM_PLAYER(object);
  //GtkWidget* dialog;
  //self->renderer = gst_player_gtk_video_renderer_new();
  //if(self->renderer){
  //  self->video_area = gst_player_gtk_video_renderer_get_widget(GST_PLAYER_GTK_VIDEO_RENDERER(self->renderer));
  //} else {
  //  dialog = gtk_message_dialog_new(GTK_WINDOW(gtk_application_get_active_window(GTK_APPLICATION (g_application_get_default()))),
  //                                   GTK_DIALOG_DESTROY_WITH_PARENT,
  //                                   GTK_MESSAGE_ERROR,
  //                                   GTK_BUTTONS_CLOSE,
  //                                   "ERROR - %s",
  //                                   "gtksink/gtkglsink gstreamer plugin not found");
  //  gtk_dialog_run(GTK_DIALOG(dialog));
  //  gtk_widget_destroy(dialog);
  //  g_application_quit(G_APPLICATION (g_application_get_default()));
  //}

  printf("gtk_ipcam_player_constructed\n");

  //g_assert (self->renderer != NULL);
  self->video_area = gtk_vlc_player_new();

  g_assert (self->video_area != NULL);

  GtkStyleContext *context;
	context = gtk_widget_get_style_context(GTK_WIDGET(self->video_area));
	gtk_style_context_add_class(context,"vlc-player");

  printf("gtk_ipcam_player for %s camera\n", gtk_ipcam_camera_obj_get_name(self->camera));
  GtkWidget* frame = gtk_frame_new(gtk_ipcam_camera_obj_get_name(self->camera));
  GtkWidget* overlay = gtk_overlay_new();
  context = gtk_widget_get_style_context(GTK_WIDGET(frame));
  gtk_style_context_add_class(context,"foscam-player");

  gtk_container_add(GTK_CONTAINER(frame),overlay);

  gtk_widget_set_halign(GTK_WIDGET(self->video_area), GTK_ALIGN_FILL);
  gtk_widget_set_valign(GTK_WIDGET(self->video_area), GTK_ALIGN_FILL);
  gtk_overlay_add_overlay(GTK_OVERLAY(overlay),self->video_area);

  self->btn_up = gtk_button_new_with_label("up");
  gtk_widget_set_halign(GTK_WIDGET(self->btn_up), GTK_ALIGN_CENTER);
  gtk_widget_set_valign(GTK_WIDGET(self->btn_up), GTK_ALIGN_START);
  gtk_overlay_add_overlay(GTK_OVERLAY(overlay),self->btn_up);

  self->btn_down = gtk_button_new_with_label("down");
  gtk_widget_set_halign(GTK_WIDGET(self->btn_down), GTK_ALIGN_CENTER);
  gtk_widget_set_valign(GTK_WIDGET(self->btn_down), GTK_ALIGN_END);
  gtk_overlay_add_overlay(GTK_OVERLAY(overlay),self->btn_down);

  self->btn_left = gtk_button_new_with_label("left");
  gtk_widget_set_halign(GTK_WIDGET(self->btn_left), GTK_ALIGN_START);
  gtk_widget_set_valign(GTK_WIDGET(self->btn_left), GTK_ALIGN_CENTER);
  gtk_overlay_add_overlay(GTK_OVERLAY(overlay),self->btn_left);

  self->btn_right = gtk_button_new_with_label("right");
  gtk_widget_set_halign(GTK_WIDGET(self->btn_right), GTK_ALIGN_END);
  gtk_widget_set_valign(GTK_WIDGET(self->btn_right), GTK_ALIGN_CENTER);
  gtk_overlay_add_overlay(GTK_OVERLAY(overlay),self->btn_right);

  gtk_container_add(GTK_CONTAINER(self),frame);

  gtk_widget_set_events(GTK_WIDGET(self), GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK);

  g_signal_connect(self, "enter-notify-event", G_CALLBACK(gtk_ipcam_player_pointer_enter_cb), NULL);
  g_signal_connect(self, "leave-notify-event", G_CALLBACK(gtk_ipcam_player_pointer_leave_cb), NULL);

  g_signal_connect (G_OBJECT(self), "realize", G_CALLBACK (gtk_ipcam_player_show_cb), NULL);
  g_signal_connect (G_OBJECT(self), "show", G_CALLBACK (gtk_ipcam_player_show_cb), NULL);

  //gtk_widget_set_events (self->video_area, GDK_EXPOSURE_MASK
  //    | GDK_LEAVE_NOTIFY_MASK
  //    | GDK_BUTTON_PRESS_MASK
  //    | GDK_POINTER_MOTION_MASK
  //    | GDK_POINTER_MOTION_HINT_MASK | GDK_ENTER_NOTIFY_MASK);

  //g_signal_connect (play->video_area, "motion-notify-event",
  //    G_CALLBACK (video_area_toolbar_show_cb), play);
  //g_signal_connect (play->video_area, "scroll-event",
  //    G_CALLBACK (video_area_toolbar_show_cb), play);
  //g_signal_connect (play->video_area, "button-press-event",
  //    G_CALLBACK (mouse_button_pressed_cb), play);
  //g_signal_connect (play->video_area, "leave-notify-event",
  //    G_CALLBACK (video_area_leave_notify_cb), play);

  //self->player =
  //    gst_player_new (self->renderer,
  //    gst_player_g_main_context_signal_dispatcher_new (NULL));

  //g_assert (self->player != NULL);

  //g_signal_connect (self->player, "position-updated",
  //    G_CALLBACK (position_updated_cb), self);
  //g_signal_connect (self->player, "duration-changed",
  //    G_CALLBACK (duration_changed_cb), self);
  //g_signal_connect (self->player, "end-of-stream", G_CALLBACK (eos_cb), self);
  //g_signal_connect (self->player, "media-info-updated",
  //    G_CALLBACK (media_info_updated_cb), self);
  //g_signal_connect (self->player, "volume-changed",
  //    G_CALLBACK (player_volume_changed_cb), self);

  /* enable visualization (by default playbin uses goom) */
  /* if visualization is enabled then use the first element */
  //gst_player_set_visualization_enabled (self->player, TRUE);
}

static void
gtk_ipcam_player_class_init(GtkIpcamPlayerClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->set_property = gtk_ipcam_player_set_property;
  gobject_class->get_property = gtk_ipcam_player_get_property;
  gobject_class->finalize = gtk_ipcam_player_finalize;
  gobject_class->constructed = gtk_ipcam_player_constructed;

  gtk_ipcam_player_param_specs
      [GTK_IPCAM_PLAYER_PROP_WIDGET] =
      g_param_spec_object ("widget", "Widget",
      "Widget to render the video into", GTK_TYPE_WIDGET,
      G_PARAM_READABLE | G_PARAM_STATIC_STRINGS);

    gtk_ipcam_player_param_specs
        [GTK_IPCAM_PLAYER_PROP_CAMERA] =
        g_param_spec_object ("camera", "Camera",
        "Camera Object related to this player", GTK_TYPE_IPCAM_CAMERA_OBJ,
        G_PARAM_READWRITE | G_PARAM_CONSTRUCT | G_PARAM_STATIC_STRINGS);

  g_object_class_install_properties (gobject_class,
      GTK_IPCAM_PLAYER_PROP_LAST, gtk_ipcam_player_param_specs);
}

static void
gtk_ipcam_player_init(GtkIpcamPlayer * self)
{
  printf("Initializing player\n");
  self->load_thread = NULL;
  self->state = GTK_IPCAM_PLAYER_STATE_IDLE;
  self->is_flipped = FALSE;
  self->is_mirrored = FALSE;
}

GtkIpcamPlayer *
gtk_ipcam_player_new(GtkIpcamCameraObj* camera)
{
  return g_object_new(GTK_TYPE_IPCAM_PLAYER, "camera", camera, NULL);
}

/*GtkWidget *
gtk_ipcam_player_get_widget(GtkIpcamPlayer * self)
{
  GtkWidget *widget;

  g_return_val_if_fail (GTK_IS_IPCAM_PLAYER(self), NULL);

  g_object_get(self, "widget", &widget, NULL);

  return widget;
}*/

GtkIpcamCameraObj *
gtk_ipcam_player_get_camera(GtkIpcamPlayer * self)
{
  GtkIpcamCameraObj *ret;

  g_return_val_if_fail (GTK_IS_IPCAM_PLAYER(self), NULL);

  g_object_get(self, "camera", &ret, NULL);

  return ret;
}

gboolean
gtk_ipcam_player_start_video(GtkIpcamPlayer* self)
{
  gtk_vlc_player_play(GTK_VLC_PLAYER(self->video_area));
  self->state = GTK_IPCAM_PLAYER_STATE_PLAYING;
  return TRUE;
}

gboolean
gtk_ipcam_player_stop_video(GtkIpcamPlayer* self)
{
  gtk_vlc_player_stop(GTK_VLC_PLAYER(self->video_area));
  self->state = GTK_IPCAM_PLAYER_STATE_STOPPED;
  return TRUE;
}

gboolean
gtk_ipcam_player_start_audio(GtkIpcamPlayer* self)
{
  return FALSE;
}

gboolean
gtk_ipcam_player_stop_audio(GtkIpcamPlayer* self)
{
  return FALSE;
}
