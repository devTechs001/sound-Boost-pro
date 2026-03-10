# SoundBoost Pro - Developer Guide

## Project Overview

SoundBoost Pro is an enterprise-grade audio enhancement application built with:
- **C++20**: Modern C++ features
- **Qt6**: Cross-platform GUI framework
- **ONNX Runtime**: Machine learning inference
- **SIMD**: Optimized DSP operations

## Architecture

### Layered Architecture

```
┌─────────────────────────────────────────┐
│           Application Layer             │
│  (MainWindow, Views, Components)        │
├─────────────────────────────────────────┤
│            Service Layer                │
│  (ServiceLocator, EventBus)             │
├─────────────────────────────────────────┤
│             Core Layer                  │
│  (AudioEngine, DSP, Effects, ML)        │
├─────────────────────────────────────────┤
│          Platform Layer                 │
│  (Windows WASAPI, Linux PulseAudio)     │
└─────────────────────────────────────────┘
```

### Key Components

#### Audio Engine
- Real-time audio processing
- Lock-free ring buffers
- Effect chain management
- ML integration

#### DSP Pipeline
1. Input capture
2. DC offset removal
3. Effect chain processing
4. Spectrum analysis
5. Output

#### Effect Chain Order
1. Noise Gate
2. Bass Booster
3. Equalizer
4. Compressor
5. Stereo Widener
6. Reverb
7. Limiter

## Building

### Development Build

```bash
mkdir build && cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON \
    -DENABLE_PROFILING=ON \
    -DENABLE_ASAN=ON
cmake --build . --parallel
```

### Release Build

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_LTO=ON \
    -DENABLE_SIMD=ON
cmake --build . --parallel
```

## Testing

```bash
# Run all tests
ctest --output-on-failure

# Run specific test
./SoundBoostUnitTests --gtest_filter=AudioBufferTest.*

# Coverage
cmake .. -DCMAKE_BUILD_TYPE=Debug -DCODE_COVERAGE=ON
cmake --build .
ctest
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage
```

## Code Style

We use clang-format with the project's `.clang-format` configuration:

```bash
# Format all source files
find src -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
```

## Adding New Effects

1. Create header inheriting from `EffectBase`:
```cpp
class MyEffect : public EffectBase {
public:
    void initialize(int sampleRate, int bufferSize) override;
    void process(AudioBuffer& buffer) override;
    void reset() override;
    std::string getName() const override { return "MyEffect"; }
};
```

2. Implement in .cpp file
3. Add to effect chain in `AudioEngine::setupDefaultEffects()`

## ML Model Integration

1. Place ONNX model in `resources/models/`
2. Update `ModelLoader` to load the model
3. Add inference logic in respective detector class

## Debugging

### Audio Debugging
```cpp
// Log audio levels
LOG_DEBUG("Input level: {} dB", level);

// Use profiler for performance
PROFILE_SCOPE("MyEffect::process");
```

### GUI Debugging
```bash
# Enable Qt debug output
export QT_LOGGING_RULES="*.debug=true"
```

## Performance Tips

1. Use SIMD operations for bulk processing
2. Minimize allocations in audio thread
3. Use lock-free data structures
4. Profile regularly with `ENABLE_PROFILING=ON`

## Contributing

1. Fork the repository
2. Create feature branch
3. Make changes
4. Run tests
5. Submit PR

See CONTRIBUTING.md for detailed guidelines.
