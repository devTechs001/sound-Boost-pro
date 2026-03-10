// src/platform/windows/WindowsAudioCapture.cpp
#ifdef PLATFORM_WINDOWS

#include "WindowsAudioCapture.hpp"
#include "../../utils/Logger.hpp"

#include <mmdeviceapi.h>
#include <audioclient.h>
#include <avrt.h>

#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "avrt.lib")

namespace SoundBoost {

WindowsAudioCapture::WindowsAudioCapture() {
    // Initialize COM
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
}

WindowsAudioCapture::~WindowsAudioCapture() {
    stop();
    CoUninitialize();
}

bool WindowsAudioCapture::initialize(int sampleRate, int bufferSize, int channels) {
    m_sampleRate = sampleRate;
    m_bufferSize = bufferSize;
    m_channels = channels;

    // Create device enumerator
    IMMDeviceEnumerator* deviceEnumerator = nullptr;
    HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(IMMDeviceEnumerator),
        reinterpret_cast<void**>(&deviceEnumerator)
    );

    if (FAILED(hr)) {
        LOG_ERROR("Failed to create device enumerator: 0x{:08X}", hr);
        return false;
    }

    // Get default capture device
    IMMDevice* defaultDevice = nullptr;
    hr = deviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &defaultDevice);

    if (FAILED(hr)) {
        LOG_ERROR("Failed to get default capture device: 0x{:08X}", hr);
        deviceEnumerator->Release();
        return false;
    }

    // Activate audio client
    IAudioClient* audioClient = nullptr;
    hr = defaultDevice->Activate(__uuidof(IAudioClient), CLSCTX_INPROC_SERVER, nullptr, reinterpret_cast<void**>(&audioClient));

    if (FAILED(hr)) {
        LOG_ERROR("Failed to activate audio client: 0x{:08X}", hr);
        defaultDevice->Release();
        deviceEnumerator->Release();
        return false;
    }

    // Get mix format
    WAVEFORMATEX* mixFormat = nullptr;
    hr = audioClient->GetMixFormat(&mixFormat);

    if (FAILED(hr)) {
        LOG_ERROR("Failed to get mix format: 0x{:08X}", hr);
        audioClient->Release();
        defaultDevice->Release();
        deviceEnumerator->Release();
        return false;
    }

    // Initialize audio client
    REFERENCE_TIME hnsDefaultPeriod = 0;
    REFERENCE_TIME hnsMinimumPeriod = 0;
    hr = audioClient->GetDevicePeriod(&hnsDefaultPeriod, &hnsMinimumPeriod);

    if (FAILED(hr)) {
        LOG_ERROR("Failed to get device period: 0x{:08X}", hr);
        CoTaskMemFree(mixFormat);
        audioClient->Release();
        defaultDevice->Release();
        deviceEnumerator->Release();
        return false;
    }

    // Use shared mode
    WAVEFORMATEX* closestMatch = nullptr;
    hr = audioClient->IsFormatSupported(
        AUDCLNT_SHAREMODE_SHARED,
        mixFormat,
        &closestMatch
    );

    if (hr == S_FALSE && closestMatch) {
        CoTaskMemFree(mixFormat);
        mixFormat = closestMatch;
    }

    REFERENCE_TIME hnsBufferDuration = hnsDefaultPeriod;
    hr = audioClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        0,
        hnsBufferDuration,
        0,
        mixFormat,
        nullptr
    );

    if (FAILED(hr)) {
        LOG_ERROR("Failed to initialize audio client: 0x{:08X}", hr);
        CoTaskMemFree(mixFormat);
        audioClient->Release();
        defaultDevice->Release();
        deviceEnumerator->Release();
        return false;
    }

    CoTaskMemFree(mixFormat);

    // Get capture client
    IAudioCaptureClient* captureClient = nullptr;
    hr = audioClient->GetService(__uuidof(IAudioCaptureClient), reinterpret_cast<void**>(&captureClient));

    if (FAILED(hr)) {
        LOG_ERROR("Failed to get capture client: 0x{:08X}", hr);
        audioClient->Release();
        defaultDevice->Release();
        deviceEnumerator->Release();
        return false;
    }

    // Store for later use
    m_audioClient = audioClient;
    m_captureClient = captureClient;
    m_device = defaultDevice;
    m_deviceEnumerator = deviceEnumerator;

    m_initialized = true;
    LOG_INFO("Windows Audio Capture initialized: {}Hz, {} channels", sampleRate, channels);
    return true;
}

void WindowsAudioCapture::start() {
    if (m_running.exchange(true)) {
        return;
    }

    HRESULT hr = m_audioClient->Start();
    if (FAILED(hr)) {
        LOG_ERROR("Failed to start audio client: 0x{:08X}", hr);
        m_running.store(false);
        return;
    }

    LOG_INFO("Windows Audio Capture started");
}

void WindowsAudioCapture::stop() {
    if (!m_running.exchange(false)) {
        return;
    }

    if (m_audioClient) {
        m_audioClient->Stop();
    }

    LOG_INFO("Windows Audio Capture stopped");
}

std::vector<AudioDevice> WindowsAudioCapture::getAvailableDevices() const {
    std::vector<AudioDevice> devices;
    // Implementation for device enumeration
    return devices;
}

bool WindowsAudioCapture::setDevice(const std::string& deviceId) {
    // Implementation for device switching
    return true;
}

} // namespace SoundBoost

#endif // PLATFORM_WINDOWS
