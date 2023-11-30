cmake_minimum_required(VERSION 3.14)
include(FetchContent)

project(
    listSelect
    VERSION 0.1.0
    DESCRIPTION "Short description"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)
option(LSEL_STATIC "Create a static binary" OFF)
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

add_library(listSelect_lib
    src/ListSelect.cpp
    src/ListSelect.hpp
)

target_include_directories(
    listSelect_lib ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/source>"
)

target_compile_features(listSelect_lib PUBLIC cxx_std_20)
if(LSEL_STATIC)
   set(CMAKE_CXX_FLAGS "-static")
endif()

target_link_libraries(listSelect_lib
   PRIVATE ftxui::screen
           ftxui::dom
           ftxui::component
)
# ---- Declare executable ----

add_executable(listSelect_exe
   src/main.cpp
)
target_link_libraries(listSelect_exe
   PRIVATE ftxui::component
)
set_property(TARGET listSelect_exe
   PROPERTY OUTPUT_NAME lSel
)

target_compile_features(listSelect_exe PRIVATE
   cxx_std_20
)

target_link_libraries(listSelect_exe
   PRIVATE listSelect_lib
   #PUBLIC listSelect_lib
)

