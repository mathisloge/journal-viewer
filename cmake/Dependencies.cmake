function(fetch_dependencies)
    set(tmp_BUILD_TESTING ${BUILD_TESTING})
    set(BUILD_TESTING OFF)

    FetchContent_Declare(imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG 790f2b9a7aa2a23a2fb47ed8f48293f27c3ac658 # docking
    )

    set(FMT_INSTALL OFF)
    set(FMT_TEST OFF)
    FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG        11.1.4
        FIND_PACKAGE_ARGS
    )

    set(QUILL_DISABLE_NON_PREFIXED_MACROS OFF)
    FetchContent_Declare(
        quill
        GIT_REPOSITORY https://github.com/odygrd/quill.git
        GIT_TAG        v8.2.0
        FIND_PACKAGE_ARGS
    )

    set(NFD_BUILD_TESTSc OFF)
    set(NFD_INSTALL OFF)
    FetchContent_Declare(
        nfd
        GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended
        GIT_TAG        a1a401062819beb8c3da84518ab1fe7de88632db
        FIND_PACKAGE_ARGS
    )

    FetchContent_Declare(
        cmakerc
        GIT_REPOSITORY https://github.com/vector-of-bool/cmrc
        GIT_TAG        952ffddba731fc110bd50409e8d2b8a06abbd237
        FIND_PACKAGE_ARGS
    )

    FetchContent_Declare(
        proxy
        GIT_REPOSITORY https://github.com/microsoft/proxy
        GIT_TAG        3.3.0
        FIND_PACKAGE_ARGS
    )

    FetchContent_Declare(
        entt
        GIT_REPOSITORY https://github.com/skypjack/entt
        GIT_TAG        v3.15.0
        FIND_PACKAGE_ARGS
    )

    FetchContent_Declare(
        asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio
        GIT_TAG        4730c543ba2b8f9396ef1964a25ccc26eb1aea64
    )

    FetchContent_MakeAvailable(fmt nfd quill cmakerc proxy entt)


    set(BUILD_TESTING ${tmp_BUILD_TESTING})
    if(BUILD_TESTING)
        set(CATCH_INSTALL_DOCS OFF)
        FetchContent_Declare(
            Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG        v3.8.0
            FIND_PACKAGE_ARGS
        )
        FetchContent_MakeAvailable(Catch2)
    endif()
endfunction()
