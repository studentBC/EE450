#include "servermain.h"

using namespace std;
#define MAXDATASIZE 128

unordered_map<string,string>db;
int getSocket (string port) {
    int sockfd, numbytes;  
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN]= "127.0.0.1";; 
	string input = "", clientID = "";
	//read user input
	//cout << "Main server is up and running." << endl;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;//either ipv4 or ipv6 internet domain
	hints.ai_socktype = SOCK_DGRAM;// for UDP usage
	hints.ai_protocol = IPPROTO_UDP; //use UDP
	if ((rv = getaddrinfo("127.0.0.1", &port[0], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	//we should use addrinfo and bind the socket port
	//we should get state list first by sending '$'
	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		//cout <<p->ai_family <<" socket type: " << p->ai_socktype <<" protocol: " << p->ai_protocol << endl;
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}
	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}
	return sockfd;
}

int main()
{
    int sockfd, sockfdA, sockfdB, numbytes;  
    struct addrinfo hints, *servinfo, *p;
    int rv, count;
    char s[INET6_ADDRSTRLEN]= "127.0.0.1";; 
	string input = "", clientID = "", sname = "";
	cout <<"Main server is up and running."<<endl;
	if (db.empty()) {
		sockfdA = getSocket("30544");
		char sent[128];
		sent[0] = '$';
		send(sockfdA , sent, 2, 0 );
		char buf[MAXDATASIZE];
		do {
			numbytes = recv(sockfdA, buf, MAXDATASIZE, 0);
			if (numbytes < 0) {
				cout <<"---------- recevie error -----------" << endl;
				perror("recv");
				freeaddrinfo(servinfo); // all done with this structure
				close(sockfdA);
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
		sockfdB = getSocket("31544");
		send(sockfdB , sent, 2, 0 );
		do {
			numbytes = recv(sockfdB, buf, MAXDATASIZE, 0);
			//cout <<"numbytes: " << numbytes<<endl;
			if (numbytes < 0) {
				cout <<"---------- recevie error -----------" << endl;
				perror("recv");
				freeaddrinfo(servinfo); // all done with this structure
				close(sockfdB);
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
			sockfd = sockfdA;
		} else {
			sname = "B";
			sockfd = sockfdB;
		}
		cout << input <<" shows up in server "<< sname<<endl;

		count = 0;
		send(sockfd , &input[0], input.size()+1, 0 );

		if (db.count(input)) cout <<"The Main Server has sent request for "<<input<<" to server A using UDP over port 30544"<<endl;
		else cout <<"The Main Server has sent request for "<<input<<" to server B using UDP over port 31544"<<endl;

		char buf[MAXDATASIZE];
		string ans;
		do {
			//cout << "sent to " << sockfd << endl;
			numbytes = recv(sockfd, buf, MAXDATASIZE, 0);
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
