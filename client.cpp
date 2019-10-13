#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<iostream>
#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<iostream>
#include<pthread.h>
#include <fcntl.h>
#include<sys/stat.h>
#include <arpa/inet.h>
#include<unordered_map>
#include<sstream>
#include <fstream>
#include <openssl/sha.h>
#include<vector>

using namespace std;
unordered_map<string,vector<string>> filedetails;
unordered_map<string,string> filenamedetails;
int download=1;
int ccc=1;

struct downloaddetails{
	int portno;
	string sha;
	int chunk;
	string path;
	int index;
};

void error(const char* msg){
	perror(msg);
	exit(1);
}

string readtrackerinfo(){

	string path = "tracker_info.txt";
	ifstream tracker(path.c_str());
	string port;
	getline(tracker,port);
	tracker.close();
	return port;		
}

int startconnection(int portno)
{

	int port = portno;	
	int sockfd,n;
	char buffer[255];
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	
	if(sockfd < 0){
		error("Error Opening Socket");
		return -1;
	}
	string ip = "127.0.0.1";
	server = gethostbyname(ip.c_str());
	bzero((char *)&serv_addr,sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);
	
	if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0){
		cout << "Error Connecting" << endl;
		return -1;
	}

	return sockfd;
}

string logintosystem(string username, string password,int peerportno){
	string pt = readtrackerinfo();
	int portno = atoi(pt.c_str());
	int sockfd = startconnection(portno); if(sockfd == -1) return "False";
	char buffer[100];
	memset(buffer,'\0',sizeof(buffer));
	if(sockfd == -1)
	{
		cout << "No Tracker is Online...Try later.." << endl;
		return "False";
	}
	string command="";
	command = "2"+ username + " " + password + " " + to_string(peerportno);
	send(sockfd,command.c_str(),command.size(),0);
	recv(sockfd,buffer,sizeof(buffer),0);
	string recvedcommand;
	int i=0;
	while(buffer[i] != '\0'){
		recvedcommand = recvedcommand + buffer[i];
		i++;
	}
	close(sockfd);
	return recvedcommand;
}

string createuser(string username , string password){

	char buffer[100];
	memset(buffer,'\0',sizeof(buffer));
	string pt = readtrackerinfo();
	if(pt.size() == 0) return "False";
	int portno = atoi(pt.c_str());

	int sockfd = startconnection(portno); if(sockfd == -1) return "False";
	if(sockfd == -1)
	{
		cout << "No Tracker is Online...Try later.." << endl;
		return "False";
	}

	string command="";
	command = "1"+ username + " " + password;
	send(sockfd,command.c_str(),command.size(),0);
	recv(sockfd,buffer,sizeof(buffer),0);
	string recvedcommand;
	int i=0;
	while(buffer[i] != '\0'){
		recvedcommand = recvedcommand + buffer[i];
		i++;
	}
	close(sockfd);
	return recvedcommand;
}

string creategroup(string gid,int peerportno){

	char buffer[100];
	memset(buffer,'\0',sizeof(buffer));
	string pt = readtrackerinfo();
	if(pt.size() == 0) return "False";
	int portno = atoi(pt.c_str());

	int sockfd = startconnection(portno); if(sockfd == -1) return "False";
	if(sockfd == -1)
	{
		cout << "No Tracker is Online...Try later.." << endl;
		return "False";
	}


	string command="";
	string xx= "xxx";
	command = "3"+ xx + " " + gid + " " + to_string(peerportno);

	send(sockfd,command.c_str(),command.size(),0);
	recv(sockfd,buffer,sizeof(buffer),0);
	string recvedcommand;
	int i=0;
	while(buffer[i] != '\0'){
		recvedcommand = recvedcommand + buffer[i];
		i++;
	}
	close(sockfd);
	return recvedcommand;
}

