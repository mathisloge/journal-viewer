cmake_minimum_required(VERSION 3.28)
project(journal-viewer VERSION 0.0.1 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(CTest)
include(FetchContent)
include(GenerateExportHeader)
include(FeatureSummary)
include(GNUInstallDirs)
include(GetGitRevisionDescription)
include(Dependencies)

option(DO_FETCH_DEPENDENCIES "true if cmake should fetch most of the dependencies" OFF)
fetch_dependencies()

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


find_package(PkgConfig REQUIRED)
pkg_check_modules(libsystemd REQUIRED IMPORTED_TARGET libsystemd)


find_package(fmt CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(quill CONFIG REQUIRED)
find_package(nfd CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(CMakeRC CONFIG REQUIRED)

add_subdirectory(journal)
add_subdirectory(journal-viewer)


install(TARGETS journal-viewer)
include(CPack)
