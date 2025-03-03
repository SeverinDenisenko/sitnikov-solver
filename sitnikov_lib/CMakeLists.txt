add_library(sitnikov STATIC
    sitnikov.cpp
)

target_include_directories(sitnikov PUBLIC include)
target_link_libraries(sitnikov PUBLIC ODESolversLib)

if(MSVC)
    target_compile_options(sitnikov PRIVATE /W4 /WX)
else()
    target_compile_options(sitnikov PRIVATE -Wall -Wextra -Werror)
endif()
