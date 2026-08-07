cmake_minimum_required(VERSION 3.13)

project(kash VERSION 1.2.2)

find_package(Git QUIET)

option(KASH_STATIC_READLINE "Statically link GNU readline (release builds)" OFF)

if(KASH_STATIC_READLINE)
  find_path(READLINE_INCLUDE_DIR readline/readline.h
    HINTS /opt/homebrew/opt/readline/include /usr/local/opt/readline/include)
  find_library(READLINE_LIBRARY NAMES libreadline.a readline
    HINTS /opt/homebrew/opt/readline/lib /usr/local/opt/readline/lib)
  if(APPLE)
    find_library(READLINE_NCURSES_LIBRARY NAMES ncurses)
  else()
    find_library(READLINE_NCURSES_LIBRARY NAMES libtinfo.a libncurses.a)
  endif()
endif()

if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} describe --tags --exact-match
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE GIT_TAG_RESULT
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
  )

  if(GIT_TAG_RESULT EQUAL 0)
    set(GIT_VERSION "${GIT_TAG}")
  else()
    execute_process(
      COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      OUTPUT_VARIABLE GIT_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  endif()

  execute_process(
    COMMAND ${GIT_EXECUTABLE} diff-index --quiet HEAD --
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE GIT_DIRTY
  )

  if(NOT GIT_DIRTY EQUAL 0)
    string(APPEND GIT_VERSION "-dirty")
  endif()
else()
  set(GIT_VERSION "unknown")
endif()

file(GLOB_RECURSE SOURCE_FILES src/*.c src/*.h)

set(CMAKE_C_STANDARD 23) # Enable the C23 standard
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(kash ${SOURCE_FILES})

include_directories(${CMAKE_SOURCE_DIR})

if(KASH_STATIC_READLINE)
  target_include_directories(kash PRIVATE ${READLINE_INCLUDE_DIR})
  target_link_libraries(kash PRIVATE ${READLINE_LIBRARY} ${READLINE_NCURSES_LIBRARY})
else()
  target_link_libraries(kash PRIVATE readline)
endif()

target_link_libraries(kash PRIVATE m)
target_compile_definitions(kash PRIVATE
  PROGRAM_VERSION="${GIT_VERSION}"
)
