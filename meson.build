project('eagleshot', 'c',
  version : '0.1.1',
  default_options : ['warning_level=3'])

# Configure a header file to pass the version number to the source code
conf_data = configuration_data()
conf_data.set_quoted('VERSION', meson.project_version())

configure_file(
  output : 'config.h',
  configuration : conf_data
)

# Add the directory containing config.h to the include path
inc = include_directories('.')

# Compile the executable
executable('eagleshot',
           'eagleshot.c',
           include_directories : inc,
           install : true)
