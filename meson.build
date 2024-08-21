project('eagleshot', 'c',  # Define project name and language
  version : '0.1.2',  # Set project version
  default_options : ['warning_level=3'])  # Set default compiler warning level

add_project_arguments('-D_POSIX_C_SOURCE=200809L', language : 'c')  # Add POSIX.1-2008 compatibility flag

# Configure a header file to pass the version number to the source code
conf_data = configuration_data()  # Create configuration data object
conf_data.set_quoted('VERSION', meson.project_version())  # Set VERSION macro with project version

configure_file(  # Generate configuration header file
  output : 'config.h',  # Output file name
  configuration : conf_data  # Use the configuration data object
)

# Add the directory containing config.h to the include path
inc = include_directories('.')  # Set include directory to current directory

# Compile the executable
executable('eagleshot',  # Name of the executable
           'eagleshot.c',  # Source file
           include_directories : inc,  # Include directories
           install : true)  # Install the executable when running 'meson install'
