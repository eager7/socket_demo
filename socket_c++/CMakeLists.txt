cmake_minimum_required(VERSION 3.5)
set(TARGET "my_socket")
project(socket_c__)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
aux_source_directory(mSocket/ SOURCE_FILES)
aux_source_directory(mThread/ SOURCE_FILES)
#set(SOURCE_FILES mSocket/Address.cpp mSocket/CommonSocket.cpp mSocket/Datagram.cpp mSocket/SocketException.cpp mSocket/TCP.cpp mSocket/UDP.cpp)


###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
# include_directories(${PROJECT_SOURCE_DIR}/src/Interface/CoreStructures/Inc)
include_directories(
        ./mSocket/
        ./mThread/
)

link_directories(./lib)

## Declare a C++ executable
add_executable(${TARGET} ${SOURCE_FILES})

## Specify libraries to link a library or executable target against
target_link_libraries( ${TARGET}  libpthread.so )

#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

## Mark executable scripts (Python etc.) for installation
## in contrast to setup.py, you can choose the destination
# install(PROGRAMS
#   scripts/my_python_script
#   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark executables and/or libraries for installation
# install(TARGETS eyesight_node eyesight_node_node
#   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark cpp header files for installation
# install(DIRECTORY include/${PROJECT_NAME}/
#   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#   FILES_MATCHING PATTERN "*.h"
#   PATTERN ".svn" EXCLUDE
# )

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(FILES
#   # myfile1
#   # myfile2
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )