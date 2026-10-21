add_executable(CoreScriptCompiler
  src/main.cpp

  $<TARGET_OBJECTS:AppServer>
  $<TARGET_OBJECTS:Core>
  $<TARGET_OBJECTS:RakNet>
  $<TARGET_OBJECTS:BulletPhysics>
  $<TARGET_OBJECTS:NetworkServer>
  $<TARGET_OBJECTS:Graphics>
)

if(AYA_OS_WINDOWS)
  target_sources(CoreScriptCompiler PRIVATE resources/winrc.h resources/script.rc)
  set_target_properties(CoreScriptCompiler PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

target_include_directories(CoreScriptCompiler PRIVATE src resources)
set_target_properties(CoreScriptCompiler PROPERTIES OUTPUT_NAME "Aya.CoreScriptCompiler")

add_custom_target(CompileCoreScripts 
  $<TARGET_FILE:CoreScriptCompiler> --source "${CMAKE_SOURCE_DIR}/client/resources/content/scripts" --output "${CMAKE_SOURCE_DIR}/engine/app/Script/LuaGenCS.inl" 
  DEPENDS CoreScriptCompiler
  COMMENT "Compiling CoreScript bytecode"
  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_custom_target(CompileShaders
  python ${CMAKE_SOURCE_DIR}/client/resources/shaders/compile_shaders.py --packs glsl3 glsles3
  COMMENT "Compiling shaders"
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/client/resources/shaders"
)