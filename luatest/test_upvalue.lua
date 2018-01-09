function lua_func_f1()
    print("lua_func_f1")
    cfunc_f1()
end

function lua_func_f2()
    print("lua_func_f2")
    cfunc_f2()
    lua_func_f1()
end

lua_func_f1()
lua_func_f2()
print("test upvalue finished")


