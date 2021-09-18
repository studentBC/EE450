#include "crc.h"
#include <algorithm>
#define crc12 "1100000001111"
#define rcrc12 "1111000000011"
using namespace std;
bitset<12> compute(bitset<1000>temp, int length) {
	bitset<13>c12(rcrc12), checksum;
	int pos = 0;
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
	bitset<12>answer;
	for (int i= 0; i < 12; i++) answer[i] = checksum[i];
	return answer; 
}
int main () {
	string line;
	vector<bitset<1000> >data;
	vector<int>len;
	vector<string>st;
	ifstream infile("dataTx.txt");
	while (getline(infile, line)) {
		st.push_back( line);
		reverse(line.begin(), line.end());
		bitset<1000>tmp(line);
		len.push_back(line.size());
		data.push_back(tmp);
	}
	//lets compute crc checksum
	for (int i = 0; i < data.size(); i++) {
		bitset<12>ans = compute(data[i], len[i]+12);
		string sans = ans.to_string();
		reverse(sans.begin(), sans.end());
		cout <<"codeword:"<< endl << st[i];
		cout << sans;
		cout << endl;
		cout <<"crc:"<<endl << sans  << endl;
	}
	return 0;
}

