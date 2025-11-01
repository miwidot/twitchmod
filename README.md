# TwitchMod - Twitch Moderator Client

A professional, cross-platform Twitch moderator client with mIRC-style interface built with Qt6 and C++.

## Features

### Current (v1.0.0)
- ✅ **mIRC-Style Interface** - Classic 3-panel layout (channels, chat, users)
- ✅ **Multi-Channel Support** - Moderate multiple channels simultaneously
- ✅ **Channel Management** - Organized view of channels where you're a moderator
- ✅ **User List** - See all users in channel with mod/VIP badges
- ✅ **Context Menu Actions** - Right-click users for quick mod actions
- ✅ **Cross-Platform** - Windows (.exe) and macOS (.app)

### Planned Features
- 🔄 **Full Mod Actions** - Ban, timeout, delete messages
- 🔄 **AutoMod Queue** - Review and manage AutoMod flagged messages
- 🔄 **Predictions & Polls** - Create and manage predictions/polls
- 🔄 **Chat Settings** - Slow mode, follower-only, sub-only modes
- 🔄 **User Info Panel** - View user history, previous bans, account age
- 🔄 **Mod Logs** - Track all moderation actions
- 🔄 **Custom Themes** - Dark/Light themes and custom colors
- 🔄 **Keyboard Shortcuts** - Quick actions via hotkeys

## Prerequisites

### Windows
- Windows 10/11 (64-bit)
- Visual Studio 2019 or newer (with C++ desktop development)
- Qt 6.5 or newer
- CMake 3.16 or newer

### macOS
- macOS 11 (Big Sur) or newer
- Xcode 13 or newer
- Qt 6.5 or newer
- CMake 3.16 or newer

## Installation

### Installing Qt6

#### Windows
1. Download Qt Online Installer from https://www.qt.io/download-open-source
2. Install Qt 6.x.x for MSVC 2019 64-bit
3. Add Qt bin directory to PATH:
   ```
   C:\Qt\6.x.x\msvc2019_64\bin
   ```

#### macOS
**Option 1: Official Installer**
1. Download Qt Online Installer from https://www.qt.io/download-open-source
2. Install Qt 6.x.x for macOS

**Option 2: Homebrew**
```bash
brew install qt@6
```

### Installing CMake

#### Windows
Download and install from https://cmake.org/download/

#### macOS
```bash
brew install cmake
```

## Building from Source

### Windows

1. Open Command Prompt or PowerShell
2. Navigate to project directory:
   ```cmd
   cd C:\path\to\twitchmod
   ```
3. Run build script:
   ```cmd
   build-scripts\build-windows.bat
   ```
4. Find executable in `build-windows\Release\TwitchMod.exe`

**Manual Build:**
```cmd
mkdir build-windows
cd build-windows
cmake .. -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
cd Release
windeployqt.exe TwitchMod.exe --release --no-translations
```

### macOS

1. Open Terminal
2. Navigate to project directory:
   ```bash
   cd /path/to/twitchmod
   ```
3. Run build script:
   ```bash
   ./build-scripts/build-macos.sh
   ```
4. Find app bundle in `build-macos/TwitchMod.app`

**Manual Build:**
```bash
mkdir build-macos
cd build-macos
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
macdeployqt TwitchMod.app -dmg
```

## Setup & Configuration

✅ **NO SETUP REQUIRED!** TwitchMod comes with a built-in Client ID.

Just download, build, and run! The app will work immediately.

### Quick Start (End Users)

1. **Download/Build** TwitchMod
2. **Run** the app
3. **Click** File → Connect to Twitch
4. **Enter code** at twitch.tv/activate
5. **Done!** Start moderating

✨ **That's it!** No API keys, no configuration files, no environment variables needed.

### Developer Setup (Optional)

If you're forking TwitchMod and want to use your own Client ID:

**macOS/Linux:**
```bash
export TWITCH_CLIENT_ID="your_client_id"
```

**Windows PowerShell:**
```powershell
$env:TWITCH_CLIENT_ID="your_client_id"
```

For complete instructions including troubleshooting, see **[SETUP.md](SETUP.md)**.

## Usage

### First Time Setup
1. Launch TwitchMod
2. Click **File > Connect to Twitch**
3. A dialog shows you an **activation code** and opens twitch.tv/activate
4. Enter the code on Twitch's website
5. Grant permissions for all moderator scopes
6. Return to TwitchMod - you're connected!

✨ **Super simple!** No localhost server, no port conflicts - just enter a code!

