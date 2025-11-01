# TwitchMod - Professional Twitch Moderator Client

[![Build Status](https://github.com/miwidot/twitchmod/workflows/Build%20TwitchMod/badge.svg)](https://github.com/miwidot/twitchmod/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A professional, cross-platform Twitch moderator client with mIRC-style interface built with Qt6 and C++.

![TwitchMod Screenshot](docs/screenshot.png)

## 🎯 Features

- ✅ **mIRC-Style Interface** - Classic 3-panel layout (channels, chat, users)
- ✅ **Full OAuth Integration** - Secure Twitch authentication
- ✅ **Multi-Channel Support** - Moderate multiple channels simultaneously
- ✅ **Complete Mod Actions** - Ban, timeout, delete messages
- ✅ **AutoMod Management** - Review and manage AutoMod queue
- ✅ **Predictions & Polls** - Create and manage predictions/polls
- ✅ **User Management** - View users with mod/VIP badges
- ✅ **Context Menu Actions** - Right-click for quick mod actions
- ✅ **Cross-Platform** - Windows and macOS support

## 📦 Download

**Latest Release:** [Download here](https://github.com/miwidot/twitchmod/releases/latest)

- **Windows:** `TwitchMod-Windows.zip`
- **macOS:** `TwitchMod-macOS.zip`

## 🚀 Quick Start

### 1. Download and Install

#### Windows
1. Download `TwitchMod-Windows.zip`
2. Extract to a folder
3. Run `TwitchMod.exe`

#### macOS
1. Download `TwitchMod-macOS.zip`
2. Extract and drag `TwitchMod.app` to Applications
3. Right-click and select "Open" (first time only due to unsigned app)

### 2. Setup Twitch App

Before first use, you need to create a Twitch application:

1. Go to https://dev.twitch.tv/console/apps
2. Click "Register Your Application"
3. Fill in:
   - **Name**: TwitchMod (or your preferred name)
   - **OAuth Redirect URLs**: `http://localhost:8080/callback`
   - **Category**: Chat Bot
4. Click "Create"
5. Copy your **Client ID** and **Client Secret**

### 3. Configure TwitchMod

**Option A: Environment Variables (Recommended)**
```bash
# macOS/Linux
export TWITCH_CLIENT_ID="your_client_id_here"
export TWITCH_CLIENT_SECRET="your_client_secret_here"

# Windows (PowerShell)
$env:TWITCH_CLIENT_ID="your_client_id_here"
$env:TWITCH_CLIENT_SECRET="your_client_secret_here"
```

**Option B: Rebuild from Source**
Edit `src/twitch/twitchauth.h` and replace:
```cpp
static constexpr const char* CLIENT_ID = "YOUR_CLIENT_ID_HERE";
static constexpr const char* CLIENT_SECRET = "YOUR_CLIENT_SECRET_HERE";
```

### 4. Connect and Moderate!

1. Launch TwitchMod
2. Click **File > Connect to Twitch**
3. Authenticate in browser
4. Start moderating!

## 🛠️ Building from Source

### Prerequisites

- **CMake** 3.16+
- **Qt6** 6.5+
- **C++17** compiler (MSVC 2019+, Clang, GCC)

### macOS

```bash
# Install dependencies
brew install qt@6 cmake

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release

# Deploy
macdeployqt TwitchMod.app -dmg
codesign --force --deep --sign - TwitchMod.app
```

### Windows

```cmd
# Install Qt6 and Visual Studio 2022 first

# Build
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

# Deploy
cd Release
windeployqt TwitchMod.exe --release --no-translations
```

### Automated Builds

The project uses GitHub Actions for automatic builds on every push:
- 🍎 macOS builds on `macos-latest`
- 🪟 Windows builds on `windows-latest`
- 📦 Automatic releases on version tags

## 📖 Usage

### Moderating Channels

1. Your mod channels appear in the left panel under "Moderating"
2. Click a channel to open chat
3. Right-click users for mod actions:
   - **Ban User** - Permanent ban
   - **Timeout** - 1min, 5min, 10min, 30min options
   - **Delete Messages** - Remove recent messages
   - **View User Info** - See user details

### Creating Predictions

1. Click **Mods > Create Prediction**
2. Enter title and outcomes (2-10 options)
3. Set duration (15s - 1800s)
4. Manage from active predictions panel

### Creating Polls

1. Click **Mods > Create Poll**
2. Enter question and choices (2-5 options)
3. Set duration (15s - 1800s)
4. End poll when ready

## 🔑 Required Twitch Scopes

TwitchMod requests the following scopes:

**Chat:**
- `chat:read`, `chat:edit`

**Moderation:**
- `moderator:manage:banned_users`, `moderator:read:banned_users`
- `moderator:manage:chat_messages`, `moderator:read:chat_messages`
- `moderator:manage:chat_settings`, `moderator:read:chat_settings`
- `moderator:read:chatters`

**AutoMod:**
- `moderator:manage:automod`, `moderator:manage:automod_settings`
- `moderator:manage:blocked_terms`, `moderator:read:blocked_terms`

**Predictions & Polls:**
- `channel:manage:predictions`, `channel:read:predictions`
- `channel:manage:polls`, `channel:read:polls`

**Other:**
- `moderator:read:followers`, `moderator:manage:warnings`
- `moderator:manage:shield_mode`, `moderator:manage:announcements`

See [full scope list](README.md#twitch-api-scopes) in README.

## 🤝 Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Credits

- **Qt6** - Cross-platform GUI framework
- **Twitch API** - Channel moderation API
- **Inspired by:** mIRC (interface), Chatterino (Twitch chat client)

## 📧 Support

- **Issues:** [GitHub Issues](https://github.com/miwidot/twitchmod/issues)
- **Discussions:** [GitHub Discussions](https://github.com/miwidot/twitchmod/discussions)

## 🗺️ Roadmap

- [ ] WebSocket EventSub integration (live chat)
- [ ] AutoMod queue panel
- [ ] User info panel with history
- [ ] Custom themes and colors
- [ ] Keyboard shortcuts
- [ ] Settings dialog
- [ ] Channel points management
- [ ] Stream notifications
- [ ] Multi-language support

---

**Built with ❤️ by the TwitchMod team**
