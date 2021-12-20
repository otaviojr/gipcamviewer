#ifndef __GTK_IPCAM_FFMPEG_RENDERER_H__
#define __GTK_IPCAM_FFMPEG_RENDERER_H__

#include <gtk/gtk.h>

typedef struct _GtkIpcamFFMpegRenderer
    GtkIpcamFFMpegRenderer;
typedef struct _GtkIpcamFFMpegRendererClass
    GtkIpcamFFMpegRendererClass;

#define GTK_TYPE_IPCAM_FFMPEG_RENDERER                         (gtk_ipcam_ffmpeg_renderer_get_type ())
#define GTK_IS_IPCAM_FFMPEG_RENDERER(obj)                      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_IPCAM_FFMPEG_RENDERER))
#define GTK_IS_IPCAM_FFMPEG_RENDERER_CLASS(klass)              (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_IPCAM_FFMPEG_RENDERER))
#define GTK_IPCAM_FFMPEG_RENDERER_GET_CLASS(obj)               (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_IPCAM_FFMPEG_RENDERER, GtkIpcamFFMpegRendererClass))
#define GTK_IPCAM_FFMPEG_RENDERER(obj)                         (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_IPCAM_FFMPEG_RENDERER, GtkIpcamFFMpegRenderer))
#define GTK_IPCAM_FFMPEG_RENDERER_CLASS(klass)                 (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_IPCAM_PFFMPEG_RENDERER, GtkIpcamFFMpegRendererClass))
#define GTK_IPCAM_FFMPEG_RENDERER_CAST(obj)                    ((GtkIpcamFFMpegRenderer*)(obj))

GType gtk_ipcam_ffmpeg_renderer_get_type(void);

GtkWidget * gtk_ipcam_ffmpeg_renderer_new();

void gtk_ipcam_ffmpeg_renderer_play(GtkIpcamFFMpegRenderer* self);
void gtk_ipcam_ffmpeg_renderer_stop(GtkIpcamFFMpegRenderer* self);
gboolean gtk_ipcam_ffmpeg_renderer_load_uri(GtkIpcamFFMpegRenderer* self, const gchar* uri);

#endif //__GTK_IPCAM_FFMPEG_RENDERER_H__
