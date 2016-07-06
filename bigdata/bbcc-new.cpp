#include <algorithm>
#include <bitset>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::chrono::duration<double> timeelp;
std::chrono::time_point<std::chrono::system_clock> timebeg, timeend;

class mybbc
{
private:
	bool ftag;
	char prod;
	int card, cases, i, oddzero, oddone;
	long long j, onen, ones, zeron, zeros;
	std::vector<char> run, tail;

public:
	std::ofstream fout;

	void compress(const char* bit, const long long len)
	{
		timebeg = std::chrono::system_clock::now();
		cases = 0;
		card = len * 4;
		ftag = false;
		j = 0;
		ones = 0;
		zeros = 0;
		run.clear();

		//while(true/*j < len*/)
		for(j = 0; j < len; j++)
		{
			onen = 0;
			prod = 0;
			zeron = 0;
			for(i = 7; i > -1; i--)
			{
				if(j >= len)
				{
					if((zeros == 0) && (ones == 0))
					{
						cases = 1;
					}
					else if((zeros < 4) && (ones == 0))
					{
						cases = 1;
					}
					else if((zeros > 3) && (ones == 0))
					{
						cases = 3;
					}
					else if((ones < 4) && (zeros == 0))
					{
						cases = 2;
					}
					else
					{
						cases = 4;
					}
					//tail.push_back(prod);
					tail.push_back(bit[j]);
					ftag = true;
					timeend = std::chrono::system_clock::now();
					timeelp += timeend - timebeg;
					break;
				} /* if(j >= len) */
				//prod = prod | (bit[j] << i);
				if((bit[j] << i))
				{
					onen++;
					oddone = i;
				}
				else
				{
					zeron++;
					oddzero = i;
				}
				//j++;
			}
			if(cases != 0)
			{
				if(((bit[j] & 0xFF) == 0) || ((bit[j] & 0xFF) == 0xFF) || (j >= len))
				{
					if((j >= len) && (!ftag))
					{
						tail.push_back(bit[j]);
						ftag = true;
					}
					switch(cases)
					{
					case 1:
						while(tail.size() > 15)
						{
							type1(0, zeros, 15);
							run.insert(run.end(), tail.begin(), tail.begin() + 15);
							tail.erase(tail.begin(), tail.begin() + 15);
							zeros = 0;
						}
						type1(0, zeros, tail.size());
						run.insert(run.end(), tail.begin(), tail.end());
						zeros = 0;
						break;
					case 2:
						while(tail.size() > 15)
						{
							type1(1, ones, 15);
							run.insert(run.end(), tail.begin(), tail.begin() + 15);
							tail.erase(tail.begin(), tail.begin() + 15);
							ones = 0;
						}
						type1(1, ones, tail.size());
						run.insert(run.end(), tail.begin(), tail.end());
						ones = 0;
						break;
					case 3:
						if(tail.size() > 15)
						{
							type3(0, zeros, 15);
							run.insert(run.end(), tail.begin(), tail.begin() + 15);
							tail.erase(tail.begin(), tail.begin() + 15);
							zeros = 0;
							while(tail.size() > 15)
							{
								type1(0, zeros, 15);
								run.insert(run.end(), tail.begin(), tail.begin() + 15);
								tail.erase(tail.begin(), tail.begin() + 15);
							}
							type1(0, zeros, tail.size());
							run.insert(run.end(), tail.begin(), tail.end());
						}
						else
						{
							type3(0, zeros, tail.size());
							run.insert(run.end(), tail.begin(), tail.end());
						}
						zeros = 0;
						break;
					case 4:
						if(tail.size() > 15)
						{
							type3(1, ones, 15);
							run.insert(run.end(), tail.begin(), tail.begin() + 15);
							tail.erase(tail.begin(), tail.begin() + 15);
							ones = 0;
							while(tail.size() > 15)
							{
								type1(1, ones, 15);
								run.insert(run.end(), tail.begin(), tail.begin() + 15);
								tail.erase(tail.begin(), tail.begin() + 15);
							}
							type1(1, ones, tail.size());
							run.insert(run.end(), tail.begin(), tail.end());
						}
						else
						{
							type3(1, ones, tail.size());
							run.insert(run.end(), tail.begin(), tail.end());
						}
						ones = 0;
						break;
					}
					tail.clear();
					cases = 0;
					zeros = 0;
					ones = 0;
					if(j >= len)
					{
						return;
					}
				} /* if(prod...) */
				else
				{
					tail.push_back(bit[j]);
					continue;
				}
			} /* if(cases != 0) */

			if((bit[j] & 0xFF) == 0) /* 0-fill. */
			{
				zeros++;
				if(ones > 0)
				{
					/* Process one first. */
					if(zeros < 4)
					{
						//cases = 1;
						type1(1, ones, 0);
					}
					else
					{
						//cases = 3;
						type3(1, ones, 0);
					}
					//tail.push_back(0);
					ones = 0;
				}
				continue;
			}
			if((bit[j] & 0xFF) == 0xFF) /* 1-fill. */
			{
				ones++;
				if(zeros > 0)
				{
					/* Process zero first. */
					if(ones < 4)
					{
						//cases = 2;
						type1(0, zeros, 0);
					}
					else
					{
						//cases = 4;
						type3(0, zeros, 0);
					}
					zeros = 0;
				}
				continue;
			}
			if((onen == 1) && (ones == 0)) /* 0-fill with 1 odd. */
			{
				if(zeros < 4)
				{
					type2(0, zeros, oddone);
				}
				else
				{
					type4(0, zeros, oddone);
				}
				cases = 0;
				zeros = 0;
				continue;
			}
			if((zeron == 1) && (zeros == 0)) /* 1-fill with 0 odd. */
			{
				if(ones < 4)
				{
					type2(1, ones, oddzero);
				}
				else
				{
					type4(1, ones, oddzero);
				}
				cases = 0;
				ones = 0;
				continue;
			}
			if((zeros == 0) && (ones == 0))
			{
				cases = 1;
			}
			else if((zeros < 4) && (ones == 0))
			{
				cases = 1;
			}
			else if((zeros > 3) && (ones == 0))
			{
				cases = 3;
			}
			else if((ones < 4) && (zeros == 0))
			{
				cases = 2;
			}
			else
			{
				cases = 4;
			}
			tail.push_back(bit[j]);
		}
		timeend = std::chrono::system_clock::now();
		timeelp += timeend - timebeg;
	}

