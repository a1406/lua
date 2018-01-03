CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
project(lua)

FILE(GLOB SRC_LIST
  "./*.c"
  )
list(REMOVE_ITEM SRC_LIST "${CMAKE_CURRENT_SOURCE_DIR}/./lua.c")
#message(${SRC_LIST})

set (CFLAGS -Wall -g  -O0 -std=c99 -DLUA_USE_LINUX -DLUA_COMPAT_5_2)
set (CWARNS -pedantic -Wextra -Wshadow -Wsign-compare -Wundef -Wwrite-strings -Wredundant-decls -Wdisabled-optimization -Waggregate-return -Wdouble-promotion   -Wdeclaration-after-statement -Wmissing-prototypes -Wnested-externs -Wstrict-prototypes -Wc++-compat -Wold-style-definition)
set (LDFLAGS -g -Wl,-E)

add_definitions(${CFLAGS})
add_definitions(${CWARNS})
add_definitions(${LDFLAGS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
link_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_library  (lua ${SRC_LIST})
add_subdirectory(stack_test)

FILE(GLOB SRC_LIST
  "./lua.c"
  )

link_libraries(lua)
link_libraries(m)
link_libraries(dl)
link_libraries(readline)
add_executable  (clua ${SRC_LIST})
set_target_properties(clua PROPERTIES OUTPUT_NAME "lua")
