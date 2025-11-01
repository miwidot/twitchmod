cmake_minimum_required(VERSION 3.16)

project(TwitchMod VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Network
    WebSockets
)

# Source files
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/chatwidget.cpp
    src/channellist.cpp
    src/userlist.cpp
    src/twitch/twitchapi.cpp
    src/twitch/twitchauth.cpp
    src/twitch/twitchwebsocket.cpp
    src/twitch/oauthserver.cpp
)

# Header files
set(HEADERS
    src/mainwindow.h
    src/chatwidget.h
    src/channellist.h
    src/userlist.h
    src/twitch/twitchapi.h
    src/twitch/twitchauth.h
    src/twitch/twitchwebsocket.h
    src/twitch/oauthserver.h
)

# Create executable
add_executable(TwitchMod ${SOURCES} ${HEADERS})

# Platform-specific settings
if(WIN32)
    # Windows: No console window, use .exe
    set_target_properties(TwitchMod PROPERTIES
        WIN32_EXECUTABLE ON
    )
    # Windows icon (if you add one later)
    # target_sources(TwitchMod PRIVATE resources/app.rc)
elseif(APPLE)
    # macOS: Create .app bundle
    set_target_properties(TwitchMod PROPERTIES
        MACOSX_BUNDLE ON
        MACOSX_BUNDLE_BUNDLE_NAME "TwitchMod"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.twitchmod.app"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    )
    # macOS icon (if you add one later)
    # set(MACOSX_BUNDLE_ICON_FILE icon.icns)
endif()

# Link Qt libraries
target_link_libraries(TwitchMod PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
    Qt6::WebSockets
)

# Include directories
target_include_directories(TwitchMod PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Installation
install(TARGETS TwitchMod
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)

# Deploy Qt dependencies (windeployqt/macdeployqt will be called manually)
message(STATUS "")
message(STATUS "========================================")
message(STATUS "TwitchMod Configuration:")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Platform: ${CMAKE_SYSTEM_NAME}")
message(STATUS "========================================")
message(STATUS "")
