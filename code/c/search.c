#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <search.h>

char *data[] = { "alpha", "bravo", "charlie", "delta",
     "echo", "foxtrot", "golf", "hotel", "india", "juliet",
     "kilo", "lima", "mike", "november", "oscar", "papa",
     "quebec", "romeo", "sierra", "tango", "uniform",
     "victor", "whisky", "x-ray", "yankee", "zulu"
};
struct timeval a;
struct timeval b;

void h() {

    ENTRY e, *ep;
    int i;

    hcreate(30);
    for (i = 0; i < 24; i++) {
        e.key = data[i];
        e.data = (void *) i;
        ep = hsearch(e, ENTER);
        if (ep == NULL) {
            fprintf(stderr, "entry failed\n");
            return;
        }
    }
    for (i = 22; i < 26; i++) {
        e.key = data[i];
        ep = hsearch(e, FIND);
    }

}

void *root = NULL;

int strcmp(const char*,const char*);

int ts(void)  {
    int i;
    char **val;

    for (i = 0; i < 24; i++) {
	val = data[i];
	val = tsearch(val, &root, (void*)strcmp);
	
    }


    for (i = 22; i < 26; i++) {
	val = data[i];
	val = tfind(val, &root, (void*)strcmp);
    }


}



double get_timeval_delta(struct timeval *after, struct timeval *before) {
    return (double) after->tv_sec - (double) before->tv_sec + (double) after ->tv_usec/1000000.  - (double) before->tv_usec/1000000.;
}
    

int main(void) {
    gettimeofday(&a, NULL);
    h();
    gettimeofday(&b, NULL);
    printf("hsearch: %g\n",((double)get_timeval_delta(&b,&a))*1000000);

    gettimeofday(&a, NULL);
    ts();
    gettimeofday(&b, NULL);
    printf("tsearch: %g\n",((double)get_timeval_delta(&b,&a))*1000000);

}

