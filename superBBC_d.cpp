#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <string.h>

using namespace std;

int GetFileLen(FILE *input)
{
	int length = 0;
	fseek(input, 0, SEEK_END);
	length = ftell(input);
	fseek(input, 0, SEEK_SET);
	return length;
}

int decompress(char * dirname, int min, int max)
{
	int range=max-min+1;

	int i=0,j,z=0;
	int size;
	char name[50];
	FILE* src;

	vector<char*> rbuffer;
	max=0;//max is reused
	vector<int> cnt;
	for(i=0; i<range; i++)
	{
		sprintf(name,"%s/%d",dirname,min+i);
		src=fopen(name,"rb");
		if(src==NULL)
			continue;
		size=GetFileLen(src);
		char* tmp=new char[size];
		fread(tmp,sizeof(char),size,src);
		rbuffer.push_back(tmp);
		cnt.push_back(size);
		fclose(src);

	}

	struct timeval start,end;

	range=rbuffer.size();//range is reused

	int* k=new int[range];

	int a=1;

	int hits;
	double time=0;
	int blockindex,superbucket;
	int m,t;

	superbucket=(rbuffer[0][0]*256)/32*7;
	blockindex=(rbuffer[0][0]*256)*7-7;

	int* index=new int[superbucket];

	for(m=0; m<superbucket; m++)
		index[m]=0;
	for(a=0; a<100;a++){
		hits=0;
		z=0;
		for(m=0; m<range; m++)
			k[m]=1;
		gettimeofday(&start,NULL);
		while(k[0]<cnt[0])
		{
			for(m=0; m<range; m++)
			{
				i=-1;

				while(i<blockindex)
				{
					if(rbuffer[m][k[m]]>0)
					{
						i+=7;
						t=31-(i&31);
						index[i>>5]|=rbuffer[m][k[m]]<<t;

						if(t>25)
						{
							t=32-t;
							index[(i>>5)-1]|= rbuffer[m][k[m]]>>t;
						}
						//index is known each bit
					}
					else
					{
						j=(rbuffer[m][k[m]]&0x7f)*7;
						if(!j)
							j=896;
						i+=j;
						//index is known each bit
					}
					k[m]++;
					if(k[m]>=cnt[m])
						break;

				}
			}

			for(j=0; j<superbucket; j++)
			{
				if(index[j]==0xffffffff)
				{
					hits+=32;
					for(i=0; i<32; i++)
					{
						z++;
					}
				}
				else if(index[j]==0)
					z+=32;

				else
				{
					for(t=0; t<32; t++)
					{
						z++;
						if((index[j]<<t)<0)
						{
							hits++;
						}

					}
				}
				index[j]=0;
			}
		}
		gettimeofday(&end,NULL);
		time+=(end.tv_sec-start.tv_sec)*1000 + (double)(end.tv_usec-start.tv_usec)/1000;
	}

	cout<<(double)time/a<<"ms"<<endl;
	printf("%d \n",hits);
	return 0;
}

int main(int argc, char * argv[])
{
	char token[8][50];
	int max, min;
	int count=0;
	char *tok=argv[1];
	tok = strtok(tok, " ");
	if(argc < 2)
	{
		printf("Usage: FROM <compression directory> WHERE <min value> '<' or '<=' <column name> '<' or '<='  <max value>\n");
		return 0;
	}

	while(tok || count < 8)
	{
		strcpy(token[count],tok);
		count++;
		tok=strtok(NULL, " ");
	}
	
	if(count>8 ||strcmp(token[0],"FROM") || strcmp(token[2],"WHERE"))
	{	
		printf("Usage: FROM <compression directory> WHERE <min value> '<' or '<=' <column name> '<' or '<='  <max value>\n");
		return 0;
	}
	min=atoi(token[3]);
	max=atoi(token[7]);
	if(!strcmp(token[4],"<"))
		min++;
	if(!strcmp(token[6],"<"))
		max--;
	if (min>max)
	{
		printf("check the range\n");
		return 0;	
	}
	decompress(token[1], min, max);

	//	}

	return 0;
}
