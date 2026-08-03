include(CheckIncludeFiles)
check_include_files(sys/timerfd.h HAVE_SYS_TIMERFD_H)

if(HAVE_SYS_TIMERFD_H)
  add_executable(timerfd timerfd.cpp)
endif()