	void save(const char* filename)
	{
		fout.open(filename);
		fout.write((char*)&card, sizeof(int));
		fout.write(&run[0], run.size());
		fout.close();
	}

	void type1(char fillb, int filll, int tail)
	{
		char intm;
		intm = 1;
		intm = intm << 1 | fillb;
		intm = intm << 2 | filll;
		intm = intm << 4 | tail;
		run.push_back(intm);
	}

	void type2(char fillb, int filll, char odd)
	{
		char intm;
		intm = 1;
		intm = intm << 1 | fillb;
		intm = intm << 2 | filll;
		intm = intm << 3 | odd;
		run.push_back(intm);
	}

	void type3(char fillb, long long filll, int tail)
	{
		char intm;
		intm = 1;
		intm = intm << 1 | fillb;
		intm = intm << 4 | tail;
		filll -= 4;
		run.push_back(intm);

		if(filll > 0x3FFFFFFFFFF)
		{
			run.push_back(0x80 | (char)(filll >> 42));
		}
		if(filll > 0x7FFFFFFFF)
		{
			run.push_back(0x80 | (char)(filll >> 35));
		}
		if(filll > 0xFFFFFFF)
		{
			run.push_back(0x80 | (char)(filll >> 28));
		}
		if(filll > 0x1FFFF)
		{
			run.push_back(0x80 | (char)(filll >> 21));
		}
		if(filll > 0x3FFF)
		{
			run.push_back(0x80 | (char)(filll >> 14));
		}
		if(filll > 0x7F)
		{
			run.push_back(0x80 | (char)(filll >> 7));
		}
		run.push_back(filll & 0x7F);
	}

	void type4(char fillb, long long filll, char odd)
	{
		char intm;
		intm = 1;
		intm = intm << 1 | fillb;
		intm = intm << 3 | odd;
		filll -= 4;
		run.push_back(intm);

		if(filll > 0x3FFFFFFFFFF)
		{
			run.push_back(0x80 | (char)(filll >> 42));
		}
		if(filll > 0x7FFFFFFFF)
		{
		        run.push_back(0x80 | (char)(filll >> 35));
		}
		if(filll > 0xFFFFFFF)
		{
			run.push_back(0x80 | (char)(filll >> 28));
		}
		if(filll > 0x1FFFF)
		{
			run.push_back(0x80 | (char)(filll >> 21));
		}
		if(filll > 0x3FFF)
		{
			run.push_back(0x80 | (char)(filll >> 14));
		}
		if(filll > 0x7F)
		{
			run.push_back(0x80 | (char)(filll >> 7));
		}
		run.push_back(filll & 0x7F);
	}
};

int main(int argc, char* argv[])
{
	char* bit;
	char buff[10], foutname[50];
	int buf, card, k;
	//int *buf;
	long long i, j, len, num_t;
	mybbc bbc;
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

        //len = std::count(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>(), '\n');
        fin.seekg(0, fin.end);
	len = fin.tellg();
	fin.seekg(0, fin.beg);
	len /= sizeof(int);
	len /= 8;
        bit = new char[len];
        //buf = new int[len];
        //fin.read((char*)buf, len * sizeof(int));
        i = 0;
	/* Obsolete.
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
			for(j = 0; j < 8; j++)
			{
				fin.read((char *)&buf, sizeof(int));
				if(buf == k)
				{
					bit[i] = bit[i] | (1 << (7 - j));
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

		bbc.compress(bit, len);
		sprintf(foutname, "./bbc-new/%d", k);
		bbc.save(foutname);
		std::cout << timeelp.count() << std::endl;
		//std::cout << i << " " << num_t << std::endl;
	}

	//std::cout << timeelp.count() << std::endl;
	fin.close();
	return 0;
}
