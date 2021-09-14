// Sautto, Marcello  CS230 Section 12159  Date completed: 4/27
// Third Laboratory Assignment – Cache Simulation

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

using namespace std;

#define ADDRESS_SIZE 16
#define BLOCK_SIZE 4

struct address_struct
{

	int tempAddress = 0, tempData = 0, bitOffsetSize = 0, bitOffsetExponent = 0, bitOffset = 0, cacheLineBits = 0, cacheIndex = 0, cacheIndexExponent = 0;
	string tag = "", binaryAddressString = "";

	vector<int> tempAddressBinary;

	address_struct(int cacheLineSize, int cacheIndex, int cacheTotalSize, int cacheAssociativity, int mainMemorySize, int bitOffsetSize, int cacheLineBitSize, int tagSize, int tAddress)
	{
		tempAddress = tAddress;

		tempAddressBinary.clear();

		//cout << "Data In Binary: ";
		//convert current address to binary
		for (int temp = tempAddress; temp > 0; temp /= 2)
			tempAddressBinary.emplace_back(temp % 2);

		tempAddressBinary.resize(16);

		ostringstream oss, convertBinStr;

		for (int x = 0; x < bitOffsetSize; x++)
		{
			bitOffset += tempAddressBinary[x] * pow(2, x);
			//if (tempAddressBinary[x] == 1)
				//bitOffsetExponent = x - 1; //sub 1 to index properly    
		}

		bitOffsetExponent = (bitOffset / BLOCK_SIZE);
		for (int y = bitOffsetSize; y < ADDRESS_SIZE - tagSize; y++)
		{
			cacheLineBits += tempAddressBinary[y] * pow(2, y - bitOffsetSize);
		}

		for (int z = ADDRESS_SIZE - 1; z >= bitOffsetSize + cacheLineBitSize; z--)
		{
			oss << tempAddressBinary[z];
		}
		tag = oss.str();


		reverse(tempAddressBinary.begin(), tempAddressBinary.end());

	}

};

struct writeBackSim
{
	int cacheLineSize = 0, cacheTotalSize = 0, cacheAssociativity = 0, mainMemorySize = 0, cacheIndex = 0, bitOffsetSize = 0, cacheLineBitSize = 0, tagSize = 0;

	vector<vector<int>> memory; //contains main memory
	vector<vector<int>> cache; //contains cache
	vector<vector<string>> tagV; //tag array
	vector<vector<bool>> validDirtyVector; //valid/dirty vector
	vector<string> memoryAdd;
	vector<string> cacheLineBitV;