### Moderating Channels
1. Your mod channels appear in the left panel under "Moderating"
2. Click a channel to open chat
3. Right-click users for mod actions:
   - **Ban User** - Permanent ban
   - **Timeout** - 1min, 5min, 10min, 30min
   - **Delete Messages** - Remove recent messages
   - **View User Info** - See user details

### Creating Predictions
1. Click **Mods > Create Prediction**
2. Enter title and outcomes
3. Set duration (15s - 30min)
4. Manage from chat interface

### Creating Polls
1. Click **Mods > Create Poll**
2. Enter question and choices
3. Set duration (15s - 30min)
4. End poll when ready

## Project Structure

```
twitchmod/
├── src/
│   ├── main.cpp              # Entry point
│   ├── mainwindow.cpp/h      # Main window (mIRC-style layout)
│   ├── chatwidget.cpp/h      # Chat display and input
│   ├── channellist.cpp/h     # Channel tree view
│   ├── userlist.cpp/h        # User list with mod actions
│   └── twitch/
│       ├── twitchauth.cpp/h      # OAuth authentication
│       ├── twitchapi.cpp/h       # REST API wrapper
│       └── twitchwebsocket.cpp/h # EventSub WebSocket
├── build-scripts/
│   ├── build-windows.bat     # Windows build script
│   └── build-macos.sh        # macOS build script
├── resources/
│   └── icons/                # Application icons (TODO)
├── CMakeLists.txt            # CMake build configuration
└── README.md                 # This file
```

## Twitch API Scopes

TwitchMod requests the following scopes for full functionality:

**Chat:**
- `chat:read` - Read chat messages
- `chat:edit` - Send chat messages

**Moderation:**
- `moderator:manage:banned_users` - Ban/unban users
- `moderator:read:banned_users` - View ban list
- `moderator:manage:chat_messages` - Delete messages
- `moderator:read:chat_messages` - Read deleted messages
- `moderator:manage:chat_settings` - Change slow mode, etc.
- `moderator:read:chatters` - View who's in chat

**AutoMod:**
- `moderator:manage:automod` - Approve/deny AutoMod messages
- `moderator:manage:automod_settings` - Configure AutoMod
- `moderator:manage:blocked_terms` - Manage blocked terms

**Predictions & Polls:**
- `channel:manage:predictions` - Create/end predictions
- `channel:read:predictions` - View predictions
- `channel:manage:polls` - Create/end polls
- `channel:read:polls` - View polls

**Other:**
- `moderator:read:followers` - View followers
- `moderator:manage:warnings` - Warn users
- `moderator:manage:shield_mode` - Enable shield mode
- `moderator:manage:announcements` - Send announcements

## Troubleshooting

### Windows Build Issues

**Qt not found:**
```
Add Qt to PATH: C:\Qt\6.x.x\msvc2019_64\bin
```

**CMake can't find Visual Studio:**
```
Install Visual Studio 2019/2022 with C++ desktop development workload
```

**windeployqt fails:**
```
Manually add Qt6 DLLs from Qt\6.x.x\msvc2019_64\bin\
```

### macOS Build Issues

**Qt not found:**
```bash
# If installed via Homebrew:
export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"

# Or add to ~/.zshrc:
echo 'export PATH="/opt/homebrew/opt/qt@6/bin:$PATH"' >> ~/.zshrc
```

**Xcode command line tools missing:**
```bash
xcode-select --install
```

**macdeployqt fails:**
```bash
# Find Qt installation:
brew --prefix qt@6

# Add to PATH and retry:
export PATH="$(brew --prefix qt@6)/bin:$PATH"
macdeployqt build-macos/TwitchMod.app -dmg
```

## Development

### Code Style
- C++17 standard
- Qt naming conventions (camelCase for methods, PascalCase for classes)
- Use `const` and `constexpr` where possible
- Prefer smart pointers for dynamic allocation

### Adding New Features
1. Create feature branch
2. Implement in relevant widget class
3. Add Twitch API integration in `src/twitch/`
4. Update UI components
5. Test on both platforms
6. Submit pull request

## License

MIT License - See LICENSE file for details

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test on both Windows and macOS if possible
5. Submit a pull request

## Support

For issues, questions, or feature requests:
- Open an issue on GitHub
- Check existing issues first

## Credits

Built with:
- **Qt6** - Cross-platform GUI framework
- **C++17** - Programming language
- **Twitch API** - Channel moderation
- **CMake** - Build system

Inspired by:
- **mIRC** - Classic IRC client interface
- **Chatterino** - Modern Twitch chat client

---

**Version:** 1.0.0
**Status:** Beta - Core features functional, API integration in progress
**Platforms:** Windows 10/11, macOS 11+
