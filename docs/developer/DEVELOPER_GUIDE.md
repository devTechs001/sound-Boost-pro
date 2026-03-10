# SoundBoost Pro - Developer Guide

## Table of Contents

1. [Getting Started](#getting-started)
2. [Architecture Overview](#architecture-overview)
3. [Building from Source](#building-from-source)
4. [Code Style](#code-style)
5. [Testing](#testing)
6. [Debugging](#debugging)
7. [Contributing](#contributing)

## Getting Started

### Prerequisites

- **Compiler**: GCC 11+, Clang 13+, or MSVC 2022+
- **CMake**: 3.20 or later
- **Qt**: 6.6 or later
- **vcpkg**: For dependency management
- **Git**: For version control

### Clone the Repository

```bash
git clone https://github.com/soundboostpro/soundboost-pro.git
cd soundboost-pro
git submodule update --init --recursive
```

## Architecture Overview

### Directory Structure

```
soundboost-pro/
├── src/
│   ├── core/          # Audio engine, DSP, effects, ML
│   ├── gui/           # Qt-based user interface
│   ├── platform/      # Platform-specific code
│   ├── utils/         # Utilities (logging, config)
│   └── services/      # Application services
├── tests/             # Test suite
├── docs/              # Documentation
├── scripts/           # Build and deployment scripts
└── resources/         # Assets
```

### Key Components

1. **AudioEngine**: Core audio processing
2. **EffectChain**: Effect processing pipeline
3. **MLEngine**: Machine learning inference
4. **MainWindow**: Primary UI window
5. **ServiceLocator**: Dependency injection

## Building from Source

### Linux

```bash
# Install dependencies
sudo apt-get install -y build-essential cmake ninja-build \
    qt6-base-dev qt6-multimedia-dev libpulse-dev libasound2-dev

# Build
./scripts/build/build_linux.sh
```

### Windows

```powershell
# Using PowerShell
.\scripts\build\build_all.ps1 -Target all
```

### macOS

```bash
# Build
./scripts/build/build_macos.sh
```

### Custom Build Options

```bash
cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTS=ON \
    -DENABLE_ML=ON \
    -DENABLE_SIMD=ON \
    -DENABLE_PROFILING=OFF
```

### Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_TESTS` | ON | Build test suite |
| `BUILD_DOCS` | ON | Build documentation |
| `ENABLE_ML` | ON | Enable ML features |
| `ENABLE_SIMD` | ON | Enable SIMD optimizations |
| `ENABLE_ASAN` | OFF | Enable Address Sanitizer |
| `ENABLE_TSAN` | OFF | Enable Thread Sanitizer |
| `ENABLE_PROFILING` | OFF | Enable profiling |

## Code Style

### Formatting

We use `clang-format` for consistent code style:

```bash
# Format all source files
find src -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
```

### Linting

We use `clang-tidy` for code quality:

```bash
# Run clang-tidy
run-clang-tidy -p build src/
```

### Naming Conventions

- **Classes/Structs**: `CamelCase` (e.g., `AudioEngine`)
- **Functions/Methods**: `camelCase` (e.g., `processAudio`)
- **Variables**: `lower_case` (e.g., `bufferSize`)
- **Members**: `m_` prefix (e.g., `m_bufferSize`)
- **Constants**: `UPPER_CASE` (e.g., `MAX_BUFFER_SIZE`)
- **Namespaces**: `lower_case` (e.g., `SoundBoost`)

### Code Organization

```cpp
// Header file structure
#pragma once

// 1. Includes (sorted by scope)
#include <system_header>
#include <qt_header>
#include "local_header.hpp"

// 2. Forward declarations
class SomeClass;

// 3. Namespace
namespace SoundBoost {

// 4. Class definition
class MyClass {
public:
    // Public interface
private:
    // Private members
};

} // namespace SoundBoost
```

## Testing

### Running Tests

```bash
# Build and run all tests
cd build
ctest --output-on-failure

# Run specific test
ctest -R test_audio_buffer --output-on-failure
```

### Writing Tests

```cpp
#include <gtest/gtest.h>
#include "core/audio/AudioBuffer.hpp"

using namespace SoundBoost;

TEST(AudioBufferTest, Construction) {
    AudioBuffer buffer(512, 2, 48000);
    
    EXPECT_EQ(buffer.getNumSamples(), 512);
    EXPECT_EQ(buffer.getNumChannels(), 2);
    EXPECT_EQ(buffer.getSampleRate(), 48000);
}

TEST_F(AudioBufferFixture, ApplyGain) {
    // Test fixture for shared setup
    buffer->applyGain(2.0f);
    EXPECT_NEAR(buffer->getPeak(), 1.0f, 0.001f);
}
```

### Test Categories

- **Unit Tests**: Individual components
- **Integration Tests**: Component interactions
- **Performance Tests**: Benchmarks

## Debugging

### Logging

```cpp
#include "utils/Logger.hpp"

LOG_DEBUG("Debug message: {}", value);
LOG_INFO("Info message: {}", value);
LOG_WARN("Warning: {}", issue);
LOG_ERROR("Error: {}", error);
LOG_CRITICAL("Critical: {}", criticalIssue);
```

### Profiling

```cpp
#include "utils/Profiler.hpp"

void expensiveFunction() {
    PROFILE_SCOPE("expensiveFunction");
    // Code to profile
}
```

### Debug Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_ASAN=ON \
    -DENABLE_UBSAN=ON
```

### Common Issues

#### Memory Leaks

```bash
# Using AddressSanitizer
cmake -B build -DENABLE_ASAN=ON
```

#### Race Conditions

```bash
# Using ThreadSanitizer
cmake -B build -DENABLE_TSAN=ON
```

## Contributing

### Pull Request Process

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Write/update tests
5. Ensure all tests pass
6. Update documentation
7. Submit a pull request

### Commit Message Format

```
<type>(<scope>): <subject>

<body>

<footer>
```

**Types**: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`

**Example**:
```
feat(eq): add low-shelf filter type

Added support for low-shelf filter in the equalizer.
This allows for more precise bass control.

Closes #123
```

### Code Review Checklist

- [ ] Code follows style guidelines
- [ ] Tests are included
- [ ] Documentation is updated
- [ ] No compiler warnings
- [ ] Performance impact is acceptable

## API Documentation

### Generating Doxygen Docs

```bash
cmake -B build -DBUILD_DOCS=ON
cd build
make docs
```

Docs will be in `build/docs/api/html/`.

## Release Process

1. Update version in `CMakeLists.txt`
2. Update `CHANGELOG.md`
3. Create release branch
4. Tag the release
5. Build release artifacts
6. Publish release

## Support

For development questions:
- **GitHub Discussions**: https://github.com/soundboostpro/discussions
- **Discord**: https://discord.gg/soundboostpro
