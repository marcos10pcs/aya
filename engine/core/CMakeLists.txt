add_library(Core STATIC)

set(SOURCES
  src/aya/atomic.hpp
  src/aya/BaldPtr.hpp
  src/aya/boost.cpp
  src/aya/callable.hpp
  src/aya/CEvent.cpp
  src/aya/CEvent.hpp
  src/aya/Crypt.cpp
  src/aya/Crypt.hpp
  src/aya/Debug.hpp
  src/aya/Declarations.hpp
  src/aya/DenseHash.hpp
  src/aya/GlobalVectorItem.hpp
  src/aya/Intrusive/Set.hpp
  src/aya/intrusive_ptr_target.hpp
  src/aya/intrusive_weak_ptr.hpp
  src/aya/Log.hpp
  src/aya/make_shared.hpp
  src/aya/MathUtil.cpp
  src/aya/MathUtil.hpp
  src/aya/Memory.cpp
  src/aya/Memory.hpp
  src/aya/Nil.hpp
  src/aya/object_pool.hpp
  src/aya/ProcessPerfCounter.cpp
  src/aya/ProcessPerfCounter.hpp
  src/aya/Profiler.cpp
  src/aya/RbxDbgInfo.cpp
  src/aya/RbxDbgInfo.hpp
  src/aya/RbxStrings.hpp
  src/aya/rbxTime.hpp
  src/aya/RunningAverage.hpp
  src/aya/Signal.cpp
  src/aya/signal.hpp
  src/aya/SystemUtil.hpp
  src/aya/Tasks/Coordinator.cpp
  src/aya/Tasks/Coordinator.hpp
  src/aya/TaskScheduler.cpp
  src/aya/TaskScheduler.hpp
  src/aya/TaskScheduler.Job.cpp
  src/aya/TaskScheduler.Job.hpp
  src/aya/TaskScheduler.Thread.cpp
  src/aya/ThreadSafe.cpp
  src/aya/threadsafe.hpp
  src/aya/Time.cpp
  src/aya/trie.hpp
  src/util/Debug.cpp
  src/util/Log.cpp
  src/util/RbxCrash.cpp
  src/util/RegistryUtil.hpp
  src/util/SafeToLower.hpp
  src/util/ScopedSingleton.hpp
  src/util/StreamHelpers.cpp
  src/util/stringbuffer.hpp
  src/util/ublas_ext.hpp
  src/AyaAssert.cpp
  src/AyaAssert.hpp
  src/AyaFormat.cpp
  src/AyaFormat.hpp
  src/CPUCount.cpp
  src/CPUCount.hpp
  src/FastLog.cpp
  src/FastLog.hpp
  src/FastLogStream.cpp
  src/FastLogStream.hpp
  src/HardwareInfo.hpp
  src/RbxBase.hpp
  src/RbxHash.hpp
  src/RbxPlatform.hpp
  src/SelectState.hpp
  src/StringConv.cpp
  src/StringConv.hpp

  ${CMAKE_SOURCE_DIR}/client/core/format_string.cpp
  ${CMAKE_SOURCE_DIR}/client/core/SimpleJSON.cpp
)

# Platform-specific configuration
if(AYA_OS_LINUX OR AYA_OS_MACOS)
  list(APPEND SOURCES src/aya/Android/SystemUtil.cpp)
  if (AYA_OS_LINUX)
    list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/client/core/linux/CookiesEngine.cpp)
  elseif(AYA_OS_MACOS)
    list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/client/core/mac/CookiesEngine.cpp)
  endif()
elseif(AYA_OS_WINDOWS)
  list(APPEND SOURCES
    ${CMAKE_SOURCE_DIR}/client/core/windows/CookiesEngine.cpp
    src/aya/Windows/SystemUtil.cpp
    src/aya/SystemUtil.hpp
    src/util/RegistryUtil.cpp
  )
elseif(AYA_OS_ANDROID)
  list(APPEND SOURCES
    src/aya/Android/ifaddrs.cpp
    src/aya/Android/ifaddrs.hpp
  )
endif()

target_include_directories(Core
  PUBLIC
    ${CMAKE_SOURCE_DIR}/engine/core/src
  PRIVATE
    ${CMAKE_SOURCE_DIR}/engine/app/src
    ${CMAKE_SOURCE_DIR}/engine/gfx/src
)

target_sources(Core PRIVATE ${SOURCES})