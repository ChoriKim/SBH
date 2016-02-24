#include "superBBC.h"

void CBBC::zero(char cnt)
{
	cnt = cnt | 0x80; //This means fill = 1 and value = 0(10xxxxxx).
	run.push_back(cnt);

}

void CBBC::one(char cnt)
{
	cnt = cnt | 0xc0; //This means fill = 1 and value = 1(11xxxxxx).
	run.push_back(cnt);

}

void CBBC::BBC_alg(bool* clients, int size, FILE * dst, int bucketsize)
{
	char temp;
	int cnt_0=0, cnt_1=0, j, k = 0, bucket = 0;

	run.clear();

	while(1)//k is the number of input
	{
		temp=0;
		for(j=0; j<7; j++)
		{
			if(k>=size)	
				break;
			temp = temp | (clients[k]<< (6-j));
			k++;
		}
		bucket++;
		

		if((temp&0xff) == 0)
		{
			cnt_0++;
			
			if(cnt_0==128)
			{
				cnt_0=0;
				zero(cnt_0);
			}
			else if(bucket==bucketsize)
			{
				zero(cnt_0);
				cnt_0=0;
			}	

		}
		else
		{	
			if( cnt_0 > 0)
			{
				zero(cnt_0);	
				cnt_0=0;
			}

		run.push_back(temp);
		}
		if(k>=size)	
			break;
		if(bucket==bucketsize)
			bucket=0;
	}
		if( cnt_0 > 0)
			zero(cnt_0);

		fputc(bucketsize/256,dst);//write bucketsize
		fwrite(&run[0],sizeof(char),run.size(),dst);
		fclose(dst);

}

int CBBC::GetFileLen(FILE *input)
{
	int length = 0;
	fseek(input, 0, SEEK_END);
	length = ftell(input);
	fseek(input, 0, SEEK_SET);
	return length;
}

int main(int argc, char* argv[])
{
	if(argc != 3) {
		printf("Usage: %s <CSV file> <destionation folder>\n", argv[0]);
		exit(1);
	}

	//file input
	CBBC bbc;
	FILE* src=fopen(argv[1],"rb");
	char locate[50];

	int len=bbc.GetFileLen(src);
	FILE* dst;//=fopen(argv[2],"wb");
	len /= 4;
	int* buffer=new int[len];
	bool* clients=new bool[len];
	int i;
	int j;
	int k = 0;
	int l = 0;
	int bucket;
	int crd = 0;
	int runlen;
	fread(buffer,sizeof(int),len,src);

	for(i = 0; i < len; i++) {
		if(buffer[i] > crd) {
			crd = buffer[i];
		}
	}
	crd++;
	printf("Measured cardinality is %d.\n", crd);
	printf("# of bucket (256, 512, 1024) : ");
	scanf("%d", &bucket);
	for(k=0; k<crd; k++)
	{
		sprintf(locate,"%s/%d",argv[2],k);
		dst=fopen(locate,"wb");

		for(i=0; i<len; i++)
		{
			if(k==buffer[i])
			{
				clients[i]=true;
			}
			else
				clients[i]=false;
		}

		bbc.BBC_alg(clients,len,dst,bucket);
	}

	fclose(src);
	return 0;
}
