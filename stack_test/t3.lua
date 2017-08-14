g_int = 100

function print_t2()
    print "print_t2"
end

function print_t1()
    print "print_t1"
    print_t2()
end

function debug_func()
    g_int = g_int + 1
    local a = 100
    local b = 200
    local c = 300
    print ("a = ", a)
    print_t1()
    a = a + b + c;
    print ("a = ", a)	
    a = a * 100
    print ("a = ", a)	
    print "function t3f3"
end

function init_t3()
    g_ff[1] = function ()
	print "function t3f1"
    end
    g_ff[2] = function ()
	print "function t3f2"
    end
    g_ff[3] = function ()
	g_int = g_int + 1
        local a = 100
	local b = 200
	local c = 300
	print ("a = ", a)
	print_t1()
	a = a + b + c;
	print ("a = ", a)	
	a = a * 100
	print ("a = ", a)	
	print "function t3f3"
    end
    g_ff[4] = debug_func

    test_ff[1] = 11
    test_ff[2] = 22
    test_ff[111] = "adf"
    test_ff["222"] = {1,2,3}
end


init_t3()

