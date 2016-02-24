#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
using namespace std;

class CBBC
{
public:
//	CBBC();
//	~CBBC();

	void BBC_alg(bool*,int,FILE*,int);
	int GetFileLen(FILE *input);
	vector<char> run;
	void zero(char cnt);
	void one(char cnt);
};
