#include "servermain.h"

using namespace std;
#define MAXDATASIZE 128
unordered_map<string,string>db;


int main()
{
    int sockfd, numbytes;  
    struct addrinfo hints, *servinfo, *p;
    int rv, count;
	socklen_t  alen;
	uint32_t destPort;  
	struct sockaddr_in destAddr, serverAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = htons(30544);
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


	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("UDP Socket Create Error");
		exit(1);
	}

	memset(&serverAddr, 0, sizeof serverAddr);
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(32544);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	// bind the socket with udp server address
	if (bind(sockfd, (struct sockaddr*) &serverAddr, sizeof serverAddr ) < 0) exit(1);
	//for debug
	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	if (getsockname(sockfd, (struct sockaddr *)&sin, &len) == -1) perror("getsockname");
	else printf("port number %d\n", ntohs(sin.sin_port));
	/*	perror("UDP Bind Error");
		exit(1);
	}*/

	if (db.empty()) {
		char sent[128];
		sent[0] = '$';
		destAddr.sin_port = htons(30544);
		//sendto(sockfd , &sent, 2, 0,  &destAddr, alen);
		sendto(sockfd , &sent, 2, 0,  (struct sockaddr*)&destAddr, (socklen_t)sizeof(destAddr));
		char buf[MAXDATASIZE];
		do {
			numbytes = recvfrom(sockfd, buf, MAXDATASIZE, 0, (struct sockaddr*)&destAddr, &alen);
			if (numbytes < 0) {
				cout <<"---------- recevie error -----------" << endl;
				perror("recv");
				freeaddrinfo(servinfo); // all done with this structure
				close(sockfd);
				exit(1);
			}
			string state = "";
			numbytes--;
			for (int i = 0; i < numbytes; i++) state.push_back(buf[i]);
			//cout << state << endl;
			if (state[0]!='$') db[state] = "30544";
		} while(buf[0]!='$');
		cout<<"Main server has received the state list from server A using UDP over port 32544"<<endl;
		//cout <<"--------- lets start to process server B ----------" << endl;
		destAddr.sin_port = htons(31544);
		sendto(sockfd , sent, 2, 0,  (struct sockaddr*)&destAddr, (socklen_t)sizeof(destAddr));
		do {
			numbytes = recvfrom(sockfd, buf, MAXDATASIZE, 0, (struct sockaddr*)&destAddr, &alen);
			//cout <<"numbytes: " << numbytes<<endl;
			if (numbytes < 0) {
				cout <<"---------- recevie error -----------" << endl;
				perror("recv");
				freeaddrinfo(servinfo); // all done with this structure
				close(sockfd);
				exit(1);
			}
			string state = "";
			numbytes--;
			for (int i = 0; i < numbytes; i++) state.push_back(buf[i]);
			//cout << state << endl;
			if (state[0]!='$') db[state] = "31544";
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


	while(1) {
		cout << "Enter state name:" << endl;
		getline(cin, input);

		if (db.find(input) == db.end()) {
			cout << input<<" does not show up in server A&B"<<endl;
			cout <<endl<<"-----Start a new query-----" << endl;
			//cout <<db[input] << endl;
			continue;
		}
		if (db[input] == "30544") {
			sname = "A";
			destPort = 30544;
		} else {
			sname = "B";
			destPort = 31544;
		}
		cout << input <<" shows up in server "<< sname<<endl;

		count = 0;
		destAddr.sin_port = htons(destPort);
		//sendto(sockfd , sent, 2, 0,  &destAddr, alen);
		sendto(sockfd , &input[0], input.size()+1, 0, (struct sockaddr*)&destAddr, (socklen_t)sizeof(destAddr));

		if (db.count(input)) cout <<"The Main Server has sent request for "<<input<<" to server A using UDP over port 30544"<<endl;
		else cout <<"The Main Server has sent request for "<<input<<" to server B using UDP over port 31544"<<endl;

		char buf[MAXDATASIZE];
		string ans;
		do {
			numbytes = recvfrom(sockfd, buf, MAXDATASIZE, 0, (struct sockaddr*)&destAddr, &alen);
			//cout << "sent to " << sockfd << endl;
			//numbytes = recv(sockfd, buf, MAXDATASIZE, 0);
			if (numbytes < 0) {
				cout <<"---------- recevie error -----------" << endl;
				perror("recv");
				freeaddrinfo(servinfo); // all done with this structure
				close(sockfd);
				exit(1);
			}
			string state(buf);
			if (state[0] == '$') break;
			count++;
			ans+=state;
			ans+=",";
		} while(buf[0]!='$');
		cout <<"The Main server has received searching results of "<< input <<" from server " << sname <<endl;
		ans.pop_back();
		//cout << ans<< endl;
		cout <<endl << "There are "<<count<<" distinct cities in Indiana. Their names are" << endl;
		cout << ans << endl;
		cout <<endl<<"-----Start a new query-----" << endl;
	}

	freeaddrinfo(servinfo); // all done with this structure
	close(sockfd);
    return 0;
}
