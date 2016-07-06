#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>

std::chrono::duration<double> timecpu, timeelp;
std::chrono::time_point<std::chrono::system_clock> timebeg, timecpubeg, timecpuend, timeend;

class mywahd
{
private:
	int i;
	long long buf, hit, j;
	long long* bufcount, * bufflag, * bufvec;
public:
	int beg, end;
	long long len;
	std::vector<std::ifstream*> streamvec;

	int decompress(void)
	{
		timebeg = std::chrono::system_clock::now();
		hit = 0;
		j = 0;
		bufcount = new long long[end - beg + 1];
		bufflag = new long long[end - beg + 1];
		bufvec = new long long[end - beg + 1];
		for(i = beg; i < end + 1; i++)
		{
			bufcount[i - beg] = 0;
			//bufnum[i - beg] = -1;
		}
		while(j < len / 4)
		{
			buf = 0;
			for(i = beg; i < end + 1; i++)
			{
				if(bufcount[i - beg] == 0) /* init. */
				{
					//bufnum[i - beg]++;
					//streamvec[i - beg]->get((char *)bufvec[i - beg], sizeof(long long));
					streamvec[i - beg]->read(reinterpret_cast<char *>(&bufvec[i - beg]), sizeof(long long));
					timecpubeg = std::chrono::system_clock::now();
					if(bufvec[i - beg] < 0) /* fill. */
					{
						bufcount[i - beg] = bufvec[i - beg] & 0x3FFFFFFFFFFFFFFF;
						if((bufvec[i - beg] & 0x4000000000000000) != 0) /* 1-fill. */
						{
							bufflag[i - beg] = 0x7FFFFFFFFFFFFFFF;
							buf = buf | bufflag[i - beg];
						}
						else /* 0-fill. */
						{
							bufflag[i - beg] = 0;
						}
						bufcount[i - beg]--;
					}
					else /* literal. */
					{
						buf = buf | bufvec[i - beg];
					}
				}
				else /* more words to search. */
				{
					timecpubeg = std::chrono::system_clock::now();
					bufcount[i - beg]--;
					buf = buf | bufflag[i - beg];
				}
				timecpuend = std::chrono::system_clock::now();
				timecpu += timecpuend - timecpubeg;
			}
			timecpubeg = std::chrono::system_clock::now();
			j += 63;
			if(j < len / 4)
			{
				if(buf == 0x7FFFFFFFFFFFFFFF)
				{
					hit += 63;
				}
				else if(buf != 0)
				{
					for(i = 1; i < 64; i++)
					{
						if((buf << i) < 0)
						{
							hit++;
						}
					}
				}
			}
			else
			{
				if(buf != 0)
				{
					for(i = 1; i < 64 - j + (len / 4); i++)
					{
						if((buf << i) < 0)
						{
							hit++;
						}
					}
				}
			}
			timecpuend = std::chrono::system_clock::now();
			timecpu += timecpuend - timecpubeg;
		}
		timeend = std::chrono::system_clock::now();
		timeelp = timeend - timebeg;
		std::cout << "CPU+I/O: " << timeelp.count() << std::endl;
		std::cout << "CPU: " << timecpu.count() << std::endl;
		return hit;
	}
};

int main(int argc, char* argv[])
{
	char finname[50];
	int hit, i, j;
	long long len;
	long long* buf;
	mywahd wahd;
	std::ifstream* fin;

	wahd.beg = std::stoi(argv[1]);
	wahd.end = std::stoi(argv[2]);

	for(i = wahd.beg; i < wahd.end + 1; i++)
	{
		sprintf(finname, "wah/%d", i);
		fin = new std::ifstream(finname);
		fin->read((char *)&wahd.len, sizeof(long long));
		len = (len - 4) / sizeof(long long);
		wahd.streamvec.push_back(fin);
	}

	hit = wahd.decompress();
	//std::cout << hit << std::endl;

	/* Close files. */
	while(!wahd.streamvec.empty())
	{
		fin = wahd.streamvec.back();
		fin->close();
		wahd.streamvec.pop_back();
	}
	return 0;
}
