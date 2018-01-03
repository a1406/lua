remove_definitions(${CFLAGS})
remove_definitions(${CWARNS})
remove_definitions(${LDFLAGS})
add_definitions( -g -O0 -I.. -L..)
link_libraries(m)
link_libraries(dl)
link_libraries(lua)

add_executable  (test1 test1.c)
add_executable  (test2 test2.c script.c)
add_executable  (test_global test_global.c)

