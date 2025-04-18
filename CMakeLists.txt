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

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_MODULE_STD OFF) # only with gcc15+
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(DO_FETCH_DEPENDENCIES "true if cmake should fetch most of the dependencies" OFF)
if(DO_FETCH_DEPENDENCIES)
    fetch_dependencies()
endif()

get_git_head_revision(GIT_REF GIT_SHA)

find_package(PkgConfig REQUIRED)
pkg_check_modules(libsystemd REQUIRED IMPORTED_TARGET libsystemd)

find_package(fmt CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(quill CONFIG REQUIRED)
find_package(nfd CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(CMakeRC CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(proxy CONFIG REQUIRED)

cmrc_add_resource_library(journal-viewer-resources ALIAS journal::viewer-resources NAMESPACE jrn
    journal-viewer/SourceCodePro-Regular.ttf
    LICENSE
)

add_subdirectory(core)
add_subdirectory(journal)
add_subdirectory(journal-viewer)


install(TARGETS journal-viewer)
set(CPACK_PACKAGE_CONTACT "mathisloge <mathisloge@tuta.io>")
include(CPack)
