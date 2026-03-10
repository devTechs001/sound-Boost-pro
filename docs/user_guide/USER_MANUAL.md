# SoundBoost Pro User Manual

## Table of Contents

1. [Introduction](#introduction)
2. [Installation](#installation)
3. [Getting Started](#getting-started)
4. [Main Interface](#main-interface)
5. [Audio Effects](#audio-effects)
6. [Equalizer](#equalizer)
7. [Bass Booster](#bass-booster)
8. [ML Features](#ml-features)
9. [Profiles](#profiles)
10. [Settings](#settings)
11. [Troubleshooting](#troubleshooting)

## Introduction

SoundBoost Pro is an enterprise-grade audio enhancement suite that provides
real-time audio processing with machine learning capabilities. It captures
system audio and enhances it with professional-quality effects.

### Key Features

- **Real-time Audio Processing**: Ultra-low latency audio enhancement
- **10-Band Parametric Equalizer**: Full control over frequency response
- **Bass Booster**: Deep, powerful bass enhancement with harmonic synthesis
- **Compressor/Limiter**: Professional dynamics processing
- **Stereo Widener**: Enhanced stereo imaging
- **Reverb**: High-quality algorithmic reverb
- **ML-Powered Analysis**: Automatic genre detection and adaptive EQ
- **Profile System**: Save and recall your favorite settings
- **Cross-Platform**: Windows, macOS, Linux, Android, iOS

## Installation

### Windows

1. Download `SoundBoostPro-Setup.exe`
2. Run the installer as Administrator
3. Follow the installation wizard
4. Launch from Start Menu or Desktop shortcut

### macOS

1. Download `SoundBoostPro.dmg`
2. Open the DMG file
3. Drag SoundBoost Pro to Applications
4. Launch from Applications or Spotlight

### Linux

#### AppImage
```bash
chmod +x SoundBoostPro-*.AppImage
./SoundBoostPro-*.AppImage
```

#### Debian/Ubuntu
```bash
sudo dpkg -i soundboostpro_*.deb
sudo apt-get install -f  # Install dependencies
```

## Getting Started

### First Launch

1. **Audio Device Selection**: On first launch, select your audio output device
2. **System Audio Capture**: Enable system audio capture in Settings
3. **Default Profile**: The app starts with the "Flat" profile

### Quick Start

1. Click the **Power** button to enable audio processing
2. Use the **Master Volume** slider to adjust output level
3. Try a preset from the **Profile** dropdown
4. Adjust individual effects as needed

## Main Interface

### Navigation Bar

- **Home**: Overview with spectrum analyzer and meters
- **Equalizer**: 10-band parametric EQ
- **Bass**: Bass booster controls
- **Effects**: Additional effects (compressor, reverb, etc.)
- **Analyzer**: Detailed audio analysis
- **ML Insights**: Machine learning features
- **Settings**: Application configuration

### Master Controls

- **Power Button**: Enable/disable all processing
- **Master Volume**: Overall output level (0-200%)
- **Bypass Button**: Temporarily disable effects
- **Profile Selector**: Quick profile switching

## Audio Effects

### Effect Chain Order

Effects are processed in this order:
1. Noise Gate
2. Bass Booster
3. Equalizer
4. Compressor
5. Stereo Widener
6. Reverb
7. Limiter

Each effect can be individually enabled/disabled.

## Equalizer

### Band Frequencies

| Band | Frequency | Description |
|------|-----------|-------------|
| 1 | 31 Hz | Sub-bass |
| 2 | 62 Hz | Bass |
| 3 | 125 Hz | Low-mids |
| 4 | 250 Hz | Mids |
| 5 | 500 Hz | Mids |
| 6 | 1 kHz | Upper-mids |
| 7 | 2 kHz | Presence |
| 8 | 4 kHz | Brilliance |
| 9 | 8 kHz | Air |
| 10 | 16 kHz | Super-highs |

### Presets

- **Flat**: No EQ curve
- **Bass Boost**: Enhanced low frequencies
- **Treble Boost**: Enhanced high frequencies
- **V-Shape**: Boosted bass and treble, reduced mids
- **Vocal Boost**: Enhanced mid frequencies for vocals
- **Rock**: Classic rock sound
- **Jazz**: Warm, smooth jazz tone
- **Electronic**: Punchy bass, crisp highs

## Bass Booster

### Parameters

- **Frequency** (20-200 Hz): Center frequency of bass boost
- **Gain** (0-18 dB): Amount of bass boost
- **Width** (0.5-4.0 Q): Bandwidth of the boost
- **Harmonics** (0-100%): Add harmonic warmth
- **Sub Bass**: Enable sub-bass synthesis

### Tips

- For deep bass, use lower frequency (40-60 Hz)
- For punchy bass, use higher frequency (80-120 Hz)
- Enable harmonics for warmer sound on small speakers
- Use sub-bass sparingly to avoid muddiness

## ML Features

### Genre Detection

Automatically detects music genre with confidence score:
- Blues, Classical, Country, Disco, Hip-Hop
- Jazz, Metal, Pop, Reggae, Rock
- Electronic, Folk, R&B, Soul, Punk

### Adaptive EQ

When enabled, automatically adjusts EQ based on:
- Detected genre
- Voice/instrument content
- Frequency balance

### Beat Detection

Real-time BPM detection and beat tracking:
- Current BPM display
- Beat phase indicator
- Beat-synchronized visualizations

## Profiles

### Managing Profiles

1. **Save Current**: Click "Save Profile" to save current settings
2. **Load**: Select profile from dropdown
3. **Delete**: Right-click profile and select "Delete"
4. **Export/Import**: Share profiles via JSON files

### Default Profiles

- **Flat**: No enhancement
- **Bass Heavy**: Deep bass boost
- **Vocal Boost**: Enhanced vocals
- **Gaming**: Spatial awareness for games
- **Movies**: Cinematic sound

## Settings

### Audio Settings

- **Sample Rate**: 44100, 48000, or 96000 Hz
- **Buffer Size**: 128-2048 samples (lower = less latency)
- **Audio Device**: Input and output device selection

### UI Settings

- **Theme**: Dark, Light, or Neon
- **Minimize to Tray**: Keep running in system tray
- **Start Minimized**: Start hidden in system tray
- **Language**: Interface language

### Advanced

- **SIMD Optimization**: Enable CPU optimization
- **ML Processing**: Enable/disable ML features
- **Logging**: Debug logging level

## Troubleshooting

### No Audio Output

1. Check audio device is selected correctly
2. Verify master volume is not zero
3. Check Windows/macOS audio settings
4. Restart the application

### High CPU Usage

1. Increase buffer size in settings
2. Disable unused effects
3. Disable ML features if not needed

### Crackling/Popping Audio

1. Increase buffer size
2. Close other audio applications
3. Update audio drivers
4. Check CPU usage

### Application Won't Start

1. Run as Administrator (Windows)
2. Check antivirus exclusions
3. Reinstall Visual C++ Runtime
4. Check log file for errors

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| Ctrl+M | Toggle master on/off |
| Ctrl+B | Toggle bypass |
| Ctrl+1-7 | Switch views |
| Ctrl+S | Save current profile |
| Ctrl+Q | Quit application |

## Support

- **Documentation**: https://docs.soundboostpro.com
- **Support Email**: support@soundboostpro.com
- **GitHub Issues**: https://github.com/soundboostpro/issues
