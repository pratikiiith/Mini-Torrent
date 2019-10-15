#include<vector>
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
#include<sstream>
#include<netdb.h>
#include <arpa/inet.h>
#include<unordered_map>
#include <fstream>
#include <openssl/sha.h>

using namespace std;

unordered_map<string,string> logindetails; 			// username // pwd
unordered_map<int,bool> online;					// username and peerportno
unordered_map<int,string> portandip;
unordered_map<int,vector<pair<int,int>>> pendingrequest;		// ownerportno and pendingrequest vector(containing peerportno)
unordered_map<string,vector<int>> groupmembers;     // gid and groupmembers including owner (id == peerportno) [0]POS==owner
unordered_map<string,vector<string>> filegroupdetails; //sha -> gid,gid
unordered_map<string,vector<int>> filesharingdetails; // sha->size,port,port...
char ch='\n';
string trackerpath;
int number;
int countt=1;

struct trackerdetails{
	string ip;
	int portno;
};


void error(const char* msg){
	perror(msg);
	exit(1);
}

bool storelogindetails(string key,string value){
	
	if(logindetails.find(key) != logindetails.end()){
		return false;
	}
	else{
		logindetails[key] = value;
		return true;	
	}
	
	// logindetails[key].second = true;
}

bool checkdetails(string key,string value,int portno,string ip)
{
	if(logindetails.find(key) != logindetails.end()){
		if(logindetails[key] == value){
			online[portno]= true;
			portandip[portno] = ip;
			return true;
		}
	}
	return false;
}

bool creategroup(string gid,int portno){

	if(groupmembers.find(gid) == groupmembers.end()){
		groupmembers[gid].push_back(portno);
		return true;
	}
	else{
		return false;
	}
}

void startcopyfrombackupfile()
{
	string path = "backup1.txt";
	ifstream login(path.c_str());
	string temp;
	string k,v,j;
	while(getline(login,temp)){
		stringstream ss(temp);
		ss >> k;
		ss >> v;
		logindetails[k] = v;
	}
	login.close();
	remove(path.c_str());

	path = "backup2.txt";
	ifstream user(path.c_str());
	temp="";
	while(getline(user,temp)){
		stringstream ss(temp);
		ss >> k;
		int n = atoi(k.c_str());
		online[n] = true;
	}
	user.close();
	remove(path.c_str());

	path = "backup3.txt";
	ifstream pending(path.c_str());
	temp="";
	int n1,n2,n3;
	while(getline(pending,temp)){
		stringstream ss(temp);
		ss >> k;
		n1 = atoi(k.c_str());
		while(ss >> v){
			n2 = atoi(v.c_str());
			ss >> j; 
			n3 = atoi(j.c_str());
			pendingrequest[n1].push_back(make_pair(n2,n3));
		}
	}
	pending.close();
	remove(path.c_str());

	path = "backup4.txt";
	ifstream groupmem(path.c_str());
	temp="";
	while(getline(groupmem,temp)){
		stringstream ss(temp);
		ss >> k;
		while(ss >> v){
			n1 = atoi(v.c_str());
			groupmembers[k].push_back(n1);
		}
	}

	groupmem.close();
	remove(path.c_str());

	path = "backup5.txt";
	ifstream filegroup(path.c_str());
	temp="";
	while(getline(filegroup,temp)){
		stringstream ss(temp);
		ss >> k;
		while(ss >> v){
			filegroupdetails[k].push_back(v);
		}
	}
	filegroup.close();
	remove(path.c_str());

	path = "backup6.txt";
	ifstream filesharing(path.c_str());
	temp="";
	while(getline(filesharing,temp)){
		stringstream ss(temp);
		ss >> k;
		while(ss >> v){
			n1 = atoi(v.c_str());
			filesharingdetails[k].push_back(n1);
		}
	}

	filesharing.close();
	remove(path.c_str());

}

void getallfiles()
{

}

