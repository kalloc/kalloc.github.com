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
array={}
math.randomseed(os.time())
for i=1,25 do
    array[i]=math.random(0,100)
end

io.write("До:    ");for el in pairs(array) do io.write(array[el]," "); end print ""
qsort(array,1,#array)
io.write("После: ");for el in pairs(array) do io.write(array[el]," "); end print ""

