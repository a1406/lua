CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(lua)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
ADD_DEFINITIONS(-DLUA_USE_LINUX -DLUA_COMPAT_5_2 -g -O0)
SET (CFLAGS -std=c99)
SET (CWARNS -pedantic -Wextra -Wshadow -Wsign-compare -Wundef -Wwrite-strings -Wredundant-decls -Wdisabled-optimization -Waggregate-return -Wdouble-promotion   -Wdeclaration-after-statement -Wmissing-prototypes -Wnested-externs -Wstrict-prototypes -Wc++-compat -Wold-style-definition)
SET (LDFLAGS -Wl,-E)

FUNCTION (build_lualib)
  FILE(GLOB SRC_LIST
    "./*.c"
    )
  LIST(REMOVE_ITEM SRC_LIST "${CMAKE_CURRENT_SOURCE_DIR}/./lua.c")
  #MESSAGE(${SRC_LIST})
  ADD_LIBRARY  (lualib ${SRC_LIST})

  SET (LUALIB_FLAGS ${CWARNS} ${CFLAGS} ${CXXFLAGS} ${LDFLAGS})
  SET_TARGET_PROPERTIES(lualib PROPERTIES
    COMPILE_OPTIONS "${LUALIB_FLAGS}"
    OUTPUT_NAME "lua"      
    )
ENDFUNCTION()  

FUNCTION (build_lua)
  FILE(GLOB SRC_LIST
    "./lua.c"
    )
  ADD_EXECUTABLE  (clua ${SRC_LIST})
  
  TARGET_LINK_LIBRARIES(clua lua)
  TARGET_LINK_LIBRARIES(clua m)
  TARGET_LINK_LIBRARIES(clua dl)
  TARGET_LINK_LIBRARIES(clua readline)

  SET (LUA_FLAGS ${CWARNS} ${CFLAGS} ${CXXFLAGS} ${LDFLAGS})  
  SET_TARGET_PROPERTIES(clua PROPERTIES
    COMPILE_OPTIONS "${LUA_FLAGS}"
    OUTPUT_NAME "lua"      
    )
ENDFUNCTION()  

ADD_SUBDIRECTORY(stack_test)
ADD_SUBDIRECTORY(luatest)
build_lua()
build_lualib()
