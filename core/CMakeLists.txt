add_library(core STATIC)
add_library(journal::core ALIAS core)

target_sources(core
    PUBLIC
        FILE_SET headers
            TYPE HEADERS
            BASE_DIRS include
            FILES
                include/journal/core.hpp
                include/journal/logger.hpp
    PRIVATE
        core.cpp
        logger.cpp
)

target_compile_features(core PUBLIC cxx_std_23)
target_compile_definitions(core PUBLIC QUILL_DISABLE_NON_PREFIXED_MACROS)


target_link_libraries(core PUBLIC fmt::fmt quill::quill)
