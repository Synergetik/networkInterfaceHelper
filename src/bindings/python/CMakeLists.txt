# Python Bindings CMake file

find_package(Python 3.11 COMPONENTS Interpreter Development REQUIRED)

set(PYTHON_PROJECT_TARGET ${PROJECT_NAME}_python)
set(PYTHON_OUTPUT ${PROJECT_NAME})
set(PYBIND11_FINDPYTHON TRUE)
set(PYTHON_EXECUTABLE ${Python_EXECUTABLE})
message(STATUS "NIH: Using python library from ${Python_LIBRARIES}")

include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        v2.13.6
)
FetchContent_MakeAvailable(pybind11)

# Setup project
cu_setup_project(${PYTHON_PROJECT_TARGET} ${LA_NIH_VERSION} "Network Interface Helper Python Bindings Library")

# Public header files
set (PUBLIC_HEADER_FILES
	${CU_ROOT_DIR}/src/bindings/python/networkInterfaceHelper_python.hpp
)

# Common files
set (SOURCE_FILES_COMMON
	${CU_ROOT_DIR}/src/bindings/python/networkInterfaceHelper_python.cpp
)

# Group sources
source_group("Header Files\\Public" FILES ${PUBLIC_HEADER_FILES})
source_group("Source Files" FILES ${SOURCE_FILES_COMMON})

# Configure module
pybind11_add_module(${PYTHON_PROJECT_TARGET} ${SOURCE_FILES_COMMON})
set_target_properties(${PYTHON_PROJECT_TARGET} PROPERTIES OUTPUT_NAME ${PYTHON_OUTPUT} SUFFIX .pyd)

# Using NIH library
target_link_libraries(${PYTHON_PROJECT_TARGET} PRIVATE ${PYTHON_OUTPUT}_static)

# Add custom command to generate stubs
add_custom_command(
    TARGET ${PYTHON_PROJECT_TARGET} POST_BUILD
    COMMAND ${Python_EXECUTABLE} -m pybind11_stubgen ${PYTHON_OUTPUT} --output-dir ${CMAKE_CURRENT_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating Python stubs with pybind11-stubgen"
)