string listgroup(){

	string pt = readtrackerinfo();
	if(pt.size() == 0) return "False";
	int portno = atoi(pt.c_str());
	// cout << "portno" << portno << endl;
	int sockfd = startconnection(portno);
	char buffer[100];
	memset(buffer,'\0',sizeof(buffer));
	if(sockfd == -1)
	{
		cout << "No Tracker is Online...Try later.." << endl;
		return "False";
	}

	string command="";
	command = "4";

	send(sockfd,command.c_str(),command.size(),0);
	recv(sockfd,buffer,sizeof(buffer),0);
	string recvedcommand;
	int i=0;
	while(buffer[i] != '\0'){
		recvedcommand = recvedcommand + buffer[i];
		i++;
	}
	close(sockfd);
	return recvedcommand;
}

string joingroup(string gid,int peerportno){
	char buffer[100];
	memset(buffer,'\0',sizeof(buffer));
	string pt = readtrackerinfo();
	if(pt.size() == 0) return "False";
	int portno = atoi(pt.c_str());

	int sockfd = startconnection(portno); if(sockfd == -1) return "False";
	if(sockfd == -1)
	{
		cout << "No Tracker is Online...Try later.." << endl;
		return "False";
	}
	
	string command="";
	string xx= "xxx";
	command = "5"+ xx + " " + gid + " " + to_string(peerportno);

	send(sockfd,command.c_str(),command.size(),0);
	recv(sockfd,buffer,sizeof(buffer),0);
	string recvedcommand;
	int i=0;
	while(buffer[i] != '\0'){
		recvedcommand = recvedcommand + buffer[i];
		i++;
	}
	close(sockfd);
	return recvedcommand;
}

string listjoiningrequest(string gid,int peerportno){
	string pt = readtrackerinfo();
	if(pt.size() == 0) return "False";
	int portno = atoi(pt.c_str());
	// cout << "portno" << portno << endl;
	int sockfd = startconnection(portno);  if(sockfd == -1) return "False";
	char buffer[2048];
	memset(buffer,'\0',sizeof(buffer));
	if(sockfd == -1)
	{
		cout << "No Tracker is Online...Try later.." << endl;
		return "False";
	}

	string command="";
	string xx= "xxx";
	command = "6"+ xx + " " + gid + " " + to_string(peerportno);

	send(sockfd,command.c_str(),command.size(),0);
	recv(sockfd,buffer,sizeof(buffer),0);
	string recvedcommand;
	int i=0;
	while(buffer[i] != '\0'){
		recvedcommand = recvedcommand + buffer[i];
		i++;
	}
	close(sockfd);
	return recvedcommand;
}

string acceptrequest(string gid,int userportno,int peerportno){
	string pt = readtrackerinfo();
	if(pt.size() == 0) return "False";
	int portno = atoi(pt.c_str());
	// cout << "portno" << portno << endl;
	int sockfd = startconnection(portno);
	char buffer[100];
	memset(buffer,'\0',sizeof(buffer));
	if(sockfd == -1)
	{
		cout << "No Tracker is Online...Try later.." << endl;
		return "False";
	}

	string command="";
	string xx= "xxx";
	// cout << gid << " " << userportno << " " << peerportno << endl;
	command = "7"+ xx + " " + gid + " " + to_string(userportno) + " " + to_string(peerportno);

	send(sockfd,command.c_str(),command.size(),0);
	recv(sockfd,buffer,sizeof(buffer),0);
	string recvedcommand;
	int i=0;
	while(buffer[i] != '\0'){
		recvedcommand = recvedcommand + buffer[i];
		i++;
	}
	close(sockfd);
	return recvedcommand;	
}

string leavegroup(string gid,int peerportno){

	string pt = readtrackerinfo();
	if(pt.size() == 0) return "False";
	int portno = atoi(pt.c_str());
	// cout << "portno" << portno << endl;
	int sockfd = startconnection(portno);  if(sockfd == -1) return "False";
	char buffer[100];
	memset(buffer,'\0',sizeof(buffer));
	if(sockfd == -1)
	{
		cout << "No Tracker is Online...Try later.." << endl;
		return "False";
	}

	string command="";
	string xx= "xxx";
	command = "8"+ xx + " " + gid + " " + to_string(peerportno);

	send(sockfd,command.c_str(),command.size(),0);
	recv(sockfd,buffer,sizeof(buffer),0);
	string recvedcommand;
	int i=0;
	while(buffer[i] != '\0'){
		recvedcommand = recvedcommand + buffer[i];
		i++;
	}
	close(sockfd);
	return recvedcommand;	
}

