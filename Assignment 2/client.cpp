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

using namespace std;

int call_socket(char *hostname, unsigned short portnum)
{
	struct sockaddr_in sa;
	struct hostent *hp;
	int a;
	int s;

	if ((hp = gethostbyname(hostname)) == NULL)
	{
		errno = ECONNREFUSED;
		return(-1);
	}

	memset(&sa, 0, sizeof(sa));
	memcpy((char*) &sa.sin_addr, hp->h_addr, hp->h_length);
	sa.sin_family = hp->h_addrtype;
	sa.sin_port = htons((u_short)portnum);

	if ((s=socket(hp->h_addrtype, SOCK_STREAM, 0)) < 0)
	{
		return(-1);
	}
	if (connect(s, (struct sockaddr*)&sa, sizeof (sa)) < 0)
	{
		close(s);
		return(-1);
	}
	return(s);
}

int main(int argc, char* argv[])
{
	int n;
	int s;
	int b;
	char username[256];
	string hostname;
	string usern;
	string terminated = "Server Terminated";
	string doesnotexist = "does not exist.";

	cout << "Enter a server host name: " << endl;
	cin >> hostname;
	while (hostname != "localhost")
	{
		cout << "incorrect server host name, please enter it again" << endl;
		cin >> hostname;
	}
	char *hostn[hostname.size()+1];
	strcpy(*hostn, hostname.c_str());

	cout << "Enter a server port number: " << endl;
	cin >> s; 
	while (s <2000 || s > 65535)
	{
		cout << "Server port number must be between 2000 and 65535. Please enter the server port number again." << endl;
		cin >> s;
	}

	if ((b = call_socket(*hostn,s)) < 0)
	{
		perror("call Socket");
		exit(1);
	}

	cout << "Enter a user name: " << endl;
	cin >> usern;
	strcpy(username, usern.c_str());
	n = write(b, username, strlen(username));
	if (n < 0)
	{
		cout << "Error writing to socket" << endl;
	}

	bzero(username, 256);
	n = read(b,username,255);
	if (n < 0)
	{
		cout << "Error reading from socket" << endl;
	}

	if (username == terminated)
	{
		printf("%s\n",username);
	}
	else if (username == doesnotexist)
	{
		cout << "The username: " << usern << ", ";
		printf("%s\n",username);
	}
	else
	{
		cout << "The public key for " << usern << " is: ";
		printf("%s\n",username);
	}

	close(b);
	return 0;
}