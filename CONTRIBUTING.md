# Contributing to SoundBoost Pro

Thank you for your interest in contributing to SoundBoost Pro!

## Code of Conduct

- Be respectful and inclusive
- Provide constructive feedback
- Focus on what's best for the community

## How to Contribute

### Reporting Bugs

1. Check existing issues first
2. Use the bug report template
3. Include:
   - Steps to reproduce
   - Expected vs actual behavior
   - System information
   - Screenshots if applicable

### Suggesting Features

1. Check existing feature requests
2. Use the feature request template
3. Describe the use case clearly

### Pull Requests

1. **Fork** the repository
2. **Create a branch**:
   ```bash
   git checkout -b feature/amazing-feature
   ```
3. **Make your changes**
4. **Run tests**:
   ```bash
   cmake --build build --target test
   ```
5. **Format code**:
   ```bash
   find src -name "*.cpp" -o -name "*.hpp" | xargs clang-format -i
   ```
6. **Commit** with clear messages:
   ```bash
   git commit -m "feat: add amazing feature"
   ```
7. **Push** and create PR

### Commit Message Format

```
type: subject

body (optional)

footer (optional)
```

Types:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation
- `style`: Formatting
- `refactor`: Code restructuring
- `test`: Tests
- `chore`: Maintenance

### Code Style

- Follow C++20 standards
- Use clang-format (see .clang-format)
- Write meaningful comments for complex logic
- Add tests for new features

### Development Setup

```bash
# Clone fork
git clone https://github.com/YOUR_USERNAME/sound-Boost-pro.git
cd sound-Boost-pro

# Create build directory
mkdir build && cd build

# Configure
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_TESTS=ON \
    -DENABLE_ASAN=ON

# Build
cmake --build . --parallel

# Run tests
ctest --output-on-failure
```

### Code Review Process

1. All PRs require at least one review
2. CI checks must pass
3. Code must follow project style
4. New features need tests

## Questions?

- Open an issue for general questions
- Check existing documentation

Thank you for contributing! 🎉