void getsha(char *res,int start,string path){

		FILE *f = fopen(path.c_str(),"rb");
	   char buffer[20];
      memset(buffer,'\0',sizeof(buffer));
      fseek(f,start,SEEK_SET);
	   fread(buffer,1,20,f);

		unsigned char SHA_Buffer[SHA_DIGEST_LENGTH];
		char buf[SHA_DIGEST_LENGTH*2];
		int i;
		memset(buf,'\0',sizeof(buf));
      memset(SHA_Buffer,'\0',sizeof(SHA_Buffer));
		SHA1((unsigned char *)buffer, 20, SHA_Buffer);
		
		for (i=0; i < SHA_DIGEST_LENGTH; i++) {
			sprintf((char*)&(buf[i*2]), "%02x", SHA_Buffer[i]);
		}
      memset(res,'\0',sizeof(res));
		for(int i=0;i<SHA_DIGEST_LENGTH;i++){
				res[i]=buf[i];
			}
	fclose(f);
}

string addshatomap(string path){

	FILE* fp = fopen(path.c_str(),"rb");
	fseek(fp,0,SEEK_END);
	int size = ftell(fp);
	fclose(fp);
	vector<string> v;
	string finalsha = "";
	int curr=0;
	while(size >0){
      char *res = new char[SHA_DIGEST_LENGTH*2];
      getsha(res,curr,path);
      string s="";
      for(int i=0;i<SHA_DIGEST_LENGTH;i++)
      {
            s= s+res[i];
      }
      v.push_back(s);
      finalsha = finalsha + s;
      curr= curr + 524288;
      size = size-524288;
  	}
  	filedetails[finalsha] = v;
  	filenamedetails[finalsha] = path;
  	return finalsha;
}

string uploadfile(string path,string sha,string gid,int peerportno){

	string pt = readtrackerinfo();
	if(pt.size() == 0) return "False";
	int portno = atoi(pt.c_str());
	// cout << "portno" << portno << endl;
	int sockfd = startconnection(portno);  if(sockfd == -1) return "False";
	char buffer[100];
	memset(buffer,'\0',sizeof(buffer));
	if(sockfd == -1)
	{
		cout << "No Tracker is Online...Try later.." << endl;
		return "False";
	}

	FILE* fp = fopen(path.c_str(),"rb");
	fseek(fp,0,SEEK_END);
	int size = ftell(fp);
	fclose(fp);

	string xx="xx";
	string command="";
	command = "9"+ xx + " " + sha + " " + gid + " " + to_string(peerportno) + " " + to_string(size);

	send(sockfd,command.c_str(),command.size(),0);
	recv(sockfd,buffer,sizeof(buffer),0);
	string recvedcommand;
	int i=0;
	while(buffer[i] != '\0'){
		recvedcommand = recvedcommand + buffer[i];
		i++;
	}
	close(sockfd);
	return recvedcommand;	
}

string listfile(string gid,int peerportno){
	string pt = readtrackerinfo();
	if(pt.size() == 0) return "False";
	int portno = atoi(pt.c_str());
	// cout << "portno" << portno << endl;
	int sockfd = startconnection(portno);  if(sockfd == -1) return "False";
	char buffer[2048];
	memset(buffer,'\0',sizeof(buffer));
	if(sockfd == -1)
	{
		cout << "No Tracker is Online...Try later.." << endl;
		return "False";
	}

	string command="";
	string xx= "xxx";
	command = "10"+ xx + " " + gid + " "+ to_string(peerportno);

	send(sockfd,command.c_str(),command.size(),0);
	recv(sockfd,buffer,sizeof(buffer),0);
	string recvedcommand;
	int i=0;
	while(buffer[i] != '\0'){
		recvedcommand = recvedcommand + buffer[i];
		i++;
	}
	close(sockfd);
	return recvedcommand;	
}