void *handleall(void *fd)
{
	int sock = *(int *)fd;
	int commandno;
		char buffer[1024];
		memset(buffer,'\0',1024);
		int len;
		recv(sock,buffer,sizeof(buffer), 0);
		string command;
		int i=0;
		while(buffer[i]!='\0'){
			command = command + buffer[i];
			i++;			
		}
		// cout << "command recv is " << command << endl;

		stringstream ss(command);
		ss >> commandno;
		string loginport,loginpwd,temp,order;
		int tempnum;

		if(commandno == 20){
			startcopyfrombackupfile();
		}

		if(commandno == 21){

				memset(buffer,'\0',1024);		
				string path = "backup" + to_string(countt) + ".txt";
				string cc= "ok";
				send(sock,cc.c_str(),cc.size(),0);
				FILE *fp = fopen(path.c_str(),"wb");
				char Buf [1] ; 
				// int file_size;
				int n;
				// recv(sock, &file_size, sizeof(file_size), 0); 
				while ( ( n = recv(sock,Buf,1, 0)) > 0){
					fwrite (Buf , sizeof (char), n, fp);
					// cout << Buf;
					memset ( Buf , '\0', 1);
					// file_size = file_size - n;
				} 
				fclose(fp);
				countt++;
		}

		if(commandno==1){/////////////////////////////////////////////////////////////create user
				ss >> loginport;
				ss >> loginpwd;
				if(storelogindetails(loginport,loginpwd)){
					order= "Login Details Saved";
				}
				else{
					order= "Username Already Exist";
				}
	 			send(sock,order.c_str(),order.size(),0);	
			}
		else if(commandno == 2){/////////////////////////////////////////////////////login system
				ss >> loginport; // username
				ss >> loginpwd; // pwd
				ss >> tempnum; //peerportno
				string ip;
				ss >> ip;
				if(checkdetails(loginport,loginpwd,tempnum,ip))
				{

					order="Logged in Successfully";
					send(sock,order.c_str(),order.size(),0);
				}
				else{
					order = "Wrong Credential";
					send(sock,order.c_str(),order.size(),0);
				}
			}
		else if(commandno == 3){ ////////////////////////////////////////////////////////create group
				ss >> temp;//junk
				ss >> temp;//gid
				ss >> tempnum; // portno
				if(creategroup(temp,tempnum)){
					order= "Group Created";	
				}
				else{
					order = "Group Already Exist";
				}
				send(sock,order.c_str(),order.size(),0);	
		}
		else if(commandno == 4) //////////////////////////////////////////////////// List Group id
		{	
			string message = "";
			for(auto i=groupmembers.begin();i!=groupmembers.end();i++)
			{
				message = message + i->first + " ";
			}
			send(sock,message.c_str(),message.size(),0);
		}

		else if(commandno == 5) //////////////////////////////////////////////////// Join group
		{	
			ss >> temp;    //junk
			ss >> temp;    //gid
			ss >> tempnum; //portno

			int owner = groupmembers[temp][0]; //group owner
			if(owner == tempnum) {
				order = "You are Owner of this group";
			}
			else{
				int f = atoi(temp.c_str());
				pendingrequest[owner].push_back(make_pair(f,tempnum)); // add pending request to owner's pending vector // gid
				order= "Request sent to Owner";
			}
			
			send(sock,order.c_str(),order.size(),0);
		}

		else if(commandno == 6) ///////////////////////////////////////////////////list pending request
		{
			ss >> temp;
			ss >> temp;//gid
			ss>> tempnum;//peerportno
			int gid = atoi(temp.c_str());
			order = "";
			string gg = "Group ID: ";
			string cc = "Cliend ID: ";
			if(pendingrequest.find(tempnum) == pendingrequest.end()){
				order = "No Request Available";
			}
			else{
				for(int i=0;i<pendingrequest[tempnum].size();i++)
				{
					if(pendingrequest[tempnum][i].first == gid){
						order = order + gg + " " + temp + " " +cc +" " + to_string(pendingrequest[tempnum][i].second) + "&";
					}
				}
				if(order.size()==0) order="No Pending Request";
			}
			send(sock,order.c_str(),order.size(),0);
		}
		
		else if(commandno == 7) ///////////////////////////////////////////////////accept request
		{
			int peer;
			ss >> temp; //junk
			ss >> temp; // gid
			ss>> tempnum; // userportno
			ss >> peer; // peerport
			order = "";
			int gid = atoi(temp.c_str());
			auto i = pendingrequest[peer].begin();
			for(int j=0;j<pendingrequest[peer].size();j++)
			{
				if(pendingrequest[peer][j].first == gid && pendingrequest[peer][j].second == tempnum){
					pendingrequest[peer].erase(i+j);
				}
			}
			if(pendingrequest[peer].size() == 0){
				pendingrequest.erase(peer);
			}

			groupmembers[temp].push_back(tempnum);
			order = "Request Accepted";
			send(sock,order.c_str(),order.size(),0);
		}
		else if(commandno == 8) ///////////////////////////////////////////////////leave group
		{
			string tempp;
			int peer;
			ss >> tempp; //junk
			ss >> temp; // gid
			// ss >> tempp;
			ss>> tempnum; // peerportno
			order = "";
			// cout << temp << " " << tempnum << endl;
			cout << groupmembers[temp][0] << " is " << endl;
			if(groupmembers[temp][0] == tempnum){
				order = "You are Owner of Group..You can't Leave..You Opted for it";
			}
			else{
				for(auto i = groupmembers[temp].begin();i!=groupmembers[temp].end();i++){
					if(*i == tempnum){
						groupmembers[temp].erase(i);
						break;}
					}
				order="Group Exit";
			}	
			send(sock,order.c_str(),order.size(),0);
		}

		else if(commandno == 9) ///////////////////////////////////////////////////file upload
		{
			string gid;
			int size;
			ss >> temp;
			ss >> temp; // sha
			ss >> gid; // gid
			ss >> tempnum; //peerportno
			ss >> size;
			bool groupfound = true;
			bool member = false;
			// cout << temp << " "  << gid << " " << tempnum << endl;
			if(groupmembers.find(gid) == groupmembers.end()){ ////////////////check whether group exist or not
				order = "Group Not Found";
				send(sock,order.c_str(),order.size(),0);
				groupfound=false;
			}
			if(groupfound){									////////////////////////////check member is in group or not

				for(int i=0;i<groupmembers[gid].size();i++){
					if(groupmembers[gid][i] == tempnum){
						member = true;
					} 
				}	
			}

			if(member){ /////////////////////////add details to file gid...portno
				bool f= false;
				for(int i=0;i<filegroupdetails[temp].size();i++){
					if(filegroupdetails[temp][i] == gid) f = true;
				}				
					if(!f) filegroupdetails[temp].push_back(gid); /// sha->gid

					if(filesharingdetails.find(temp) == filesharingdetails.end()) filesharingdetails[temp].push_back(size);
					bool found = false;
					for(int i=0;i<filesharingdetails[temp].size();i++){
						if(filesharingdetails[temp][i] == tempnum) found=true;
					}
					if(!found) filesharingdetails[temp].push_back(tempnum);  /// sha->[p1 ,p2 ,...]
					order = "File uploaded";
			}	
			else{
				order = "Client is not a member of Group";
			}
			
			send(sock,order.c_str(),order.size(),0);
			
			}
		
			else if(commandno == 10){/////////////////////////////////////////////list files
				string tempp;
				ss >> tempp; // junk
				ss >> temp; // gid
				ss >> tempnum; // portno
				int gg = atoi(temp.c_str());
				order = "";

				for(auto i = filegroupdetails.begin();i!=filegroupdetails.end();i++)
				{
					for(int j=0;j < i->second.size();j++){
						if(i->second[j] == temp){
							order = order + i->first + " ";
							break;
						}
					}
				}
				send(sock,order.c_str(),order.size(),0);
			}

			else if(commandno == 11){ /////////////////////////////////////////////////download
				// command = "11"+ xx + " " + gid + " " + to_string(peerportno) + " " + filename + " " + path;
				string filename;
				ss >> temp;
				ss >> temp; // gid
				ss >> tempnum; //portno//////////////////////////////////////////////////needs to check permission
				ss >> filename; // sha
				bool ingroup = false;
				bool fileingroup = false;
				// portno is in group or not
				for(int i=0;i<groupmembers[temp].size();i++)
				{
					if(groupmembers[temp][i] == tempnum) ingroup = true;
				}
				for(int i=0;i<filegroupdetails[filename].size();i++)
				{
					if(filegroupdetails[filename][i] == temp) fileingroup = true; 
				}
				// cout << ingroup << " " << fileingroup << endl;
				if(ingroup && fileingroup){
					int t = filesharingdetails[filename][0]; 	
					order = order + to_string(t) + " "; /// file size appended
					string ip;
					string pp;
					for(int i=1;i<filesharingdetails[filename].size();i++){
						
						ip = portandip[filesharingdetails[filename][i]]; /// ip of port
						pp = to_string(filesharingdetails[filename][i]); // port 
						if(online[atoi(pp.c_str())]){
							order = order + " " + pp + " " + ip; // append port and ip
						}
						
					}
				}	
				else{
					order = "File not Available or Client is not a member of group" ;
				}
				send(sock,order.c_str(),order.size(),0);		
			}

			else if(commandno == 12){ //filesharing  sha->portno
				string gid;
				string sha;
				ss >> gid;
				ss >> gid;
				ss >> sha;
				ss >> tempnum;
				auto j = filegroupdetails[sha].begin();
				for (int i = 0;i<filegroupdetails[sha].size(); ++i)
				{
					if(filegroupdetails[sha][i] == gid) filegroupdetails[sha].erase(j+i);
				}
				if(filegroupdetails[sha].size() == 0){
					filegroupdetails.erase(sha);
				}
				order = "done";
				send(sock,order.c_str(),order.size(),0);		
			}
			else if(commandno == 13){
				ss >> temp;
				ss >> tempnum;
				online[tempnum] = false;
				order = "done";
				send(sock,order.c_str(),order.size(),0);
			}
		close(sock);
}


