// src/gui/views/MLInsightsView.hpp
#pragma once

#include <QWidget>
#include <string>

namespace SoundBoost {

class AudioEngine;
class QLabel;

class MLInsightsView : public QWidget {
    Q_OBJECT

public:
    explicit MLInsightsView(AudioEngine& audioEngine, QWidget* parent = nullptr);
    ~MLInsightsView() override = default;

    void updateAnalysis(const std::string& genre, float bpm, bool hasVoice);

private:
    void setupUI();

    AudioEngine& m_audioEngine;

    QLabel* m_genreLabel{nullptr};
    QLabel* m_bpmLabel{nullptr};
    QLabel* m_voiceLabel{nullptr};
};

} // namespace SoundBoost
