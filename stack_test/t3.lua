function init_t3()
    g_ff[1] = function ()
	print "function t3f1"
    end
    g_ff[2] = function ()
	print "function t3f2"
    end
    g_ff[3] = function ()
	local a = 100
	print ("a = ", a)
	a = a + 200;
	print ("a = ", a)	
	a = a * 100
	print ("a = ", a)	
	print "function t3f3"
    end
    
end


init_t3()

