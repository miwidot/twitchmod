# TwitchMod Setup Guide

## Step 1: Register Your Twitch Application

1. Go to https://dev.twitch.tv/console/apps
2. Log in with your Twitch account
3. Click **"Register Your Application"**
4. Fill in the application details:
   - **Name**: `TwitchMod` (or any name you prefer)
   - **OAuth Redirect URLs**: `http://localhost` (not used but required field)
   - **Category**: `Application Integration`
5. Click **"Create"**
6. After creation, click **"Manage"** on your new application
7. Copy your **Client ID** - that's all you need!

✅ **No Client Secret needed!** TwitchMod uses **Device Code Grant Flow** - the cleanest OAuth method for desktop apps!

## Step 2: Configure Client ID

⚠️ **IMPORTANT**: TwitchMod uses **Device Code Grant Flow** - you only need the **Client ID**, NO Client Secret required!

### macOS / Linux

Add this line to your `~/.zshrc` or `~/.bashrc`:

```bash
export TWITCH_CLIENT_ID="your_client_id_here"
```

Then reload your shell:
```bash
source ~/.zshrc  # or source ~/.bashrc
```

**To run the app with environment variable:**
```bash
cd build
TWITCH_CLIENT_ID="your_client_id" ./TwitchMod.app/Contents/MacOS/TwitchMod
```

### Windows

**PowerShell:**
```powershell
$env:TWITCH_CLIENT_ID="your_client_id_here"
```

**Command Prompt:**
```cmd
set TWITCH_CLIENT_ID=your_client_id_here
```

**Permanent (System Environment Variables):**
1. Open Start Menu → Search "Environment Variables"
2. Click "Edit the system environment variables"
3. Click "Environment Variables..."
4. Under "User variables" click "New..."
5. Add `TWITCH_CLIENT_ID` with your value
6. Click "OK" to save

## Step 3: Build TwitchMod

### macOS

```bash
./build-scripts/build-macos.sh
```

### Windows

```powershell
.\build-scripts\build-windows.bat
```

Or use CMake manually:
```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

## Step 4: Run TwitchMod

### macOS

```bash
cd build
./TwitchMod.app/Contents/MacOS/TwitchMod
```

Or double-click `TwitchMod.app` in the `build` folder.

### Windows

```cmd
cd build\Release
TwitchMod.exe
```

## Step 5: Authenticate with Twitch

1. Launch TwitchMod
2. Click **File → Connect to Twitch**
3. A dialog will appear with:
   - **Link to twitch.tv/activate**
   - **Activation code** (e.g., ABCD-EFGH)
4. Your browser opens automatically to twitch.tv/activate
5. **Enter the activation code** shown in the dialog
6. Click **"Authorize"** to grant permissions
7. Return to TwitchMod - you're now connected!

✨ **That's it!** No localhost server, no redirects - just enter the code on Twitch's website!

## Troubleshooting

### "TWITCH_CLIENT_ID environment variable not set"

Make sure you've set the environment variables correctly:
- On macOS/Linux: Run `echo $TWITCH_CLIENT_ID` to verify
- On Windows: Run `echo %TWITCH_CLIENT_ID%` (cmd) or `$env:TWITCH_CLIENT_ID` (PowerShell)

If empty, follow Step 2 again and restart your terminal/shell.

### "status":400,"message":"invalid client"

This error means:
- Your CLIENT_ID is incorrect
- You haven't set the TWITCH_CLIENT_ID environment variable
- The Client ID is for a different Twitch app

Double-check your Client ID at https://dev.twitch.tv/console/apps

⚠️ **Note**: With Device Code Grant Flow, you DON'T need a Client Secret! Only the Client ID is required.

### Device code expired

If you wait too long (usually 15 minutes) before entering the code, it expires. Just:
- Click "File → Connect to Twitch" again
- A new code will be generated

### Browser doesn't open

Check that your default browser is set correctly. You can manually open the OAuth URL from the terminal output.

## Required Scopes

TwitchMod requests these Twitch scopes for full moderator functionality:

**Chat:**
- `chat:read`, `chat:edit`

**Moderation:**
- `moderator:manage:banned_users`
- `moderator:read:banned_users`
- `moderator:manage:chat_messages`
- `moderator:read:chat_messages`
- `moderator:manage:chat_settings`
- `moderator:read:chatters`

**AutoMod:**
- `moderator:manage:automod`
- `moderator:read:automod_settings`
- `moderator:manage:automod_settings`
- `moderator:manage:blocked_terms`
- `moderator:read:blocked_terms`

**Predictions & Polls:**
- `channel:manage:predictions`
- `channel:read:predictions`
- `channel:manage:polls`
- `channel:read:polls`

**Other:**
- `user:read:email`
- `moderator:read:followers`
- `moderator:read:moderators`
- `moderator:read:vips`
- `moderator:manage:warnings`
- `moderator:read:warnings`
- `moderator:manage:unban_requests`
- `moderator:read:unban_requests`
- `moderator:read:shield_mode`
- `moderator:manage:shield_mode`
- `moderator:manage:announcements`
- `moderator:read:shoutouts`
- `moderator:manage:shoutouts`
