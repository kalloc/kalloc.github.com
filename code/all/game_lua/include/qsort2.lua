require("math")

printf = function (s,...)  return io.write (string.format(s,...)) end

function qsort(array,l,r) 

    local l1,r1,m
    
    if(l>r) then return; end

    m=math.floor((l+r)/2)
    l1=l
    r1=r

    while l<r do
	while array[l]<=array[m] and l<m do
	    l=l+1
	end

	while array[r]>array[m]  do
	    r=r-1
	end
	
	t=array[r]
	array[r]=array[l]
	array[l]=t
	
	if l == m then 
	    m=r 
	elseif r ==  m then 
	    m=l 
	end
	
    end

    if l1 < m-1 then qsort(array,l1,m-1)  end
    if m+1 < r1 then qsort(array,m+1,r1)  end

end



function myqsort(array,l,r) 

    if(l>r) then return; end
    m=l-1
    local i=l

    while i < r do
         if array[i] < array[r] then
             m=m+1

             t=array[m]
             array[m]=array[i]
             array[i]=t
         end
	i=i+1
    end
    m=m+1


    t=array[m]
    array[m]=array[r]
    array[r]=t

    myqsort(array, l, m-1)
    myqsort(array, m+1, r)



end



function mysort(array) 

    local i=1

    while i < #array  do

	if not array[i+1] then 
	    return
	end

	if array[i] > array[i+1] then
	    t=array[i]
	    array[i]=array[i+1]
	    array[i+1]=t
	    i=1
	else 
	    i=i+1
	end
    end
end

math.randomseed(os.time())

local time_start=os.clock()
for z=1,100 do
    array={}
    for i=1,100 do
	array[i]=math.random(0,100)
    end
--   io.write("До:    ");for el in pairs(array) do io.write(array[el]," "); end print ""
--    mysort(array)
--    io.write("После: ");for el in pairs(array) do io.write(array[el]," "); end print ""
end
printf("my sort work %.2f\n",os.clock()-time_start)


local time_start=os.clock()
for z=1,1 do
    array={}
    for i=1,10 do
	array[i]=math.random(0,100)
    end
--   io.write("До:    ");for el in pairs(array) do io.write(array[el]," "); end print ""
    qsort(array,1,#array)
--    io.write("После: ");for el in pairs(array) do io.write(array[el]," "); end print ""
end
printf("qsort work %.2f\n",os.clock()-time_start)

local time_start=os.clock()
for z=1,1 do
    array={}
    for i=1,10 do
	array[i]=math.random(0,100)
    end
--   io.write("До:    ");for el in pairs(array) do io.write(array[el]," "); end print ""
    myqsort(array,1,#array)
--    io.write("После: ");for el in pairs(array) do io.write(array[el]," "); end print ""
end
printf("myqsort work %.2f\n",os.clock()-time_start)


array={}

for i=1,8 do
    array[i]=math.random(0,100)
end



