# SoundBoost Pro - Frequently Asked Questions

## General Questions

### What is SoundBoost Pro?

SoundBoost Pro is a professional-grade audio enhancement application that provides real-time audio processing with machine learning capabilities. It can enhance any audio playing on your system with effects like equalization, bass boost, compression, and more.

### Is SoundBoost Pro free?

SoundBoost Pro offers both a free version with basic features and a Pro version with advanced features including ML-powered analysis, additional effects, and priority support.

### Which platforms are supported?

SoundBoost Pro supports:
- Windows 10 and later
- macOS 10.15 (Catalina) and later
- Linux (Ubuntu 20.04+, Fedora 34+, Arch Linux)
- Android 8.0 and later
- iOS 14 and later

## Audio Questions

### How low is the latency?

SoundBoost Pro achieves ultra-low latency:
- **Minimum**: ~3ms (with 128 sample buffer at 48kHz)
- **Recommended**: ~10ms (with 512 sample buffer at 48kHz)
- **Stable**: ~20ms (with 1024 sample buffer at 48kHz)

### Will this work with Spotify/YouTube/Netflix?

Yes! SoundBoost Pro processes all system audio, so it works with any application that produces sound.

### Can I use SoundBoost Pro for recording?

SoundBoost Pro is designed for playback enhancement. For recording, you would need to route the processed audio through a virtual audio cable to your recording software.

### Does it work with Bluetooth headphones?

Yes, SoundBoost Pro works with any audio output device including Bluetooth headphones. Note that Bluetooth adds its own latency.

### Can I use it with a USB audio interface?

Absolutely! SoundBoost Pro supports all audio devices that your operating system recognizes.

## Effects Questions

### What's the difference between Bass Booster and EQ?

- **Bass Booster**: Specialized effect focused on low frequencies with harmonic enhancement and sub-bass synthesis
- **Equalizer**: Full control over all frequency bands (31Hz - 16kHz)

Use Bass Booster for powerful, warm bass. Use EQ for precise frequency adjustments.

### What does the Compressor do?

A compressor reduces the dynamic range of audio - making loud sounds quieter and quiet sounds louder. This results in:
- More consistent volume
- Enhanced detail in quiet passages
- Protection against sudden loud sounds

### When should I use the Stereo Widener?

Stereo Widener enhances the stereo image, making audio sound wider and more spacious. Great for:
- Music with narrow stereo imaging
- Gaming (enhanced spatial awareness)
- Movies (more immersive experience)

Avoid using it with mono content or if you experience phase issues.

### What does the Limiter do?

The Limiter prevents audio from exceeding a certain level (ceiling), preventing distortion and clipping. It's always active as the last effect in the chain to protect your speakers and ears.

## ML Features Questions

### What are ML features?

Machine Learning features use trained neural networks to:
- Detect the genre of music you're listening to
- Identify beats and calculate BPM
- Detect voice vs. instrument content
- Suggest optimal EQ settings

### Do ML features require internet?

No! All ML processing happens locally on your device. No data is sent to the cloud.

### How accurate is genre detection?

Genre detection is approximately 85-90% accurate for clear, well-produced music. Accuracy may vary with:
- Mixed genres
- Low-quality audio
- Unusual or experimental music

### Does ML processing affect performance?

ML processing runs on a separate thread and is optimized for minimal CPU usage. Typical impact is <2% CPU usage. You can disable ML features if needed.

### What if I don't want ML features?

ML features are optional. You can disable them in Settings > ML. The app works perfectly without them.

## Profile Questions

### How do I save my settings?

1. Configure your effects as desired
2. Click the "Save Profile" button
3. Enter a name for your profile
4. Click "Save"

### Can I share profiles with friends?

Yes! Export profiles as JSON files and share them:
1. Right-click on a profile
2. Select "Export"
3. Send the JSON file
4. Your friend can import it

### How many profiles can I create?

There's no limit to the number of profiles you can create.

