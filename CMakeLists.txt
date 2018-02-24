cmake_minimum_required(VERSION 3.9)
project(variant LANGUAGES CXX)

add_executable(main
	main.cxx
	type_traits.hxx
	variant.hxx
)

set_target_properties(main PROPERTIES CXX_STANDARD 11)
if(MSVC)
    target_compile_options(main PRIVATE /W4)
else()
    target_compile_options(main PRIVATE -Wall PRIVATE -Wextra)
endif()

