#include "servermain.h"
#include <algorithm>
#define BUFLEN 128
using namespace std;
unordered_map<string, unordered_set<string>>db;
void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


char* getPacketData (int sockfd) {
   int     n;
   char    buf[BUFLEN];
   while ((n = recv(sockfd, buf, BUFLEN, 0)) > 0)
	   write(STDOUT_FILENO, buf, n);
   if (n < 0)
	   err_sys("recv error");
   return buf[0];
}


int main () {
	bool odd = true;
	// Read from the text file
	ifstream MyReadFile("list.txt");
	string line, city;
	while (getline (MyReadFile, line)) {
		if (odd) {
			city = line;
			odd = false;
		} else {
			std::stringstream ss(line);
			for (string i; ss >> i;) {
				db[city].insert(i);
				if (ss.peek() == ',')
					ss.ignore();
			}
			odd = true;
		}
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

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
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
        if (!pid) { // this is the child process
			//need to get data from socket?
			char* city = getPacketData(sockfd);
			string ans;
			if (db[state].count(city)) ans = city+" is associated with state "+ state;
			else ans = city+" not found";
			len = ans.size();

            close(sockfd); // child doesn't need the listener
            if (send(new_fd, ans, len, 0) == -1) {
                perror("send successfully !");
			} else perror("send failed !");
 
            close(new_fd);
            exit(0);
        } else if (pid < 0) {
			err_sys("fork error");
		} else {
			/* parent */
			if ((pid = waitpid(pid, &status, 0)) < 0)
				err_sys("waitpid error");
		}
        close(new_fd);  // parent doesn't need this
    }

    return 0;

}
