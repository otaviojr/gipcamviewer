project('gipcamviewer', ['c'],
  version: '0.1',
  license: [
        'GPL-2.0',
  ],
  default_options : [ 'warning_level=1',
                      'buildtype=debugoptimized' ])

cc = meson.get_compiler('c')

gnome = import('gnome')

#VLC marshallerss
marshallers = gnome.genmarshal('closure_marshaller',
sources : 'src/closure_marshallers.list',
install_header : false)

closure_marshaller_c = marshallers[0]
closure_marshaller_h = marshallers[1]

sources = ['src/main.c',
            'src/gtk_ipcam_util.h',
            'src/gtk_ipcam_util.c',
            'src/gtk_ipcam_player.h',
            'src/gtk_ipcam_player.c',
            'src/gtk_ipcam_camera_obj.h',
            'src/gtk_ipcam_camera_obj.c',
            'src/gtk_ipcam_camera_group_obj.h',
            'src/gtk_ipcam_camera_group_obj.c',
            'src/gtk_ipcam_camera_driver_obj.h',
            'src/gtk_ipcam_camera_driver_obj.c',
            'src/gtk_ipcam_preference_obj.h',
            'src/gtk_ipcam_preference_obj.c',
            'src/gtk_ipcam_dialog_cameras_edit.h',
            'src/gtk_ipcam_dialog_cameras_edit.c',
            'src/gtk_ipcam_dialog_groups_edit.h',
            'src/gtk_ipcam_dialog_groups_edit.c',
            'src/gtk_ipcam_dialog_camera_add.h',
            'src/gtk_ipcam_dialog_camera_add.c',
            'src/gtk_ipcam_dialog_groups_add.h',
            'src/gtk_ipcam_dialog_groups_add.c',
            'src/gtk_ipcam_ffmpeg_renderer.h',
            'src/gtk_ipcam_ffmpeg_renderer.c',
            closure_marshaller_c,
            closure_marshaller_h
          ]

#X11 Dependencies
x11_dep = dependency('x11')

#GTK Dependencies
gtk_dep = dependency('gtk+-3.0')

glib_dep = [
  dependency('glib-2.0', version: '>= 2.38')
]

gobject_dep = dependency('gobject-2.0', version: '>= 2.38')
gmodule_dep = dependency('gmodule-2.0')
glibjson_dep = dependency('json-glib-1.0')

#ffmpeg Dependencies
ffmpeg_dep = [
  dependency('libavformat'),
  dependency('libavcodec'),
  dependency('libavutil'),
  dependency('libswscale'),
  dependency('libswresample'),
]

#sdl2
sdl2_dep = [
  dependency('sdl2')
]

#Lua deps
lua_dep = dependency('lua-5.3')

deps = [
        #x11_dep,
        glib_dep,
        gobject_dep,
        gmodule_dep,
        glibjson_dep,
        gtk_dep,
        ffmpeg_dep,
        sdl2_dep,
        lua_dep]

cdata = configuration_data()
check_headers = [
  ['HAVE_INTTYPES_H', 'inttypes.h'],
]

foreach h : check_headers
  if cc.has_header(h.get(1))
    cdata.set(h.get(0), 1)
  endif
endforeach

check_functions = [
  # check token HAVE_GETTEXT
  ['HAVE_GETTEXT', 'gettext', true],
]

foreach f : check_functions
  if cc.has_function(f.get(1))
    cdata.set(f.get(0), 1)
  else
    if f.get(2) == true
      error('@0@: not found'.format(f.get(1)))
    endif
  endif
endforeach

cdata.set('PREFIX', '"@0@"'.format(get_option('prefix')))
cdata.set('LIBDIR', '"@0@"'.format(get_option('libdir')))
cdata.set('DATADIR', '"@0@"'.format(get_option('datadir')))

configure_file(output : 'config.h', configuration : cdata)

compiler = meson.get_compiler('c')

code_http = '''
  #include <stdio.h>
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>

  int main(int argc, char **argv)
  {
    lua_State* l;
    l = luaL_newstate();
    luaL_openlibs(l);
    if(luaL_dostring(l, "local http = require \"http.request\""))
    {
      printf("Couldn't load file: %s\n", lua_tostring(l, -1));
      lua_close(l);
      return -1;
    }
    lua_close(l);
    printf("SUCCESS");
    return 0;
  }
'''

code_socket = '''
  #include <stdio.h>
  #include <lua.h>
  #include <lualib.h>
  #include <lauxlib.h>

  int main(int argc, char **argv)
  {
    lua_State* l;
    l = luaL_newstate();
    luaL_openlibs(l);
    if(luaL_dostring(l, "local http = require \"socket\""))
    {
      printf("Couldn't load file: %s\n", lua_tostring(l, -1));
      lua_close(l);
      return -1;
    }
    lua_close(l);
    printf("SUCCESS");
    return 0;
  }
'''

flags = run_command('pkg-config','--cflags','--libs','lua5.3')
a_flags = flags.stdout().strip().split(' ')

result = compiler.run(code_http, args : a_flags, name : 'lua5.3 has http plugin')
if result.returncode() != 0 or result.compiled() == false
  error('Lua HTTP support not installed: ' + result.stdout())
endif

result = compiler.run(code_socket, args : a_flags, name : 'lua5.3 has socket plugin')
if result.returncode() != 0 or result.compiled() == false
  error('Lua Socket support not installed: ' + result.stdout())
endif

exe = executable('gipcamviewer', sources, dependencies : deps, c_args : ['-Wno-incompatible-pointer-types','-Wno-discarded-qualifiers'],install : true)

ui_data_files = [
               'resources/ui/main_area.ui',
               'resources/ui/main_header.ui',
               'resources/ui/main_popover_menu.ui',
               'resources/ui/cameras_edit.ui',
               'resources/ui/groups_edit.ui',
               'resources/ui/cameras_add.ui',
               'resources/ui/groups_add.ui',
               'resources/ui/null_layer.ui'
             ]

install_data(ui_data_files, install_dir : join_paths(get_option('datadir'),'gipcamviewer','resources', 'ui'))

css_data_files = [
               'resources/css/main.css'
             ]

install_data(css_data_files, install_dir : join_paths(get_option('datadir'),'gipcamviewer','resources', 'css'))

fonts_data_files = [
               'resources/fonts/material/MaterialIcons-Regular.ttf'
             ]

install_data(fonts_data_files, install_dir : join_paths('/usr','share','fonts', 'truetype', 'material'))

drivers_data_files = [
                'resources/drivers/common/http_digest.lua',
                'resources/drivers/common/md5.lua',
                'resources/drivers/AMCREST_HD_RTSP.json',
                'resources/drivers/AMCREST_HD_RTSP.lua',
                'resources/drivers/FOSCAM_SD_HTTP.json',
                'resources/drivers/FOSCAM_SD_HTTP.lua',
                'resources/drivers/FOSCAM_HD_RTSP.json',
                'resources/drivers/FOSCAM_HD_RTSP.lua',
                'resources/drivers/HIKVISION_HD_RTSP.json',
                'resources/drivers/HIKVISION_HD_RTSP.lua'
             ]

install_data(drivers_data_files, install_dir : join_paths(get_option('datadir'),'gipcamviewer','drivers'))

test('Check Generated Application', exe)