string downloadfile(string gid,string filename,int peerportno)
{

	string pt = readtrackerinfo();
	if(pt.size() == 0) return "False";
	int portno = atoi(pt.c_str());
	int sockfd = startconnection(portno);  if(sockfd == -1) return "False";
	char buffer[100];
	memset(buffer,'\0',sizeof(buffer));
	if(sockfd == -1)
	{
		cout << "No Tracker is Online...Try later.." << endl;
		return "False";
	}

	string xx="xx";
	string command="";
	command = "11"+ xx + " " + gid + " " + to_string(peerportno) + " " + filename;
	send(sockfd,command.c_str(),command.size(),0);
	recv(sockfd,buffer,sizeof(buffer),0);
	string recvedcommand;
	int i=0;
	while(buffer[i] != '\0'){
		recvedcommand = recvedcommand + buffer[i];
		i++;
	}
	close(sockfd);
	return recvedcommand;	

}

int connectforchunk(int portno)
{

	int port = portno;	
	int sockfd,n;
	char buffer[255];
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	
	if(sockfd < 0){
		error("Error Opening Socket");
		return -1;
	}
	string ip = "127.0.0.1";
	server = gethostbyname(ip.c_str());
	bzero((char *)&serv_addr,sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);
	
	if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0){
		cout << "Error Connecting" << endl;
		return -1;
	}
	cout << "Connected to server " << endl;
	return sockfd;
}

vector<string> getchunkdetails(vector<string> &server,string sha){

	vector<string> result;
	for(int i=0;i<server.size();i++)
	{
		cout << "Server msg sended to " << server[i] << endl;
		string p = server[i];
		int port = atoi(p.c_str());
		cout << "port is " << endl;
		int sockchunk = connectforchunk(port);
		char buffer[100];
		memset(buffer,'\0',sizeof(buffer));
		if(sockchunk == -1)
		{
			cout << "Peer is not online" << endl;
			return {};
		}
		string command = "file";
		command = command + " " + sha;
		// cout << "command sent is "<< command << endl;
		send(sockchunk,command.c_str(),command.size(),0);
		recv(sockchunk,buffer,sizeof(buffer),0);
		
		string recvedcommand;
		int k=0;
		while(buffer[k] != '\0'){
			recvedcommand = recvedcommand + buffer[k];
			k++;
		}
		close(sockchunk);
		// cout << "Comm resv is " << recvedcomman?d << endl;
		result.push_back(recvedcommand);
	}

	return result;
}

// void downloadfilefromchunk(unordered_map<string,vector<int>> &m,string path,string sha){
	
// 	int arr v[200];
// 	memset(v,0,sizeof(arr));
// 	for(auto i = m.begin();i!=m.end();i++){

// 		string portstring = i->first;
		
// 		for(int j=0;j<m[portstring].size();j++){
			
// 			if(v[j] == 0){
// 				v[j] =1;
// 				int chunk = m[portstring][j];
// 				int portno = atoi(portstring.c_str());
// 				int sock = connectforchunk(portno);
// 				string command = "download";
// 				command = command + " " + sha + " " + to_string(chunk);
// 				send(sock,command.c_str(),command.size(),0);

// 				FILE *fp = fopen(path.c_str(),"r+");	
// 				char Buffer [1] ;
// 				int file_size = (512*1024);
// 				int start = 0 + chunk*(512*1024); 
// 				fseek(fp,start,SEEK_SET);
// 				int n;
// 				while ( ( n = recv(sock,Buffer,1, 0)) > 0 && file_size > 0) {
// 					fwrite (Buffer , sizeof (char), n, fp);
// 					memset ( Buffer , '\0', 1);
// 					file_size = file_size - n;
// 				}
// 				close(sock);
// 				fclose(fp);	
// 			}	
// 		}		
// 	}
// }