void *kernelthread(void *kernelpointer){
	struct trackerdetails *args = (struct trackerdetails *)kernelpointer;
	int portno = args->portno;
	string ip = args->ip;
	int sockfd,n;
	int *new_sock; // for each pthread
	struct sockaddr_in serveraddress , clientaddress;


	cout << "in kernel-1" << endl;

	// socket created
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0){
		error("error opening socket");
		}
	
	//setup hostname,portno,ipv4 
	bzero((char *)&serveraddress,sizeof(serveraddress));
	serveraddress.sin_family = AF_INET;
	serveraddress.sin_addr.s_addr = INADDR_ANY;
	serveraddress.sin_port= htons(portno);
  
    serveraddress.sin_addr.s_addr = inet_addr(ip.c_str()); 
	//bind and start listning
	if(bind(sockfd,(struct sockaddr *)&serveraddress, sizeof(serveraddress)) < 0) error("error bind socket");
	listen(sockfd,100);
	int clientsockfd[100];
	int sss=0;
	while(1)
	{
		// cout << "waiting for request" << endl;
		socklen_t clilen = sizeof(clientaddress);
		pthread_t server_thread;
		clientsockfd[sss] = accept(sockfd,(struct sockaddr *)&clientaddress,&clilen);
		
		auto pid =pthread_create(&server_thread,NULL,handleall,&clientsockfd[sss]);

		if(pid!=0){
			perror("thread failed in main connection");
			exit(-1);
			}
			else{
			cout<<"Success from main thread"<<endl;
			}

		
	}

	pthread_exit(NULL);

}


