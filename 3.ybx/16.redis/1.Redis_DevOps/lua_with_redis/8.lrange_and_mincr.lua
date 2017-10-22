local mylist = redis.call("lrange", KEYS[1], 0, -1)

local count = 0

for index,key in ipairs(mylist)
do
	redis.call("incr",key)
	count = count + 1
end
return count
