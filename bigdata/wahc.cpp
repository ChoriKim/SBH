#include <algorithm>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::chrono::duration<double> timeelp;
std::chrono::time_point<std::chrono::system_clock> timebeg, timeend;

class mywah
{
private:
	char upper;
	int i, numupper = 0;
	long long card, j, ones, prod, zeros;
	std::vector<long long> run;

public:
	std::ofstream fout;

	void compress(const long long* bit, const long long len)
	{
		timebeg = std::chrono::system_clock::now();
		card = len * 4;
		j = 0;
		ones = 0;
		zeros = 0;
		run.clear();

		//while(j < len)
		for(j = 0; j < len; j++)
		{
			//prod = 0;
			/*
			for(i = 0; i < 63; i++)
			{
				if(j >= len)
				{
					break;
				}
				if(bit[j])
				{
					prod = prod | (1LL << (62 - i));
				}
				j++;
			}
			*/

			if(bit[j] == 0) /* 0-fill. */
			{
				zeros++;
				if(ones > 0)
				{
					one(ones);
					ones = 0;
				}
				if(zeros == 0x3FFFFFFFFFFFFFFF)
				{
					zero(zeros);
					zeros = 0;
				}
			}
			else if(bit[j] == 0x7FFFFFFFFFFFFFFF) /* 1-fill. */
			{
				ones++;
				if(zeros > 0)
				{
					zero(zeros);
					zeros = 0;
				}
				if(ones == 0x3FFFFFFFFFFFFFFF)
				{
					one(ones);
					ones = 0;
				}
			}
			else /* literal. */
			{
				if(zeros > 0)
				{
					zero(zeros);
					zeros = 0;
				}
				if(ones > 0)
				{
					one(ones);
					ones = 0;
				}
				run.push_back(bit[j]);
			}
		}
		if(zeros > 0)
		{
			zero(zeros);
			zeros = 0;
		}
		if(ones > 0)
		{
			one(ones);
			ones = 0;
		}
		timeend = std::chrono::system_clock::now();
		timeelp += timeend - timebeg;
	}

	void one(long count) /* 11xxx. */
	{
		count = count | 0xC000000000000000;
		run.push_back(count);
	}

	void save(const char* filename)
	{
		fout.open(filename);
		fout.write((char*)&card, sizeof(long long));
		fout.write((char*)&run[0], run.size() * sizeof(long long));
		fout.close();
	}

	void zero(long count) /* 10xxx. */
	{
		count = count | 0x8000000000000000;
		run.push_back(count);
	}
};

int main(int argc, char* argv[])
{
	//bool* bit;
	char buff[10], foutname[50];
	long long *bit;
	int buf, card, k, num_t;
	//int* buf;
	long long i, j, len;
	mywah wah;
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
	if(!fin)
	{
		std::cout << "Invalid input file." << std::endl;
		return 1;
	}

	fin.seekg(0, fin.end);
	len = fin.tellg();
	fin.seekg(0, fin.beg);
	len /= sizeof(int);
	len /= 63;
	//len = std::count(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>(), '\n');
        //fin.seekg(0, fin.beg);
	bit = new long long[len];
	//buf = new int[len];
	//fin.read((char*)buf, len * sizeof(int));
	i = 0;
	/*
	while(!fin.eof())
        {
                fin.getline(buff, 10);
                buf[i] = std::atoi(buff);
                i++;
                if(i == len) break;
        }
	*/
	//num_t = 0;
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
			for(j = 0; j < 63; j++)
			{
				fin.read((char *)&buf, sizeof(int));
				if(buf == k)
				{
					bit[i] = bit[i] | (1LL << (62 - j));
					//bit[j] = true;
					num_t++;
				}
			/*
			else
			{
				bit[j] = false;
			}
			*/
			}
			i++;
			if(i == len) break;
		}
		fin.clear();
		fin.seekg(0, fin.beg);

		wah.compress(bit, len);
		sprintf(foutname, "./wah-new/%d", k);
		wah.save(foutname);
		std::cout << timeelp.count() << std::endl;
	}
	//std::cout << timeelp.count() << std::endl;
	fin.close();
	return 0;
}
