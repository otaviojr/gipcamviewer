#include <glib.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <SDL2/SDL.h>

#include "gtk_ipcam_ffmpeg_renderer.h"

static gboolean
gtk_ipcam_ffmpeg_renderer_on_window_draw(GtkIpcamFFMpegRenderer *self, cairo_t *cr, gpointer data);
static gboolean
gtk_ipcam_ffmpeg_renderer_unload(GtkIpcamFFMpegRenderer* self);

typedef enum _GtkIpcamFFMpegRendererState
{
  GTK_IPCAM_FFMPEG_RENDERER_STATE_IDLE,
  GTK_IPCAM_FFMPEG_RENDERER_STATE_LOADED,
  GTK_IPCAM_FFMPEG_RENDERER_STATE_PLAYING
} GtkIpcamFFMpegRendererState;

struct _GtkIpcamFFMpegRenderer
{
  GtkDrawingArea parent;

  GtkIpcamFFMpegRendererState state;

  pthread_t background_thread;
  pthread_mutex_t lock;

  gint refresh_timeout;

  GdkPixbuf *pixbuf;

  AVFormatContext *pFormatCtx;
  AVCodecContext *pCodecCtx;
  AVCodecContext *pAudioCodecCtx;
  AVCodec *pCodec;
  AVCodec *pAudioCodec;

  int videoStream;
  int audioStream;

  struct SwsContext *sws_ctx;

  SDL_AudioDeviceID audioDevice;
  SwrContext * resampler;
  uint8_t *s_resample_buf;
  unsigned int s_resample_buf_len;

  GValue uri;
  GValue mute;
};

struct _GtkIpcamFFMpegRendererClass
{
  GtkDrawingAreaClass parent_class;

  void (*play)	(GtkIpcamFFMpegRenderer *self);
  void (*stop)	(GtkIpcamFFMpegRenderer *self);
  void (*ended)	(GtkIpcamFFMpegRenderer *self);
};

enum
{
  GTK_IPCAM_FFMPEG_RENDERER_PROP_0,
  GTK_IPCAM_FFMPEG_RENDERER_PROP_URI,
  GTK_IPCAM_FFMPEG_RENDERER_PROP_MUTE,
  GTK_IPCAM_FFMPEG_RENDERER_PROP_LAST
};

enum
{
  GTK_IPCAM_FFMPEG_RENDERER_SIGNAL_0,
  GTK_IPCAM_FFMPEG_RENDERER_PLAY,
  GTK_IPCAM_FFMPEG_RENDERER_STOP,
  GTK_IPCAM_FFMPEG_RENDERER_ENDED,
  GTK_IPCAM_FFMPEG_RENDERER_SIGNAL_LAST
};

G_DEFINE_TYPE (GtkIpcamFFMpegRenderer, gtk_ipcam_ffmpeg_renderer, GTK_TYPE_DRAWING_AREA);

static GParamSpec
* gtk_ipcam_ffmpeg_renderer_param_specs[GTK_IPCAM_FFMPEG_RENDERER_PROP_LAST] = { NULL, };

static guint
gtk_ipcam_ffmpeg_renderer_signals[GTK_IPCAM_FFMPEG_RENDERER_SIGNAL_LAST] = {0, };

