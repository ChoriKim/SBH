#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>

int main(int argc, char* argv[])
{
	if(argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	FILE * src=fopen(argv[1], "w");
	srand((unsigned) time(NULL));

	int attrnum;
	int i;

	printf("Enter cardinality: ");
	scanf("%d",&attrnum);

	int * attr=new int[attrnum];
	int * data=new int[1000000];

	for(i=0; i<attrnum; i++)
	{
		attr[i]=i;
	}

	int random;
	for(i = 0; i < 1000000; i++)
	{ 
		random = rand() % attrnum;
		data[i] = attr[random];
	}

	fwrite(data, sizeof(int), 1000000, src);
	fclose(src);
}
