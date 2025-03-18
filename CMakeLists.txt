cmake_minimum_required(VERSION 3.15)
project(example)

set(CMAKE_C_STANDARD 23)

add_executable(example
    main.c
    ${CMAKE_CURRENT_BINARY_DIR}/resources.c
)

# The following is what generates the resources.c file
FILE(GLOB RESOURCES res/*)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/resources.c
    COMMAND python3 scripts/resource_fs --output ${CMAKE_CURRENT_BINARY_DIR}/resources.c
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${RESOURCES}
    DEPENDS config.yaml
)

add_custom_target(generate_file ALL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/resources.c
)