static void
gtk_ipcam_ffmpeg_renderer_get_property(GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec)
{
  GtkIpcamFFMpegRenderer *self = GTK_IPCAM_FFMPEG_RENDERER(object);

  switch (prop_id) {
    case GTK_IPCAM_FFMPEG_RENDERER_PROP_URI:
      g_value_set_string(value, g_value_get_string(&self->uri));
      break;
    case GTK_IPCAM_FFMPEG_RENDERER_PROP_MUTE:
      g_value_set_uint(value, g_value_get_uint(&self->mute));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gtk_ipcam_ffmpeg_renderer_set_property(GObject * object, guint prop_id, const GValue * value,
    GParamSpec * pspec)
{
  GtkIpcamFFMpegRenderer *self = GTK_IPCAM_FFMPEG_RENDERER(object);
  printf("gtk_ipcam_ffmpeg_renderer_set_property\n");
  switch (prop_id) {
    case GTK_IPCAM_FFMPEG_RENDERER_PROP_URI:
      g_value_set_string(&self->uri, g_value_get_string(value));
      break;
    case GTK_IPCAM_FFMPEG_RENDERER_PROP_MUTE:
      g_value_set_uint(&self->mute, g_value_get_uint(value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gtk_ipcam_ffmpeg_renderer_finalize(GObject * object)
{
  GtkIpcamFFMpegRenderer *self = GTK_IPCAM_FFMPEG_RENDERER(object);
  gboolean unload = FALSE;

  printf("gtk_ipcam_ffmpeg_renderer_finalize\n");

  g_source_remove(self->refresh_timeout);

  if(self->state != GTK_IPCAM_FFMPEG_RENDERER_STATE_IDLE){
    self->state = GTK_IPCAM_FFMPEG_RENDERER_STATE_IDLE;
    unload = TRUE;
  }

  if(self->background_thread > 0){
    pthread_join(self->background_thread, NULL);
    self->background_thread = 0;
  }

  if(self->pixbuf){
    g_object_unref(self->pixbuf);
    self->pixbuf = NULL;
  }

  if (self->resampler) {
    swr_free(&self->resampler);
    self->resampler = NULL;
  }

  if(self->s_resample_buf){
    av_freep(&self->s_resample_buf);
  }

  if(self->audioDevice >= 0){
    SDL_CloseAudioDevice(self->audioDevice);
  }

  if(unload == TRUE){
    gtk_ipcam_ffmpeg_renderer_unload(self);
  }

  g_value_unset(&self->uri);
  g_value_unset(&self->mute);

  G_OBJECT_CLASS
      (gtk_ipcam_ffmpeg_renderer_parent_class)->finalize(object);

  printf("gtk_ipcam_ffmpeg_renderer_finalized\n");
}

static void
gtk_ipcam_ffmpeg_renderer_constructed(GObject* object)
{
  GtkIpcamFFMpegRenderer *self = GTK_IPCAM_FFMPEG_RENDERER(object);

  printf("gtk_ipcam_ffmpeg_renderer_constructed\n");
  g_signal_connect (self, "draw", G_CALLBACK(gtk_ipcam_ffmpeg_renderer_on_window_draw), NULL);
  printf("gtk_ipcam_ffmpeg_renderer_constructed finished\n");
}

static void
gtk_ipcam_ffmpeg_renderer_class_init(GtkIpcamFFMpegRendererClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  printf("gtk_ipcam_ffmpeg_renderer_class_init\n");

  gobject_class->set_property = gtk_ipcam_ffmpeg_renderer_set_property;
  gobject_class->get_property = gtk_ipcam_ffmpeg_renderer_get_property;
  gobject_class->finalize = gtk_ipcam_ffmpeg_renderer_finalize;
  gobject_class->constructed = gtk_ipcam_ffmpeg_renderer_constructed;

  gtk_ipcam_ffmpeg_renderer_param_specs
      [GTK_IPCAM_FFMPEG_RENDERER_PROP_URI] =
      g_param_spec_string ("uri", "URI",
      "URI of the video to be rendered", NULL,
      G_PARAM_READWRITE);

  gtk_ipcam_ffmpeg_renderer_param_specs
      [GTK_IPCAM_FFMPEG_RENDERER_PROP_MUTE] =
      g_param_spec_uint ("mute", "Mute",
      "If audio should be played", 0, 10000, 1,
      G_PARAM_READWRITE);

  gtk_ipcam_ffmpeg_renderer_signals[GTK_IPCAM_FFMPEG_RENDERER_PLAY] =
    g_signal_new("play",
                  G_TYPE_FROM_CLASS(gobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                  G_STRUCT_OFFSET(GtkIpcamFFMpegRendererClass, play),
                  NULL,
                  NULL,
                  NULL,
                  G_TYPE_NONE,
                  0,
                  NULL);

  gtk_ipcam_ffmpeg_renderer_signals[GTK_IPCAM_FFMPEG_RENDERER_STOP] =
    g_signal_new("stop",
                  G_TYPE_FROM_CLASS(gobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                  G_STRUCT_OFFSET(GtkIpcamFFMpegRendererClass, stop),
                  NULL,
                  NULL,
                  NULL,
                  G_TYPE_NONE,
                  0,
                  NULL);

  gtk_ipcam_ffmpeg_renderer_signals[GTK_IPCAM_FFMPEG_RENDERER_ENDED] =
    g_signal_new("ended",
                  G_TYPE_FROM_CLASS(gobject_class),
                  G_SIGNAL_RUN_LAST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
                  G_STRUCT_OFFSET(GtkIpcamFFMpegRendererClass, ended),
                  NULL,
                  NULL,
                  NULL,
                  G_TYPE_NONE,
                  0,
                  NULL);

  g_object_class_install_properties (gobject_class,
      GTK_IPCAM_FFMPEG_RENDERER_PROP_LAST, gtk_ipcam_ffmpeg_renderer_param_specs);
}

static gboolean
gtk_ipcam_ffmpeg_renderer_invalidate_cb (void *ptr)
{
  if (GTK_IS_WIDGET (ptr))
  {
    gtk_widget_queue_draw (GTK_WIDGET (ptr));
    return TRUE;
  }

  return FALSE;
}

static void
gtk_ipcam_ffmpeg_renderer_init(GtkIpcamFFMpegRenderer * self)
{
  printf("Initializing renderer\n");
  self->background_thread = 0;
  self->pixbuf = NULL;
  self->audioDevice = 0;
  self->resampler = NULL;
  self->s_resample_buf = NULL;
  self->s_resample_buf_len = 0;

  pthread_mutex_init(&self->lock, NULL);

  self->refresh_timeout = g_timeout_add (1000 / 60, gtk_ipcam_ffmpeg_renderer_invalidate_cb, self);

  g_value_init(&self->uri, G_TYPE_STRING);
  g_value_init(&self->mute, G_TYPE_UINT);
}

GtkWidget *
gtk_ipcam_ffmpeg_renderer_new()
{
  return GTK_WIDGET(g_object_new(GTK_TYPE_IPCAM_FFMPEG_RENDERER, NULL));
}

static void gtk_ipcam_ffmpeg_renderer_pixmap_destroy_notify(guchar *pixels,
				  gpointer data) {
    printf("Destroy pixmap\n");
}

static gboolean
gtk_ipcam_ffmpeg_renderer_on_window_draw(GtkIpcamFFMpegRenderer *self, cairo_t *cr, gpointer data)
{
  (void)data;
  gint width, height, video_width, video_height, top = 0, left = 0;

  if(!GTK_IS_IPCAM_FFMPEG_RENDERER(self) && GDK_IS_PIXBUF(self->pixbuf)){
    return FALSE;
  }

  pthread_mutex_lock(&self->lock);

  width = gtk_widget_get_allocated_width(GTK_WIDGET(self));
  height = gtk_widget_get_allocated_height(GTK_WIDGET(self));

  if(self->pixbuf){
    video_width = gdk_pixbuf_get_width (self->pixbuf);
    video_height = gdk_pixbuf_get_height (self->pixbuf);

    if(video_width <= 0 || video_height <= 0)
      return FALSE;

    printf("Drawing frame (%d->%d) (%d->%d)\n", width, video_width, height, video_height);

    if(video_width > video_height){
      video_height = (video_height*width)/video_width;
      video_width = width;
      top = (height/2)-(video_height/2);
    } else {
      video_width = (video_width*height)/video_height;
      video_height = height;
      left = (width/2)-(video_width/2);
    }

    printf("Drawing frame - Changed - (%d->%d) (%d->%d)\n", width, video_width, height, video_height);

    GdkPixbuf *temp = gdk_pixbuf_scale_simple(self->pixbuf, video_width, video_height, GDK_INTERP_BILINEAR);
    if(temp){
      gdk_cairo_set_source_pixbuf(cr, temp, left, top);
      cairo_paint(cr);
      g_object_unref(temp);
    }
  }

  pthread_mutex_unlock(&self->lock);

  return FALSE;
}

static gboolean
decode_audio(GtkIpcamFFMpegRenderer *self, AVPacket *p_packet, AVFrame* p_frame)
{
  uint8_t* p_cp_buf;
  int cp_len;

  if (self->resampler != NULL){
    // Resampling input parameter 1: The number of input audio samples is p_frame->nb_samples
    // Resample input parameter 2: input audio buffer
    const uint8_t **in = (const uint8_t **)p_frame->extended_data;

    // Resampling output parameters: the number of output audio samples (256 samples added)
    int64_t out_count = (int64_t)(p_frame->nb_samples * 44100 / p_frame->sample_rate) + 256;

    // Resample output parameters: output audio buffer size (in bytes)
    int buf_size  = av_samples_get_buffer_size(NULL, p_frame->channels, out_count, AV_SAMPLE_FMT_S16, 0);
    if (buf_size < 0){
      printf("av_samples_get_buffer_size() failed\n");
      return FALSE;
    }

    if (self->s_resample_buf == NULL){
      av_fast_malloc(&self->s_resample_buf, &self->s_resample_buf_len, buf_size);
    }
    if (self->s_resample_buf == NULL)
    {
      return FALSE;
    }

    // Resample output parameter 1: output audio buffer size
    // Resample output parameter 2: output audio buffer
    uint8_t **out = &self->s_resample_buf;

    // Audio resampling: The return value is the number of samples of a single channel in the audio data obtained after resampling
    int nb_samples = swr_convert(self->resampler, out, out_count, in, p_frame->nb_samples);
    if (nb_samples < 0) {
      printf("swr_convert() failed\n");
      return FALSE;
    }
    if (nb_samples == out_count)
    {
      printf("audio buffer is probably too small\n");
    }
    // The size of one frame of audio data returned by resampling (in bytes)
    p_cp_buf = self->s_resample_buf;
    cp_len = nb_samples * p_frame->channels * av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
  } else {
    return FALSE;
  }

  SDL_QueueAudio(self->audioDevice,
                   p_cp_buf,
                   cp_len);
  return TRUE;
}

static void*
gtk_ipcam_ffmpeg_renderer_play_background(void* user_data)
{
  GtkIpcamFFMpegRenderer *self = GTK_IPCAM_FFMPEG_RENDERER(user_data);

  /* based on code from
     http://www.cs.dartmouth.edu/~xy/cs23/gtk.html
     http://cdry.wordpress.com/2009/09/09/using-custom-io-callbacks-with-ffmpeg/
  */

  AVPacket packet;
  AVFrame *pFrame = NULL, *audioFrame = NULL;
  int ret;

  g_assert(self->pFormatCtx != NULL);
  g_assert(self->pCodecCtx != NULL);

  AVFrame *picture_RGB;
  uint8_t* buffer;

  int width = self->pCodecCtx->width;
  int height = self->pCodecCtx->height;

  picture_RGB = av_frame_alloc();
  buffer = av_malloc (av_image_get_buffer_size(AV_PIX_FMT_RGB24, width+1, height+1, 1) * sizeof(uint8_t));
  av_image_fill_arrays(picture_RGB->data, picture_RGB->linesize, buffer, AV_PIX_FMT_RGB24, width, height, 1);

  g_signal_emit(self, gtk_ipcam_ffmpeg_renderer_signals[GTK_IPCAM_FFMPEG_RENDERER_PLAY], 0, NULL);

  while(self->state == GTK_IPCAM_FFMPEG_RENDERER_STATE_PLAYING && av_read_frame(self->pFormatCtx, &packet) >=0) {
    printf("Parsing frame\n");
    if(packet.stream_index == self->videoStream) {

      pthread_mutex_lock(&self->lock);

      ret = avcodec_send_packet(self->pCodecCtx, &packet);
      // Decode video frame
      while(ret >= 0){
        pFrame=av_frame_alloc();
        if(pFrame == NULL) {
          printf("Fail allocating frame\r\n");
          break;
        }

        ret = avcodec_receive_frame(self->pCodecCtx, pFrame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            printf("Error while decoding 1\n");
            av_frame_free(&pFrame);
            break;
        } else if (ret < 0) {
            printf("Error while decoding 2\n");
            av_frame_free(&pFrame);
            break;
        }

        if(self->pixbuf){
          g_object_unref(self->pixbuf);
          self->pixbuf = NULL;
        }

        self->sws_ctx = sws_getContext(width, height, self->pCodecCtx->pix_fmt, width, height,
           AV_PIX_FMT_RGB24, SWS_BICUBIC, NULL, NULL, NULL);

        if(self->sws_ctx){
          if(sws_scale(self->sws_ctx,  (uint8_t const * const *) pFrame->data,
            pFrame->linesize, 0, height, picture_RGB->data, picture_RGB->linesize) > 0){
            self->pixbuf = gdk_pixbuf_new_from_data(picture_RGB->data[0], GDK_COLORSPACE_RGB,
              0, 8, width, height, picture_RGB->linesize[0], gtk_ipcam_ffmpeg_renderer_pixmap_destroy_notify, self);

            sws_freeContext(self->sws_ctx);
            self->sws_ctx = NULL;

            printf("Updating display\r\n");
            //gtk_widget_queue_draw(GTK_WIDGET(self));
          }
        }

        av_frame_free(&pFrame);
        pFrame = NULL;
      }

      pthread_mutex_unlock(&self->lock);
    } else if(packet.stream_index == self->audioStream) {
      printf("Audio frame\n");
      ret = avcodec_send_packet(self->pAudioCodecCtx, &packet);
      // Decode video frame
      while(ret >= 0){
        pFrame = av_frame_alloc();

        if(pFrame == NULL) {
          printf("Fail allocating audio frame\r\n");
          break;
        }

        ret = avcodec_receive_frame(self->pAudioCodecCtx, pFrame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            printf("Error while decoding audio 1\n");
            av_frame_free(&pFrame);
            av_frame_free(&audioFrame);
            break;
        } else if (ret < 0) {
            printf("Error while decoding audio 2\n");
            av_frame_free(&pFrame);
            av_frame_free(&audioFrame);
            break;
        }

        decode_audio(self, &packet, pFrame);

        av_frame_free(&pFrame);
        pFrame = NULL;
      }
    }

    av_packet_unref(&packet);

    g_thread_yield();
  }

  // Free the RGB image
  av_free(buffer);
  av_free(picture_RGB);

  g_signal_emit(self, gtk_ipcam_ffmpeg_renderer_signals[GTK_IPCAM_FFMPEG_RENDERER_ENDED], 0, NULL);

  printf("Video over!\n");

  return NULL;
}

void
gtk_ipcam_ffmpeg_renderer_play(GtkIpcamFFMpegRenderer* self)
{
  printf("Starting video with uri: %s\n",g_value_get_string(&self->uri));

  self->state = GTK_IPCAM_FFMPEG_RENDERER_STATE_PLAYING;

  if(self->background_thread > 0) {
    pthread_join(self->background_thread, NULL);
    self->background_thread = 0;
  }
  pthread_create(&self->background_thread, NULL, gtk_ipcam_ffmpeg_renderer_play_background, (void*)self);
}

void
gtk_ipcam_ffmpeg_renderer_stop(GtkIpcamFFMpegRenderer* self)
{
  printf("gtk_ipcam_ffmpeg_renderer_stop\n");

  if(self->state != GTK_IPCAM_FFMPEG_RENDERER_STATE_IDLE)
    self->state = GTK_IPCAM_FFMPEG_RENDERER_STATE_LOADED;

  g_signal_emit(self, gtk_ipcam_ffmpeg_renderer_signals[GTK_IPCAM_FFMPEG_RENDERER_STOP], 0, NULL);

  if(self->background_thread > 0){
    pthread_join(self->background_thread, NULL);
    self->background_thread = 0;
  }
}

static gboolean
gtk_ipcam_ffmpeg_renderer_unload(GtkIpcamFFMpegRenderer* self)
{
  printf("gtk_ipcam_ffmpeg_renderer_unload\r\n");

  if(self->pCodecCtx){
    avcodec_free_context(&self->pCodecCtx);
  }

  if(self->pFormatCtx){
    avformat_close_input(&self->pFormatCtx);
    avformat_free_context(self->pFormatCtx);
  }

  avformat_network_deinit();
  return TRUE;
}

gboolean
gtk_ipcam_ffmpeg_renderer_load_uri(GtkIpcamFFMpegRenderer* self, const gchar* uri)
{
  int ret;
  SDL_AudioSpec want, have;
  AVDictionary *optionsDict = NULL;
  AVCodec *decoder = NULL;
  AVCodec *audio_decoder = NULL;

  g_return_val_if_fail (GTK_IS_IPCAM_FFMPEG_RENDERER(self), FALSE);

  g_object_set(G_OBJECT(self), "uri", uri, NULL);

  printf("Loading uri: %s\r\n", uri);

  if(self->state != GTK_IPCAM_FFMPEG_RENDERER_STATE_IDLE){
    gtk_ipcam_ffmpeg_renderer_stop(self);
    gtk_ipcam_ffmpeg_renderer_unload(self);
  }

  /* FFMpeg stuff */
  printf("Loading media\r\n");
  self->pFormatCtx = avformat_alloc_context();
  if(avformat_open_input(&self->pFormatCtx, uri, NULL, NULL)!=0){
    printf("Error opening uri\r\n");
    return FALSE;
  }

  // Retrieve stream information
  printf("Reading media info\r\n");
  if(avformat_find_stream_info(self->pFormatCtx, NULL)<0){
    printf("Error reading media info\r\n");
    return FALSE;
  }

  // Dump information about file onto standard error
  printf("Media info:\r\n");
  av_dump_format(self->pFormatCtx, 0, uri, 0);

  // Find the first video stream
  /*self->videoStream=-1;
  for(i=0; i < self->pFormatCtx->nb_streams; i++){
    if(self->pFormatCtx->streams[i]->codecpar->codec_type==AVMEDIA_TYPE_VIDEO) {
      self->videoStream=i;
      break;
    }
  }*/

  ret = av_find_best_stream(self->pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &decoder, 0);
  if (ret < 0) {
    fprintf(stderr, "Cannot find a video stream in the input file\n");
    return -1;
  }
  self->videoStream = ret;

  ret = av_find_best_stream(self->pFormatCtx, AVMEDIA_TYPE_AUDIO, -1, -1, &audio_decoder, 0);
  if (ret < 0) {
    fprintf(stderr, "Cannot find a audio stream in the input file\n");
  }
  self->audioStream = ret;

  if (!(self->pCodecCtx = avcodec_alloc_context3(decoder))){
    printf("Error allocating codec\n");
    return FALSE;
  }

  avcodec_parameters_to_context(self->pCodecCtx, self->pFormatCtx->streams[self->videoStream]->codecpar);

  // Find the decoder for the video stream
  self->pCodec = avcodec_find_decoder(self->pCodecCtx->codec_id);
  if(self->pCodec==NULL) {
    printf("Unsupported codec!\n");
    return FALSE; // Codec not found
  }

  // Open codec
  if(avcodec_open2(self->pCodecCtx, self->pCodec, &optionsDict)<0){
    printf("Error opening codec!\n");
    return FALSE; // Codec not found
  }

  if(self->audioStream >= 0){
    if (!(self->pAudioCodecCtx = avcodec_alloc_context3(audio_decoder))){
      printf("Error allocating codec\n");
      return FALSE;
    }

    avcodec_parameters_to_context(self->pAudioCodecCtx, self->pFormatCtx->streams[self->audioStream]->codecpar);

    // Find the decoder for the audio stream
    self->pAudioCodec = avcodec_find_decoder(self->pAudioCodecCtx->codec_id);
    if(self->pCodec==NULL) {
      printf("Unsupported audio codec!\n");
      return FALSE;
    }

    // Open codec
    if(avcodec_open2(self->pAudioCodecCtx, self->pAudioCodec, &optionsDict)<0){
      printf("Error opening codec!\n");
      return FALSE;
    }

    self->resampler = swr_alloc_set_opts(NULL,
                                   self->pAudioCodecCtx->channel_layout,
                                   AV_SAMPLE_FMT_S16,
                                   44100,
                                   self->pAudioCodecCtx->channel_layout,
                                   self->pAudioCodecCtx->sample_fmt,
                                   self->pAudioCodecCtx->sample_rate,
                                   0,
                                   NULL);
    if (self->resampler == NULL || swr_init(self->resampler) < 0){
      printf("Cannot create sample rate converter for conversion of %d Hz %s %d channels to %d Hz %s %d channels!\n",
              self->pAudioCodecCtx->sample_rate, av_get_sample_fmt_name(self->pAudioCodecCtx->sample_fmt), self->pAudioCodecCtx->channels,
              44100, av_get_sample_fmt_name(AV_SAMPLE_FMT_S16), self->pAudioCodecCtx->channels);
              if(self->resampler)
                swr_free(&self->resampler);
              self->resampler = NULL;
    }

    SDL_Init(SDL_INIT_AUDIO);

    SDL_zero(want);
    SDL_zero(have);
    want.freq = 44100;
    want.channels = self->pAudioCodecCtx->channels;
    want.format = AUDIO_S16SYS;
    want.samples = self->pAudioCodecCtx->sample_rate;
    self->audioDevice = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (self->audioDevice == 0) {
      printf("Failed to open audio: %s\r\n", SDL_GetError());
    } else {
      if (have.format != want.format) { /* we let this one thing change. */
        printf("We didn't get Float32 audio format.\r\n");
      }
      printf("Audio device opened\r\n");
      SDL_PauseAudioDevice(self->audioDevice, 1);
    }
  }

  self->state = GTK_IPCAM_FFMPEG_RENDERER_STATE_LOADED;

  printf("URI loaded: %s\r\n", uri);
  return TRUE;
}

gboolean
gtk_ipcam_ffmpeg_renderer_set_mute(GtkIpcamFFMpegRenderer* self, gint mute)
{
  g_return_val_if_fail (GTK_IS_IPCAM_FFMPEG_RENDERER(self), FALSE);
  g_object_set(G_OBJECT(self), "mute", mute, NULL);
  SDL_PauseAudioDevice(self->audioDevice, mute);
  return TRUE;
}

gint
gtk_ipcam_ffmpeg_renderer_get_mute(GtkIpcamFFMpegRenderer* self)
{
  guint ret;

  g_return_val_if_fail (GTK_IS_IPCAM_FFMPEG_RENDERER(self), 0);

  g_object_get(self, "mute", &ret, NULL);

  return ret;
}