string readtrackerinfo(){

	ifstream tracker(trackerpath.c_str());
	string port;
	string ip;
	string result;
	int i=0;
	while(i!=number){
		getline(tracker,result);
		i++;
	}
	stringstream ss(result);
	ss >> port;
	ss >> ip;
	tracker.close();
	result ="";
	result = port + " " + ip;
	// cout << result << endl;
	return result;		
}

// int startconnection(int portno){
// 		int port = portno;	
// 		int sockfd,n;
// 		char buffer[255];
// 		struct sockaddr_in serv_addr;
// 		struct hostent *server;
		
// 		sockfd = socket(AF_INET,SOCK_STREAM,0);
		
// 		if(sockfd < 0){
// 			error("Error Opening Socket");
// 			return -1;
// 		}
// 		string ip = "127.0.0.1";
// 		server = gethostbyname(ip.c_str());
// 		bzero((char *)&serv_addr,sizeof(serv_addr));
		
// 		serv_addr.sin_family = AF_INET;
// 		bcopy((char *)server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
// 		serv_addr.sin_port = htons(port);
		
// 		if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0){
// 			cout << "Error Connecting" << endl;
// 			return -1;
// 		}

// 		return sockfd;
// }

int startconnection()
{
	
	string pt = readtrackerinfo();
	int port;
	string ipp;
	stringstream ss(pt);
	ss >> port;
	ss >> ipp;
	// cout << port << " "  << ipp << endl;
	int sockfd,n;
	char buffer[255];
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	
	if(sockfd < 0){
		error("Error Opening Socket");
		return -1;
	}
	// string ip = "127.0.0.1";
	server = gethostbyname(ipp.c_str());
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

void copyfunction(string path){

		char buffer[100];
		memset(buffer,'\0',sizeof(buffer));

		int sockfd = startconnection();
		if(sockfd == -1)
		{
			cout << "No Tracker is Online...Try later.." << endl;
		}
		string command="";
		command = "21";
		send(sockfd,command.c_str(),command.size(),0);
		recv(sockfd,buffer,sizeof(buffer), 0);
		memset(buffer,'\0',sizeof(buffer));
		// int sock = startconnection();
    	string  filepath = path;
	    FILE *fp;
	    fp = fopen(filepath.c_str(),"rb");
	    fseek ( fp , 0 , SEEK_END);
	  	int size = ftell ( fp );
	  	rewind (fp);
		// // send ( sockfd , &size, sizeof(size), 0);
		int n;
		char Buffer[1] ; 
			while ( ( n = fread( Buffer , sizeof(char) , 1 , fp ) ) > 0  && size > 0 ){
				send (sockfd , Buffer, n, 0 );
				// cout << Buffer;
		   	 	memset ( Buffer , '\0', 1);
				size = size - n ;
		}
		fclose(fp);
		memset(buffer,'\0',sizeof(buffer));
		remove(filepath.c_str());
		close( sockfd);

}

void copyfunction1(){


		char buffer[100];
		memset(buffer,'\0',sizeof(buffer));

		int sockfd = startconnection();
		if(sockfd == -1)
		{
			cout << "No Tracker is Online...Try later.." << endl;
		}
		string command="";
		command = "20";
		send(sockfd,command.c_str(),command.size(),0);
		close( sockfd);

}

void createbackupforall()
{
	FILE* fp = fopen("backup1.txt","wb");
	string temp;
	int n;
	for(auto i=logindetails.begin();i!=logindetails.end();i++)
	{
		temp = i->first + " " + i->second;
		fwrite(temp.c_str(),1,temp.size(),fp);
		fwrite(&ch,1,sizeof(ch),fp);
	}
	fclose(fp);

	FILE* f2 = fopen("backup2.txt","wb");
	// string temp;
	for(auto i=online.begin();i!=online.end();i++)
	{
		n = i->first;
		temp = to_string(n);
		fwrite(temp.c_str(),1,temp.size(),f2);
		fwrite(&ch,1,sizeof(ch),f2);
	}
	fclose(f2);

	FILE* f3 = fopen("backup3.txt","wb");
	for(auto i=pendingrequest.begin();i!=pendingrequest.end();i++)
	{
		n = i->first;
		temp = to_string(n);
		for(int j=0;j<pendingrequest[n].size();j++){
			temp = temp + " " + to_string(pendingrequest[n][j].first) + " " + to_string(pendingrequest[n][j].second);
		}
		fwrite(temp.c_str(),1,temp.size(),f3);
		fwrite(&ch,1,sizeof(ch),f3);
	}

	FILE* f4 = fopen("backup4.txt","wb");
	for(auto i=groupmembers.begin();i!=groupmembers.end();i++)
	{
		temp = i->first;
		string tt = i->first;
		for(int j=0;j<groupmembers[tt].size();j++){
			temp = temp + " " + to_string(groupmembers[tt][j]);
		}
		fwrite(temp.c_str(),1,temp.size(),f4);
		fwrite(&ch,1,sizeof(ch),f4);
	}

	FILE* f5 = fopen("backup5.txt","wb");
	for(auto i=filegroupdetails.begin();i!=filegroupdetails.end();i++)
	{
		temp = i->first;
		string tt = i->first;
		for(int j=0;j<filegroupdetails[tt].size();j++){
			temp = temp + " " + filegroupdetails[tt][j];
		}
		fwrite(temp.c_str(),1,temp.size(),f5);
		fwrite(&ch,1,sizeof(ch),f5);
	}

	FILE* f6 = fopen("backup6.txt","wb");
	for(auto i=filesharingdetails.begin();i!=filesharingdetails.end();i++)
	{
		temp = i->first;
		string tt = i->first;
		for(int j=0;j<filesharingdetails[tt].size();j++){
			temp = temp + " " + to_string(filesharingdetails[tt][j]);
		}
		fwrite(temp.c_str(),1,temp.size(),f6);
		fwrite(&ch,1,sizeof(ch),f6);
	}

	
	fclose(fp);
	fclose(f2);
	fclose(f3);
	fclose(f4);
	fclose(f5);
	fclose(f6);	

}



int main(int argc,char *argv[]){

	trackerpath = argv[1];
	number = atoi(argv[2]);
	string address = readtrackerinfo();
	int portno;
	string ip;
	stringstream ss(address);
	ss >> portno;
	ss >> ip;

	struct trackerdetails td;
	td.portno = portno;
	td.ip = ip;
	
	// portaddedtotracker(trackerip,portno);

	pthread_t kernel;
	int i=0;
	
	auto pid = pthread_create(&kernel,NULL,kernelthread,(void *)&td);
	string s= "quit";
	
	while(1){
		string input;
		getline(cin,input);
		if(input == s){
			createbackupforall();
			// portdeletedfromtracker(portno); 

			if(number == 1) number = 2;
			else number=1;
			string p = "backup";
			for(int i=1;i<7;i++){	
					p = p + to_string(i) + ".txt";
					copyfunction(p);
					p="backup";
			}
				copyfunction1();
			return 0;
		} 
	}
	return 0;
} 	




	