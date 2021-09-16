#include "crc.h"
#include <algorithm>
#define crc12 "1100000001111"
#define rcrc12 "1111000000011"
using namespace std;
bitset<13> compute(bitset<1000>temp, int length) {
	bitset<13>c12(rcrc12), checksum;
	//cout <<"length is " << length << endl;
	//cout << "original c12: " <<c12.to_string() << endl;
	//cout <<"original data: " << temp.to_string() << endl;
	int pos = 0;
	/*while (!temp[pos]) {
		temp>>=1;
	}*/ 
	for (int i= 0; i < 13; i++) checksum[i] = temp[i];
	bitset<1000>ans;
	//cout <<"cs: "<< checksum.to_string() << endl;
	//cout <<"begin at pos: " << pos << endl;
	pos=12;
	while (pos < length) {
		/*
		cout <<"==================================" << endl;
		for (int i= 0; i < 13; i++) cout << checksum[i];
		cout << endl << "pos: " << pos << endl;
		cout <<"==================================" << endl;
		*/  
		//cout <<"we got:  " << checksum.to_string() << endl;
		while (!checksum[0] && pos < length) {
			ans[pos] = 0;
			pos++;
			checksum>>=1;
			checksum[12] = temp[pos];
		}
		/* 
		cout <<"==================================" << endl;
		for (int i= 0; i < 13; i++) cout << checksum[i];
		cout << endl << "pos: " << pos << endl;
		cout << endl <<"==================================" << endl;
		if (pos+1 == length) {
			cout <<"## pos is " << pos << endl;
			break;
		} else*/ if (pos < length && checksum[0]) {
			checksum^=c12;
			ans[pos] = 1;
		}
	} 
	cout <<"----------------answer-----------------" << endl << endl;
	//checksum <<=1;
	//cout <<"crc  : " << checksum.to_string() << endl;
	for (int i= 0; i < 12; i++) cout << checksum[i];
	//cout <<"ans  : " << ans.to_string() << endl;
	cout <<endl <<"----------------------------------" << endl;
	//cout <<"we got divide: " << ans.to_string() << endl;
	//010111100100
	return checksum; 
}
bitset<8> internetChecksum (vector<bitset<9> > bt) {
	bool carry = false;
	for (int i = 1; i < bt.size(); i++) {
		carry = false;
		for (int j = 0; j < 9; j ++) {
			if (carry) bt[0][j] = 1;
			if (bt[0][j]&bt[i][j]) {
				bt[0][j] = 0;
				carry = true;
			} else if (bt[0][j] | bt[i][j]) {
				bt[0][j] = 1;
				carry = false;
			} else {
				bt[0][j] = 0;
				carry = false;
			}
		}
		while (bt[0][8]) {
			bt[0][8] = 0;
			for (int j = 0; j < 9; j ++) {
				if (bt[0][j]) {
					bt[0][j] = 0;
				} else {
					bt[0][j] = 1;
					break;
				}
			}
		}
	}
	bitset<8>ans;
	for (int i = 0; i < 8; i++) ans[i] = bt[0][i];
	return ans;
}
int main () {
	string line, flip;
	int count = 0;
	vector<bitset<1000> >data, wrong;
	vector<int>len;
	ifstream infile("dataVs.txt");
	//ifstream infile("bug.txt");
	//start to read file;
	while (getline(infile, line)) {
		istringstream iss(line);
		iss >> line >> flip;
		cout << line <<" error: " << flip << endl;
		reverse(line.begin(), line.end());
		bitset<1000>tmp(line);
		bitset<1000>temp(line);
		len.push_back(line.size());
		//cout << tmp.to_string() << endl;
		for (int i = 0; i < len.back(); i++) {
			if (flip[i] == '1') temp.flip(i);
		}
		data.push_back(tmp);
		wrong.push_back(temp);
	}
	//lets compute crc checksum
	cout <<"================ start =================" << endl;
	for (int i = 0; i < data.size(); i++) {
		bitset<13>crcOutcome = compute(data[i], len[i]+12);
		string scrc, crcvalid;
		scrc.pop_back();
		for (int k = len[i], j = 0; j < 12 ;k++, j++) {
			wrong[i][k] = crcOutcome[j];
			if (crcOutcome[j]) scrc.push_back('1');
			else scrc.push_back('0');
		}
		if (compute(wrong[i], len[i]+12).count() == 0) crcvalid = "pass";
		else crcvalid = "not pass";
		cout << "crc: "<< scrc <<" result: "<< crcvalid << endl;
		//start to compute 2 parity checksum
		count = len[i]/8;
		vector<bitset<9> >group, ng;
		for (int a = 0, j = 0; a < count; a++) {
			bitset<9>tt, ff; //true and false data
			for (int k = 7; k > -1; k--, j++) {
				tt[k] = data[i][j]; 
				ff[k] = wrong[i][j];
			}
			group.push_back(tt);
			ng.push_back(ff);
		}
		bitset<8>cks = internetChecksum(group);
		bitset<8>fcks = internetChecksum(ng);
		
		if (cks == fcks) crcvalid = "pass";
		else crcvalid = "not pass";
		cout << "checksum: "<< cks.to_string() <<" result: "<< crcvalid << endl;
	}
	return 0;
}

