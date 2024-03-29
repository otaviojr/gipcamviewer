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

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>


#include "gtk_ipcam_player.h"
#include "gtk_ipcam_ffmpeg_renderer.h"
#include "gtk_ipcam_util.h"

struct _GtkIpcamPlayer
{
  GtkEventBox parent;

  GtkWidget* btn_down;
  GtkWidget* btn_up;
  GtkWidget* btn_left;
  GtkWidget* btn_right;
  GtkWidget* btn_mute;

  GtkWidget* video_area;
  GtkIpcamCameraObj* camera;

  gint state;

  GThread* background_thread;

  gboolean is_flipped;
  gboolean is_mirrored;
  gboolean can_pan;
  gboolean can_tilt;
  gboolean is_unmapped;
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

  self->state = GTK_IPCAM_PLAYER_STATE_STOPPED;

  if(self->background_thread)
  {
    g_thread_join(self->background_thread);
    g_thread_unref(self->background_thread);
    self->background_thread = NULL;
  }

  if(self->camera != NULL)
  {
    g_object_unref(self->camera);
    self->camera = NULL;
  }

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
  if(self->background_thread)
  {
    g_thread_join(self->background_thread);
    g_thread_unref(self->background_thread);
  }
  self->background_thread = g_thread_new("player_run_background", func, self);
}

static void
gtk_ipcam_player_video_ended_cb(GtkWidget* widget, gpointer user_data)
{
  GtkIpcamPlayer* self;
  printf("Video ended\n");
  if(!GTK_IS_IPCAM_PLAYER(user_data)){
    return;
  }
  self = GTK_IPCAM_PLAYER(user_data);
  if(self->state == GTK_IPCAM_PLAYER_STATE_PLAYING){
    printf("Trying to reload...\n");
    gtk_ipcam_player_load(self);
  }
}

static void
gtk_ipcam_player_up_cb(GtkWidget* widget, gpointer user_data)
{
  printf("going up\n");
  GtkIpcamPlayer* self = GTK_IPCAM_PLAYER(user_data);
  gtk_ipcam_player_run_backgroup(self,gtk_ipcam_player_move_up_background);
}

static void
gtk_ipcam_player_down_cb(GtkWidget* widget, gpointer user_data)
{
  printf("going down\n");
  GtkIpcamPlayer* self = GTK_IPCAM_PLAYER(user_data);
  gtk_ipcam_player_run_backgroup(self,gtk_ipcam_player_move_down_background);
}

static void
gtk_ipcam_player_left_cb(GtkWidget* widget, gpointer user_data)
{
  printf("going left\n");
  GtkIpcamPlayer* self = GTK_IPCAM_PLAYER(user_data);
  gtk_ipcam_player_run_backgroup(self,gtk_ipcam_player_move_left_background);
}

static void
gtk_ipcam_player_right_cb(GtkWidget* widget, gpointer user_data)
{
  printf("going right\n");
  GtkIpcamPlayer* self = GTK_IPCAM_PLAYER(user_data);
  gtk_ipcam_player_run_backgroup(self,gtk_ipcam_player_move_right_background);
}

static void
gtk_ipcam_player_mute_cb(GtkWidget* widget, gpointer user_data)
{
  GtkIpcamPlayer* self = GTK_IPCAM_PLAYER(user_data);
  if(gtk_ipcam_player_is_audio_playing(self))
  {
    printf("mute\n");
    gtk_ipcam_player_stop_audio(self);
    gtk_button_set_label(GTK_BUTTON(self->btn_mute),"\ue04f");
  }
  else
  {
    printf("unmute\n");
    gtk_ipcam_player_start_audio(self);
    gtk_button_set_label(GTK_BUTTON(self->btn_mute),"\ue04e");
  }
}

