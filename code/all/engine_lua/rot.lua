local aaa={10000,6000,2200,3400}

local max=0
local min=99999999999999
for k,v in pairs(aaa) do
    if max < v then
	max=v
    end

    if min > v then
	min=v
    end
end


local=
print(max,min)