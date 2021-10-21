#include "servermain.h"
#include <algorithm>
#define MAXLINE 1024
using namespace std;
unordered_map<string, vector<string> >db;
string stateList;
void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

int main () {
	bool odd = true;
	string key = "";
	// Read from the text file
	ifstream MyReadFile("dataA.txt");
	string line, city;
	while (getline (MyReadFile, line)) {
		if (odd) {
			city = line;
			city.pop_back();
			odd = false;
		} else {
			line+=",";
			string s = "";
			for (char c : line) {
				if (c == ',') {
					db[city].push_back(s);
					s="";
				} else s.push_back(c);
			}
			//db[city].push_back(line);
			//cout << line << endl;
			odd = true;
		}
	}
	cout <<"Server A is up and running using UDP on port 30544" << endl;
	/*for (auto& it: db) {
		cout << it.first << endl;
		for (string s : it.second) cout << s <<" => ";;
		cout << endl;
	}*/
	stateList.pop_back();
	// Close the file
	MyReadFile.close();
	
	int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;

    // Creating socket file descriptor
	// 0 is default otherwise we should use IPPROTO_UDP
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(30544);

    // Filling client information
    cliaddr.sin_family    = AF_INET; // IPv4
    cliaddr.sin_addr.s_addr = INADDR_ANY;
    cliaddr.sin_port = htons(32544);

    int len, n;
    // Bind the socket with the server address
	len = sizeof(servaddr);
    if (  bind(sockfd, (const struct sockaddr *)&servaddr, (socklen_t)len)  < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
	/*for (auto& it: db) {
		cout << it.first << endl;
		for (string s : it.second) cout<<s <<" -> ";
		cout <<endl;
	}*/

    len = sizeof(cliaddr);  //len is value/resuslt
	while (1) {
		n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, (socklen_t *) &len);
		buffer[n] = '\0';
		if (buffer[0] == '$') {
			for (auto& it: db) {
				sendto(sockfd, &it.first[0], it.first.size()+1, 0, (const struct sockaddr *) &cliaddr, (socklen_t)len);
			}
			sendto(sockfd, "$", 2, 0, (const struct sockaddr *) &cliaddr, (socklen_t)len);
			cout <<"Server A has sent a state list to Main Server"<<endl;
			continue;
		}
		key = "";
		n--;
		for (int i = 0; i < n; i++) key.push_back(buffer[i]);
		//cout <<"its length: " << n << endl;
		//cout <<"key is " << key << endl;
		//for (string s: db[key]) cout << s <<" -> ";
		cout<<"Server A has received a request for "<< key << endl;
		cout <<"Server A found "<<  db[key].size() <<" distinct cities for " << key <<":"<<endl;
		for (string s: db[key]) {
			cout << s << ",";
			if (sendto(sockfd, &s[0], s.size()+1, 0, (const struct sockaddr *) &cliaddr, (socklen_t)len) < 0) {
				perror("sending error ");
				close(sockfd);
				exit(EXIT_FAILURE);
			}
		}
		cout << endl;
		sendto(sockfd, "$", 2, 0, (const struct sockaddr *) &cliaddr, (socklen_t)len);
		cout <<"Server A has sent the results to Main Server"<<endl;
	}
    return 0;

}
