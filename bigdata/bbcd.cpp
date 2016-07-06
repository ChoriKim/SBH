#include <bitset>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>

std::chrono::duration<double> timecpu, timeelp;
std::chrono::time_point<std::chrono::system_clock> timebeg, timecpubeg, timecpuend, timeend;

class mybbcd
{
private:
	char buf;
	char* bufflag, * bufvec;
	int hit, i;
	long long j;
	long long* bufcount, * bufnum, * tailcount;
public:
	int a, beg, d ,end, t;
	long long len;
	std::vector<std::ifstream*> streamvec;

	int decompress(void)
	{
		timebeg = std::chrono::system_clock::now();
		hit = 0;
		j = 0;
		bufcount = new long long[end - beg + 1];
		bufflag = new char[end - beg + 1];
		bufnum = new long long[end - beg + 1];
		bufvec = new char[end - beg + 1];
		tailcount = new long long[end - beg + 1];
		for(i = beg; i < end + 1; i++)
		{
			bufcount[i - beg] = -1;
			bufnum[i - beg] = -1;
			tailcount[i - beg] = -1;
		}
		t = 0;
		while(j < len / 4)
		{
			buf = 0;
			for(i = beg; i < end + 1; i++)
			{
				if(bufcount[i - beg] == -1) /* init. */
				{
					streamvec[i - beg]->get(bufvec[i - beg]);
					timecpubeg = std::chrono::system_clock::now();
					if(bufvec[i - beg] < 0) /* case 1. */
					{
						bufcount[i - beg] = (bufvec[i - beg] >> 4) & 0x3;
						if((bufvec[i - beg] & 0x40) != 0) /* 1-fill. */
						{
							bufflag[i - beg] = 0xF;
						}
						else /* 0-fill. */
						{
							bufflag[i - beg] = 0;
						}
						if(bufcount[i - beg] != 0)
						{
							if(bufflag[i - beg] == 0xF) /* 1-fill. */
							{
								buf = buf | 0xFF;
							}
							bufcount[i - beg]--;
							/* If only one byte of fill exists, should move to the tail. */
							if(bufcount[i - beg] == 0)
							{
								/* Tail will be processed in the next loop. */
								tailcount[i - beg] = bufvec[i - beg] & 0xF;
								/* If no tail exists, just generate a new loop. */
								if(tailcount[i - beg] == 0)
								{
									bufcount[i - beg] = -1;
									tailcount[i - beg] = -1;
								}
							}
							timecpuend = std::chrono::system_clock::now();
							timecpu += timecpuend - timecpubeg;
						}
						else /* tail. */
						{
							tailcount[i - beg] = bufvec[i - beg] & 0xF;
							timecpuend = std::chrono::system_clock::now();
							timecpu += timecpuend - timecpubeg;
							/* Tail will be processed in this loop. */
							if(tailcount[i - beg] > 0)
							{
								streamvec[i - beg]->get(bufvec[i - beg]);
								timecpubeg = std::chrono::system_clock::now();
								buf = buf | bufvec[i - beg];
								tailcount[i - beg]--;
								/* Only a single byte of tail, generate a new loop. */
								if(tailcount[i - beg] == 0)
								{
									bufcount[i - beg] = -1;
									tailcount[i - beg] = -1;
								}
							}
							/* Should not happen. */
							else if(tailcount[i - beg] == 0)
							{
								bufcount[i - beg] = -1;
								tailcount[i - beg] = -1;
							}
							timecpuend = std::chrono::system_clock::now();
							timecpu += timecpuend - timecpubeg;
						}
					}
					else if((bufvec[i - beg] & 0x40) != 0) /* case 2. */
					{
						bufcount[i - beg] = (bufvec[i - beg] >> 3) & 0x3;
						if((bufvec[i - beg] & 0x20) != 0) /* 1-fill. */
						{
							bufflag[i - beg] = 0xE;
						}
						else /* 0-fill. */
						{
							bufflag[i - beg] = 0x1;
						}
						if(bufcount[i - beg] != 0)
						{
							if(bufflag[i - beg] == 0xE)
							{
								buf = buf | 0xFF;
							}
							bufcount[i - beg]--;
							/* Odd bit will be processed in the next loop. */
							if(bufcount[i - beg] == 0)
							{
								tailcount[i - beg] = bufvec[i - beg] & 0x7;
							}
						}
						else /* odd bit. */
						{
							/* Odd bit will be processed in this loop. */
							tailcount[i - beg] = bufvec[i - beg] & 0x7;
							tailcount[i - beg] = 1 << tailcount[i - beg];
							if(bufflag[i - beg] == 0xE)
							{
								tailcount[i - beg] = (~tailcount[i - beg]) & 0xFF;
							}
							buf = buf | tailcount[i - beg];
							/* Go to next loop. */
							tailcount[i - beg] = -1;
							bufcount[i - beg] = -1;
						}
						timecpuend = std::chrono::system_clock::now();
						timecpu += timecpuend - timecpubeg;
					}
					else if((bufvec[i - beg] & 0x20) != 0) /* case 3. */
					{
						tailcount[i - beg] = bufvec[i - beg] & 0xF;
						if(tailcount[i - beg] == 0)
						{
							/* No tails. */
							tailcount[i - beg] = -1;
						}
						if((bufvec[i - beg] & 0x10) != 0) /* 1-fill. */
						{
							bufflag[i - beg] = 0xD;
						}
						else /* 0-fill. */
						{
							bufflag[i - beg] = 0x2;
						}
						timecpuend = std::chrono::system_clock::now();
						timecpu += timecpuend - timecpubeg;
						streamvec[i - beg]->get(bufvec[i - beg]);
						timecpubeg = std::chrono::system_clock::now();
						bufcount[i - beg] = 0;
						while(bufvec[i - beg] < 0)
						{
							bufcount[i - beg] = (bufcount[i - beg] << 7) | (bufvec[i - beg] & 0x7F);
							timecpuend = std::chrono::system_clock::now();
							timecpu += timecpuend - timecpubeg;
							streamvec[i - beg]->get(bufvec[i - beg]);
							timecpubeg = std::chrono::system_clock::now();
						}
						bufcount[i - beg] = (bufcount[i - beg] << 7) | (bufvec[i - beg] & 0x7F);
						bufcount[i - beg] += 4;
						if(bufflag[i - beg] == 0xD)
						{
							buf = buf | 0xFF;
						}
						bufcount[i - beg]--;
						timecpuend = std::chrono::system_clock::now();
						timecpu += timecpuend - timecpubeg;
					}
					else /* case 4. */
					{
						tailcount[i - beg] = bufvec[i - beg] & 0x7;
						if((bufvec[i - beg] & 0x8) != 0) /* 1-fill. */
						{
							bufflag[i - beg] = 0xC;
						}
						else /* 0-fill. */
						{
							bufflag[i - beg] = 0x3;
						}
						timecpuend = std::chrono::system_clock::now();
						timecpu += timecpuend - timecpubeg;
						streamvec[i - beg]->get(bufvec[i - beg]);
						timecpubeg = std::chrono::system_clock::now();
						bufcount[i - beg] = 0;
						while(bufvec[i - beg] < 0)
						{
							bufcount[i - beg] = (bufcount[i - beg] << 7) | (bufvec[i - beg] & 0x7F);
							timecpuend = std::chrono::system_clock::now();
							timecpu += timecpuend - timecpubeg;
							streamvec[i - beg]->get(bufvec[i - beg]);
							timecpubeg = std::chrono::system_clock::now();
						}
						bufcount[i - beg] = (bufcount[i - beg] << 7) | (bufvec[i - beg] & 0x7F);
						bufcount[i - beg] += 4;
						if(bufflag[i - beg] == 0xC)
						{
							buf = buf | 0xFF;
						}
						bufcount[i - beg]--;
						timecpuend = std::chrono::system_clock::now();
						timecpu += timecpuend - timecpubeg;
					}
				} /* if(bufcount[i - beg] == -1) */
				else if(bufcount[i - beg] > 0) /* more words to search. */
				{
					timecpubeg = std::chrono::system_clock::now();
					bufcount[i - beg]--;
					if(bufflag[i - beg] > 0xB)
					{
						buf = buf | 0xFF;
					}
					if(bufcount[i - beg] == 0)
					{
						if((bufflag[i - beg] == 0xF) || (bufflag[i - beg] == 0)) /* case 1. */
						{
							tailcount[i - beg] = bufvec[i - beg] & 0xF;
							/* Go to next loop. */
							if(tailcount[i - beg] == 0)
							{
								bufcount[i - beg] = -1;
								tailcount[i - beg] = -1;
							}
						}
						else if((bufflag[i - beg] == 0xE) || (bufflag[i - beg] == 0x1)) /* case 2. */
						{
							tailcount[i - beg] = bufvec[i - beg] & 0x7;
						}
						else if((bufflag[i - beg] == 0xD) || (bufflag[i - beg] == 0x2)) /* case 3. */
						{
							if(tailcount[i - beg] == -1)
							{
								/* Go to next loop. */
								bufcount[i - beg] = -1;
							}
						}
						/* Case 4? */
					}
					timecpuend = std::chrono::system_clock::now();
					timecpu += timecpuend - timecpubeg;
				}
				/* Consider 0 also because the odd bit exists. Looked till here. */
				else if(tailcount[i - beg] >= 0)
				{
					timecpubeg = std::chrono::system_clock::now();
					if((bufflag[i - beg] == 0xC) || (bufflag[i - beg] == 0x3)) /* case 4. */
					{
						tailcount[i - beg] = 1 << tailcount[i - beg];
						if(bufflag[i - beg] == 0xC)
						{
							tailcount[i - beg] = (~tailcount[i - beg]) & 0xFF;
						}
						buf = buf | tailcount[i - beg];
						tailcount[i - beg] = -1;
						bufcount[i - beg] = -1;
					}
					else if((bufflag[i - beg] == 0xE) || (bufflag[i - beg] == 0x1)) /* case 2. */
					{
						tailcount[i - beg] = 1 << tailcount[i - beg];
						if(bufflag[i - beg] == 0xE)
						{
							tailcount[i - beg] = (~tailcount[i - beg]) & 0xFF;
						}
						buf = buf | tailcount[i - beg];
						tailcount[i - beg] = -1;
						bufcount[i - beg] = -1;
					}
					else /* case 1 or 3. */
					{
						if(tailcount[i - beg] > 0)
						{
							bufnum[i - beg]++;
							buf = buf | bufvec[i - beg];
							tailcount[i - beg]--;
						}
						if(tailcount[i - beg] == 0)
						{
							bufcount[i - beg] = -1;
							tailcount[i - beg] = -1;
						}
					}
					timecpuend = std::chrono::system_clock::now();
					timecpu += timecpuend - timecpubeg;
				}
				else /* i.e.: bufcount = 0, tailcount = -1. */
				{
					std::cout << "Should not!" << std::endl;
					return 1;
				}
			} /* for(i = beg; i < end + 1; i++) */
			timecpubeg = std::chrono::system_clock::now();
			a = 0;
			if(j < len / 4)
			{
				if(buf == 0xFF)
				{
					hit += 8;
					a += 8;
				}
				else if(buf != 0)
				{
					for(i = 0; i < 8; i++)
					{
						if(((buf >> i) & 0x1) == 0x1)
						{
							hit++;
							a++;
						}
					}
				}
			}
			else
			{
				if(buf != 0)
				{
					for(i = 0; i < 8 - j + (len / 4); i++)
					{
						if(((buf >> (7 - i)) & 0x1) == 0x1)
						{
							hit++;
						}
					}
				}
			}
			j += 8;
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
	char* buf;
	int hit, i, j;
	long long len;
	mybbcd bbcd;
	std::ifstream *fin;

	bbcd.beg = std::stoi(argv[1]);
	bbcd.end = std::stoi(argv[2]);

	for(i = bbcd.beg; i < bbcd.end + 1; i++)
	{
		sprintf(finname, "bbc/%d", i);
		fin = new std::ifstream(finname);
		fin->read((char *)&bbcd.len, sizeof(long long));
		bbcd.streamvec.push_back(fin);
	}

	hit = bbcd.decompress();

	/* Close files. */
	while(!bbcd.streamvec.empty())
	{
		fin = bbcd.streamvec.back();
		fin->close();
		bbcd.streamvec.pop_back();
	}
	//std::cout << hit << std::endl;
	return 0;
}

