add_executable(Player
  src/main.cpp
  resources/qt.qrc

  src/Application.cpp
  src/Application.hpp
  src/AvatarViewService.cpp
  src/AvatarViewService.hpp
  src/Document.cpp
  src/Document.hpp
  src/FunctionMarshaller.cpp
  src/FunctionMarshaller.hpp
  src/GameVerbs.cpp
  src/GameVerbs.hpp
  src/LauncherView.cpp
  src/LauncherView.hpp
  src/RenderJob.cpp
  src/RenderJob.hpp
  src/View.cpp
  src/View.hpp
  src/Window.cpp
  src/Window.hpp
  ${CMAKE_SOURCE_DIR}/client/core/GrayChatBar.hpp
  ${CMAKE_SOURCE_DIR}/client/core/GrayChatBar.cpp

  $<TARGET_OBJECTS:AppPlayer>
  $<TARGET_OBJECTS:Core>
  $<TARGET_OBJECTS:RakNet>
  $<TARGET_OBJECTS:BulletPhysics>
  $<TARGET_OBJECTS:NetworkPlayer>
  $<TARGET_OBJECTS:Graphics>
)

target_sources(Player PRIVATE
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
  target_sources(Player PRIVATE
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

  set_target_properties(Player PROPERTIES WIN32_EXECUTABLE TRUE)
elseif(AYA_OS_LINUX OR AYA_OS_MACOS)
  if(AYA_OS_LINUX)
    target_sources(Player PRIVATE ${CMAKE_SOURCE_DIR}/engine/gfx/src/GL/ContextGLX.cpp)
  elseif(AYA_OS_MACOS)
    target_sources(Player PRIVATE ${CMAKE_SOURCE_DIR}/engine/gfx/src/GL/ContextGLMac.mm)
  endif()
endif()

target_include_directories(Player PRIVATE src resources)
set_target_properties(Player PROPERTIES OUTPUT_NAME "Aya.Player")