void *writemsg(void *fd){
	struct downloaddetails *args = (struct downloaddetails *)fd;
			int chunk = args->chunk;
			int port = args->portno;
			int index = args->index;
			string sha = args->sha;
			string path = args->path;

			cout << sha << " " << port << " " << path  << " " << chunk << index << endl;	
			// int sockfd,n;
			// char buffer[255];
			// struct sockaddr_in serv_addr;
			// struct hostent *server;
			
			// sockfd = socket(AF_INET,SOCK_STREAM,0);
			
			// if(sockfd < 0){
			// 	error("Error Opening Socket");
			// }
			// string ip = "127.0.0.1";
			// server = gethostbyname(ip.c_str());
			// bzero((char *)&serv_addr,sizeof(serv_addr));
			
			// serv_addr.sin_family = AF_INET;
			// bcopy((char *)server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
			// serv_addr.sin_port = htons(port);
			
			// if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0){
			// 	cout << "Error Connecting" << endl;
			// }
			// cout << "Connected to server " << endl;


			// string command = "download";
			// command = command + " " + sha + " " + to_string(chunk);

			// cout << "comm via thread " << command << endl;
			// send(sockfd,command.c_str(),command.size(),0);

			// FILE *fp = fopen(path.c_str(),"r+");	
			// char Buffer [1] ;
			// int file_size = (512*1024);
			// int start = 0 + chunk*(512*1024); 
			// fseek(fp,start,SEEK_SET);
			// while ( ( n = recv(sockfd,Buffer,1, 0)) > 0 && file_size > 0) {
			// 	fwrite (Buffer , sizeof (char), n, fp);
			// 	memset ( Buffer , '\0', 1);
			// 	file_size = file_size - n;
			// }
			// close(sockfd);
			// fclose(fp);	
}

void downloadmultiplefile(unordered_map<string,vector<int>> &m,string path,string sha){

	pthread_t threads[10];
	for(auto i = m.begin();i!=m.end();i++){

		string portstring = i->first;
		int ss = m[portstring].size();
		int j=0;
		struct downloaddetails dd[5];
		while(j!=2){
			pthread_t th;
			dd[j].index = j;
			dd[j].sha = sha;
			dd[j].path = path;
			dd[j].portno = atoi(portstring.c_str());
			dd[j].chunk = m[portstring][j];
			cout << "downloadmultiplefile " << dd[j].sha << " " << dd[j].portno << " " << dd[j].path << " " << dd[j].chunk  << " " << dd[j].index << endl;
	     	pthread_create(&threads[j], NULL, writemsg, (void *)&dd[j]);
	     	j++;
	     }	
	}

	pthread_join(threads[0],NULL);
	pthread_join(threads[1],NULL);
	pthread_join(threads[2],NULL);	
}

void dotheaction(string msg,string path,string sha){
	
	int size; string temp; 
	vector<string> server;
	
	stringstream ss(msg);
	ss >> size;
	
	while(ss >> temp){ // tracker have sent size and port to contact
		server.push_back(temp);
	}

	char ch='0';
	string downloadfile = "downloadfile" + to_string(download); // create empty file using size given
    FILE *f = fopen(downloadfile.c_str(),"wb");
    int i=0;
    while(i<size){
       fwrite(&ch,sizeof(ch),1,f);
       i++;
    }
    fclose(f);

    vector<string> chunkdetails;
    chunkdetails = getchunkdetails(server,sha); // get chunkdetails from port given by tracker
    
    unordered_map<string,vector<int>> mm; // chunk details stored in map [port->chunk number]
    int a;
    for(int i=0;i<chunkdetails.size();i++)
    {
    	stringstream kk(chunkdetails[i]);
    	while(kk >> a){
	    	mm[server[i]].push_back(a);
	    }
    }
    for(auto i = mm.begin();i!=mm.end() ;i++)
    {
    	string nn = i->first;
    	for(int j=0;j<mm[nn].size();j++)
    	{
    		cout << i->first << " " << mm[nn][j]<< endl;
    	}
    }
    downloadmultiplefile(mm,downloadfile,sha);
    // download++;
}