	vector<int> binaryAddress = { 0 };
	writeBackSim(int cls, int cts, int ca, int mms)
	{
		cacheLineSize = cls;
		cacheTotalSize = cts;
		cacheAssociativity = ca;
		mainMemorySize = mms;

		//check if any commands A-D are invalid
		if (cacheLineSize == 0 || cacheTotalSize == 0 || cacheAssociativity == 0 || mainMemorySize == 0)
		{
			cout << "ERROR: Command A, B, C, or D omitted" << endl;
			exit(EXIT_FAILURE);
		}
		else if (cacheLineSize > cacheTotalSize * 0.1)
		{
			cout << "Invalid Cache Line Size" << endl;
			exit(EXIT_FAILURE);
		}
		else if (cacheTotalSize > mainMemorySize * 0.1)
		{
			cout << "Invalid Cache Size: Too Large" << endl;
			exit(EXIT_FAILURE);
		}

		cacheIndex = cacheTotalSize / cacheLineSize / cacheAssociativity;
		//initialize memory, cache, and associativity
		memory.resize(mainMemorySize / cacheLineSize, vector<int>(ADDRESS_SIZE / BLOCK_SIZE));
		memoryAdd.resize(mainMemorySize / cacheLineSize);
		cacheLineBitV.resize(mainMemorySize / cacheLineSize);
		cache.resize(cacheLineSize, vector<int>(cacheAssociativity * BLOCK_SIZE));
		tagV.resize(cacheLineSize, vector<string>(cacheAssociativity));
		validDirtyVector.resize(cacheLineSize, vector<bool>(cacheAssociativity * 2));

		for (int i = 0; pow(2, i) <= cacheLineSize; i++)
			bitOffsetSize = i;

		for (int i = 0; pow(2, i) <= cacheIndex; i++)
			cacheLineBitSize = i;

		tagSize = cacheLineSize - bitOffsetSize - cacheLineBitSize;

		for (int i = 0; i < memoryAdd.size(); i++) //creates memory addresses in binary
		{
			ostringstream memAddConv, cacheLineBitConv;
			binaryAddress.clear();
			for (int j = i * ADDRESS_SIZE; j > 0; j /= 2)
			{
				binaryAddress.emplace_back(j % 2);
			}

			binaryAddress.resize(ADDRESS_SIZE);
			reverse(binaryAddress.begin(), binaryAddress.end());

			for (int k = 0; k < tagSize; k++)
			{
				memAddConv << binaryAddress[k];
			}
			memoryAdd[i] = memAddConv.str();

			for (int l = tagSize; l < ADDRESS_SIZE - bitOffsetSize; l++)
			{
				cacheLineBitConv << binaryAddress[l];
			}
			cacheLineBitV[i] = cacheLineBitConv.str();

		}


	}
};

class WriteBackCache
{
public:

	void writeCache(int tempAddress, int tempData, writeBackSim& wbs, address_struct& astruct),
		readCache(int tempAddress,writeBackSim& wbs, address_struct& astruct),
		commandF(int tempAddress, writeBackSim& wbs, address_struct& astruct),
		printCache(writeBackSim& wbs),
		printMemory(writeBackSim& wbs);

};


int main()
{
	char tempChar, writeOrRead;
	int tempAddress = 0, tempData = 0;
	int simValues[4] = { 0 };
	vector<vector<bool>> lastUsedCacheLine;
	WriteBackCache wbc;
	ifstream in("data.txt");

	//Validate variables A-D
	while (in >> tempChar && in >> tempData)
	{
		if (tempData > 0 && tempData % 2 == 0)
		{
			if (tempChar == 'A') //Line size of the cache
				simValues[0] = tempData;

			else if (tempChar == 'B') //Total size of the cache
				simValues[1] = tempData;

			else if (tempChar == 'C') //Associativity of the cache
				simValues[2] = tempData;

			else if (tempChar == 'D') //Total size of main memory
			{
				simValues[3] = tempData;
				break; //done reading commands
			}
		}
		else
			break;

	}

	//carry out commands E and F if A-D are valid
	writeBackSim wbs(simValues[0], simValues[1], simValues[2], simValues[3]);

	while (!in.eof())
	{
		in >> tempChar;

		if (tempChar == 'G')
			wbc.printCache(wbs);


		else if (tempChar == 'H')
			wbc.printMemory(wbs);


		else
		{
			in >> tempAddress;

			//takes address and manipulates it to index the data in the cache/memory
			address_struct astruct(wbs.cacheLineSize, wbs.cacheIndex, wbs.cacheTotalSize, wbs.cacheAssociativity, wbs.mainMemorySize, wbs.bitOffsetSize, wbs.cacheLineBitSize, wbs.tagSize, tempAddress);

			if (tempChar == 'E') //Address, type of access, and data for simulated memory accesses.
			{
				in >> writeOrRead;
				

				if (writeOrRead == 'W') //write data to cache
				{
					in >> tempData;
					wbc.writeCache(tempAddress, tempData, wbs, astruct);
				}
					

				//check if memory address in cache contains the respective data
				//if not, check the main memory
				//if not there, we got a cache miss
				else if (writeOrRead == 'R')
					wbc.readCache(tempAddress, wbs, astruct);
			}
			else if (tempChar == 'F') //Address of data to be displayed
				wbc.commandF(tempAddress, wbs, astruct);
		}

		tempChar = 0;

	}


	return 0;
}


