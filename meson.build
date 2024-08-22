project('eagleshot', 'c',
  version : '0.1.2',
  default_options : ['warning_level=3'])

add_project_arguments('-D_POSIX_C_SOURCE=200809L', language : 'c')

conf_data = configuration_data()
conf_data.set_quoted('VERSION', meson.project_version())

configure_file(
  output : 'config.h',
  configuration : conf_data
)

inc = include_directories('include', 'src')

src_files = [
  'src/main.c',
  'src/screenshot.c',
  'src/utils.c'
]

executable('eagleshot',
           src_files,
           include_directories : inc,
           install : true)
