# SoundBoost Pro

Enterprise Cross-Platform Audio Enhancement Suite

## Overview

SoundBoost Pro is a professional-grade, real-time audio enhancement application built with modern C++20 and Qt6. It provides advanced audio processing capabilities including:

- **Bass Boost**: Enhanced low-frequency response with harmonic enhancement
- **Parametric Equalizer**: 10-band ISO-standard equalizer
- **Dynamic Processing**: Compressor and limiter for controlled dynamics
- **Spatial Effects**: Stereo widening for immersive sound
- **ML-Powered Analysis**: Genre detection, BPM analysis, and adaptive EQ suggestions

## Features

### Audio Processing
- Real-time system audio capture
- Low-latency audio processing
- SIMD-optimized DSP algorithms (AVX2, SSE, NEON)
- Lock-free ring buffers for thread-safe audio processing

### Machine Learning
- ONNX Runtime integration for ML inference
- Genre classification (15 genres)
- BPM detection
- Voice activity detection
- Adaptive EQ suggestions

### User Interface
- Modern, customizable themes (Dark, Light, Neon)
- Real-time spectrum analyzer
- VU meters
- Interactive EQ band widgets
- System tray integration

### Cross-Platform
- Windows (WASAPI)
- Linux (PulseAudio, PipeWire, ALSA)
- macOS (CoreAudio)
- Android (AAudio, OpenSL ES)
- iOS (CoreAudio/AVFoundation)

## Building

### Prerequisites

- CMake 3.20+
- C++20 compatible compiler (GCC 10+, Clang 12+, MSVC 2019+)
- Qt 6.5+
- vcpkg (for dependency management)

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/soundboostpro/soundboost-pro.git
cd soundboost-pro

# Build using the build script
./scripts/build/build_all.sh

# Or manually
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel
```

### Build Options

| Option | Description | Default |
|--------|-------------|---------|
| `BUILD_TESTS` | Build test suite | ON |
| `BUILD_DOCS` | Build documentation | ON |
| `ENABLE_ML` | Enable ML features | ON |
| `ENABLE_SIMD` | Enable SIMD optimizations | ON |
| `ENABLE_LTO` | Enable link-time optimization | ON |

## Project Structure

```
SoundBoostPro/
├── src/
│   ├── core/          # Audio engine, DSP, effects, ML
│   ├── gui/           # Qt-based user interface
│   ├── platform/      # Platform-specific implementations
│   ├── services/      # Application services
│   ├── network/       # Online features
│   └── utils/         # Utilities and helpers
├── tests/
│   ├── unit/          # Unit tests
│   ├── integration/   # Integration tests
│   └── performance/   # Benchmarks
├── resources/         # Icons, presets, translations
├── docs/              # Documentation
└── scripts/           # Build and deployment scripts
```

## Testing

```bash
# Run all tests
cd build
ctest --output-on-failure

# Run specific test suite
./SoundBoostUnitTests
./SoundBoostIntegrationTests
```

## License

This project is proprietary software. All rights reserved.

## Support

For support and inquiries:
- Website: https://soundboostpro.com
- Email: support@soundboostpro.com

## Version

2.0.0
