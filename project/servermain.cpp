#include "servermain.h"

using namespace std;
#define MAXDATASIZE 1024
//state, userID, UDP port number
unordered_map<string, unordered_map<string, string>>db;
int sockUDP = -1, sockTCP = -1;
struct sockaddr_in destAddr, serverAddr;
struct sigaction sa;
struct addrinfo *UDPservinfo;
string udpPortNumber = "112112", portNumber = "112";
socklen_t  alen = sizeof(destAddr), sin_size;
/*
 * this function will get userID and his corresponding state from backend UDP server
 */
void getDB () {
    int sockfd, numbytes;  
    struct addrinfo hints, *p;
    int rv, count;
	//socklen_t  alen;
	inet_aton("127.0.0.1", &(destAddr.sin_addr));
	//alen = sizeof(destAddr);
	/*
	struct sockaddr *destAddr = new struct sockaddr;
	destAddr->sa_family = AF_INET;
	destAddr->sa_addr = 
	*/
    char s[INET6_ADDRSTRLEN]= "127.0.0.1";; 
	string input = "", clientID = "";
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
	udpPortNumber = to_string(ntohs(sin.sin_port));
	/*	perror("UDP Bind Error");
		exit(1);
	}*/
	string city = "lol";
	vector<string>stateA, stateB;
	if (db.empty()) {
		char sent[128];
		sent[0] = '$';
		destAddr.sin_port = htons(30544);
		//sendto(sockfd , &sent, 2, 0,  &destAddr, alen);
		sendto(sockUDP , &sent, 2, 0,  (struct sockaddr*)&destAddr, (socklen_t)sizeof(destAddr));
		char buf[MAXDATASIZE];
		do {
			//memset(buf, 0, sizeof buf);
			numbytes = recvfrom(sockUDP, buf, MAXDATASIZE, 0, (struct sockaddr*)&destAddr, &alen);
			if (numbytes < 0) {
				cout <<"---------- recevie error -----------" << endl;
				perror("recv");
				freeaddrinfo(UDPservinfo); // all done with this structure
				close(sockUDP);
				exit(1);
			}
			//string tmp(buf);
			//cout << tmp << endl;
			string state = "";
			numbytes--;
			for (int i = 0; i < numbytes; i++) {
				//cout << buf[i];
				state.push_back(buf[i]);
			}
			//cout <<endl<< state << " its size  " << state.size() << " numsize " << numbytes << endl;
			if (isalpha(state[0])) {
				//state.pop_back();
				city = state;
				stateA.push_back(city);
			} else db[city][state] = "30544";
		} while(buf[0]!='$');
		cout<<"Main server has received the state list from server A using UDP over port "<< udpPortNumber  <<endl;
		//cout <<"--------- lets start to process server B ----------" << endl;
		destAddr.sin_port = htons(31544);
		sendto(sockUDP, sent, 2, 0,  (struct sockaddr*)&destAddr, (socklen_t)sizeof(destAddr));
		do {
			//memset(buf, 0, sizeof buf);
			numbytes = recvfrom(sockUDP, buf, MAXDATASIZE, 0, (struct sockaddr*)&destAddr, &alen);
			//cout <<"numbytes: " << numbytes<<endl;
			if (numbytes < 0) {
				cout <<"---------- recevie error -----------" << endl;
				perror("recv");
				freeaddrinfo(UDPservinfo); // all done with this structure
				close(sockUDP);
				exit(1);
			}
			string state = "";
			numbytes--;
			for (int i = 0; i < numbytes; i++) state.push_back(buf[i]);
			//cout << state << endl;
			if (isalpha(state[0])) {
				//state.pop_back();
				city = state;
				stateB.push_back(city);
			} else db[city][state] = "31544";
		} while(buf[0]!='$');

		cout<<"Main server has received the state list from server B using UDP over port " << udpPortNumber  <<endl;
	}
	cout <<"Server A"<<endl;
	for (string& s: stateA) {
		cout << s << endl;
	}
	
	cout <<endl<<"Server B"<<endl;
	for (string& s: stateB) {
		cout << s << endl;
	}
	
	//free after usage?
	freeaddrinfo(UDPservinfo); // all done with this structure

}
/*
 * get result from UDP server
 * the input format will be state$userID
 * sname = UDP server name
 */
vector<string> getUDPresult (uint32_t destPort, string input, string sname, string userID) {
	int count = 0, numbytes;
	destAddr.sin_port = htons(destPort);
	//cout << "send input " << input << " to dest " << destPort << endl;
	sendto(sockUDP, &input[0], input.size()+1, 0, (struct sockaddr*)&destAddr, (socklen_t)sizeof(destAddr));
	cout <<"Main Server has sent request of User "<<userID<<" to server "<<sname <<" using UDP over port "<<udpPortNumber << endl;

	//if (sname == "A") cout <<"The Main Server has sent request for "<<input<<" to server A using UDP over port 30544"<<endl;
	//else cout <<"The Main Server has sent request for "<<input<<" to server B using UDP over port 31544"<<endl;
	vector<string>result;
	char buf[MAXDATASIZE];
	string ans;
	do {
		numbytes = recvfrom(sockUDP, buf, MAXDATASIZE, 0, (struct sockaddr*)&destAddr, &alen);
		//cout << "sent to " << sockfd << endl;
		//numbytes = recv(sockfd, buf, MAXDATASIZE, 0);
		if (numbytes < 0) {
			cout <<"---------- recevie error -----------" << endl;
			perror("recv");
			freeaddrinfo(UDPservinfo); // all done with this structure
			close(sockUDP);
			exit(1);
		}
		string res(buf);
		if (res[0] == '$')break;
		else if (res[0] == '#') {
			cout<< "Main server has received “User "<< userID <<": Not found” from server " << sname << endl;
			return {};
		}
		count++;
		ans+=res;
		ans+=",";
		result.push_back(res);
	} while(buf[0]!='$');
	cout <<"The Main server has received searching results of User "<< userID <<" from server " << sname <<endl;
	ans.pop_back();
	//cout << ans<< endl;
	//cout <<endl << "There are "<<count<<" distinct cities in Indiana. Their names are" << endl;
	//cout << ans << endl;
	freeaddrinfo(UDPservinfo); // all done with this structure
	return result;
}

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

