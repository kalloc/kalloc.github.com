{
	domain = $9;
	sub(/^www\./, "", domain);

	if($10 == 1) {
	    domains[domain] += $8;
	}
	if($10 == 0) {
	    domainsRus[domain] += $8;
	}
        domainsTotal[domain] += $8;
}


END {
    displayTraffic(domainsRus,"русский трафик");
    displayTraffic(domains,"Не русский трафик");
    displayTraffic(domainsTotal,"Весь трафик");
} 

function displayTraffic(domains,type) {

	count=0;
	for(key in domains) {
	    size=int(domains[key]/1024/1024)
	    domains[key]=size
	    domains_dig[count]=size;
	    domains_name[count]=key;
	    domains_size[size]=count;
	    domains_tosort[count]=size;
	    count++
	}


	bsort(domains_tosort,length(domains_tosort));
	printf "\n\n\t%s\n",type
	

	for (i=0;i < length(domains_tosort);i++)
	{
		key=domains_size[domains_tosort[i]]
		if ( int(domains_dig[key]) > 100) {
			printf "%40s - %10s mb\n",domains_name[key],domains_dig[key] 
		}
	}



}


function bsort(A,max) {
	t=1
	while(t) {
	    t=0

	    for (i = 0; i <= max; i++) {
		    if(A[i] < A[i+1]) {
			swap(A,i+1,i);
			t=1
		    }
		    
	    }
	}
	
}
function swap(A, i, j,   t) {
	t = A[i]; A[i] = A[j]; A[j] = t
}

