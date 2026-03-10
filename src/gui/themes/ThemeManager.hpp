// src/gui/themes/ThemeManager.hpp
#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>

class QWidget;

namespace SoundBoost {

struct ThemeColors {
    std::string background;
    std::string surface;
    std::string primary;
    std::string primaryVariant;
    std::string secondary;
    std::string text;
    std::string textSecondary;
    std::string border;
    std::string accent;
    std::string success;
    std::string warning;
    std::string error;
};

class Theme {
public:
    virtual ~Theme() = default;
    virtual std::string getName() const = 0;
    virtual ThemeColors getColors() const = 0;
    virtual std::string generateStyleSheet() const;
};

class DarkTheme : public Theme {
public:
    std::string getName() const override { return "dark"; }
    ThemeColors getColors() const override;
};

class LightTheme : public Theme {
public:
    std::string getName() const override { return "light"; }
    ThemeColors getColors() const override;
};

class NeonTheme : public Theme {
public:
    std::string getName() const override { return "neon"; }
    ThemeColors getColors() const override;
    std::string generateStyleSheet() const override;
};

class ThemeManager {
public:
    ThemeManager();
    ~ThemeManager();

    void registerTheme(std::unique_ptr<Theme> theme);
    void applyTheme(const std::string& themeName, QWidget* root);

    std::vector<std::string> getAvailableThemes() const;
    const Theme* getCurrentTheme() const { return m_currentTheme; }

private:
    std::map<std::string, std::unique_ptr<Theme>> m_themes;
    Theme* m_currentTheme{nullptr};
};

} // namespace SoundBoost
