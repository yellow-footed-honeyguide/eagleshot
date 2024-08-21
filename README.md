# Wayland screenshot utility

## Overview

- 🦅 eagleshot is a lightweight and efficient Wayland screenshot utility
- 📸 Captures screenshots with eagle-eye precision
- 🚀 Simple to use, fast, and reliable
- 🖥️ Designed specifically for Wayland compositors

## Features

- 🎯 Area selection for precise captures
- 💾 Automatic saving to your ~/Pictures directory
- 🔄 Integration with grim and slurp for optimal performance
- 📏 Minimal dependencies for a lean system footprint

## System Dependencies

- grim
- slurp

## Installation

```
git clone https://github.com/your-username/eagleshot.git
cd eagleshot
mkdir build && cd build
meson ..
ninja
sudo ninja install
```

## Usage
```
eagleshot
```

Simply run the command and select the area you want to capture.

## Options

-v, --version: Show program's version number and exit

## Contributing

Contributions to eagleshot are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT LICENSE - see the MIT LICENSE file for details.

## Author

- **Sergey Veneckiy**
- Email: s.venetsky@gmail.com
- GitHub: [@yellow-footed-honeyguide](https://github.com/yellow-footed-honeyguide)

## Acknowledgments

This project uses grim and slurp for Wayland screen capture functionality.