void WriteBackCache::printCache(writeBackSim& wbs)
{
	cout << endl;
	cout << "=======TAGS======V/D========CACHE==================== " << endl << endl;
	for (int it1 = 0; it1 < wbs.cache.size(); it1++)
	{

		for (int i = 0; i < wbs.cacheAssociativity; i++)
		{
			if (wbs.tagV[it1][i] != "")
				cout << wbs.tagV[it1][i] << " ";
			else
				cout << "       ";
		}

		for (int i = 0; i < wbs.cacheAssociativity * 2; i += 2)
		{
			cout << "  " << wbs.validDirtyVector[it1][i] << wbs.validDirtyVector[it1][i + 1];
		}
		for (int it2 = 0; it2 < wbs.cache[it1].size(); it2++)
		{
			if (it2 % BLOCK_SIZE == 0)
				cout << " | ";

			cout << wbs.cache[it1][it2] << " ";
		}

		cout << endl;
	}


}

void WriteBackCache::printMemory(writeBackSim& wbs)
{
	cout << endl;

	cout << "=======ADDRESSES=======  =============MEMORY=============" << endl << endl;
	for (int i = 0; i < wbs.memory.size(); i++)
	{
		cout << i * ADDRESS_SIZE << "----";
		cout << wbs.memoryAdd[i] << wbs.cacheLineBitV[i] << "____";
		for (int j = 0; j < wbs.memory[i].size(); j++)
		{
			if (j % BLOCK_SIZE == 0)
				cout << " | ";

			cout << wbs.memory[i][j] << " ";
		}
		cout << endl;
	}
}

void WriteBackCache::writeCache(int tempAddress, int tempData, writeBackSim& wbs, address_struct& astruct)
{
	int i = 0;
	while (i < wbs.cacheAssociativity) //look for a spot to put data in the cache line
	{
		if (wbs.cache[astruct.cacheLineBits][astruct.bitOffsetExponent + BLOCK_SIZE * i] == 0 && (wbs.tagV[astruct.cacheLineBits][i] == astruct.tag || wbs.tagV[astruct.cacheLineBits][i] == ""))
		{
			wbs.cache[astruct.cacheLineBits][astruct.bitOffsetExponent + BLOCK_SIZE * i] = tempData;
			wbs.tagV[astruct.cacheLineBits][i] = astruct.tag;
			wbs.validDirtyVector[astruct.cacheLineBits][(2 * i)] = wbs.validDirtyVector[astruct.cacheLineBits][(2*i) + 1] = 1;
			break;
		}
		i++;
	}

	if (i == wbs.cacheAssociativity) //if our cache is full, we must move the oldest associative cache line to memory
	{
		for (int i = 0; i < wbs.memory.size(); i++) //while we havent found a place in the memory for the data, keep looking
		{
			if (wbs.memoryAdd[i + astruct.cacheLineBits] == wbs.tagV[astruct.cacheLineBits][(astruct.bitOffset + wbs.cacheAssociativity) % wbs.cacheAssociativity]) //if the current memory line isnt occupied and we are looking at the least recently used cache, evict the cache line and store the incoming data
			{
				for (int j = 0; j < BLOCK_SIZE; j++) //write cache line to memory and clear current contents of cache line
				{
					wbs.memory[i + astruct.cacheLineBits][j] = wbs.cache[astruct.cacheLineBits][j + BLOCK_SIZE * ((i + wbs.cacheAssociativity) % wbs.cacheAssociativity)];
					wbs.cache[astruct.cacheLineBits][j + BLOCK_SIZE * ((i + wbs.cacheAssociativity) % wbs.cacheAssociativity)] = 0;
				}

				wbs.cache[astruct.cacheLineBits][astruct.bitOffsetExponent] = tempData;
				wbs.tagV[astruct.cacheLineBits][(i + wbs.cacheAssociativity) % wbs.cacheAssociativity] = astruct.tag;
				break;
			}
		}
	}

}

