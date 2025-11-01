# TwitchMod Setup Guide

## Step 1: Register Your Twitch Application

1. Go to https://dev.twitch.tv/console/apps
2. Log in with your Twitch account
3. Click **"Register Your Application"**
4. Fill in the application details:
   - **Name**: `TwitchMod` (or any name you prefer)
   - **OAuth Redirect URLs**: `http://localhost:8080/callback`
   - **Category**: `Application Integration`
5. Click **"Create"**
6. After creation, click **"Manage"** on your new application
7. Copy your **Client ID**
8. Click **"New Secret"** and copy your **Client Secret**

⚠️ **IMPORTANT**: Keep your Client Secret private! Never share it or commit it to version control.

## Step 2: Configure Environment Variables

TwitchMod reads your Twitch credentials from environment variables for security.

### macOS / Linux

Add these lines to your `~/.zshrc` or `~/.bashrc`:

```bash
export TWITCH_CLIENT_ID="your_client_id_here"
export TWITCH_CLIENT_SECRET="your_client_secret_here"
```

Then reload your shell:
```bash
source ~/.zshrc  # or source ~/.bashrc
```

**To run the app with environment variables:**
```bash
cd build
TWITCH_CLIENT_ID="your_client_id" TWITCH_CLIENT_SECRET="your_client_secret" ./TwitchMod.app/Contents/MacOS/TwitchMod
```

### Windows

**PowerShell:**
```powershell
$env:TWITCH_CLIENT_ID="your_client_id_here"
$env:TWITCH_CLIENT_SECRET="your_client_secret_here"
```

**Command Prompt:**
```cmd
set TWITCH_CLIENT_ID=your_client_id_here
set TWITCH_CLIENT_SECRET=your_client_secret_here
```

**Permanent (System Environment Variables):**
1. Open Start Menu → Search "Environment Variables"
2. Click "Edit the system environment variables"
3. Click "Environment Variables..."
4. Under "User variables" click "New..."
5. Add `TWITCH_CLIENT_ID` with your value
6. Add `TWITCH_CLIENT_SECRET` with your value
7. Click "OK" to save

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
3. Your browser will open with Twitch's authorization page
4. Click **"Authorize"** to grant permissions
5. The browser will redirect to a success page
6. Return to TwitchMod - you're now connected!

## Troubleshooting

### "TWITCH_CLIENT_ID environment variable not set"

Make sure you've set the environment variables correctly:
- On macOS/Linux: Run `echo $TWITCH_CLIENT_ID` to verify
- On Windows: Run `echo %TWITCH_CLIENT_ID%` (cmd) or `$env:TWITCH_CLIENT_ID` (PowerShell)

If empty, follow Step 2 again and restart your terminal/shell.

### "status":400,"message":"invalid client"

This error means:
- Your CLIENT_ID or CLIENT_SECRET is incorrect
- You haven't set the environment variables
- The credentials are for a different Twitch app

Double-check your credentials at https://dev.twitch.tv/console/apps

### "Failed to start OAuth callback server on port 8080"

Port 8080 is already in use. Either:
- Stop the application using port 8080
- Or change the redirect URI in your Twitch app settings to use a different port

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
