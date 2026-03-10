// src/platform/linux/PulseAudioCapture.cpp
#ifdef PLATFORM_LINUX

#include "PulseAudioCapture.hpp"
#include "../../utils/Logger.hpp"

namespace SoundBoost {

PulseAudioCapture::PulseAudioCapture() = default;

PulseAudioCapture::~PulseAudioCapture() {
    stop();

    if (m_stream) {
        pa_stream_unref(m_stream);
    }

    if (m_context) {
        pa_context_disconnect(m_context);
        pa_context_unref(m_context);
    }

    if (m_mainloop) {
        pa_threaded_mainloop_stop(m_mainloop);
        pa_threaded_mainloop_free(m_mainloop);
    }
}

bool PulseAudioCapture::initialize(int sampleRate, int bufferSize, int channels) {
    m_sampleRate = sampleRate;
    m_bufferSize = bufferSize;
    m_channels = channels;

    // Create mainloop
    m_mainloop = pa_threaded_mainloop_new();
    if (!m_mainloop) {
        LOG_ERROR("Failed to create PulseAudio mainloop");
        return false;
    }

    // Get mainloop API
    pa_mainloop_api* api = pa_threaded_mainloop_get_api(m_mainloop);

    // Create context
    m_context = pa_context_new(api, "SoundBoost Pro");
    if (!m_context) {
        LOG_ERROR("Failed to create PulseAudio context");
        return false;
    }

    // Set context state callback
    pa_context_set_state_callback(m_context, contextStateCallback, this);

    // Connect to server
    if (pa_context_connect(m_context, nullptr, PA_CONTEXT_NOFLAGS, nullptr) < 0) {
        LOG_ERROR("Failed to connect to PulseAudio server");
        return false;
    }

    // Start mainloop
    if (pa_threaded_mainloop_start(m_mainloop) < 0) {
        LOG_ERROR("Failed to start PulseAudio mainloop");
        return false;
    }

    // Wait for context to be ready
    pa_threaded_mainloop_lock(m_mainloop);
    while (!m_contextReady.load()) {
        pa_threaded_mainloop_wait(m_mainloop);
    }

    pa_context_state_t state = pa_context_get_state(m_context);
    if (state != PA_CONTEXT_READY) {
        pa_threaded_mainloop_unlock(m_mainloop);
        LOG_ERROR("PulseAudio context failed to become ready");
        return false;
    }
    pa_threaded_mainloop_unlock(m_mainloop);

    // Create stream
    pa_sample_spec spec;
    spec.format = PA_SAMPLE_FLOAT32LE;
    spec.rate = sampleRate;
    spec.channels = channels;

    pa_threaded_mainloop_lock(m_mainloop);

    m_stream = pa_stream_new(m_context, "SoundBoost Capture", &spec, nullptr);
    if (!m_stream) {
        pa_threaded_mainloop_unlock(m_mainloop);
        LOG_ERROR("Failed to create PulseAudio stream");
        return false;
    }

    pa_stream_set_state_callback(m_stream, streamStateCallback, this);
    pa_stream_set_read_callback(m_stream, streamReadCallback, this);

    pa_threaded_mainloop_unlock(m_mainloop);

    LOG_INFO("PulseAudio capture initialized: {}Hz, {} channels", sampleRate, channels);
    return true;
}

void PulseAudioCapture::contextStateCallback(pa_context* c, void* userdata) {
    auto* capture = static_cast<PulseAudioCapture*>(userdata);

    pa_context_state_t state = pa_context_get_state(c);

    switch (state) {
        case PA_CONTEXT_READY:
        case PA_CONTEXT_FAILED:
        case PA_CONTEXT_TERMINATED:
            capture->m_contextReady.store(true);
            pa_threaded_mainloop_signal(capture->m_mainloop, 0);
            break;
        default:
            break;
    }
}

void PulseAudioCapture::streamStateCallback(pa_stream* s, void* userdata) {
    auto* capture = static_cast<PulseAudioCapture*>(userdata);
    pa_threaded_mainloop_signal(capture->m_mainloop, 0);
}

void PulseAudioCapture::streamReadCallback(pa_stream* s, size_t length, void* userdata) {
    auto* capture = static_cast<PulseAudioCapture*>(userdata);

    const void* data;
    size_t nbytes;

    if (pa_stream_peek(s, &data, &nbytes) < 0) {
        LOG_ERROR("Failed to read from PulseAudio stream");
        return;
    }

    if (data && nbytes > 0) {
        size_t numSamples = nbytes / sizeof(float) / capture->m_channels;
        AudioBuffer buffer(numSamples, capture->m_channels, capture->m_sampleRate);
        buffer.setData(static_cast<const float*>(data), nbytes / sizeof(float));

        std::lock_guard<std::mutex> lock(capture->m_callbackMutex);
        if (capture->m_callback) {
            capture->m_callback(buffer);
        }
    }

    pa_stream_drop(s);
}

void PulseAudioCapture::start() {
    if (m_running.exchange(true)) {
        return;
    }

    pa_threaded_mainloop_lock(m_mainloop);

    std::string sourceName = m_monitorMode ? 
        "@DEFAULT_MONITOR@" : m_currentDeviceId;

    pa_buffer_attr bufferAttr;
    bufferAttr.maxlength = (uint32_t)-1;
    bufferAttr.fragsize = m_bufferSize * m_channels * sizeof(float);

    if (pa_stream_connect_record(m_stream, sourceName.c_str(), &bufferAttr,
            static_cast<pa_stream_flags_t>(PA_STREAM_ADJUST_LATENCY)) < 0) {
        LOG_ERROR("Failed to connect PulseAudio stream");
        m_running.store(false);
        pa_threaded_mainloop_unlock(m_mainloop);
        return;
    }

    pa_threaded_mainloop_unlock(m_mainloop);
    LOG_INFO("PulseAudio capture started");
}

void PulseAudioCapture::stop() {
    if (!m_running.exchange(false)) {
        return;
    }

    pa_threaded_mainloop_lock(m_mainloop);

    if (m_stream) {
        pa_stream_disconnect(m_stream);
    }

    pa_threaded_mainloop_unlock(m_mainloop);
    LOG_INFO("PulseAudio capture stopped");
}

std::vector<AudioDevice> PulseAudioCapture::getAvailableDevices() const {
    return std::vector<AudioDevice>();
}

bool PulseAudioCapture::setDevice(const std::string& deviceId) {
    bool wasRunning = m_running.load();
    if (wasRunning) stop();

    m_currentDeviceId = deviceId;

    if (wasRunning) start();

    return true;
}

} // namespace SoundBoost

#endif // PLATFORM_LINUX
