macro(journal_provide_dependency method dep_name)
  if("${dep_name}" MATCHES "^(imgui)$")
    list(APPEND mycomp_provider_args ${method} ${dep_name})
    FetchContent_MakeAvailable(imgui)
    list(POP_BACK mycomp_provider_args dep_name method)
    if("${method}" STREQUAL "FIND_PACKAGE")
      set(${dep_name}_FOUND TRUE)
      add_library(imgui_fetched STATIC)
      add_library(imgui::imgui ALIAS imgui_fetched)
      target_sources(imgui_fetched PRIVATE
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
      )
      target_include_directories(imgui_fetched PUBLIC "$<BUILD_INTERFACE:${imgui_SOURCE_DIR}>" "$<BUILD_INTERFACE:${imgui_SOURCE_DIR}/backends>")
    elseif(NOT "${dep_name}" STREQUAL "imgui")
      FetchContent_SetPopulated(${dep_name}
        SOURCE_DIR "${imgui_SOURCE_DIR}"
        BINARY_DIR "${imgui_BINARY_DIR}"
      )
    endif()
  elseif("${dep_name}" MATCHES "^(asio)$")
    list(APPEND mycomp_provider_args ${method} ${dep_name})
    FetchContent_MakeAvailable(asio)
    list(POP_BACK mycomp_provider_args dep_name method)
    if("${method}" STREQUAL "FIND_PACKAGE")
      set(${dep_name}_FOUND TRUE)
      add_library(asio_fetched INTERFACE)
      add_library(asio::asio ALIAS asio_fetched)
      target_include_directories(asio_fetched INTERFACE "$<BUILD_INTERFACE:${asio_SOURCE_DIR}>")
    elseif(NOT "${dep_name}" STREQUAL "asio")
      FetchContent_SetPopulated(${dep_name}
        SOURCE_DIR "${imgui_SOURCE_DIR}"
        BINARY_DIR "${imgui_BINARY_DIR}"
      )
    endif()
  endif()
endmacro()
cmake_language(
  SET_DEPENDENCY_PROVIDER journal_provide_dependency
  SUPPORTED_METHODS
    FIND_PACKAGE
    FETCHCONTENT_MAKEAVAILABLE_SERIAL
)
