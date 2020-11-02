function(get_compiler_warnings target_name)
    option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" TRUE)

    set(MSVC_WARNINGS
        /W4
    )

    set(CLANG_WARNINGS
        -Wall
        -Wextra
        -Wno-writable-strings
        -Wno-microsoft-goto
        -Wno-unused-parameter
        -Wno-unused-macros
        -Wno-unused-variable
        -Wno-missing-prototypes
        -Wno-zero-as-null-pointer-constant
        -Wno-old-style-cast
        -Wno-double-promotion
        -Wno-c++98-compat-pedantic
        -Wno-newline-eof
        -Wno-reserved-id-macro
        -Wno-implicit-int-float-conversion
        -Wno-language-extension-token
    )

    if(WARNINGS_AS_ERRORS)
        set(MSVC_WARNINGS ${MSVC_WARNINGS} /WX)
        set(CLANG_WARNINGS ${CLANG_WARNINGS} -Werror)
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        set(PROJECT_WARNINGS ${CLANG_WARNINGS})
    elseif(MSVC)
        set(PROJECT_WARNINGS ${MSVC_WARNINGS})
    else()
        message(AUTHOR_WARNING "No compiler warnings set for `${CMAKE_CXX_COMPILER_ID}` compiler.")
    endif()

    target_compile_options(${target_name} INTERFACE ${PROJECT_WARNINGS})
endfunction(get_compiler_warnings)
