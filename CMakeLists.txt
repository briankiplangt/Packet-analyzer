cmake_minimum_required(VERSION 3.16)
project(PacketAnalyzer2026)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ✅ ROBUST Qt6 CONFIGURATION - Works with Visual Studio consistently
# Set Qt6 installation path - this will work from any location
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../6.5.3/msvc2019_64")
    # Local Qt installation found (one level up)
    set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../6.5.3/msvc2019_64")
    message(STATUS "Using local Qt 6.5.3 installation: ${CMAKE_PREFIX_PATH}")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/6.5.3/msvc2019_64")
    # Local Qt installation found (same level)
    set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/6.5.3/msvc2019_64")
    message(STATUS "Using local Qt 6.5.3 installation: ${CMAKE_PREFIX_PATH}")
elseif(DEFINED ENV{Qt6_DIR})
    # Use environment variable if set
    set(CMAKE_PREFIX_PATH "$ENV{Qt6_DIR}")
    message(STATUS "Using Qt6_DIR environment variable: ${CMAKE_PREFIX_PATH}")
elseif(EXISTS "C:/Qt/6.5.3/msvc2019_64")
    # Fallback to standard Qt installation
    set(CMAKE_PREFIX_PATH "C:/Qt/6.5.3/msvc2019_64")
    message(STATUS "Using standard Qt installation: ${CMAKE_PREFIX_PATH}")
else()
    message(FATAL_ERROR "Qt 6.5.3 not found! Please ensure Qt is installed or set Qt6_DIR environment variable")
endif()

# Additional Qt paths for Visual Studio integration
list(APPEND CMAKE_PREFIX_PATH 
    "${CMAKE_PREFIX_PATH}/lib/cmake"
    "${CMAKE_PREFIX_PATH}/lib/cmake/Qt6"
)

# Find Qt6 with better error handling
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Qml Quick QuickControls2 Sql Network)

# Verify Qt version
if(Qt6_VERSION VERSION_LESS "6.5.0")
    message(WARNING "Qt version ${Qt6_VERSION} found, but 6.5.3 is recommended")
endif()

message(STATUS "Qt6 found: ${Qt6_VERSION} at ${Qt6_DIR}")

# Enable Qt MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Source files
set(SOURCES
    main.cpp
    src/database/DatabaseManager.cpp
    src/core/PacketCaptureEngine.cpp
    src/models/PacketAnalyzerModel.cpp
)

# Header files with Q_OBJECT (for MOC)
set(HEADERS
    src/database/DatabaseManager.h
    src/core/PacketCaptureEngine.h
    src/models/PacketAnalyzerModel.h
)

# QML resources
set(RESOURCES
    resources.qrc
)

# Create executable
add_executable(PacketAnalyzer2026 ${SOURCES} ${HEADERS} ${RESOURCES})

# Link Qt libraries
target_link_libraries(PacketAnalyzer2026 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::Qml 
    Qt6::Quick 
    Qt6::QuickControls2 
    Qt6::Sql 
    Qt6::Network
)

# Windows-specific libraries for packet capture
if(WIN32)
    target_link_libraries(PacketAnalyzer2026 ws2_32 iphlpapi)
    
    # ✅ FIX WIN32_LEAN_AND_MEAN redefinition for Visual Studio
    target_compile_definitions(PacketAnalyzer2026 PRIVATE WIN32_LEAN_AND_MEAN)
endif()

# ✅ VISUAL STUDIO INTEGRATION - Ensure proper Qt deployment
if(MSVC)
    # Set Visual Studio startup project
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT PacketAnalyzer2026)
    
    # Add Qt DLL deployment for debugging in Visual Studio
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_CONFIGURATION_TYPES)
        # Find windeployqt
        find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS ${Qt6_DIR}/../../../bin)
        
        if(WINDEPLOYQT_EXECUTABLE)
            # Add post-build step to deploy Qt DLLs
            add_custom_command(TARGET PacketAnalyzer2026 POST_BUILD
                COMMAND ${WINDEPLOYQT_EXECUTABLE} --debug --qmldir ${CMAKE_SOURCE_DIR} $<TARGET_FILE:PacketAnalyzer2026>
                COMMENT "Deploying Qt libraries for Visual Studio debugging"
            )
            message(STATUS "windeployqt found: ${WINDEPLOYQT_EXECUTABLE}")
        else()
            message(WARNING "windeployqt not found - manual Qt DLL deployment may be required")
        endif()
    endif()
endif()

# Set target properties
set_target_properties(PacketAnalyzer2026 PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

# Set output directory
set_target_properties(PacketAnalyzer2026 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)