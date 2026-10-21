add_executable(WebHelper src/main.cpp)

if(AYA_OS_WINDOWS)
  target_sources(WebHelper PRIVATE
    resources/winrc.h
    resources/script.rc
  )

  set_target_properties(WebHelper PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

target_include_directories(WebHelper PRIVATE src resources)
set_target_properties(WebHelper PROPERTIES OUTPUT_NAME "Aya.WebHelper")
target_link_libraries(WebHelper PRIVATE cefdll_wrapper)