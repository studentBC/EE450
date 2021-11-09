#include "servermain.h"
#include <algorithm>
#define MAXLINE 1024
using namespace std;
unordered_map<string, vector< vector<string>>>rawData;
unordered_map<string, unordered_set<string>>prepareToSend;
//state, userID, common friends
unordered_map<string, unordered_map<string, unordered_set<string>>>db;
string stateList;
bool check (string& target, vector<string>& input) {
	for (string s : input) if (s == target) return true;
	return false;
}
void processData(string filename) {
	// Read from the text file
	ifstream MyReadFile(filename);
	string line, city = "";
	bool start = false;
	while (getline (MyReadFile, line)) {
		if (isalpha(line[0])) {
			vector<vector<string>>temp;
			city = line;
			rawData[city] = temp;
			//start = true;
		} else {
			vector<string>users;
			stringstream ss(line);
			while (ss.good()) {
				string substr;
				getline(ss, substr, ',');
				users.push_back(substr);
				prepareToSend[city].insert(substr);
			}
			rawData[city].push_back(users);
		}
	}
	//union the same interest friend together and store them into a map
	for (auto& it : prepareToSend) {
		for (string target : it.second) {
			for (int i = 0; i < rawData[it.first].size(); i++) {
				if (check(target, rawData[it.first][i])) {
					for (int j = 0; j < rawData[it.first][i].size(); j++) {
						if (target != rawData[it.first][i][j]) db[it.first][target].insert(rawData[it.first][i][j]);
					}
				}
			}
		}
	}

	/*for (auto& it: db) {
		cout << it.first << endl;
		for (string s : it.second) cout << s <<" => ";;
		cout << endl;
	}*/
	// Close the file
	MyReadFile.close();

}

int main () {
	bool turn = true;
	string key = "", userID;
	processData("dataA.txt");	
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
    servaddr.sin_port = htons(31544);

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

	cout <<"Server B is up and running using UDP on port 31544" << endl;
    len = sizeof(cliaddr);  //len is value/resuslt
	while (1) {
		n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, ( struct sockaddr *) &cliaddr, (socklen_t *) &len);
		buffer[n] = '\0';
		if (buffer[0] == '$') {
			for (auto& it: prepareToSend) {
				sendto(sockfd, &it.first[0], it.first.size()+1, 0, (const struct sockaddr *) &cliaddr, (socklen_t)len);
				for (auto& s : it.second) {
					sendto(sockfd, &s, s.size()+1, 0, (const struct sockaddr *) &cliaddr, (socklen_t)len);
				}
			}
			sendto(sockfd, "$", 2, 0, (const struct sockaddr *) &cliaddr, (socklen_t)len);
			cout <<"Server B has sent a state list to Main Server"<<endl;
			continue;
		}
		userID = key = "";
		turn = false;
		n--;
		//the input format will be state$userID
		for (int i = 0; i < n; i++) {
			if (buffer[i] == '$') turn = true;
			else {
				if (turn) userID.push_back(buffer[i]); 
				else key.push_back(buffer[i]);
			}
		}
		cout<<"Server B has received a request for "<< key << endl;
		cout <<"Server B found the following possible friends for "<<  userID <<" in " << key <<":"<<endl;
		if (!(db.count(key) && db[key].count(userID))) {
			cout <<"User " << userID <<" does not show up in " << key << endl;
			//# means that we cannot find the corresponding userID here
			sendto(sockfd, "#", 2, 0, (const struct sockaddr *) &cliaddr, (socklen_t)len);
			cout <<"Server B has sent “" << userID <<" not found” to Main Server"<<endl;
			continue;
		}
		//cout <<"its length: " << n << endl;
		//cout <<"key is " << key << endl;
		//for (string s: db[key]) cout << s <<" -> ";
		string reply = "";
		for (auto& it: db[key][userID]) {
			reply+= it;
			reply.push_back(',');
		}
		for (string s: db[key][userID]) {
			//cout << s << ",";
			if (sendto(sockfd, &s[0], s.size()+1, 0, (const struct sockaddr *) &cliaddr, (socklen_t)len) < 0) {
				perror("sending error ");
				close(sockfd);
				exit(EXIT_FAILURE);
			}
		}
		reply.pop_back();
		cout << reply << endl;
		sendto(sockfd, "$", 2, 0, (const struct sockaddr *) &cliaddr, (socklen_t)len);
		cout <<"Server B has sent the results to Main Server"<<endl;
	}
    return 0;

}
