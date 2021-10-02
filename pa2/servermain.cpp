#include "servermain.h"
#include <algorithm>
#define BUFLEN 128
using namespace std;
unordered_map<string, unordered_set<string> >db;
void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}
string convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*
char* getPacketData (int sockfd) {
   int     n;
   char* buf = new char[BUFLEN];
   cout <<"===========================" << endl;
   if (n = recv(sockfd, buf, BUFLEN, 0) > 0) {
		for (int i = 0; i < n; i++) cout << buf[i];
   }
   if (n < 0) {
	   perror("recv error");
   		exit(1);
   }
   return buf;
}*/

string findDB (string city) {
	for (auto& it: db) {
		if (db[it.first].count(city)) return it.first;
	}
	return "";
}

int initserver(int type, const struct sockaddr *addr, socklen_t alen, int qlen) {
	int fd;
	int err = 0;
	if ((fd = socket(addr->sa_family, type, 0)) < 0)
	   return(-1);
	if (bind(fd, addr, alen) < 0)
	   goto errout;
	if (type == SOCK_STREAM || type == SOCK_SEQPACKET) {
	   if (listen(fd, qlen) < 0) goto errout;
	}
    return(fd);
errout:
    err = errno;
    close(fd);
    errno = err;
    return(-1);
}


int main () {
	bool odd = true;
	// Read from the text file
	ifstream MyReadFile("list.txt");
	cout <<"Upon reading the state lists:" << endl;
	string line, city;
	while (getline (MyReadFile, line)) {
		if (odd) {
			city = line;
			odd = false;
		} else {
			string i; size_t pos = 0;
			while ((pos = line.find(',')) != string::npos) {
				db[city].insert(line.substr(0, pos));
				line.erase(0, pos+1);
			}
			odd = true;
		}
	}
	cout << endl;
	for (auto& it: db) {
		cout << it.first << ":" << endl;
		for (string s : it.second) cout << s << endl;
		cout << endl;
	}
	// Close the file
	MyReadFile.close();

	//Socket programming from Beej's sample code
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv, len = 0;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; //address family
    hints.ai_socktype = SOCK_STREAM;//address family type
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, serverPort, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
/*
 * struct addrinfo {
     int               ai_flags;
     int               ai_family;
     int               ai_socktype;
     int               ai_protocol;
     socklen_t         ai_addrlen;
     struct sockaddr  *ai_addr;		address
     char             *ai_canonname;   canonical name of host 
     struct addrinfo  *ai_next;        next in list 
 }*/
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (::bind(sockfd, p->ai_addr, p->ai_addrlen) < 0) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, SOMAXCONN) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);// additional signal to block
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");
	int pid = -1;
    while(1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        //new_fd = accept(sockfd, NULL, NULL);
        if (new_fd == -1) { //socket fd is in non blocking mode p.609
            perror("accept");
            continue;
        }
		//converts a binary address in network byte order into a text string
        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        printf("server: got connection from %s\n", s);
		pid = fork();
		cout <<"pid is " << pid << endl;
        if (!pid) { // this is the child process
			//need to get data from socket?
			//char* input = getPacketData(sockfd);
			cout <<"===== ben 1 =====" << endl;
			bool start = false;
			string ans, city, clientID, state, input;
			char    buf[BUFLEN]; int n = recv(new_fd, buf, BUFLEN, 0);
			if (n) {
				cout <<"n is " << n << endl;
				for (int i = 0; i < n; i++) {
					input.push_back(buf[i]);
				}
			}
			cout <<"receive input: " << input << endl;
			//get city + client id
			for (int i = 0; i < input.size();i++) {
				if (!start) clientID.push_back(input[i]);
				else city.push_back(input[i]);
				if (input[i] == ',') start = true;
			}
			cout <<" ben 2 " << endl;
			state = findDB(city);
			if (state.size()) ans = city+" is associated with state "+ state;
			else ans = city+" not found";
			len = ans.size()+1;

			cout << "ans: " << ans << endl;
            close(sockfd); // child doesn't need the listener
            if (send(new_fd, &ans, len, 0) == -1) {
				perror("send failed !");
			} 
			cout << "Main Server has sent searching result to client "<< clientID <<" using TCP over port " << to_string(p->ai_protocol) << endl;
			cout << "The Main Server has sent "+city+": Not found” to client " << clientID << " using TCP over port " << to_string(p->ai_protocol) << endl;;

            close(new_fd);
            exit(0);
        } else if (pid < 0) {
			perror("fork error");
			exit(1);
		} else {
			/* parent */
			int status;
			if ((pid = waitpid(pid, &status, 0)) < 0)
				perror("waitpid error");
				exit(1);
		}
        close(new_fd);  // parent doesn't need this
    }

    return 0;

}