void *sendchunkdetails(void *fd_pointer)
{
	cout << "Thread-" << ccc<< endl;
	ccc++;
	printf("Hello Server Handler \n");
	int sock = *(int *)fd_pointer;
	char buffer[1024];
	memset(buffer,'\0',1024);
	recv(sock,buffer,sizeof(buffer), 0);
	string msg="";
	int i=0;
	while(buffer[i] != '\0'){
		msg = msg + buffer[i];
		i++;
	}
	cout << "Msg recv from client " << msg << endl;
	stringstream ss(msg); 
	string result;
	ss >> result;
	if(result == "file"){
		string sentmsg="";
		int j=0;
		ss >> result;
		for(auto i = filedetails[result].begin();i!=filedetails[result].end();i++){
				if(*i != "0") sentmsg = sentmsg + to_string(j) + " ";
				j++;
		}
		// cout << "Msg send to client " << sentmsg << endl;
		send(sock,sentmsg.c_str(),sentmsg.size(),0);	
	}
	else if(result == "download"){
		ss >> result;
		int chunk;
		string  filepath =filenamedetails[result];
		cout << "File Details " << filepath << endl;
		ss >> chunk;
	    FILE *fp;
	    cout << "download command from client  " << result << " " << chunk << endl;
	    fp = fopen(filepath.c_str(),"rb");
	    int start = chunk*(512*1024);
		int end = start + (512*1024);
		int size = end-start+1;
		fseek ( fp , start , SEEK_SET);
		int n;
		char Buffer[1] ; 
		while ( ( n = fread( Buffer , sizeof(char) , 1 , fp ) ) > 0  && size > 0 ){
			send (sock , Buffer, n, 0 );
			memset ( Buffer , '\0', 1);
			size = size - n ;
		}
		fclose(fp);
	}
	close( sock);
	pthread_exit(NULL);
 
}


void *runserver(void *port){
	int portno = *(int *)port;
	int sockfd,n;
	int *new_sock; // for each pthread
	struct sockaddr_in serveraddress , clientaddress;
	sockfd = socket(AF_INET,SOCK_STREAM,0);// socket created
	if(sockfd < 0){
		error("error opening socket");
		}
	
	//setup hostname,portno,ipv4 
	bzero((char *)&serveraddress,sizeof(serveraddress));
	serveraddress.sin_family = AF_INET;
	serveraddress.sin_addr.s_addr = INADDR_ANY;
	serveraddress.sin_port= htons(portno);

	//bind and start listning
	if(bind(sockfd,(struct sockaddr *)&serveraddress, sizeof(serveraddress)) < 0) error("error bind socket");
	listen(sockfd,100);
	int clientsockfd[100];
	pthread_t threads[10];
	int sss=0;
	while(1)
	{

		// cout << "waiting for request" << endl;
		socklen_t clilen = sizeof(clientaddress);
		// pthread_t server_thread;
		clientsockfd[sss] = accept(sockfd,(struct sockaddr *)&clientaddress,&clilen);
		
		auto pid = pthread_create(&threads[sss],NULL,sendchunkdetails,&clientsockfd[sss]);

		if(pid!=0){
			perror("Server thread failed in main connection");
			exit(-1);
			}
			else{
			cout<<"Server success from main thread"<<endl;
			}		
	}
}

