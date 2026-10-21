cmake_minimum_required(VERSION 3.22)

project(Aya)

########## Project Options ##########

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(CPACK_PACKAGE_VENDOR "Aya")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Aya Game")

set(CPACK_PACKAGE_VERSION_MAJOR 1)
set(CPACK_PACKAGE_VERSION_MINOR 0)
set(CPACK_PACKAGE_VERSION_PATCH 0)

option(ENABLE_CHROMIUM_FRAMES "Enable ChromiumFrames with CEF integration" ON)
option(ENABLE_VOICE_CHAT "Enable experimental voice chat" OFF)
option(ENABLE_DISCORD_INTEGRATION "Enable Discord Rich Presence integration" ON)
option(AYA_TEST_BUILD "Enable testing utilities" ON)
option(CEF_USE_SANDBOX "Enable CEF sandbox" OFF)
option(ADDRESS_SANITIZER "Enable address sanitization" OFF)
option(USE_PRIVATE_RESOURCES "Use private resources" ON)
option(NO_QT "Don't build Qt targets" OFF)
set(CEF_VERSION "140.1.14+geb1c06e+chromium-140.0.7339.185" CACHE STRING "CEF version to use (if ChromiumFrames are enabled)")

if(NOT CI)
  if(AYA_TEST_BUILD)
    add_compile_definitions(AYA_TEST_BUILD)

    if (USE_PRIVATE_RESOURCES)
      add_compile_definitions(USE_PRIVATE_RESOURCES)
    endif()
  endif()
endif()

