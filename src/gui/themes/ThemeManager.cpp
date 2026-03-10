// src/gui/themes/ThemeManager.cpp
#include "ThemeManager.hpp"
#include <QWidget>
#include <QApplication>
#include <sstream>

namespace SoundBoost {

ThemeColors DarkTheme::getColors() const {
    return {
        "#1a1a2e", "#16213e", "#0f4c75", "#1b6ca8", "#3282b8",
        "#ffffff", "#9e9e9e", "#2d3748", "#00d9ff",
        "#4caf50", "#ff9800", "#f44336"
    };
}

ThemeColors LightTheme::getColors() const {
    return {
        "#f5f5f5", "#ffffff", "#1976d2", "#1565c0", "#42a5f5",
        "#212121", "#757575", "#e0e0e0", "#00bcd4",
        "#4caf50", "#ff9800", "#f44336"
    };
}

ThemeColors NeonTheme::getColors() const {
    return {
        "#0d0d0d", "#1a1a1a", "#ff00ff", "#cc00cc", "#00ffff",
        "#ffffff", "#888888", "#333333", "#00ff00",
        "#00ff00", "#ffff00", "#ff0000"
    };
}

std::string NeonTheme::generateStyleSheet() const {
    auto baseStyle = Theme::generateStyleSheet();
    auto colors = getColors();

    std::ostringstream ss;
    ss << baseStyle;

    ss << R"(
QPushButton:hover {
    box-shadow: 0 0 10px )" << colors.primary << R"(;
}

QLabel#viewTitle {
    color: )" << colors.primary << R"(;
    text-shadow: 0 0 10px )" << colors.primary << R"(;
}
)";

    return ss.str();
}

ThemeManager::ThemeManager() {
    registerTheme(std::make_unique<DarkTheme>());
    registerTheme(std::make_unique<LightTheme>());
    registerTheme(std::make_unique<NeonTheme>());
}

ThemeManager::~ThemeManager() = default;

void ThemeManager::registerTheme(std::unique_ptr<Theme> theme) {
    std::string name = theme->getName();
    m_themes[name] = std::move(theme);
}

void ThemeManager::applyTheme(const std::string& themeName, QWidget* root) {
    auto it = m_themes.find(themeName);
    if (it == m_themes.end()) {
        it = m_themes.find("dark");
    }

    if (it != m_themes.end()) {
        m_currentTheme = it->second.get();
        QString styleSheet = QString::fromStdString(m_currentTheme->generateStyleSheet());

        if (root) {
            root->setStyleSheet(styleSheet);
        } else {
            qApp->setStyleSheet(styleSheet);
        }
    }
}

std::vector<std::string> ThemeManager::getAvailableThemes() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : m_themes) {
        names.push_back(name);
    }
    return names;
}

std::string Theme::generateStyleSheet() const {
    auto colors = getColors();

    std::ostringstream ss;
    ss << R"(
QWidget {
    background-color: )" << colors.background << R"(;
    color: )" << colors.text << R"(;
    font-family: 'Segoe UI', 'Roboto', sans-serif;
    font-size: 13px;
}

QPushButton {
    background-color: )" << colors.surface << R"(;
    color: )" << colors.text << R"(;
    border: 1px solid )" << colors.border << R"(;
    border-radius: 6px;
    padding: 8px 16px;
}

QPushButton:hover {
    background-color: )" << colors.primary << R"(;
}

QSlider::groove:horizontal {
    height: 6px;
    background: )" << colors.surface << R"(;
}

QSlider::handle:horizontal {
    background: )" << colors.primary << R"(;
    width: 18px;
    height: 18px;
    border-radius: 9px;
}

QComboBox {
    background-color: )" << colors.surface << R"(;
    border: 1px solid )" << colors.border << R"(;
    border-radius: 6px;
    padding: 6px 12px;
}

QGroupBox {
    border: 1px solid )" << colors.border << R"(;
    border-radius: 8px;
    margin-top: 12px;
}
)";

    return ss.str();
}

} // namespace SoundBoost
