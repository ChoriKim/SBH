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

	void compress(const bool* bit, const long long len, const int blocksize)
	{
		timebeg = std::chrono::system_clock::now();
		block = 0;
		blocks = blocksize;
		//card = len * 4;
		j = 0;
		ones = 0;
		zeros = 0;
		run.clear();

		while(j < len)
		{
			prod = 0;
			for(i = 0; i < 7; i++)
			{
				if(j >= len)
				{
					break;
				}
				if(bit[j])
				{
					prod = prod | (1 << (6 - i));
				}
				j++;
			}
			block++;

			if(prod == 0) /* 0-fill. */
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
				run.push_back(prod);
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
	bool* bit;
	char buff[10], foutname[50];
	int card, k, num_t;
	long long i, j, len;
	int* buf;
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


	len = std::count(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>(), '\n');
	fin.seekg(0, fin.beg);
	bit = new bool[len];
	i = 0;
	while(!fin.eof())
	{
		fin.getline(buff, 10);
		buf[i] = std::atoi(buff);
		i++;
		if(i == len) break;
	}

	for(k = 0; k < card; k++)
	{
		num_t = 0;
		for(j = 0; j < len; j++)
		{
			if(k == buf[j])
			{
				bit[j] = true;
				num_t++;
			}
			else
			{
				bit[j] = false;
			}
		}
		sbh.compress(bit, len, 1024);
		sprintf(foutname, "./sbh/%d", k);
		sbh.save(foutname);
	}
	std::cout << timeelp.count() << std::endl;
	fin.close();
	return 0;
}
