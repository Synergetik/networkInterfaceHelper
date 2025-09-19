# Python Bindings CMake file

# Find python installation and generate virtual environment
set(PY_VENV_DIR "${CMAKE_BINARY_DIR}/.pyenv")

find_package(Python ${BUILD_NIH_PYTHON_BINDINGS_VERSION} EXACT COMPONENTS Interpreter Development REQUIRED QUIET)
if(NOT EXISTS "${PY_VENV_DIR}")
  execute_process(
    COMMAND "${Python_EXECUTABLE}" -m venv "${PY_VENV_DIR}"
    COMMAND_ERROR_IS_FATAL ANY
  )
endif()
set (ENV{VIRTUAL_ENV} "${PY_VENV_DIR}")

# Select virtual environment and install pybind11 stubgen
set(Python_FIND_VIRTUALENV ONLY)
unset(Python_EXECUTABLE)
find_package(Python ${BUILD_NIH_PYTHON_BINDINGS_VERSION} EXACT COMPONENTS Interpreter Development REQUIRED)

execute_process(
  COMMAND "${Python_EXECUTABLE}" -m pip install --disable-pip-version-check -q --upgrade "pybind11-stubgen~=2.5.0"
  COMMAND_ERROR_IS_FATAL ANY
)

set(PYTHON_PROJECT_TARGET ${PROJECT_NAME}_python)
set(PYTHON_OUTPUT ${PROJECT_NAME})
set(PYBIND11_FINDPYTHON TRUE)
set(PYTHON_EXECUTABLE ${Python_EXECUTABLE})
message(STATUS "NIH: Using python library from ${Python_LIBRARIES}")

# Inject pybind11 dependency
include(FetchContent)
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        ${BUILD_NIH_PYTHON_BINDINGS_PYBIND_TAG}
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
set_target_properties(${PYTHON_PROJECT_TARGET} PROPERTIES OUTPUT_NAME ${PYTHON_OUTPUT})

target_include_directories(${PYTHON_PROJECT_TARGET} INTERFACE ${CU_ROOT_DIR}/src/bindings/python)

# Using static NIH library
target_link_libraries(${PYTHON_PROJECT_TARGET} PRIVATE ${PYTHON_OUTPUT}_static)

# Add custom command to generate stubs
add_custom_target(${PYTHON_PROJECT_TARGET}_with_stubs
  COMMAND ${CMAKE_COMMAND} -E env
          PYTHONPATH=$<TARGET_FILE_DIR:${PYTHON_PROJECT_TARGET}>
          "${Python_EXECUTABLE}" -m pybind11_stubgen
          "${PYTHON_OUTPUT}"
          --output-dir $<TARGET_FILE_DIR:${PYTHON_PROJECT_TARGET}>
  WORKING_DIRECTORY $<TARGET_FILE_DIR:${PYTHON_PROJECT_TARGET}>
  DEPENDS ${PYTHON_PROJECT_TARGET}
  COMMENT "NIH: Generating Python stubs with pybind11-stubgen"
  VERBATIM
)