### What are the default profiles?

- **Flat**: No enhancement (reference)
- **Bass Heavy**: Enhanced bass for bass lovers
- **Vocal Boost**: Enhanced vocals for podcasts and vocal music
- **Gaming**: Optimized for gaming audio
- **Movies**: Cinematic sound profile

## Troubleshooting

### No sound after enabling SoundBoost Pro

1. Check that the correct output device is selected
2. Verify Master Volume is not at zero
3. Try clicking the Bypass button
4. Restart the application
5. Check your system's audio settings

### High CPU usage

1. Increase buffer size in Settings > Audio
2. Disable unused effects
3. Disable ML features
4. Close other audio applications

### Crackling or popping sounds

1. Increase buffer size (most common fix)
2. Close other audio applications
3. Update audio drivers
4. Check for background processes using audio

### Application won't start

**Windows**:
- Run as Administrator
- Check Windows Defender exclusions
- Reinstall Visual C++ Runtime

**macOS**:
- Right-click > Open (first time)
- Check Gatekeeper settings

**Linux**:
- Check dependencies: `ldd SoundBoostPro`
- Install missing libraries

### Settings not saving

1. Check write permissions to config directory
2. Run as Administrator once to set permissions
3. Check disk space

## Privacy & Security

### Does SoundBoost Pro collect data?

SoundBoost Pro respects your privacy:
- **No audio data** is collected or transmitted
- **No personal information** is required
- **Optional telemetry** can be disabled in settings

### Is it safe to use?

Yes! SoundBoost Pro:
- Is open source (core components)
- Has been security audited
- Contains no malware or spyware
- Doesn't modify system files

### Where are my settings stored?

- **Windows**: `%APPDATA%\SoundBoostPro\config.json`
- **macOS**: `~/Library/Application Support/SoundBoostPro/config.json`
- **Linux**: `~/.config/soundboostpro/config.json`

## Performance

### What are the system requirements?

**Minimum**:
- Dual-core 2.0 GHz CPU
- 4 GB RAM
- Any audio device

**Recommended**:
- Quad-core 3.0 GHz+ CPU with AVX2/NEON
- 8 GB RAM
- Low-latency audio interface

### Does it work with gaming anti-cheat software?

SoundBoost Pro operates at the audio driver level and doesn't interact with games directly. It should work with all anti-cheat software, but if you experience issues, try running the game in windowed mode or contact support.

### Can I use it while streaming?

Yes! SoundBoost Pro processes your output audio. Your stream will capture the enhanced audio if you're capturing system audio.

## Licensing

### Can I use SoundBoost Pro commercially?

The Pro version includes a commercial license. The free version is for personal use only.

### Is the source code available?

Core components are open source under the MIT license. Some advanced features are proprietary.

### Can I modify and redistribute?

Open source components can be modified and redistributed under the MIT license. Proprietary components cannot be redistributed.

## Support

### How do I contact support?

- **Email**: support@soundboostpro.com
- **GitHub Issues**: https://github.com/soundboostpro/issues
- **Discord**: https://discord.gg/soundboostpro

### What information should I include in a support request?

1. Operating system and version
2. SoundBoost Pro version
3. Audio device model
4. Description of the issue
5. Steps to reproduce
6. Screenshots (if applicable)
7. Log file (Settings > Advanced > Export Logs)

### How quickly will I get a response?

- **Pro users**: Within 24 hours
- **Free users**: Within 72 hours
- **Critical bugs**: ASAP

## Updates

### How often is SoundBoost Pro updated?

- **Major updates**: Quarterly
- **Minor updates**: Monthly
- **Bug fixes**: As needed

### How do I update?

- **Automatic**: Enable in Settings > General
- **Manual**: Download and install the latest version

### Will I lose my settings when updating?

No! Your settings and profiles are preserved during updates.

## More Questions?

Visit our documentation at https://docs.soundboostpro.com or contact support.
