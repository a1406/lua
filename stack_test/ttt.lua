g_ff = {}
test_ff = {}

function recv_func(id)
    if (g_ff[id] ~= nil)
    then
	g_ff[id]()
    end
end    
