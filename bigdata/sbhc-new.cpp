#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::chrono::duration<double> timeelp;
std::chrono::time_point<std::chrono::system_clock> timebeg, timeend;

class mysbh
{
private:
	int block, blocks, i;
	long long j, ones, prod, zeros;
	std::vector<char> run;

public:
	std::ofstream fout;

	void compress(const char* bit, const long long len, const int blocksize)
	{
		timebeg = std::chrono::system_clock::now();
		block = 0;
		blocks = blocksize;
		j = 0;
		ones = 0;
		zeros = 0;
		run.clear();


		for(j = 0; j < len; j++)
		{

			block++;

			if(bit[j] == 0) /* 0-fill. */
			{
				zeros++;
				if(block == blocksize)
				{
					zero(zeros);
					zeros = 0;
				}
			}
			else /* literal. */
			{
				if(zeros > 0)
				{
					zero(zeros);
					zeros = 0;
				}
				run.push_back(bit[j]);
			}
			if(block == blocksize)
			{
				block = 0;
			}
		}
		if(zeros > 0)
		{
			zero(zeros);
			zeros = 0;
		}
		timeend = std::chrono::system_clock::now();
		timeelp += timeend - timebeg;
	}

	void save(const char* filename)
	{
		fout.open(filename);
		fout.write((char*)&blocks, sizeof(int));
		fout.write((char*)&run[0], run.size());
		fout.close();
	}

	void zero(int count) /* 1xxx. */
	{
		while(count > 0x7F)
		{
			run.push_back((count & 0x7F) | 0x80);
			count = count >> 7;
		}
		run.push_back((count & 0x7F) | 0x80);
	}
};

int main(int argc, char* argv[])
{
	char* bit;
	char buff[10], foutname[50];
	int card, k, num_t;
	long long i, j, len;
	int buf;
	mysbh sbh;
	std::ifstream fin;

	if(argc == 3)
	{
		card = std::stoi(argv[2]);
	}
	else if(argc == 2)
	{
		std::cout << "Enter number of cardinality: ";
		std::cin >> card;
	}
	else
	{
		std::cout << "Usage: " << argv[0] << " <filename> (<cardinality>)" << std::endl;
		return 1;
	}

	fin.open(argv[1]);
	if(!fin.is_open())
	{
		std::cout << "Invalid input file." << std::endl;
		return 1;
	}

	fin.seekg(0, fin.end);
	len = fin.tellg();
	fin.seekg(0, fin.beg);
	len /= sizeof(int);

	len /= 7;
	bit = new char[len];

	i = 0;

	for(k = 0; k < 10; k++)
	{
		i = 0;
		num_t = 0;

		for(j = 0; j < len; j++)
		{
			bit[j] = 0;
		}

		while(!fin.eof())
		{
			for(j = 0; j < 7; j++)
			{
				fin.read((char *)&buf, sizeof(int));
				if(buf == k)
				{
					bit[i] = bit[i] | (1 << (6 - j));

					num_t++;
				}

			}
			i++;
			if(i == len) break;
		}
		fin.clear();
		fin.seekg(0, fin.beg);

		sbh.compress(bit, len, 1024);
		sprintf(foutname, "./sbh-new/%d", k);
		sbh.save(foutname);
		std::cout << timeelp.count() << std::endl;

	}

	fin.close();
	return 0;
}
