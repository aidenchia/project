cmake_minimum_required(VERSION 3.12)
project(CppProject)

set(CMAKE_CXX_STANDARD 17)

# Add the directories containing the source files
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
# Add the source files to the project
set(SOURCES
    step1.cpp
    CsvFeeder.cpp
    CsvFeeder.h
    Msg.h
)
# Set the output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
# Create the executable
add_executable(step1 ${SOURCES})
target_compile_options(step1 PRIVATE -g)

# Add step2
set(SOURCES_STEP2
    step2.cpp
    CsvFeeder.cpp
    CsvFeeder.h
    Msg.h
    VolSurfBuilder.h
)
# Create the executable for step2
add_executable(step2 ${SOURCES_STEP2})
target_compile_options(step2 PRIVATE -g)

# Add step3
set(SOURCES_STEP3
    step3.cpp
    CsvFeeder.cpp
    CsvFeeder.h
    Msg.h
    VolSurfBuilder.h
    CubicSmile.cpp
    CubicSmile.h
    Date.cpp
)
# Create the executable for step3
add_executable(step3 ${SOURCES_STEP3})
target_compile_options(step3 PRIVATE -g)
target_include_directories(step3 PRIVATE Solver)

# Add test_lbfgs
add_executable(test_lbfgs
    Solver/LBFGSpp/examples/example-rosenbrock-box.cpp
)
target_compile_options(test_lbfgs PRIVATE -g)
target_include_directories(test_lbfgs PRIVATE Solver)

# Add test_ts_parser
add_executable(test_ts_parser
    test_ts_parser.cpp
    CsvFeeder.cpp
)
target_compile_options(test_ts_parser PRIVATE -g)
