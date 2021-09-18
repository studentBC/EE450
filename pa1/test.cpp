#include "crc.h"
#include <algorithm>
#define crc12 "1100000001111"
#define rcrc12 "1111000000011"
#define rcrc5 "11001"
#define rcrc4 "1001"
#define bl 4
using namespace std;
bitset<bl> compute(bitset<1000>temp, int length) {
	//bitset<13>c12(rcrc12), checksum;
	bitset<bl>c12(rcrc4), checksum;
	cout << "original c12: " <<c12.to_string() << endl;
	cout <<"original data: " << temp.to_string() <<" length is " << length << endl;
	int pos = 0;
	while (!temp[pos]) {
		temp>>=1;
	}
	for (int i= 0; i < 4; i++) checksum[i] = temp[i];
	bitset<1000>ans;
	//cout <<"begin at pos: " << pos << endl;
	pos=4;
	while (pos <= length) {
		/*
		cout <<"==================================" << endl;
		for (int i= 0; i < 5; i++) cout << checksum[i];
		cout << endl << "pos: " << pos << endl;
		cout <<"==================================" << endl;
		*/ 	 
		//cout <<"we got:  " << checksum.to_string() << endl;
		while (!checksum[0]) {
			ans[0] = 0;
			ans<<=1;
			checksum>>=1;
			checksum[3] = temp[pos];
			pos++;
		}
		cout <<"==================================" << endl;
		for (int i= 0; i < 4; i++) cout << checksum[i];
		cout << endl << "pos: " << pos << endl;
		cout <<"==================================" << endl;
		//if (pos+4 >= length) break;
		checksum^=c12;
		ans[0] = 1;
		ans<<=1;
		checksum>>=1;
		checksum[3] = temp[pos];
		pos++;
	}
	//necessary?
	ans >>=1;
	cout <<"----------------answer-----------------" << endl << endl;
	cout <<"crc  : " << checksum.to_string() << endl;
	cout <<"ans  : " << ans.to_string() << endl;
	//for (int i = 0; i < 10; i++) cout << ans[i];
	cout <<endl << "----------------------------------" << endl;
	//cout <<"we got divide: " << ans.to_string() << endl;
	//010111100100
	return checksum; 
}
int main () {
	string line;
	vector<bitset<1000> >data;
	vector<int>len;
	//ifstream infile("dataTx.txt");
	ifstream infile("test.txt");
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
		compute(data[i], len[i]+4);
	}
	return 0;
}

