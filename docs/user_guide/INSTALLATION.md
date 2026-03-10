# SoundBoost Pro - Installation Guide

## System Requirements

### Minimum Requirements

- **OS**: Windows 10 / macOS 10.15 / Linux (Ubuntu 20.04+)
- **CPU**: Dual-core 2.0 GHz (AVX2 support recommended)
- **RAM**: 4 GB
- **Disk Space**: 500 MB
- **Audio**: Any audio output device

### Recommended Requirements

- **OS**: Windows 11 / macOS 12+ / Linux (Ubuntu 22.04+)
- **CPU**: Quad-core 3.0 GHz+ with AVX2/NEON
- **RAM**: 8 GB
- **Disk Space**: 1 GB (with ML models)
- **Audio**: Low-latency audio interface

## Windows Installation

### Using Installer (Recommended)

1. Download `SoundBoostPro-Setup.exe` from the official website
2. Right-click and select "Run as Administrator"
3. Follow the installation wizard:
   - Accept the license agreement
   - Choose installation directory (default: `C:\Program Files\SoundBoost Pro`)
   - Select components:
     - Core Application (required)
     - ML Models (recommended)
     - Audio Presets (recommended)
     - Start with Windows (optional)
4. Click "Install"
5. Launch from Start Menu or Desktop shortcut

### Using Winget

```powershell
winget install SoundBoostPro
```

### Using Chocolatey

```powershell
choco install soundboost-pro
```

### Manual Installation

1. Download the ZIP archive
2. Extract to desired location
3. Run `SoundBoostPro.exe`
4. (Optional) Create desktop shortcut

## macOS Installation

### Using DMG

1. Download `SoundBoostPro.dmg`
2. Open the DMG file
3. Drag SoundBoost Pro to Applications folder
4. Launch from Applications or Spotlight

**Note**: On first launch, you may need to:
- Right-click and select "Open"
- Click "Open" in the security dialog

### Using Homebrew

```bash
brew install --cask soundboost-pro
```

### Using Manual Download

1. Download the ZIP archive
2. Extract to Applications
3. Run from Applications folder

## Linux Installation

### AppImage (Universal)

```bash
# Download
wget https://github.com/soundboostpro/releases/download/v2.0.0/SoundBoostPro-2.0.0-x86_64.AppImage

# Make executable
chmod +x SoundBoostPro-*.AppImage

# Run
./SoundBoostPro-*.AppImage

# (Optional) Integrate with desktop
./SoundBoostPro-*.AppImage --appimage-integrate
```

### Debian/Ubuntu (.deb)

```bash
# Download
wget https://github.com/soundboostpro/releases/download/v2.0.0/soundboostpro_2.0.0_amd64.deb

# Install
sudo dpkg -i soundboostpro_*.deb

# Fix dependencies if needed
sudo apt-get install -f
```

### RHEL/Fedora (.rpm)

```bash
# Download
wget https://github.com/soundboostpro/releases/download/v2.0.0/soundboostpro-2.0.0-1.x86_64.rpm

# Install
sudo dnf install soundboostpro-*.rpm
```

### Arch Linux (AUR)

```bash
# Using yay
yay -S soundboost-pro

# Using paru
paru -S soundboost-pro
```

### Flatpak

```bash
# Add Flathub repository (if not already added)
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo

# Install
flatpak install flathub com.soundboostpro.app

# Run
flatpak run com.soundboostpro.app
```

### AppImageLauncher (Recommended for AppImage)

```bash
# Install AppImageLauncher
sudo add-apt-repository ppa:appimagelauncher-team/stable
sudo apt-get update
sudo apt-get install appimagelauncher

# Then simply open any AppImage file
```

## Android Installation

### Google Play Store

1. Open Google Play Store
2. Search for "SoundBoost Pro"
3. Tap "Install"

### APK Download

1. Download the APK file
2. Enable "Install from Unknown Sources" in Settings
3. Open the downloaded APK
4. Tap "Install"

## iOS Installation

### App Store

1. Open App Store
2. Search for "SoundBoost Pro"
3. Tap "Get" or the download icon

### TestFlight (Beta)

1. Install TestFlight from App Store
2. Open the TestFlight invitation link
3. Tap "Install" in TestFlight

## Post-Installation

### First Launch Configuration

1. **Audio Device Selection**
   - Go to Settings > Audio
   - Select your output device
   - Test audio playback

2. **System Audio Capture** (Windows)
   - Install virtual audio cable if needed
   - Select "Stereo Mix" or "What U Hear" as input

3. **ML Models** (Optional)
   - Download ML models from Settings > ML
   - Models are ~50 MB

### Updating

#### Automatic Updates
- The application checks for updates on startup
- Enable in Settings > General > Check for Updates

#### Manual Updates
- Download the latest version
- Install over the existing version (settings preserved)

### Uninstallation

#### Windows
```
Settings > Apps > SoundBoost Pro > Uninstall
```

#### macOS
```
Drag from Applications to Trash
```

#### Linux (deb)
```bash
sudo apt-get remove soundboostpro
```

#### Linux (rpm)
```bash
sudo dnf remove soundboostpro
```

#### Linux (Flatpak)
```bash
flatpak uninstall com.soundboostpro.app
```

## Troubleshooting Installation

### Windows: "App can't be opened"
- Right-click > Properties > Check "Unblock"
- Run as Administrator

### macOS: "App is damaged"
```bash
xattr -cr /Applications/SoundBoost\ Pro.app
```

### Linux: AppImage won't run
```bash
# Install FUSE
sudo apt-get install libfuse2

# Or use the extract option
./SoundBoostPro-*.AppImage --appimage-extract
./squashfs-root/AppRun
```

### Android: Install blocked
- Go to Settings > Security
- Enable "Unknown Sources" for your browser/file manager

## Support

For installation issues, contact:
- Email: support@soundboostpro.com
- GitHub Issues: https://github.com/soundboostpro/issues
