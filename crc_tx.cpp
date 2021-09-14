/*
#include <bitset>
#include <vector>
#include <cassert>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
*/
include "crc.h"
using namespace std;
int main () {
	string line;
	vector<bitset<1000>>data;
	ifstream infile("");
	while (getline(infile, line)) {
		istringstream iss(line);
		if (!(iss >> line)) break;
		else {
			bitset<1000>tmp(line);
			data.push_back(tmp);
		}
	}
	return 0;
}

