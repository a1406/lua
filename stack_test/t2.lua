function init_t2()
    g_ff[1] = function ()
	print "function t2f1"
    end
    g_ff[2] = function ()
	print "function t2f2"
	dofile ("t3.lua")	
	dofile ("noexist.lua")
	print "function t2f2 finished"
    end
    g_ff[3] = function ()
	print "function t2f3"
    end
    
end


init_t2()

