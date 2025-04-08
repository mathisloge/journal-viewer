add_library(journal STATIC)
add_library(journal::journal ALIAS journal)



configure_file(version.hpp.in version.hpp @ONLY)

target_compile_features(journal
    PUBLIC cxx_std_23
)
target_link_libraries(journal PUBLIC
    PkgConfig::libsystemd
    fmt::fmt
    quill::quill
    EnTT::EnTT
    msft_proxy
    asio::asio
)

target_sources(journal PRIVATE
    journal_log_manager.cpp
    journal_log_manager.hpp
    journal_entry.cpp
    journal_entry.hpp
    journal_info.cpp
    journal_info.hpp
    journal_instance_handle.hpp
    journal_instance.cpp
    journal_instance.hpp
    journal_cache.hpp
)

target_include_directories(journal PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
