// src/gui/components/PresetSelector.hpp
#pragma once

#include <QWidget>
#include <vector>
#include <string>

class QComboBox;

namespace SoundBoost {

class PresetSelector : public QWidget {
    Q_OBJECT

public:
    explicit PresetSelector(QWidget* parent = nullptr);
    ~PresetSelector() override = default;

    void setPresets(const std::vector<std::string>& presets);

signals:
    void presetSelected(const QString& preset);

private slots:
    void onCurrentTextChanged(const QString& text);

private:
    QComboBox* m_combo;
};

} // namespace SoundBoost
