cmake_minimum_required (VERSION 3.0)

project (PanelUI)

# Add the local ‘include’ directory and the wiringPi directory to grab headers
include_directories (include ../include)

# Actually find the wiringPi library object
find_library(WPI_LIB wiringPi)

# Alert the user if we do not find it
if(NOT WPI_LIB)
 message(FATAL_ERROR “wiringPi library not found”)
endif()

# Add all the *.c files in our source directory to our executable output
FILE(GLOB SRC_FILES src/*.cpp)

add_executable(PanelUI ${SRC_FILES})

# Link the pre-compiled wiringPi library to the executable we just declared 
target_link_libraries(PanelUI ${WPI_LIB})