void WriteBackCache::readCache(int tempAddress, writeBackSim& wbs, address_struct& astruct)
{
	//do something
	int i = 0;
	while (i < wbs.cacheAssociativity) //look for a spot to put data in the cache line
	{
		if (wbs.tagV[astruct.cacheLineBits][i] == astruct.tag)
		{
			cout << "Requested data is currently at address " << astruct.tempAddress << " inside the cache. " << endl << endl;
			break;
		}
		i++;
	}

	if (i == wbs.cacheAssociativity) //if our cache is full, we must move the oldest associative cache line to memory
	{
		for (int k = 0; k < wbs.memory.size(); k++) //store current cache line to memory
		{
			if (wbs.memoryAdd[k] == wbs.tagV[astruct.cacheLineBits][(i + wbs.cacheAssociativity) % wbs.cacheAssociativity])
			{
				for (int l = 0; l < BLOCK_SIZE; l++)
				{
					wbs.memory[k + astruct.cacheLineBits * ((k + wbs.cacheAssociativity) % wbs.cacheAssociativity)][l] = wbs.cache[astruct.cacheLineBits][l + BLOCK_SIZE * ((k + wbs.cacheAssociativity) % wbs.cacheAssociativity)];
				}
				break;

			}

		}
		for (int i = 0; i < wbs.memory.size(); i++) //find the address in the main memory and load it into the cache
		{
			if (wbs.memoryAdd[i] == astruct.tag) //if the current memory line isnt occupied and we are looking at the least recently used cache, evict the cache line and store the incoming data
			{
				for (int j = 0; j < BLOCK_SIZE; j++) //write cache line to memory and clear current contents of cache line
				{
					wbs.cache[astruct.cacheLineBits][j + BLOCK_SIZE * ((i + wbs.cacheAssociativity) % wbs.cacheAssociativity)] = wbs.memory[i + astruct.cacheLineBits * ((i + wbs.cacheAssociativity) % wbs.cacheAssociativity)][j];
				}
				wbs.tagV[astruct.cacheLineBits][((i + wbs.cacheAssociativity) % wbs.cacheAssociativity)] = astruct.tag;
				wbs.validDirtyVector[astruct.cacheLineBits][(wbs.cacheAssociativity * i) / wbs.cacheAssociativity] = 1;
				wbs.validDirtyVector[astruct.cacheLineBits][(((wbs.cacheAssociativity * i)) / wbs.cacheAssociativity) + 1] = 0;
				break;
			}
		}
	}
}

void WriteBackCache::commandF(int tempAddress, writeBackSim& wbs, address_struct& astruct)
{
	bool isValid = false;
	cout << "Address: " << tempAddress;
	for (int i = 0; i < wbs.cacheAssociativity; i++) //look for data in cache
	{
		if (wbs.tagV[astruct.cacheLineBits][i] == astruct.tag)
		{
			isValid = true;
			cout << " Cache: " << wbs.cache[astruct.cacheLineBits][astruct.bitOffsetExponent + BLOCK_SIZE * i] << " ";
			break;
		}
	}
	if (!isValid) //if the data isnt in the cache, print error
		cout << " Cache: -1 ";
	isValid = false;
	for (int i = 0; i < wbs.memory.size(); i++) //cache miss. pull data from memory
	{
		if (wbs.memoryAdd[i] == astruct.tag)
		{
			isValid = true;
			cout << "Memory: " << wbs.memory[i + astruct.cacheLineBits][astruct.bitOffsetExponent] << endl << endl;
			break;
		}
	}

	if (!isValid)
		cout << "Memory: -1 " << endl << endl;
}
