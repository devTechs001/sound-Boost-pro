# SoundBoost Pro - Installation Guide

## System Requirements

### Windows
- Windows 10/11 (64-bit)
- Intel Core i5 or equivalent AMD processor
- 8 GB RAM (16 GB recommended)
- 2 GB free disk space
- DirectX 11 compatible graphics

### Linux
- Ubuntu 20.04+ or equivalent
- GCC 10+ or Clang 12+
- 8 GB RAM (16 GB recommended)
- 2 GB free disk space
- PulseAudio or PipeWire

### macOS
- macOS 11.0+ (Big Sur or later)
- Apple Silicon or Intel Core i5
- 8 GB RAM (16 GB recommended)
- 2 GB free disk space

## Installation

### Windows

1. Download the installer from the releases page
2. Run `SoundBoostPro-Setup.exe`
3. Follow the installation wizard
4. Launch SoundBoost Pro from the Start Menu

### Linux (AppImage)

```bash
# Download the AppImage
wget https://github.com/devTechs001/sound-Boost-pro/releases/latest/download/SoundBoostPro-x86_64.AppImage

# Make executable
chmod +x SoundBoostPro-x86_64.AppImage

# Run
./SoundBoostPro-x86_64.AppImage
```

### Linux (DEB/RPM)

```bash
# Debian/Ubuntu
sudo dpkg -i soundboostpro_2.0.0_amd64.deb
sudo apt-get install -f  # Install dependencies

# Fedora/RHEL
sudo dnf install soundboostpro-2.0.0.x86_64.rpm
```

### macOS

```bash
# Download and mount DMG
open SoundBoostPro.dmg

# Drag to Applications folder
cp -r /Volumes/SoundBoostPro/SoundBoostPro.app /Applications/
```

## Building from Source

### Prerequisites

```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake git \
    qt6-base-dev qt6-multimedia-dev \
    libpulse-dev libasound2-dev \
    nlohmann-json3-dev

# Fedora
sudo dnf install gcc-c++ cmake git \
    qt6-qtbase-devel qt6-qtmultimedia-devel \
    pulseaudio-libs-devel alsa-lib-devel \
    nlohmann-json-devel
```

### Build Steps

```bash
# Clone repository
git clone https://github.com/devTechs001/sound-Boost-pro.git
cd sound-Boost-pro

# Create build directory
mkdir build && cd build

# Configure
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --parallel

# Install (optional)
sudo cmake --install .
```

## Configuration

Configuration files are stored in:
- **Windows**: `%APPDATA%\SoundBoostPro\`
- **Linux**: `~/.config/SoundBoostPro/`
- **macOS**: `~/Library/Application Support/SoundBoostPro/`

### Key Configuration Options

```ini
# Audio settings
audio.sample_rate = 48000
audio.buffer_size = 512
audio.channels = 2

# UI settings
ui.theme = dark
ui.minimize_to_tray = true

# ML settings
ml.model_path = models/
ml.adaptive_enabled = true
```

## Troubleshooting

### No Audio Output
1. Check audio device selection in Settings
2. Verify system audio is working
3. Try increasing buffer size

### High CPU Usage
1. Reduce sample rate
2. Increase buffer size
3. Disable ML features temporarily

### Application Won't Start
1. Check system requirements
2. Verify all dependencies are installed
3. Check logs in configuration directory

## Support

For additional help:
- GitHub Issues: https://github.com/devTechs001/sound-Boost-pro/issues
- Documentation: https://github.com/devTechs001/sound-Boost-pro/tree/main/docs
