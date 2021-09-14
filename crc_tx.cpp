#include "crc.h"
#include <algorithm>
#define crc12 "1100000001111"
#define rcrc12 "1111000000011"
using namespace std;
bitset<13> compute(bitset<1000>temp, int length) {
	bitset<13>c12(rcrc12), checksum;
	//cout << "original c12: " <<c12.to_string() << endl;
	//cout <<"original data: " << temp.to_string() << endl;
	int pos = 0;
	while (!temp[pos]) {
		temp>>=1;
	}
	for (int i= 0; i < 13; i++) checksum[i] = temp[i];
	bitset<1000>ans;
	//cout <<"begin at pos: " << pos << endl;
	pos=13;
	while (pos < length) {
		
		cout <<"==================================" << endl;
		for (int i= 0; i < 13; i++) cout << checksum[i];
		cout << endl << "pos: " << pos << endl;
		cout <<"==================================" << endl;
			 
		//cout <<"we got:  " << checksum.to_string() << endl;
		while (!checksum[0]) {
			ans[0] = 0;
			ans<<=1;
			checksum>>=1;
			checksum[12] = temp[pos];
			pos++;
		}
		checksum^=c12;
		ans[0] = 1;
		ans<<=1;
	}
	cout <<"----------------answer-----------------" << endl << endl;
	//for (int i= 0; i < 13; i++) cout << checksum[i];
	cout <<"crc  : " << checksum.to_string() << endl;
	cout <<"----------------------------------" << endl;
	//cout <<"we got divide: " << ans.to_string() << endl;
	//010111100100
	return checksum; 
}
int main () {
	string line;
	vector<bitset<1000> >data;
	vector<int>len;
	ifstream infile("dataTx.txt");
	//start to read file;
	while (getline(infile, line)) {
		cout << line << endl;
		reverse(line.begin(), line.end());
		bitset<1000>tmp(line);
		len.push_back(line.size());
		//cout << tmp.to_string() << endl;
		data.push_back(tmp);
	}
	//lets compute crc checksum
	cout <<"================ start =================" << endl;
	for (int i = 0; i < data.size(); i++) {
		//cout <<"codeword:" << data[i].to_string() << endl;
		//cout <<"crc:" << compute(data[i]).to_string()  << endl;
		compute(data[i], len[i]+12);
	}
	return 0;
}

