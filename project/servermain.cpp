#include "servermain.h"

using namespace std;
#define MAXDATASIZE 128
//state, userID, UDP port number
unordered_map<string, unordered_map<string, unordered_set<string>>>db;
int sockUDP = -1, sockTCP = -1;
struct sockaddr_in destAddr, serverAddr;
/*
 * this function will get userID and his corresponding state from backend UDP server
 */
void getDB () {
    int sockfd, numbytes;  
    struct addrinfo hints, *servinfo, *p;
    int rv, count;
	socklen_t  alen;
	inet_aton("127.0.0.1", &(destAddr.sin_addr));
	alen = sizeof(destAddr);
	/*
	struct sockaddr *destAddr = new struct sockaddr;
	destAddr->sa_family = AF_INET;
	destAddr->sa_addr = 
	*/
    char s[INET6_ADDRSTRLEN]= "127.0.0.1";; 
	string input = "", clientID = "", sname = "";
	cout <<"Main server is up and running."<<endl;


	if ((sockUDP = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("UDP Socket Create Error");
		exit(1);
	}

	memset(&serverAddr, 0, sizeof serverAddr);
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(32544);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	// bind the socket with udp server address
	int L = sizeof(serverAddr);
	int res = bind(sockUDP, (const struct sockaddr*) &serverAddr, (socklen_t)L); 
	if (res < 0) exit(1);
	//for debug
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	if (getsockname(sockUDP, (struct sockaddr *)&sin, &len) == -1) perror("getsockname");
	else printf("port number %d\n", ntohs(sin.sin_port));
	/*	perror("UDP Bind Error");
		exit(1);
	}*/
	string city = "lol";
	if (db.empty()) {
		char sent[128];
		sent[0] = '$';
		destAddr.sin_port = htons(30544);
		//sendto(sockfd , &sent, 2, 0,  &destAddr, alen);
		sendto(sockUDP , &sent, 2, 0,  (struct sockaddr*)&destAddr, (socklen_t)sizeof(destAddr));
		char buf[MAXDATASIZE];
		do {
			numbytes = recvfrom(sockUDP, buf, MAXDATASIZE, 0, (struct sockaddr*)&destAddr, &alen);
			if (numbytes < 0) {
				cout <<"---------- recevie error -----------" << endl;
				perror("recv");
				freeaddrinfo(servinfo); // all done with this structure
				close(sockUDP);
				exit(1);
			}
			string state = "";
			numbytes--;
			for (int i = 0; i < numbytes; i++) state.push_back(buf[i]);
			//cout << state << endl;
			if (isalpha(state[0])) city = state;
			else db[city][state] = "30544";
		} while(buf[0]!='$');
		cout<<"Main server has received the state list from server A using UDP over port 32544"<<endl;
		//cout <<"--------- lets start to process server B ----------" << endl;
		destAddr.sin_port = htons(31544);
		sendto(sockUDP, sent, 2, 0,  (struct sockaddr*)&destAddr, (socklen_t)sizeof(destAddr));
		do {
			numbytes = recvfrom(sockUDP, buf, MAXDATASIZE, 0, (struct sockaddr*)&destAddr, &alen);
			//cout <<"numbytes: " << numbytes<<endl;
			if (numbytes < 0) {
				cout <<"---------- recevie error -----------" << endl;
				perror("recv");
				freeaddrinfo(servinfo); // all done with this structure
				close(sockUDP);
				exit(1);
			}
			string state = "";
			numbytes--;
			for (int i = 0; i < numbytes; i++) state.push_back(buf[i]);
			//cout << state << endl;
			if (isalpha(state[0])) city = state;
			else db[city][state] = "31544";
		} while(buf[0]!='$');

		cout<<"Main server has received the state list from server B using UDP over port 32544"<<endl;
	}
	cout <<"Server A"<<endl;
	for (auto& it: db) {
		if (it.second == "30544") {
			cout << it.first<< endl;
			//cout << it.first.size() <<endl;
		}
	}
	
	cout <<endl<<"Server B"<<endl;
	for (auto& it: db) {
		if (it.second == "31544") cout << it.first <<endl;
	}


}
/*
 * get result from UDP server
 * the input format will be state$userID
 * sname = UDP server name
 */
string getUDPresult (uint32_t destPort, string input, string sname, string userID) {
	int count = 0;
	destAddr.sin_port = htons(destPort);
	sendto(sockUDP, &input[0], input.size()+1, 0, (struct sockaddr*)&destAddr, (socklen_t)sizeof(destAddr));

	//if (sname == "A") cout <<"The Main Server has sent request for "<<input<<" to server A using UDP over port 30544"<<endl;
	//else cout <<"The Main Server has sent request for "<<input<<" to server B using UDP over port 31544"<<endl;

	char buf[MAXDATASIZE];
	string ans;
	do {
		numbytes = recvfrom(sockUDP, buf, MAXDATASIZE, 0, (struct sockaddr*)&destAddr, &alen);
		//cout << "sent to " << sockfd << endl;
		//numbytes = recv(sockfd, buf, MAXDATASIZE, 0);
		if (numbytes < 0) {
			cout <<"---------- recevie error -----------" << endl;
			perror("recv");
			freeaddrinfo(servinfo); // all done with this structure
			close(sockfd);
			exit(1);
		}
		string res(buf);
		if (res[0] == '$')break;
		else if (res[0] == '#') return "Main server has received “"+userID+": Not found” from server "+sname;
		count++;
		ans+=res;
		ans+=",";
	} while(buf[0]!='$');
	cout <<"The Main server has received searching results of "<< input <<" from server " << sname <<endl;
	ans.pop_back();
	//cout << ans<< endl;
	cout <<endl << "There are "<<count<<" distinct cities in Indiana. Their names are" << endl;
	cout << ans << endl;
}

int main()
{
	uint32_t destPort; 
	int turn = 2;
	getDB();
	string request, userID, result, state, clientID;
	while(1) {
		//receive request from TCP client
		
		//get userid and state
		userID = state = "";
		turn = false;
		for (char c : request) {
			if (c == ',') {
				turn = 0;
			} else if (c == '$') {
				turn = 1;
			} else {
				if (turn == 1) userID.push_back(c);
				else if (turn == 0) state.push_back(c);
				else clientID.push_back(c);
			}
		}
		if (db.find(state) == db.end()) {
			//send # to client
		} else if (!db[state].count(userID)) {
			//send @ to client
			cout << input<<" does not show up in server A&B"<<endl;
			cout <<endl<<"-----Start a new query-----" << endl;
			//cout <<db[input] << endl;
			continue;
		}

		if (db[state][userID] == "30544") {
			sname = "A";
			destPort = 30544;
		} else {
			sname = "B";
			destPort = 31544;
		}
		cout << input <<" shows up in server "<< sname<<endl;
		result = getUDPresult (destPort, input, sname, userID);
		//send the result to client through TCP
	}

	freeaddrinfo(servinfo); // all done with this structure
	close(sockUDP);
	close(sockTCP);
    return 0;
}
