cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(
    lSel
    VERSION 0.1.0
    DESCRIPTION "Short description"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)
option(STATIC_BUILD "Create a static binary" OFF)
option(SYSTEM_FTXUI "Use ftxui libraries found on system" OFF)


if(SYSTEM_FTXUI)
   find_package(ftxui)
else()
   set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
   FetchContent_Declare(ftxui
     GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
     GIT_TAG v5.0.0
   )
   FetchContent_GetProperties(ftxui)
   if(NOT ftxui_POPULATED)
     FetchContent_Populate(ftxui)
     add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
   endif()
endif()



# ---- Declare library ----

add_library(listToggle_lib
    src/ListToggle.cpp
    src/ListToggle.hpp
)

target_include_directories(
    listToggle_lib ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
)

target_compile_features(listToggle_lib PUBLIC cxx_std_20)
if(STATIC_BUILD)
   set(CMAKE_CXX_FLAGS "-static")
endif()

target_link_libraries(listToggle_lib
   PRIVATE ftxui::dom
   PRIVATE ftxui::component
)
# ---- Declare executable ----

add_executable(listToggle_exe
   src/main.cpp
)
target_link_libraries(listToggle_exe
   PRIVATE ftxui::component
)
set_property(TARGET listToggle_exe
   PROPERTY OUTPUT_NAME lSel
)

target_compile_features(listToggle_exe PRIVATE
   cxx_std_20
)

target_link_libraries(listToggle_exe
   PRIVATE listToggle_lib
)

