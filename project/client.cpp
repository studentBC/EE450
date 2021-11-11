#include "servermain.h"

using namespace std;
#define MAXDATASIZE 128
#define idPath "clientID"
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void recordClientID (string id) {
  ofstream file; 
  file.open(idPath, ios::out);
  if (file.is_open())
  {
    file << id <<endl;
    file.close();
  } else cout << "Unable to open file";
}

string getClientID() {
//if ( access( idPath.c_str(), F_OK ) == -1 ) return 1;
  string line;
  ifstream myfile (idPath);
  int count = 0;
  if (myfile.is_open())
  {
	  getline (myfile,line);
	  //cout << line << '\n';
	  stringstream ss(line);
	  ss >> count;
  }
    myfile.close();
	count++;
	//cout << count << endl;
  return to_string(count);	
}

int main()
{
    int sockfd, numbytes;  
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN]= "127.0.0.1";; 
	string state = "", clientID = "", userID = "";
	clientID = getClientID();
	recordClientID(clientID);
	//read user input
	cout << "Client is up and running." << endl;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;//either ipv4 or ipv6 internet domain
	hints.ai_socktype = SOCK_STREAM;// for TCP usage

	if ((rv = getaddrinfo("127.0.0.1", serverPort, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}


	while(1) {
		cout << "Enter state name:" << endl;
		getline(cin, state);
		cout << "Enter user ID:" << endl;
		getline(cin, userID);

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
		//cout <<"ai addr: " << p->ai_addr->sa_data << endl;
		inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
				s, sizeof s);
		printf("client: connecting to %s\n", s);
		//cout <<"prepare to send " << input << endl;


		string res = "";
		string rawdata = clientID+","+state+"$"+userID;
		char sent[128];
		for (int i = 0; i < rawdata.size(); i++) sent[i] = rawdata[i];
		send(sockfd , sent, rawdata.size(), 0 );
		struct sockaddr_in  *sinp = (struct sockaddr_in *)p->ai_addr;
		cout << "Client has sent "<< state << " and User "<< userID  << " to Main Server using TCP over port "<< ntohs(sinp->sin_port) << endl;
    	char buf[MAXDATASIZE];
		do {
			numbytes = recv(sockfd, buf, MAXDATASIZE, 0);
			string s = "";
			for (int i = 0; i < numbytes; i++) s.push_back(buf[i]);
			//cout << s <<" : " << numbytes << endl;
			//cout <<"receive msg is " << buf <<" packet length is " << numbytes << endl;
			if (numbytes < 0) {
				cout <<"---------- recevie error -----------" << endl;
				perror("recv");
				freeaddrinfo(servinfo); // all done with this structure
				close(sockfd);
				exit(1);
			}
			res+=s;
			if (s[0] == '#') {
				cout <<state <<" : Not found" << endl;
				break;
			} else if (s[0] == '@') {
				cout << userID <<" : Not found" << endl;
				break;
			} else if (s.find('$') != std::string::npos) {
				res.pop_back();
				break;
			}
		} while (1);
		//cout << res << endl;
		cout <<"Main server has received the request on city "+state+" from client "+ clientID +" using TCP over port "<< ntohs(sinp->sin_port) << endl;
		if (!isdigit(res[0])) continue;
		while (!isdigit(res.back())) res.pop_back();
		cout <<"Client has received results from Main Server:"<<endl;
		cout <<"User "<< res <<" is/are possible friend(s) of User "<< userID<<" in " << state << endl;
		//cout <<endl<< buf << endl;
		//cout << "client: received" << buf << endl;
		cout <<"-----Start a new query-----" << endl;
	}

	freeaddrinfo(servinfo); // all done with this structure
	close(sockfd);
    return 0;
}
