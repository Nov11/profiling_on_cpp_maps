cmake_minimum_required(VERSION 3.5)
project(order)

set(CMAKE_CXX_STANDARD 11)

#set(CMAKE_CXX_FLAGS
#        "-g -Wall -Wextra -Werror -Wconversion -Wold-style-cast \
#        -Wpointer-arith -Wshadow -Wno-unused-parameter -Wwrite-strings -rdynamic")
set(SOURCE_FILES AVL.cpp Imdb.cpp Order.cpp test.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
add_executable(order ${SOURCE_FILES})
target_link_libraries(order pthread)