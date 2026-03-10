# Contributing to SoundBoost Pro

Thank you for your interest in contributing to SoundBoost Pro! This document provides guidelines and instructions for contributing.

## Code of Conduct

Please be respectful and professional in all interactions. We welcome contributors of all backgrounds.

## How to Contribute

### Reporting Bugs

1. Check existing issues to avoid duplicates
2. Use the bug report template
3. Include:
   - Steps to reproduce
   - Expected vs actual behavior
   - System information
   - Screenshots if applicable

### Suggesting Features

1. Check existing feature requests
2. Use the feature request template
3. Explain the use case clearly

### Pull Requests

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes
4. Write/update tests
5. Ensure all tests pass
6. Update documentation
7. Submit a pull request

## Development Setup

### Prerequisites

- CMake 3.20+
- C++20 compatible compiler
- Qt 6.6+
- vcpkg

### Building

```bash
# Clone the repository
git clone https://github.com/soundboostpro/soundboost-pro.git
cd soundboost-pro

# Build
./scripts/build/build_all.sh
```

## Coding Standards

### C++ Style

- Follow the existing code style
- Use `.clang-format` for formatting
- Run `clang-tidy` for linting
- Use meaningful variable names
- Add comments for complex logic

### Commit Messages

```
<type>(<scope>): <subject>

<body>

<footer>
```

Types: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`

Example:
```
feat(eq): add low-shelf filter type

Added support for low-shelf filter type in the equalizer.
This allows for more precise bass control.

Closes #123
```

## Testing

- Write unit tests for new features
- Ensure all existing tests pass
- Aim for >80% code coverage

## Documentation

- Update README.md for user-facing changes
- Add API documentation for new classes
- Update architecture docs for significant changes

## Questions?

Open an issue or contact maintainers at contributors@soundboostpro.com
