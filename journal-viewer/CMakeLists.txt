add_executable(journal-viewer main.cpp)

target_link_libraries(journal-viewer PRIVATE
    imgui::imgui
    SDL3::SDL3
    journal::journal
    nfd::nfd
)


target_sources(journal-viewer PRIVATE
    journal_log_window.cpp
    journal_log_window.hpp
    app_state.hpp
    app_state.cpp
    file_browser.hpp
    file_browser.cpp
)
