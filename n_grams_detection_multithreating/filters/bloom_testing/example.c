#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../bloom_filter.h"
#include <time.h>

int main(int argc, char **argv) {
	filter set;
	filter_init(&set,500);
	int i,counter;
	clock_t start,stop;
	void* input;

	
	printf("TESTING FILTER\n");
	input=NULL;
	counter=0;
	start=clock();
	for(i=0;i<1000;i++){
		counter+=f_append(&set,input);
		input++;
	}
	stop=clock();
	printf("%d Inserts in %d cycles\n\n",counter,(int)(stop-start));

	counter=0;
	input=NULL;
	start=clock();
	for(i=0;i<1000;i++){
		counter+=(int)f_lookup(&set,input);
		input++;
	}
	stop=clock();
	printf("%d Checks in %d cycles\n",1000,(int)(stop-start));
	printf("Expected: %d, Actual: %d\n\n",1000,counter);

	counter=0;
	input=NULL;
	start=clock();
	for(i=0;i<100000;i++){
		counter+=(int)f_lookup(&set,input);
		input++;
	}
	stop=clock();
	printf("%d Checks in %d cycles\n",100000,(int)(stop-start));
	printf("Expected: %d, Actual: %d\n\n",1000,counter);

	counter=0;
	input=NULL;
	start=clock();
	for(i=0;i<10000000;i++){
		counter+=(int)f_lookup(&set,input);
		input++;
	}
	stop=clock();
	printf("%d Checks in %d cycles\n",10000000,(int)(stop-start));
	printf("Expected: %d, Actual: %d\n\n",1000,counter);
	filter_fin(&set);
}
