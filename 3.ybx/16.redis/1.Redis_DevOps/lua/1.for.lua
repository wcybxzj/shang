#!/usr/bin/lua

arr = {"abc", "def",1111}
for i=1,3 do
	print(arr[i])
end

print ("============")

local int sum = 0
for i=1,100 do
	sum = sum+i
end

print (sum)
