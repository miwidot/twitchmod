#!/bin/bash

# TwitchMod Launcher Script
# This script helps you run TwitchMod with environment variables

# Check if environment variable is set
if [ -z "$TWITCH_CLIENT_ID" ]; then
    echo "❌ Error: TWITCH_CLIENT_ID environment variable is not set"
    echo ""
    echo "Please set your Twitch Client ID:"
    echo "  export TWITCH_CLIENT_ID=\"your_client_id_here\""
    echo ""
    echo "Or run this script with Client ID:"
    echo "  TWITCH_CLIENT_ID=\"xxx\" ./run-twitchmod.sh"
    echo ""
    echo "✅ No Client Secret needed! TwitchMod uses Implicit Grant Flow."
    echo ""
    echo "See SETUP.md for complete instructions."
    exit 1
fi

echo "✅ Client ID configured"
echo "   Client ID: ${TWITCH_CLIENT_ID:0:10}..."
echo "   Auth Method: Implicit Grant (no secret required!)"
echo ""

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "❌ Error: build directory not found"
    echo "Please build TwitchMod first:"
    echo "  ./build-scripts/build-macos.sh"
    exit 1
fi

# Check if app exists
if [ ! -d "build/TwitchMod.app" ]; then
    echo "❌ Error: TwitchMod.app not found in build directory"
    echo "Please build TwitchMod first:"
    echo "  ./build-scripts/build-macos.sh"
    exit 1
fi

echo "🚀 Launching TwitchMod..."
echo ""

# Launch the app
cd build
open TwitchMod.app

# Alternative: Run directly (shows console output)
# ./TwitchMod.app/Contents/MacOS/TwitchMod
