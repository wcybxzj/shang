--lua 2.lua

arr={"abc", "def", "hij"}

for i=1, #arr do
	print (arr[i])
end


print ("============")

for index,value in ipairs(arr)
do
	print(index)
	print(value)
end
