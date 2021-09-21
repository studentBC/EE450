#include "crc.h"
#include <algorithm>
#define crc12 "1100000001111"
#define rcrc12 "1111000000011"
using namespace std;
bitset<13> compute(bitset<1000>temp, int length) {
	//the first bits for crc will be on the bitset[0], we maintain 13 bits for carry bit
	bitset<13>c12(rcrc12), checksum;
	//the variable to record Quotient position
	int pos = 0;
	//initialize 
	for (int i= 0; i < 13; i++) checksum[i] = temp[i];
	bitset<1000>ans;
	pos=12;
	while (pos < length) {
		while (!checksum[0] && pos < length) {
			ans[pos] = 0;
			pos++;
			checksum>>=1;
			checksum[12] = temp[pos];
		}
		 if (pos < length && checksum[0]) {
			checksum^=c12;
			ans[pos] = 1;
		}
	} 
	return checksum; 
}
int main () {
	string line;
	//record input data bit 
	vector<bitset<1000> >data;
	//record input data bit length
	vector<int>len;
	ifstream infile("dataRx.txt");
	//ifstream infile("bug.txt");
	//start to read file;
	while (getline(infile, line)) {
		//reverse string so that we could make sure that the first bit will be in bitset[0]
		reverse(line.begin(), line.end());
		bitset<1000>tmp(line);
		len.push_back(line.size());
		data.push_back(tmp);
	}
	for (int i = 0; i < data.size(); i++) {
		if (!compute(data[i], len[i]+12).count()) cout <<"pass" << endl;
		else cout << "not pass" << endl;
	}
	return 0;
}