void startTCP () {
	//Socket programming from Beej's sample code
    struct addrinfo hints, *servinfo, *p;
    socklen_t sin_size;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv, len = 0;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; //address family
    hints.ai_socktype = SOCK_STREAM;//address family type
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, serverPort, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockTCP = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
			break;
        }
        if (setsockopt(sockTCP, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (::bind(sockTCP, p->ai_addr, p->ai_addrlen) < 0) {
            close(sockTCP);
            perror("server: bind");
            continue;
        }

        break;
    }
	portNumber  =  to_string(ntohs(((struct sockaddr_in *)servinfo->ai_addr)->sin_port));
	//cout <<"***  portNumber " << to_string(ntohs(((struct sockaddr_in *)servinfo->ai_addr)->sin_port)) << "     ***"   <<endl;
    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockTCP, SOMAXCONN) == -1) {
        perror("listen");
        exit(1);
    }
}

int main()
{
	uint32_t destPort; 
	int turn = 2, numbytes = -1;
	string userID, state, clientID, sname = "", input;
	vector<string>result;
	getDB();
	startTCP();
	//initialize process
	sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);// additional signal to block
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
	int pid = -1, new_fd = -1;
	cout <<"Main server is up and running." << endl;
	while(1) {
		//receive request from TCP client
		struct sockaddr_in clinetAddr;
        new_fd = accept(sockTCP, (struct sockaddr *)&clinetAddr, &sin_size);
        if (new_fd == -1) { //socket fd is in non blocking mode p.609
            perror("accept");
            break;
        }
		pid = fork();
        if (!pid) { // this is the child process
			//need to get data from socket?
			bool start = false;
			char    buf[BUFLEN]; int n = recv(new_fd, buf, BUFLEN, 0);
			string request (buf);	

			
			//string portNumber = to_string(ntohs(clinetAddr.sin_port));

            close(sockTCP); // child doesn't need the listener

			//get userid and state
			userID = state = "";
			turn = 2;
			for (char c : request) {
				if (c == ',') {
					turn = 0;
				} else if (c == '$') {
					turn = 1;
				} else {
					if (turn == 1 && isdigit(c)) userID.push_back(c);
					else if (turn == 0) state.push_back(c);
					else if (turn ==2 && isdigit(c)) clientID.push_back(c);
				}
			}
			cout <<"Main server has received the request on User "<<userID <<" in "<< state <<" from client "<<clientID <<" using TCP over port "<< portNumber << endl;
			if (db.find(state) == db.end()) {
				//send # to client
				cout << state << " does not show up in server A&B" << endl;
				if (send(new_fd, "#", 2, 0) == -1) {
					perror("send failed !");
				}
				cout <<"Main Server has sent “"<<state<< ": Not found” to client "<<clientID<<" using TCP over port "<< portNumber<<endl;
		    	close(new_fd);
				exit(EXIT_SUCCESS);

				continue;
			}
			for (auto& it: db[state]) {
				if (it.second == "30544") {
					sname = "A";
					destPort = 30544;
				} else {
					sname = "B";
					destPort = 31544;
				}
				break;
			}
			
			cout << state << " shows up in server " << sname << endl;
			input = state+"$"+userID;
			result = getUDPresult (destPort, input, sname, userID);
			if (result.size() == 0) {
				//send @ to client
				if (send(new_fd, "@", 2, 0) == -1) {
					perror("send failed !");
				}
				cout <<"Main Server has sent message to client "<<clientID <<" using TCP over "<<portNumber << endl;
				//cout << input<<" does not show up in server A&B"<<endl;
				cout <<endl<<"-----Start a new query-----" << endl;
		    	close(new_fd);
				exit(EXIT_SUCCESS);
				//cout <<db[input] << endl;
				continue;
			}
			//cout <<"Main Server has sent request of User "<<userID<<" to server "<<sname <<" using UDP over port "<<udpPortNumber << endl;
			cout <<"Main server has received searching result of User "; 
			//send the result to client through TCP
			for (string s : result) {
				cout << s <<",";
				string tmp = s+",";
				if (send(new_fd, &tmp[0], tmp.size()+1, 0) == -1) {
					perror("send failed !");
				}
			}
			
			if (send(new_fd, "$", 2, 0) == -1) {
				perror("send failed !");
			}
			cout <<" from server " << sname << endl;
			cout <<"Main Server has sent searching result(s) to client "<<clientID<<" using TCP over port "<<portNumber << endl;
		    close(new_fd);
			cout <<endl<<"-----Start a new query-----" << endl;
            //exit(0);
			exit(EXIT_SUCCESS);
        } else if (pid < 0) {
			perror("fork error");
			exit(1);
		} else {
			/* parent */
			//cout <<"parent turn " << endl;
            close(new_fd);
			
			int status;
			if ((pid = waitpid(pid, &status, WNOHANG)) < 0) {
				perror("waitpid error");
				cout <<"status is " << status << endl;
				//exit(1);
			}
		}

	}

	close(sockUDP);
	close(sockTCP);
    return 0;
}
