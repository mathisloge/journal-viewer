add_executable(journal-viewer main.cpp)

target_link_libraries(journal-viewer PRIVATE
    imgui::imgui
    SDL3::SDL3
    journal::journal
)
