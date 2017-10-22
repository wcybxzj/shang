local tables userinfo = {age=28, name="dabing"}
print("user age is"..userinfo["age"])


print ("=============================")


for key,value in pairs(userinfo)
do
	print (key.." "..value)
end
