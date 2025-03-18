cmake_minimum_required(VERSION 3.15)
project(example)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")

add_executable(example
    main.c
    ${CMAKE_CURRENT_BINARY_DIR}/resources.c
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/resources.c
    COMMAND python3 scripts/resource_gen.py --output ${CMAKE_CURRENT_BINARY_DIR}/resources.c
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS res/example_resource.txt
    DEPENDS config.yaml
)

add_custom_target(generate_file ALL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/resources.c
)
