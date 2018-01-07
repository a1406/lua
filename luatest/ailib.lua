ailib = {}

function ailib.test1(cfunc)
    print("calling ailib.test1")
    cfunc(1, 2)
    cfunc(3, 4)
    cfunc(5, 5)    
end

function ailib.get_monster_num(raid, monster_id)
    local ret = 0
    for i,v in pairs(raid.all_monster) do
	if v.id == monster_id then
	    ret = ret + 1
	end    
    end
    return ret
end

function ailib.check_monster_num(raid, monster_id, monster_num)
    print("enter check monster raid = ", raid)
    while (true) do
	local num = ailib.get_monster_num(raid, monster_id)
	print("num = ", num)
	if (num <= monster_num) then
	    print("finish check monster num")
	    return 0	
	end
	coroutine.yield()
    end
    print("bug, you should not be here")
end

all_raid = {}
raid_func = nil
function ailib.create_raid(id)
    raid = {}
    raid.handle = coroutine.create(raid_func)
    raid.all_monster = {}
    raid.id = id
    all_raid[id] = raid
    print(string.format("create raid %d %s", id, raid))
    coroutine.resume(raid.handle, raid)
--    table.insert(all_raid, raid)
--    return raid
end

function ailib.get_raid(id)
    return all_raid[id]
end

function ailib.set_raid_fun(func)
    raid_func = func
end

function ailib.add_monster(raid, monster_id, uuid)
    monster = {}
    monster.id = monster_id
    monster.uuid = uuid
    raid.all_monster[uuid] = monster
end
function ailib.del_monster(raidid, monster_id, uuid)
    print(string.format("del monster %s %s %s", raidid, monsterid, uuid));
    raid = ailib.get_raid(raidid)
    if (raid == nil) then
	print(string.format("can not get raid %d", raidid))
	return -1
    end
    if (uuid ~= 0) then
	raid.all_monster[uuid] = nil
	print(string.format("resume raid %s", raid))
	coroutine.resume(raid.handle)	
	return 0
    end
    
    for i,v in pairs(raid.all_monster) do
	if v.id == monster_id then
	    raid.all_monster[v.uuid] = nil
	    coroutine.resume(raid.handle)
	    return 0
	end    
    end
    return -1
end
return ailib
