cmake_minimum_required(VERSION 3.10)

project(OOP1_framework)

# Global configurations

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define OOP1_framework library

add_library(${PROJECT_NAME}
  src/network/TestsystemCommunicationBridge.cpp
  src/network/Client.cpp
  src/network/RetrieveMessage.cpp
  src/network/Server.cpp
  src/network/RetrieveMessage.hpp
  src/random/Random.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC include)

set_target_properties(${PROJECT_NAME} PROPERTIES
  CXX_STANDARD 23
  CXX_EXTENSIONS OFF
)

# Bad pratice but makes the student's life easier on windows
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries")

set(SFML_BUILD_AUDIO OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_WINDOW OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_GRAPHICS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK ON CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/external/sfml)

target_link_libraries(${PROJECT_NAME} PUBLIC sfml-network)

add_executable(example_client example/ClientMain.cpp)
add_executable(example_server example/ServerMain.cpp)
add_executable(client_manager example/ClientManager.cpp)

find_package(Threads REQUIRED)
target_link_libraries(example_client PRIVATE ${PROJECT_NAME} Threads::Threads rt)
target_link_libraries(example_server PRIVATE ${PROJECT_NAME} Threads::Threads rt)
target_link_libraries(client_manager PRIVATE ${PROJECT_NAME} Threads::Threads rt)


if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /DEBUG)
else()
  target_compile_options(client_manager PRIVATE 
      -Wall        # all warnings
      -Wextra      # even more warnings
      -Wshadow     # warnings for shaddowing
      -Wpedantic   # strict adheriance for C++ standard
      -Werror      # treat warnings as errors
      -g
  ) 
  target_compile_options(${PROJECT_NAME} PRIVATE 
    -Wall        # all warnings
    -Wextra      # even more warnings
    -Wshadow     # warnings for shaddowing
    -Wpedantic   # strict adheriance for C++ standard
    #-Werror      # treat warnings as errors
    -g            # debug information
  )
endif()

set_target_properties(example_client PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(example_server PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set_target_properties(client_manager PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
