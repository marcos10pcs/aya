add_executable(Server
# src/main.cpp
# src/server.cpp
  src/DummyWindow.cpp
  src/ThumbnailGenerator.cpp
  src/DummyWindow.hpp
  src/ThumbnailGenerator.hpp

  $<TARGET_OBJECTS:AppServer>
  $<TARGET_OBJECTS:Core>
  $<TARGET_OBJECTS:RakNet>
  $<TARGET_OBJECTS:BulletPhysics>
  $<TARGET_OBJECTS:NetworkServer>
  $<TARGET_OBJECTS:Graphics>
)

target_sources(Server PRIVATE
  ${CMAKE_SOURCE_DIR}/client/core/CountersClient.cpp
  ${CMAKE_SOURCE_DIR}/client/core/CountersClient.hpp
  ${CMAKE_SOURCE_DIR}/client/core/InfluxDbHelper.cpp
  ${CMAKE_SOURCE_DIR}/client/core/InfluxDbHelper.hpp
  ${CMAKE_SOURCE_DIR}/client/core/MachineConfiguration.cpp
  ${CMAKE_SOURCE_DIR}/client/core/MachineConfiguration.hpp
  ${CMAKE_SOURCE_DIR}/client/core/ReflectionMetadata.cpp
  ${CMAKE_SOURCE_DIR}/client/core/ReflectionMetadata.hpp
  ${CMAKE_SOURCE_DIR}/client/core/RenderSettingsItem.cpp
  ${CMAKE_SOURCE_DIR}/client/core/RenderSettingsItem.hpp
  ${CMAKE_SOURCE_DIR}/client/core/SDLGameController.cpp
  ${CMAKE_SOURCE_DIR}/client/core/SDLGameController.hpp
)

if(AYA_OS_WINDOWS)
  target_sources(Server PRIVATE
    resources/winrc.h
    resources/script.rc

    ${CMAKE_SOURCE_DIR}/client/core/windows/AuthenticationMarshallar.cpp
    ${CMAKE_SOURCE_DIR}/client/core/windows/AuthenticationMarshallar.hpp
    ${CMAKE_SOURCE_DIR}/client/core/windows/CheckDbg.cpp
    ${CMAKE_SOURCE_DIR}/client/core/windows/CheckDbg.hpp
    ${CMAKE_SOURCE_DIR}/client/core/windows/DumpErrorUploader.cpp
    ${CMAKE_SOURCE_DIR}/client/core/windows/DumpErrorUploader.hpp
    ${CMAKE_SOURCE_DIR}/client/core/windows/ErrorUploader.cpp
    ${CMAKE_SOURCE_DIR}/client/core/windows/ErrorUploader.hpp
    ${CMAKE_SOURCE_DIR}/client/core/windows/LogManager.cpp
    ${CMAKE_SOURCE_DIR}/client/core/windows/LogManager.hpp
    ${CMAKE_SOURCE_DIR}/client/core/windows/ScriptErrorUploader.cpp
    ${CMAKE_SOURCE_DIR}/client/core/windows/ScriptErrorUploader.hpp
    ${CMAKE_SOURCE_DIR}/client/core/windows/Tracer.cpp
    ${CMAKE_SOURCE_DIR}/client/core/windows/UserInputUtil.cpp
    ${CMAKE_SOURCE_DIR}/client/core/windows/UserInputUtil.hpp
  )

  set_target_properties(Server PROPERTIES WIN32_EXECUTABLE TRUE)
elseif(AYA_OS_LINUX OR AYA_OS_MACOS)
  target_sources(Server PRIVATE ${CMAKE_SOURCE_DIR}/engine/gfx/src/GL/ContextGLSDL2.cpp)
endif()

target_include_directories(Server PRIVATE src resources)
set_target_properties(Server PROPERTIES OUTPUT_NAME "Aya.Server")
add_custom_command(TARGET Server POST_BUILD
  COMMENT "Copying test Lua script for Aya Server"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    resources/test.lua
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)