int main(int argc,char *argv[]){

	int peerportno;
	cout << "Enter My Port Number" <<endl;
	cin >> peerportno;
	pthread_t serverthread;
	string temp11;
	getline(cin,temp11);
	
	pthread_create(&serverthread,NULL,runserver,&peerportno); ////////////////server and client port define

	string msg,username,password,temp;
	bool running = true;
	bool login =false;
	while(running){
			string command;
			cout << "Enter Command" << endl;
			cout << endl;
			getline(cin,command);
			stringstream ss(command);	
			command = "";
			ss >> command;
			
			if(command == "create_user" && login==false){////////////////////////////////////////////////////////////1
				ss >> username;
				ss >> password;
				msg = createuser(username , password);
				if(msg == "False")
					{
						cout << "No Tracker is Online..try later" << endl;
						running = false;	
					}
					else{
						cout << msg << endl;
					}
			}
			else if(command == "login" && login==false){//////////////////////////////////////////////////////////////2
				
				ss >> username;
				ss >> password;
				msg = logintosystem(username,password,peerportno);
				if(msg == "False")
					{
						running = false;	
					}
					else{
						login = true;
						cout << msg << endl;
					}
			}

			else if(command == "create_group" && login==true){//////////////////////////////////////////3
				ss >>temp;
				msg = creategroup(temp,peerportno);
				if(msg == "False")
					{
						running = false;
						cout << "Group already Exist or Tracker Not available" << endl;	
					}
					else{
						cout << msg << endl;
					}
			}

			else if(command == "list_group" && login==true){//////////////////////////////////////////4
				msg = listgroup();
				if(msg == "False")
					{
						running = false;
						cout << "Group already Exist or Tracker Not available" << endl;	
					}
					else{
						cout << msg << endl;
					}
			}

			else if(command == "join_group" && login==true){//////////////////////////////////////////5
				ss >> temp; // gid
				msg= joingroup(temp,peerportno);
				if(msg == "False")
				{
					running = false;
					cout << "Tracker Not available" << endl;	
				}
				else{
					cout << msg << endl;
				}
			}

			else if(command == "list_request" && login==true){//////////////////////////////////////////6
				ss >> temp;
				msg = listjoiningrequest(temp,peerportno);
				if(msg == "False")
				{
					running = false;
					cout << "Tracker Not available" << endl;	
				}
				else{
					string s ="";
					for(int i=0;i<msg.size();i++)
					{
						if(msg[i] == '&') {
							cout << s << endl;
							s="";}
						else s = s + msg[i];
					}
				}
			}

			else if(command == "accept_request" && login==true){ //////////////////////////////////////////7
				ss >> temp; ///gid
				int k; ////userid
				ss >> k;
				msg = acceptrequest(temp,k,peerportno);
				if(msg == "False")
				{
					running = false;
					cout << "Tracker Not available" << endl;	
				}
				else{
					cout << msg << endl;
				}	
			}
			else if(command == "leave_group" && login==true){ //////////////////////////////////////////8
				ss >> temp; ///gid
				msg = leavegroup(temp,peerportno);
				if(msg == "False")
					{
						running = false;
						cout << "Tracker Not available" << endl;	
					}
					else{
						cout << msg << endl;
					}	
				}
			else if(command == "upload_file" && login==true){ //////////////////////////////////////////9
				
				string path;
				ss >> path; ///filepath
				ss >> temp; // gid
				string sha = addshatomap(path);
				msg = uploadfile(path,sha,temp,peerportno);
				
				if(msg == "False")
				{
					running = false;
					cout << "Tracker Not available" << endl;	
				}
				else{
					cout << msg << endl;
				}	
			}
			else if(command == "list_files" && login==true){ //////////////////////////////////////////10
				// gid
				ss >> temp; // gid
				
				msg = listfile(temp,peerportno);
				
				if(msg == "False")
				{
					running = false;
					cout << "Tracker Not available" << endl;	
				}
				else{
					stringstream pp(msg);
					string m;
					int k=1;
					while(pp >> m){
						cout << "File-" << k << " " << m << endl;
						k++;
					}
				}	
			}
			else if(command == "download_file" && login==true){///////////////////////////////////////11
				//groudid filename destinationpath
				ss >> temp;
				string sha;
				ss >> sha;
				string path;
				ss >> path;

				msg = downloadfile(temp,sha,peerportno);

				if(msg == "False"){
					cout << "Tracker Not available" << endl;	
				}
				else{
					cout << msg << endl;
				}

				dotheaction(msg,path,sha);
				cout << "Download Completed " << endl;
			}
			else{
				cout << "Wrong Operation" << endl;
			}


	}

	return 0;
}

	