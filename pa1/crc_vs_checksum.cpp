#include "crc.h"
#include <algorithm>
#define crc12 "1100000001111"
#define rcrc12 "1111000000011"
#define DEBUG false
using namespace std;
//reusage function to compute crc checksum
bitset<13> compute(bitset<1000>temp, int length) {
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
#if DEBUG
	cout <<"----------------answer-----------------" << endl << endl;
	for (int i= 0; i < 12; i++) cout << checksum[i];
	cout <<endl <<"----------------------------------" << endl;
#endif
	return checksum; 
}

//function to compute checksum (group by one byte)
//we make input bitset length 9 bits so that we could compute carrybit
bitset<8> internetChecksum (vector<bitset<9> > bt) {
	for (int i = 1; i < bt.size(); i++) {
#if DEBUG
		cout <<"====== check bitset now ======" << endl;
		for (int i = 0; i < bt.size(); i++) 
			for (int j = 0; j < 9; j++) cout << bt[i][j];
			cout << endl;
		cout << endl;
#endif 
		int carry = 0;
		for (int j = 0; j < 9; j ++) {
			if (carry) { //if previous step has carry bit
				carry--;
				if (!bt[0][j]) {
					bt[0][j] = 1;
				} else {
					bt[0][j] = 0;
					carry++;
				}
			}
			if (bt[0][j]&bt[i][j]) {
				bt[0][j] = 0;
				carry++;
			} else if (bt[0][j] | bt[i][j]) {
				bt[0][j] = 1;
			} else {
				bt[0][j] = 0;
			}
		}
#if DEBUG
		cout <<"####### result sum #########" << endl;
		for (int j = 0; j < 9; j++) {
			cout << bt[0][j];
		}
		cout << endl <<"###########################" << endl;
#endif
		//there might be a carry bit after summing up so we just assume that carry bit exist and compute until it disappear
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
#if DEBUG
		cout <<"####### result sum #########" << endl;
		for (int j = 0; j < 9; j++) {
			cout << bt[0][j];
		}
		cout << endl <<"###########################" << endl;
#endif
	}
	bitset<8>ans;
	//flip everybit and output the checksum
	for (int i = 0; i < 8; i++) ans[i] = !bt[0][i];
	return ans;
}
//use the given checksum and group of bytes data to compute whether this checksum is valid or not
bool validChecksum(vector<bitset<9> > bt, bitset<9>checksum) {
	for (int i = 1; i < bt.size(); i++) {
#if DEBUG
		cout <<"====== check bitset now ======" << endl;
		for (int i = 0; i < bt.size(); i++) 
			for (int j = 0; j < 9; j++) cout << bt[i][j];
			cout << endl;
		cout << endl;
#endif 
		int carry = 0;
		for (int j = 0; j < 9; j ++) {
			if (carry) {
				carry--;
				if (!bt[0][j]) {
					bt[0][j] = 1;
				} else {
					bt[0][j] = 0;
					carry++;
				}
			}
			if (bt[0][j]&bt[i][j]) {
				bt[0][j] = 0;
				carry++;
			} else if (bt[0][j] | bt[i][j]) {
				bt[0][j] = 1;
			} else {
				bt[0][j] = 0;
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
#if DEBUG
		cout <<"####### result sum #########" << endl;
		for (int j = 0; j < 8; j++) cout << bt[0][j];
		cout << endl;
		for (int j = 0; j < 8; j++) cout << checksum[j];
		cout << endl <<"###########################" << endl;
#endif

	for (int i = 0; i < 8; i++) {
		if (!((bt[0][i]&checksum[i]) == 0 && bt[0][i]|checksum[i])) return false;
	}
	return true;
}

int main () {
	string line = "", flip = "";
	int count = 0;
	vector<bitset<1000> >data, wrong, flips;
	vector<int>len;
	ifstream infile("dataVs.txt");
	//ifstream infile("bug.txt");
	//start to read file;
	while (getline(infile, line)) {
		istringstream iss(line);
		iss >> line >> flip;
		reverse(line.begin(), line.end());
		reverse(flip.begin(), flip.end());
		bitset<1000>tmp(line);  //original dataset
		bitset<1000>temp(line); //original dataset and going to flip it to be wrong data set
		bitset<1000>tempp(flip);//fliping data set
		len.push_back(line.size());
		for (int i = 0; i < len.back(); i++) {
			if (tempp[i]) temp.flip(i); //start to flip original data by following flip data set
		}

		data.push_back(tmp);
		wrong.push_back(temp);
		flips.push_back(tempp);
	}
	//lets compute crc checksum
	for (int i = 0; i < data.size(); i++) {
		bitset<13>crcOutcome = compute(data[i], len[i]+12);
		string scrc = "", crcvalid = "";
		//scrc.pop_back();
		for (int k = len[i], j = 0; j < 12 ;k++, j++) {
			//get wrong crc by fliping bits
			if (flips[i][k] == 0) wrong[i][k] = crcOutcome[j];
			else wrong[i][k] = !crcOutcome[j];
			if (crcOutcome[j]) scrc.push_back('1');
			else scrc.push_back('0');
		}
#if DEBUG
		cout <<"---------------- the wrong bits --------------" << endl;
		for (int j = 0, k = len[i]; j < 12; j++, k++) {
			cout << wrong[i][k];
		}
		cout << endl <<"----------------------------------------------" << endl;
#endif
		//use wrong crc and wrong dataset to compute CRC again
		if (compute(wrong[i], len[i]+12).count() == 0) crcvalid = "pass";
		else crcvalid = "not pass";
		cout << "crc : "<< scrc <<"  result: "<< crcvalid << endl;
		//start to compute  checksum
		count = len[i]/8;
		vector<bitset<9> >group, ng;
		//divide original dataset and wrong dataset by one byte
		for (int a = 0, j = 0; a < count; a++) {
			bitset<9>tt, ff; //true and false data
			for (int k = 7; k > -1; k--, j++) {
				tt[k] = data[i][j]; 
				ff[k] = wrong[i][j];
				//cout << ff[k];
			}
			//cout << endl;
			group.push_back(tt);
			ng.push_back(ff);
		}
#if DEBUG
		cout <<"---------------- the wrong bits --------------" << endl;
		for (int j = 0; j < ng.size(); j++) {
			for (int k = 0; k < 8; k++) cout << ng[j][k];
			cout << endl;
		}
		for (int j = 0; j < group.size(); j++) {
			for (int k = 0; k < 8; k++) cout << group[j][k];
			cout << endl;
		}
		cout << endl <<"----------------------------------------------" << endl;
#endif
		bitset<8>cks = internetChecksum(group);
		//make error checksum
		bitset<9>fck;
		//flip checksum to get error checksum
		for (int a = len[i], b = 7; b > -1; a++, b--) {
			if (flips[i][a]) fck[b] = !cks[b];
			else fck[b] = cks[b];
		}
#if DEBUG 
		cout <<"---------------- the cks bits --------------" << endl;
		for (int k = 0; k < 8; k++) cout << cks[k];
		cout << endl;
		for (int k = 0; k < 8; k++) cout << fck[k];
		cout << endl;
		cout << endl <<"----------------------------------------------" << endl;
#endif
	
		if (validChecksum(ng, fck)) crcvalid = "pass";
		else crcvalid = "not pass";
		cout << "checksum: "<< cks.to_string() <<"  result: "<< crcvalid << endl << endl;
	}
	return 0;
}