static void
gtk_ipcam_enable_controls(GtkIpcamPlayer* self)
{
  if(gtk_ipcam_camera_obj_get_flip_controls(self->camera))
  {
    g_signal_connect(G_OBJECT(self->btn_up), "clicked", G_CALLBACK (gtk_ipcam_player_down_cb), self);
    g_signal_connect(G_OBJECT(self->btn_down), "clicked", G_CALLBACK (gtk_ipcam_player_up_cb), self);
  }
  else
  {
    g_signal_connect(G_OBJECT(self->btn_up), "clicked", G_CALLBACK (gtk_ipcam_player_up_cb), self);
    g_signal_connect(G_OBJECT(self->btn_down), "clicked", G_CALLBACK (gtk_ipcam_player_down_cb), self);
  }

  if(gtk_ipcam_camera_obj_get_mirror_controls(self->camera))
  {
    g_signal_connect(G_OBJECT(self->btn_left), "clicked", G_CALLBACK (gtk_ipcam_player_right_cb), self);
    g_signal_connect(G_OBJECT(self->btn_right), "clicked", G_CALLBACK (gtk_ipcam_player_left_cb), self);
  }
  else
  {
    g_signal_connect(G_OBJECT(self->btn_left), "clicked", G_CALLBACK (gtk_ipcam_player_left_cb), self);
    g_signal_connect(G_OBJECT(self->btn_right), "clicked", G_CALLBACK (gtk_ipcam_player_right_cb), self);
  }
  g_signal_connect(G_OBJECT(self->btn_mute), "clicked", G_CALLBACK (gtk_ipcam_player_mute_cb), self);
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
    self->can_pan = gtk_ipcam_camera_obj_can_pan(self->camera);
    self->can_tilt = gtk_ipcam_camera_obj_can_tilt(self->camera);

    gchar* camera_url = gtk_ipcam_camera_obj_get_stream_url(self->camera);

    if(camera_url != NULL && !self->is_unmapped)
    {
      printf("Playing: %s\n",camera_url);
      gtk_ipcam_ffmpeg_renderer_load_uri(GTK_IPCAM_FFMPEG_RENDERER(self->video_area), camera_url);

      g_free(camera_url);

      gtk_ipcam_player_stop_audio(self);
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
  GtkIpcamPlayer* self = GTK_IPCAM_PLAYER(user_data);
  if(self->state == GTK_IPCAM_PLAYER_STATE_PLAYING)
  {
    if(self->can_tilt)
    {
      gtk_widget_show(self->btn_up);
      gtk_widget_show(self->btn_down);
    }

    if(self->can_pan)
    {
      gtk_widget_show(self->btn_left);
      gtk_widget_show(self->btn_right);
    }
    gtk_widget_show(self->btn_mute);
    if(gtk_ipcam_player_is_audio_playing(self))
    {
      gtk_button_set_label(GTK_BUTTON(self->btn_mute),"\ue04e");
    }
    else
    {
      gtk_button_set_label(GTK_BUTTON(self->btn_mute),"\ue04f");
    }
  }
  return FALSE;
}

static gboolean
gtk_ipcam_player_pointer_leave_cb(GtkWidget* widget, GdkEvent* event, gpointer user_data)
{
  GtkIpcamPlayer* self = GTK_IPCAM_PLAYER(user_data);
  gtk_widget_hide(self->btn_up);
  gtk_widget_hide(self->btn_down);
  gtk_widget_hide(self->btn_left);
  gtk_widget_hide(self->btn_right);
  gtk_widget_hide(self->btn_mute);
  return FALSE;
}

static void
gtk_ipcam_player_unmap_cb(GtkWidget* widget, gpointer user_data)
{
  printf("unmap player\n");
  GtkIpcamPlayer* self = GTK_IPCAM_PLAYER(widget);
  self->is_unmapped = TRUE;
}

static void
gtk_ipcam_player_show_cb(GtkWidget* widget, gpointer user_data)
{
  printf("showing player\n");
  GtkIpcamPlayer* self = GTK_IPCAM_PLAYER(widget);
  self->is_unmapped = FALSE;
  gtk_widget_hide(self->btn_up);
  gtk_widget_hide(self->btn_down);
  gtk_widget_hide(self->btn_left);
  gtk_widget_hide(self->btn_right);
  gtk_widget_hide(self->btn_mute);
}

static void
gtk_ipcam_player_constructed(GObject* object)
{
  GtkIpcamPlayer *self = GTK_IPCAM_PLAYER(object);

  printf("gtk_ipcam_player_constructed\n");

  self->video_area = gtk_ipcam_ffmpeg_renderer_new();
  g_assert (self->video_area != NULL);

  GtkStyleContext *context;
	context = gtk_widget_get_style_context(GTK_WIDGET(self->video_area));
	gtk_style_context_add_class(context,"ipcam-video-player");

  printf("gtk_ipcam_player for %s camera\n", gtk_ipcam_camera_obj_get_name(self->camera));
  GtkWidget* frame = gtk_frame_new(gtk_ipcam_camera_obj_get_name(self->camera));
  GtkWidget* overlay = gtk_overlay_new();
  context = gtk_widget_get_style_context(GTK_WIDGET(frame));
  gtk_style_context_add_class(context,"ipcam-player");
  context = gtk_widget_get_style_context(GTK_WIDGET(overlay));
  gtk_style_context_add_class(context,"ipcam-player-overlay");

  gtk_widget_set_halign(GTK_WIDGET(self->video_area), GTK_ALIGN_FILL);
  gtk_widget_set_valign(GTK_WIDGET(self->video_area), GTK_ALIGN_FILL);
  gtk_container_add(GTK_CONTAINER(overlay),GTK_WIDGET(self->video_area));
  gtk_overlay_set_overlay_pass_through(GTK_OVERLAY(overlay),GTK_WIDGET(self->video_area),TRUE);
  gtk_overlay_reorder_overlay(GTK_OVERLAY(overlay), GTK_WIDGET(self->video_area),1);

  self->btn_mute = gtk_button_new_with_label("\uE04F");
  context = gtk_widget_get_style_context(GTK_WIDGET(self->btn_mute));
  gtk_widget_set_size_request(GTK_WIDGET(self->btn_mute), 30, 50);
  gtk_style_context_add_class(context,"ipcam-player-control-btn");
  gtk_style_context_add_class(context,"ipcam-player-control-mute");
  gtk_widget_set_halign(GTK_WIDGET(self->btn_mute), GTK_ALIGN_END);
  gtk_widget_set_valign(GTK_WIDGET(self->btn_mute), GTK_ALIGN_END);
  gtk_overlay_add_overlay(GTK_OVERLAY(overlay),self->btn_mute);
  gtk_overlay_reorder_overlay(GTK_OVERLAY(overlay), GTK_WIDGET(self->btn_mute),98);

  self->btn_up = gtk_button_new_with_label("\uE316");
  context = gtk_widget_get_style_context(GTK_WIDGET(self->btn_up));
  gtk_widget_set_size_request(GTK_WIDGET(self->btn_up), 50, 30);
  gtk_style_context_add_class(context,"ipcam-player-control-btn");
	gtk_style_context_add_class(context,"ipcam-player-control-up");
  gtk_widget_set_halign(GTK_WIDGET(self->btn_up), GTK_ALIGN_CENTER);
  gtk_widget_set_valign(GTK_WIDGET(self->btn_up), GTK_ALIGN_START);
  gtk_overlay_add_overlay(GTK_OVERLAY(overlay),self->btn_up);
  gtk_overlay_reorder_overlay(GTK_OVERLAY(overlay), GTK_WIDGET(self->btn_up),94);

  self->btn_down = gtk_button_new_with_label("\uE313");
  context = gtk_widget_get_style_context(GTK_WIDGET(self->btn_down));
  gtk_widget_set_size_request(GTK_WIDGET(self->btn_down), 50, 30);
  gtk_style_context_add_class(context,"ipcam-player-control-btn");
	gtk_style_context_add_class(context,"ipcam-player-control-down");
  gtk_widget_set_halign(GTK_WIDGET(self->btn_down), GTK_ALIGN_CENTER);
  gtk_widget_set_valign(GTK_WIDGET(self->btn_down), GTK_ALIGN_END);
  gtk_overlay_add_overlay(GTK_OVERLAY(overlay),self->btn_down);
  gtk_overlay_reorder_overlay(GTK_OVERLAY(overlay), GTK_WIDGET(self->btn_down),95);

  self->btn_left = gtk_button_new_with_label("\uE314");
  context = gtk_widget_get_style_context(GTK_WIDGET(self->btn_left));
  gtk_widget_set_size_request(GTK_WIDGET(self->btn_left), 30, 50);
  gtk_style_context_add_class(context,"ipcam-player-control-btn");
	gtk_style_context_add_class(context,"ipcam-player-control-left");
  gtk_widget_set_halign(GTK_WIDGET(self->btn_left), GTK_ALIGN_START);
  gtk_widget_set_valign(GTK_WIDGET(self->btn_left), GTK_ALIGN_CENTER);
  gtk_overlay_add_overlay(GTK_OVERLAY(overlay),self->btn_left);
  gtk_overlay_reorder_overlay(GTK_OVERLAY(overlay), GTK_WIDGET(self->btn_left),96);

  self->btn_right = gtk_button_new_with_label("\uE315");
  context = gtk_widget_get_style_context(GTK_WIDGET(self->btn_right));
  gtk_widget_set_size_request(GTK_WIDGET(self->btn_right), 30, 50);
  gtk_style_context_add_class(context,"ipcam-player-control-btn");
	gtk_style_context_add_class(context,"ipcam-player-control-right");
  gtk_widget_set_halign(GTK_WIDGET(self->btn_right), GTK_ALIGN_END);
  gtk_widget_set_valign(GTK_WIDGET(self->btn_right), GTK_ALIGN_CENTER);
  gtk_overlay_add_overlay(GTK_OVERLAY(overlay),self->btn_right);
  gtk_overlay_reorder_overlay(GTK_OVERLAY(overlay), GTK_WIDGET(self->btn_right),97);


  gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(overlay));
  gtk_container_add(GTK_CONTAINER(self), GTK_WIDGET(frame));

  gtk_widget_set_events(GTK_WIDGET(self), GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_POINTER_MOTION_MASK);

  g_signal_connect(self, "enter-notify-event", G_CALLBACK(gtk_ipcam_player_pointer_enter_cb), self);
  g_signal_connect(self, "motion-notify-event", G_CALLBACK(gtk_ipcam_player_pointer_enter_cb), self);
  g_signal_connect(self, "leave-notify-event", G_CALLBACK(gtk_ipcam_player_pointer_leave_cb), self);

  g_signal_connect (G_OBJECT(self), "unmap", G_CALLBACK (gtk_ipcam_player_unmap_cb), NULL);
  g_signal_connect (G_OBJECT(self), "realize", G_CALLBACK (gtk_ipcam_player_show_cb), NULL);
  g_signal_connect (G_OBJECT(self), "show", G_CALLBACK (gtk_ipcam_player_show_cb), NULL);

  g_signal_connect (G_OBJECT(self->video_area), "ended", G_CALLBACK (gtk_ipcam_player_video_ended_cb), self);

  gtk_ipcam_enable_controls(self);

  printf("gtk_ipcam_player_constructed finished\n");
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
  self->background_thread = NULL;
  self->state = GTK_IPCAM_PLAYER_STATE_IDLE;
  self->is_flipped = FALSE;
  self->is_mirrored = FALSE;
  self->can_pan = FALSE;
  self->can_tilt = FALSE;
  self->is_unmapped = TRUE;
}

