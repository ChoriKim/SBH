#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>

std::chrono::duration<double> timecpu, timeelp;
std::chrono::time_point<std::chrono::system_clock> timebeg, timecpubeg, timecpuend, timeend;

class mysbhd
{
private:
	bool comp;
	char buf;
	char * bufvec;
	int hit, i, j, k, t;
	int* bucket;
	long long* bufnum;
public:
	int beg, end;
	//std::vector<long long> filelen;
	std::vector<std::ifstream*> streamvec;

	int decompress(int blocksize)
	{
		//timebeg = std::chrono::system_clock::now();
		hit = 0;
		bucket = new int[blocksize];
		bufnum = new long long[end - beg + 1];
		bufvec = new char[end - beg + 1];
		for(i = 0; i < blocksize; i++)
		{
			bucket[i] = 0;
		}
		for(i = beg; i < end + 1; i++)
		{
			streamvec[i - beg]->get(bufvec[i - beg]);
			//bufnum[i - beg] = 0;
		}
		timebeg = std::chrono::system_clock::now();
		while(!streamvec[0]->eof())
		{
			for(i = beg; i < end + 1; i++)
			{
				comp = false;
				j = -1;
				while(j < ((blocksize / 7) << 3) * 7 - 7)
				{
					timecpubeg = std::chrono::system_clock::now();
					if(bufvec[i - beg] > 0) /* literal. */
					{
						j += 7;
						t = 31 - (j & 0x1F);
						bucket[j >> 5] = bucket[j >> 5] | (bufvec[i - beg] << t);
						comp = false;
						if(t > 25)
						{
							t = 32 - t;
							bucket[(j >> 5) - 1] = bucket[(j >> 5) - 1] | (bufvec[i - beg] >> t);
						}
					}
					else /* 0-fill. */
					{
						if(comp)
						{
							k = ((bufvec[i - beg] & 0x7F) << 7) * 7;
						}
						else
						{
							k = (bufvec[i - beg] & 0x7F) * 7;
							comp = true;
						}
						j += k;
					}
					timecpuend = std::chrono::system_clock::now();
					timecpu += timecpuend - timecpubeg;
					streamvec[i - beg]->get(bufvec[i - beg]);
					//bufnum[i - beg]++;
					if(streamvec[i - beg]->eof())
					{
						break;
					}
				}
			}

			timecpubeg = std::chrono::system_clock::now();
			for(i = 0; i < blocksize; i++)
			{
				if(bucket[i] == 0xFFFFFFFF)
				{
					hit += 32;
				}
				else
				{
					for(k = 0; k < 32; k++)
					{
						if((bucket[i] << k) < 0)
						{
							hit++;
						}
					}
				}
				bucket[i] = 0;
			}
			timecpuend = std::chrono::system_clock::now();
			timecpu += timecpuend - timecpubeg;
		}
		timeend = std::chrono::system_clock::now();
		timeelp = timeend - timebeg;
		std::cout << "CPU + I/O: " << timeelp.count() << std::endl;
		std::cout << "CPU: " << timecpu.count() << std::endl;
		return hit;
	}
};

int main(int argc, char* argv[])
{
	char finname[50];
	int blocksize, hit, i, j;
	long long len;
	mysbhd sbhd;
	std::ifstream* fin;

	sbhd.beg = std::stoi(argv[1]);
	sbhd.end = std::stoi(argv[2]);

	for(i = sbhd.beg; i < sbhd.end + 1; i++)
	{
		sprintf(finname, "sbh/%d", i);
		fin = new std::ifstream(finname);
		fin->read((char *)&blocksize, sizeof(int));
		sbhd.streamvec.push_back(fin);
	}

	hit = sbhd.decompress((blocksize >> 3) * 7);

	while(!sbhd.streamvec.empty())
	{
		fin = sbhd.streamvec.back();
		fin->close();
		sbhd.streamvec.pop_back();
	}
	//std::cout << hit << std::endl;
	return 0;
}
