#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "bloom_filter.h"
#include <time.h>

int main(int argc, char **argv) {
	b_filter obj;
	bf_init(&obj);
	filter set;
	f_init(&set);
	int i,counter;
	clock_t start,stop;
	void* input;

	printf("TESTING BLOOM FILTER\n");
	input=NULL;
	counter=0;
	start=clock();
	for(i=0;i<CAPACITY;i++){
		counter+=bf_append(&obj,input);
		input++;
	}
	stop=clock();
	printf("%d Inserts in %d cycles\n\n",counter,(int)(stop-start));

	counter=0;
	input=NULL;
	start=clock();
	for(i=0;i<CAPACITY;i++){
		counter+=(int)bf_lookup(&obj,input);
		input++;
	}
	stop=clock();
	printf("%d Checks in %d cycles\n",CAPACITY,(int)(stop-start));
	printf("Expected: %d, Actual: %d\n\n",CAPACITY,counter);

	//bf_print(&obj);
	
	counter=0;
	input=NULL;
	start=clock();
	for(i=0;i<10000;i++){
		counter+=(int)bf_lookup(&obj,input);
		input++;
	}
	stop=clock();
	printf("%d Checks in %d cycles\n",10000,(int)(stop-start));
	printf("Expected: %d, Actual: %d\n\n",CAPACITY,counter);


	counter=0;
	input=NULL;
	start=clock();
	for(i=0;i<100000;i++){
		counter+=(int)bf_lookup(&obj,input);
		input++;
	}
	stop=clock();
	printf("%d Checks in %d cycles\n",100000,(int)(stop-start));
	printf("Expected: %d, Actual: %d\n\n",CAPACITY,counter);

	counter=0;
	input=NULL;
	start=clock();
	for(i=0;i<1000000;i++){
		counter+=(int)bf_lookup(&obj,input);
		input++;
	}
	stop=clock();
	printf("%d Checks in %d cycles\n",1000000,(int)(stop-start));
	printf("Expected: %d, Actual: %d\n\n",CAPACITY,counter);


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
	for(i=0;i<1000000;i++){
		counter+=(int)f_lookup(&set,input);
		input++;
	}
	stop=clock();
	printf("%d Checks in %d cycles\n",1000000,(int)(stop-start));
	printf("Expected: %d, Actual: %d\n\n",1000,counter);
	f_fin(&set);
}
