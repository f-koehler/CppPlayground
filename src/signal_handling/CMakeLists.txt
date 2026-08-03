include(CheckIncludeFiles)
check_include_files(sys/signalfd.h HAVE_SYS_SIGNALFD_H)

add_executable(traditional traditional.cpp)

if(HAVE_SYS_SIGNALFD_H)
  add_executable(signalfd signalfd.cpp)
endif()
