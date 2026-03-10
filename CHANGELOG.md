# Changelog

All notable changes to SoundBoost Pro will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.0.0] - 2024-01-15

### Added
- Complete rewrite using C++20 and Qt6
- Machine learning-powered audio analysis
- Real-time genre detection and beat tracking
- Adaptive EQ based on audio content
- Cross-platform support (Windows, macOS, Linux, Android, iOS)
- SIMD-optimized DSP processing (AVX2, NEON)
- New modern GUI with multiple themes
- Profile system for saving presets
- Comprehensive test suite

### Changed
- Migrated from Qt5 to Qt6
- Improved audio engine architecture
- Enhanced effect chain processing
- Better memory management with lock-free buffers

### Removed
- Legacy Qt4 compatibility code
- Deprecated effect algorithms

## [1.5.0] - 2023-06-20

### Added
- Stereo widener effect
- Reverb effect with presets
- System tray integration

## [1.0.0] - 2022-01-10

### Added
- Initial release
- 10-band equalizer
- Bass booster
- Compressor/limiter
- Basic GUI