GtkIpcamPlayer *
gtk_ipcam_player_new(GtkIpcamCameraObj* camera)
{
  return g_object_new(GTK_TYPE_IPCAM_PLAYER, "camera", camera, NULL);
}

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
  printf("Playing video\n");
  gtk_ipcam_ffmpeg_renderer_play(GTK_IPCAM_FFMPEG_RENDERER(self->video_area));
  self->state = GTK_IPCAM_PLAYER_STATE_PLAYING;
  return TRUE;
}

gboolean
gtk_ipcam_player_stop_video(GtkIpcamPlayer* self)
{
  gtk_ipcam_ffmpeg_renderer_stop(GTK_IPCAM_FFMPEG_RENDERER(self->video_area));
  self->state = GTK_IPCAM_PLAYER_STATE_STOPPED;
  return TRUE;
}

gboolean
gtk_ipcam_player_start_audio(GtkIpcamPlayer* self)
{
  gtk_ipcam_ffmpeg_renderer_set_mute(GTK_IPCAM_FFMPEG_RENDERER(self->video_area), 0);
  return TRUE;
}

gboolean
gtk_ipcam_player_stop_audio(GtkIpcamPlayer* self)
{
  gtk_ipcam_ffmpeg_renderer_set_mute(GTK_IPCAM_FFMPEG_RENDERER(self->video_area), 1);
  return TRUE;
}

gboolean
gtk_ipcam_player_is_audio_playing(GtkIpcamPlayer* self)
{
  return gtk_ipcam_ffmpeg_renderer_get_mute(GTK_IPCAM_FFMPEG_RENDERER(self->video_area)) == 0;
}