if("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "^(arm64|ARM64|aarch64|AARCH64)$")
  set(AYA_ARCH_ARM64 1)
  set(FMOD_ARCHITECTURE arm64)
  add_compile_definitions(AYA_ARCH_ARM64=1)
elseif("${CMAKE_SYSTEM_PROCESSOR}" MATCHES "^(x86_64|X86_64)$")
  set(AYA_ARCH_X86 1)
  set(FMOD_ARCHITECTURE x64)
  add_compile_definitions(AYA_ARCH_X86=1)
else()
  message(FATAL_ERROR "Aya: Unsupported architecture ${CMAKE_SYSTEM_PROCESSOR}")
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  set(AYA_OS_WINDOWS 1)
  add_compile_definitions(AYA_OS_WINDOWS)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(AYA_OS_LINUX 1)
  add_compile_definitions(AYA_OS_LINUX)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(AYA_OS_MACOS 1)
  add_compile_definitions(AYA_OS_MACOS)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
  set(AYA_OS_ANDROID 1)
  add_compile_definitions(AYA_OS_ANDROID)
else()
  message(FATAL_ERROR "Aya: Unsupported system ${CMAKE_SYSTEM_NAME}")
endif()


########## Compile Options ##########
# ChromiumFrames and related SSE3 options enabled required for rendering optimizations
if(AYA_OS_ANDROID)
  set(ENABLE_CHROMIUM_FRAMES OFF)
endif()

if(ENABLE_CHROMIUM_FRAMES)
  add_compile_definitions(ENABLE_CHROMIUM_FRAMES=1)
endif()

add_compile_definitions(CEF_VERSION="${CEF_VERSION}")

if(NOT AYA_ARCH_ARM64)
  add_compile_options(-mssse3)
endif()

if(ADDRESS_SANITIZER AND NOT AYA_ARCH_ARM64)
  add_compile_options(-fsanitize=address)
  add_link_options(-fsanitize=address)
endif()

if(ENABLE_VOICE_CHAT)
  add_compile_definitions(ENABLE_VOICE_CHAT)
endif()

# Disable some annoying warnings
add_compile_options(-w)
add_compile_options(-Wnonportable-include-path)
add_compile_options(-Wundefined-var-template)
add_compile_options(-Wdelete-abstract-non-virtual-dtor)

########## Windows/MSVC Configuration ##########

# Temporary stopgap solution as QtWebEngine currently is not on WoA, pending Qt update.
# Required for Qt targets
if(AYA_OS_WINDOWS AND AYA_ARCH_ARM64)
  set(NO_QT ON)
endif()

add_compile_definitions(
  _HAS_AUTO_PTR_ETC=1
  _ENFORCE_MATCHING_ALLOCATORS=0
)

if(MSVC)
  set(CMAKE_PDB_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
  set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

  include_directories($ENV{INCLUDE})

  if(ATL_PATH)
    include_directories("${ATL_PATH}/include")
    link_directories("${ATL_PATH}/lib/x64")
  endif()

  # Windows 10
  # https://learn.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-170
  add_compile_definitions(
    WINVER=0x0A00
    _WIN32_WINNT=0x0A00
    _WINSOCKAPI_=1 # Prevents winsock.h from defining macros that conflict with winsock2.h
  )

  # Control Flow Guard
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /guard:cf")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /guard:cf")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /guard:cf")
endif()

########## Linux Configuration ##########

if(AYA_OS_LINUX)
  find_package(X11 REQUIRED)
  find_package(OpenGL REQUIRED COMPONENTS OpenGL GLX)
  set(XTRA_LIBRARIES ${X11_LIBRARIES} ${OPENGL_LIBRARIES})
endif()

########## MacOS Configuration ##########

if(AYA_OS_MACOS)
  add_compile_definitions(__APPLE__=1)
  include_directories(${CMAKE_SOURCE_DIR}/third-party/glad/include)
  set(XTRA_LIBRARIES ${X11_LIBRARIES} ${OPENGL_LIBRARIES})
  include_directories(${CMAKE_SOURCE_DIR}/third-party/QtitanRibbon/include)

  if (AYA_OS_MACOS)
    add_compile_options(-D_GLIBCXX_USE_DEPRECATED)
    add_compile_definitions(_LIBCPP_ENABLE_CXX17_REMOVED_AUTO_PTR=1)
  endif()
endif()

########## Dependencies ##########

# Create ${THIRD_PARTY_PATH} and have CMake consume it
file(TO_CMAKE_PATH "${CMAKE_SOURCE_DIR}/third-party" THIRD_PARTY_PATH)
list(APPEND CMAKE_PREFIX_PATH "${THIRD_PARTY_PATH}")

# Needed for Studio
add_compile_definitions(BOOST_THREAD_PROVIDES_FUTURE)
add_compile_definitions(BOOST_THREAD_PROVIDES_FUTURE_CONTINUATION)

# vcpkg
# TODO: Do we still need these?
include_directories(${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include)
link_directories(${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib)

find_package(avcpp CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Boost COMPONENTS filesystem system iostreams intrusive thread exception program_options url REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(JPEG REQUIRED)
find_package(Freetype REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PNG REQUIRED)
find_package(lz4 CONFIG REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(zstd REQUIRED)
find_package(pugixml CONFIG REQUIRED)
find_package(meshoptimizer CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

if(ENABLE_VOICE_CHAT)
  find_package(portaudio CONFIG REQUIRED)
endif()

if(ENABLE_DISCORD_INTEGRATION)
  find_path(DISCORD_RPC_INCLUDE_DIRS "discord_rpc.h")
  find_library(DISCORD_RPC_LIBRARY discord-rpc REQUIRED)
  add_compile_definitions(ENABLE_DISCORD_INTEGRATION)
  include_directories(${DISCORD_RPC_INCLUDE_DIRS})
endif()

# Opus
find_path(OPUS_INCLUDE_DIR opus.h PATH_SUFFIXES opus)
find_library(OPUS_LIBRARY opus)
include_directories(${OPUS_INCLUDE_DIR})

# CEF integration
if(ENABLE_CHROMIUM_FRAMES)
  include(${THIRD_PARTY_PATH}/cef/cmake/cef_cmake.cmake)
  add_subdirectory(third-party/cef)
  include_directories(${CMAKE_SOURCE_DIR}/third-party/cef/${cefName})
endif()

if(AYA_OS_WINDOWS)
  include_directories(${CMAKE_SOURCE_DIR}/third-party/fmod/windows/inc)
  link_directories(${CMAKE_SOURCE_DIR}/third-party/fmod/windows/lib/${FMOD_ARCHITECTURE})
  set(RUNTIME_FILES ${CMAKE_SOURCE_DIR}/third-party/fmod/windows/lib/${FMOD_ARCHITECTURE}/fmod.dll)
elseif(AYA_OS_MACOS)
  include_directories(${CMAKE_SOURCE_DIR}/third-party/fmod/mac/inc)
  link_directories(${CMAKE_SOURCE_DIR}/third-party/fmod/mac/lib)

  add_library(fmod SHARED IMPORTED)
  set_property(TARGET fmod PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/third-party/fmod/mac/lib/libfmod.dylib)

  set(RUNTIME_FILES ${CMAKE_SOURCE_DIR}/third-party/fmod/mac/lib/libfmod.dylib)
elseif(AYA_OS_LINUX)
  include_directories(${CMAKE_SOURCE_DIR}/third-party/fmod/linux/inc)
  link_directories(${CMAKE_SOURCE_DIR}/third-party/fmod/linux/lib/${FMOD_ARCHITECTURE})

  add_library(fmod SHARED IMPORTED)
  set_property(TARGET fmod PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/third-party/fmod/linux/lib/${FMOD_ARCHITECTURE}/libfmod.so)

  set(RUNTIME_FILES ${CMAKE_SOURCE_DIR}/third-party/fmod/linux/lib/${FMOD_ARCHITECTURE}/libfmod.so)
endif()

# Qt
if(NOT NO_QT)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTOUIC ON)
  set(CMAKE_AUTORCC ON)

  add_compile_definitions(QT_NO_KEYWORDS)

  # Arch: yay -S qt6-base qt6-webengine
  # Ubuntu: apt install qt6-base-dev qt6-webengine-dev qt6-webengine-dev-tools libqt6webenginecore6-bin
  find_package(Qt6 COMPONENTS Core Widgets Concurrent WebEngineWidgets Gui Designer Xml REQUIRED)
  
  # QtitanRibbon
  if(NOT AYA_OS_ANDROID)
    add_subdirectory(third-party/QtitanRibbon) 
  endif()

  # hack
  if(AYA_OS_LINUX)
    include_directories(${QT_BASE_INCLUDE_PATH}/QtXml)
  endif()
endif()

# Other dependencies
add_subdirectory(third-party/BulletPhysics)
add_subdirectory(third-party/RakNet)

########## Aya Project ##########

include_directories(${CMAKE_SOURCE_DIR}/client/core)

if(AYA_OS_WINDOWS)
  include_directories(${CMAKE_SOURCE_DIR}/client/core/windows)
elseif(AYA_OS_LINUX)
  include_directories(${CMAKE_SOURCE_DIR}/client/core/linux)
endif()

add_subdirectory(engine/app)
add_subdirectory(engine/core)
add_subdirectory(engine/network)
add_subdirectory(engine/gfx)

add_subdirectory(client/core-script-compiler)

if(NOT AYA_OS_ANDROID)
  if(NOT NO_QT)
    if(AYA_OS_WINDOWS)
      include(${CMAKE_SOURCE_DIR}/client/windeployqt.cmake)
    endif()

    add_subdirectory(client/studio)
    add_subdirectory(client/player)
  endif()
  add_subdirectory(client/server)
else()
  # add_subdirectory(client/mobile)
endif()

if(ENABLE_CHROMIUM_FRAMES AND NOT AYA_OS_MACOS)
  add_subdirectory(client/web-helper)
endif()

if(AYA_OS_WINDOWS)
  add_subdirectory(client/thumbnail-handler)
endif()

set(THIRD_PARTY_LIBRARIES
  avcpp::avcpp avcpp::FFmpeg
  assimp::assimp
  ${Boost_LIBRARIES}
  CURL::libcurl
  JPEG::JPEG
  Freetype::Freetype
  fmod
  $<IF:$<TARGET_EXISTS:zstd::libzstd_shared>,zstd::libzstd_shared,zstd::libzstd_static>
  lz4::lz4
  OpenSSL::SSL OpenSSL::Crypto
  SDL2::SDL2
  ZLIB::ZLIB
  PNG::PNG
  ${XTRA_LIBRARIES}
  pugixml::pugixml
  meshoptimizer::meshoptimizer
  imgui::imgui
  jxl jxl_cms jxl_threads brotlidec brotlienc brotlicommon lcms2 hwy
)

if(ENABLE_VOICE_CHAT)
  set(THIRD_PARTY_LIBRARIES ${THIRD_PARTY_LIBRARIES} ${OPUS_LIBRARY})
  set(THIRD_PARTY_LIBRARIES ${THIRD_PARTY_LIBRARIES} portaudio_static)
  set(THIRD_PARTY_LIBRARIES ${THIRD_PARTY_LIBRARIES} portaudio)

  if(MSVC)
    # annoying hack for PortAudio shittiness
    add_link_options(/FORCE:MULTIPLE)
  endif()
endif()

if(VCPKG_LIBRARY_LINKAGE STREQUAL "static" OR AYA_OS_LINUX OR AYA_OS_MACOS)
  set(THIRD_PARTY_LIBRARIES ${THIRD_PARTY_LIBRARIES} pugixml::pugixml)
else()
  set(THIRD_PARTY_LIBRARIES ${THIRD_PARTY_LIBRARIES} pugixml::shared)
endif()

if(MSVC OR AYA_OS_WINDOWS)
  set(MSVC_LIBRARIES
    pdh.lib
    Ws2_32.lib
    wininet.lib
    opengl32.lib
    Version.lib
    bcrypt.lib
    Iphlpapi.lib
    d3d11.lib
    dxgi.lib
    Winmm.lib
  )
endif()

if(ENABLE_CHROMIUM_FRAMES AND NOT AYA_OS_MACOS AND NOT NO_QT)
  set(CEF_LIBRARIES cefdll_wrapper)
  add_dependencies(Player WebHelper)
  add_dependencies(Studio WebHelper)
endif()

if(AYA_TEST_BUILD)
  add_dependencies(Player CompileCoreScripts)
endif()

if(NOT NO_QT)
  set(QT_LIBRARIES
    Qt6::Core 
    Qt6::Widgets
    Qt6::Concurrent
    Qt6::Gui
    Qt6::WebEngineWidgets
    Qt6::Xml
  )

  target_link_libraries(Player ${THIRD_PARTY_LIBRARIES} ${MSVC_LIBRARIES} ${CEF_LIBRARIES} ${DISCORD_RPC_LIBRARY})
  target_link_libraries(Studio ${THIRD_PARTY_LIBRARIES} ${MSVC_LIBRARIES} ${CEF_LIBRARIES} ${QT_LIBRARIES})

  set_property(INSTALL "$<TARGET_FILE_NAME:Player>"
    PROPERTY CPACK_START_MENU_SHORTCUTS "Aya Player"
  )

  set_property(INSTALL "$<TARGET_FILE_NAME:Studio>"
    PROPERTY CPACK_START_MENU_SHORTCUTS "Aya Studio"
  )

  if(AYA_OS_WINDOWS)
    windeployqt(Player)
    windeployqt(Studio)
  endif()
endif()

target_link_libraries(CoreScriptCompiler ${MSVC_LIBRARIES} ${THIRD_PARTY_LIBRARIES}) # TODO: Lean and mean CoreScriptCompiler soon (only compile LuaVMDummy & dependencies)
target_link_libraries(Server ${THIRD_PARTY_LIBRARIES} ${MSVC_LIBRARIES})

set_property(INSTALL "$<TARGET_FILE_NAME:Server>"
  PROPERTY CPACK_START_MENU_SHORTCUTS "Aya Server"
)
message(NOTICE "")
message(NOTICE "-- To compile CoreScripts for Player, run target 'CompileCoreScripts'")
message(NOTICE "-- To compile shaders, run target 'CompileShaders'")
message(NOTICE "-- All build artifacts are available in '${CMAKE_RUNTIME_OUTPUT_DIRECTORY}'")
message(NOTICE "")

include(CPack)
