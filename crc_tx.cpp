#include "crc.h"
#define crc12 "1100000001111"
#define rcrc12 "1111000000011"
using namespace std;
bitset<13> compute(bitset<1000>temp) {
	bitset<13>c12(rcrc12), checksum;
	cout <<"----------------------------------" << endl;
	for (int i= 0; i < 13; i++) cout << c12[i];
	cout <<"----------------------------------" << endl;
	//cout <<"original data: " << temp.to_string() << endl;
	temp<<=12;
	int pos = 0;
	for (int i = 0; i < 1000; i++) {
		if (temp[i] == 1) {
			pos = i;
			for (int j = i, k = 0; k < 13 && j < 1000; j++,k++) {
				checksum[k] = temp[j];
			}
			break;
		}
	}
	bitset<1000>ans;
	cout <<"begin at pos: " << pos << endl;
	pos+=13;
	while (pos < 987) {
		cout <<"----------------------------------" << endl;
		for (int i= 0; i < 13; i++) cout << checksum[i];
		cout << endl << "pos: " << pos << endl;
		cout <<"----------------------------------" << endl;
		//cout <<"we got:  " << checksum.to_string() << endl;
		while (checksum[0] == 0) {
			ans[999] = 0;
			ans<<=1;
			checksum<<=1;
			checksum[12] = temp[pos];
			pos++;
			cout <<"pos -> " << pos << endl;
		}
		checksum^=c12;
		ans[999] = 1;
		ans<<=1;
	}
	cout <<"----------------answer-----------------" << endl << endl;
	for (int i= 0; i < 13; i++) cout << checksum[i];
	cout <<"----------------------------------" << endl;
	//cout <<"we got divide: " << ans.to_string() << endl;
	return checksum; 
}
int main () {
	string line;
	vector<bitset<1000> >data;
	ifstream infile("dataTx.txt");
	//start to read file;
	while (getline(infile, line)) {
		bitset<1000>tmp(line);
		/*for (int i = line.size()-1, j = 999; i > -1; i--, j--) {
			if (line[i] == '1') tmp[j] = 1;
			else tmp[j] = 0;
		}*/
		cout << line << endl;
		data.push_back(tmp);
	}
	//lets compute crc checksum
	cout <<"================ start =================" << endl;
	for (int i = 0; i < data.size(); i++) {
		//cout <<"codeword:" << data[i].to_string() << endl;
		//cout <<"crc:" << compute(data[i]).to_string()  << endl;
		compute(data[i]);
	}
	return 0;
}

