add_library(ThumbnailHandler SHARED
  src/thumb_win32.cc
  src/thumb_win32.def
  src/thumb_win32.rc
  src/thumb_win32_dll.cc
)

target_link_libraries(ThumbnailHandler dbghelp.lib Version.lib)
set_target_properties(ThumbnailHandler PROPERTIES OUTPUT_NAME "Aya.ThumbnailHandler")