#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <queue>

using namespace std;

int establish(unsigned short portnum)
{
	int MAXHOSTNAME = 9;
	char myname[MAXHOSTNAME+1];
	int s;
	struct sockaddr_in sa;
	struct hostent *hp;

	memset(&sa, 0, sizeof(struct sockaddr_in));
	gethostname(myname, MAXHOSTNAME);
	hp = gethostbyname("localhost"); //changed myname to localhost.
	if (hp == NULL)
	{
		return(-1);
	}
	sa.sin_family = hp->h_addrtype;
	sa.sin_port = htons(portnum);
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		return(-1);
	}

	if (bind(s,(struct sockaddr*)&sa, sizeof(sa)) < 0)
	{
		cout << "error on binding" << endl;
	}

	listen(s,3);
	return(s);
}

int get_connection(int s)
{
	int t;
	if ((t = accept(s,NULL,NULL)) < 0)
	{
		return(-1);
	}
		return(t);
}

int main(int argc, char* argv[])
{
	deque<pair<string, string> > data;

	int n;
	int s;
	int p;
	int t;
	int i = 0;
	string file;
	string username;
	string publicKey;
	char publicKeyResponse[256];
	bool connected = true;

	ifstream textfile;

	cout << "Enter a textfile name: " << endl;
	cin >> file;

	textfile.open(file.c_str());
	while(!textfile.eof())
	{
		getline(textfile, username, ' ');
		getline(textfile, publicKey);
	 data.push_back(make_pair(username, publicKey));
	 i++;
	}

	cout << "Enter a server port number: " << endl;
	cin >> p;
	while (p <2000 || p > 65535)
	{
		cout << "Server port number must be between 2000 and 65535. Please enter the server port number again." << endl;
		cin >> p;
	}

	if ((s = establish(p)) < 0)
	{
		perror("establish");
		exit(1);
	}
	else 
	{
		cout << "Established connection:" << endl;
	}

	while(connected != false)
		{
			if ((t = get_connection(s)) < 0)
			{
				if (errno == EINTR)
				{
					continue;
				}
				perror("accept");
				exit(1);
			}

			char publicKeyResponse[256];
			bzero(publicKeyResponse, 256);
			n = read(t, publicKeyResponse, 255);
			string terminate = "terminate";
			string Terminate = "Terminate";
			string OTerminate = "Terminate."; // might only need this one, if so delete others and name this one terminate
			if (n < 0)
			{
				cout << "Error reading from socket" << endl;
			}
			printf("%s\n", publicKeyResponse);
			if(publicKeyResponse == terminate || publicKeyResponse == Terminate || publicKeyResponse == OTerminate)
			{
				n = write(t, "Server Terminated", 17);
				connected = false;
			}
			else
			{
				int u = 0;
				for(int i = 0; i < data.size(); i++)
				{
					if (data.at(i).first == publicKeyResponse)
					{
						char pKey [data.at(i).second.size()+1];
						strcpy(pKey, data.at(i).second.c_str());
						n = write(t, pKey, strlen(pKey));
					}
					else
					{
						u++;
					}
					if(u == data.size())
					{
					 	n = write(t, "does not exist.", 15);
					}
				}
			}
		}

	close(s);
	close(t);
	return 0;
}