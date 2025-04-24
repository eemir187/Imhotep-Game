cmake_minimum_required(VERSION 3.10)

project(imhotep_server)

set(SOURCES 
  src/main.cpp 
  src/checkValid.cpp
  src/Game.cpp
  src/Player.cpp
  src/Ship.cpp
  src/ConstructionSite.cpp
  src/Obelisk.cpp
  src/Pyramid.cpp
  src/Temple.cpp
  src/MarketPlace.cpp
  src/BurialChamber.cpp
  src/MarketCard.cpp
  # Add your own .cpp here (one per line, no comma)
  # src/myOwnFile.cpp
)

add_executable(imhotep_server ${SOURCES})
target_include_directories(imhotep_server PRIVATE include)

set_target_properties(imhotep_server PROPERTIES 
  CXX_STANDARD 20     # C++20
  CXX_EXTENSIONS OFF  # no compiler specific flags
)

if(MSVC)
  target_compile_options(imhotep_server PRIVATE /W4 /DEBUG)
else()
  target_compile_options(imhotep_server PRIVATE 
    -Wall        # all warnings
    -Wextra      # even more warnings
    -Wshadow     # warnings for shaddowing
    -Wpedantic   # strict adheriance for C++ standard
    -Werror      # treat warnings as errors
    -g -O0       # debug information
  )
endif()

# Add dependencies

set(FRAMEWORK_GIT_FOLDER
    "framework/"
    CACHE STRING "Set to the framework git directory")
add_subdirectory(${FRAMEWORK_GIT_FOLDER} "framework")

target_link_libraries(imhotep_server PRIVATE OOP1_framework)
