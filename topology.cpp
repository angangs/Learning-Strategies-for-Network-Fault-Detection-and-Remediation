#include "simulator.h"
#include "csfr.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <list>
#include <limits.h>
#include <istream>
#include <sstream>
#include <iterator>
#include <string>

using namespace std;						

void readMyTopology(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){
	
	std::string line;
	ifstream infile("SIM.dta");
	
	cout<<"\n\n[READ TOPOLOGY]";
	
	while ( getline(infile, line) ){
		cout<<"\n\n-------------------------------------";
		cout<<"\n\n"<<line<<"\n";
		ParsingFun(line,ws,sw,srv,hb);
	}
}

void ParsingFun(std::string line,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){
	
	int pos=0;
	std::string words[total+1];
	stringstream ss(line);
	string h;
	
	for (int i = 0; i < total+1; ++i)
		words[i] = '0';
	
	int i = 0;
	
	while(ss>>h){
		words[i] = h;
		i++;
	}
	
	if( "ws" == words[0] ){
		pos = returnNextPosition(ws,sw,srv,hb,1);
		
		ws[pos].ID = atoi(words[1].c_str());
		
		cout<<"\n\nID: "<<atoi(words[1].c_str());
		
		for(int i = 0 ; i < ws_lnk ; i++){
			cout<<"\n\nLINKER: "<<atoi(words[2+i].c_str());
			if(0 < atoi(words[2+i].c_str()) && atoi(words[2+i].c_str()) <= total){
				ws[pos].linkerID[i] = atoi(words[2+i].c_str());
			}
		}
	}
	else if( "sw" == words[0] ){		
		pos = returnNextPosition(ws,sw,srv,hb,2);
		
		sw[pos].ID = atoi(words[1].c_str());
		
		cout<<"\n\nID: "<<atoi(words[1].c_str());
		
		for(int i = 0 ; i < sw_lnk ; i++){
			cout<<"\n\nLINKER: "<<atoi(words[2+i].c_str());
			if(0 < atoi(words[2+i].c_str()) && atoi(words[2+i].c_str()) <= total){
				sw[pos].linkerID[i] = atoi(words[2+i].c_str());
			}
		}
	}
	
	else if( "hb" == words[0] ){		
		pos = returnNextPosition(ws,sw,srv,hb,3);
		
		hb[pos].ID = atoi(words[1].c_str());
		
		cout<<"\nID: "<<atoi(words[1].c_str());
		
		for(int i = 0 ; i < hb_lnk ; i++){
			cout<<"\n\nLINKER: "<<atoi(words[2+i].c_str());
			if(0 < atoi(words[2+i].c_str()) && atoi(words[2+i].c_str()) <= total){
				hb[pos].linkerID[i] = atoi(words[2+i].c_str());
			}
		}
	}
	
	else if( "srv" == words[0] ){		
		pos = returnNextPosition(ws,sw,srv,hb,4);
		
		srv[pos].ID = atoi(words[1].c_str());
		
		cout<<"\n\nID: "<<atoi(words[1].c_str());
		
		for(int i = 0 ; i < srv_lnk ; i++){
			cout<<"\n\nLINKER: "<<atoi(words[2+i].c_str());
			if(0 < atoi(words[2+i].c_str()) && atoi(words[2+i].c_str()) <= total){
				srv[pos].linkerID[i] = atoi(words[2+i].c_str());
			}
		}
	}
}

void myPrint(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){

	int i=0,j=0;
	ofstream myfile;
	myfile.open("Results.txt",ofstream::in | ofstream::out | ofstream::app);
	myfile << "###########################\nSCENARIO OF MY TOPOLOGY!\n###########################\n"<<endl;
	myfile << "##################### HUB LINKS #####################\n"<<endl;

	for(j=0;j<hb_id; j++){
		myfile << "hb id is: " << hb[j].ID<<"\n"<<endl;

		for(i=0;i<hb_lnk; i++)
			if(hb[j].linkerID[i]!=0)
				myfile << "hb linkerID is: " << hb[j].linkerID[i]<<"\n"<<endl;
	}

	myfile << "##################### SERVERS LINKS #####################\n"<<endl;
	for(j=0;j<srv_id; j++){
		myfile << "srv id is: " << srv[j].ID<<"\n";

		for(i=0;i<srv_lnk; i++)
			if(srv[j].linkerID[i]!=0)
				myfile << "srv linkerID is: " << srv[j].linkerID[i]<<"\n";
	}

	myfile <<"##################### SWITCHES LINKS #####################\n"<<endl;
	for(j=0;j<sw_id; j++){
		myfile << "sw id is: " << sw[j].ID<<"\n";

		for(i=0;i<sw_lnk; i++)
			if(sw[j].linkerID[i]!=0)
				myfile << "sw linkerID is: " << sw[j].linkerID[i]<<"\n"<<endl;
	}

	myfile << "##################### WORKSTATIONS LINKS #####################\n"<<endl;
	for(j=0;j<ws_id; j++){
		myfile << "ws id is: " << ws[j].ID<<"\n";

		for(i=0;i<ws_lnk; i++)
			if(ws[j].linkerID[i]!=0)
				myfile << "ws linkerID is: " <<ws[j].linkerID[i]<<"\n"<<endl;	
	}
}

bool is_ws(int id,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){
		for(int i = 0; i < ws_id; i++){
			if(ws[i].ID == id)
				return true;
		}
		return false;
}

bool is_sw(int id,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){
	for(int i = 0; i < sw_id; i++){
			if(sw[i].ID == id)
				return true;
		}
		return false;
}

bool is_srv(int id,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){
	for(int i = 0; i < srv_id; i++){
			if(srv[i].ID == id)
				return true;
		}
		return false;
}

bool is_hb(int id,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){
	for(int i = 0; i < hb_id; i++){
			if(hb[i].ID == id)
				return true;
		}
		return false;
}

int pos_ws(int id,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){
	for(int i=0; i<ws_id; i++){
		if(ws[i].ID == id)
			return i;
	}
	return 0;
}

int pos_sw(int id,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){
	for(int i=0; i<sw_id; i++){
		if(sw[i].ID == id)
			return i;
	}
	return 0;
}

int pos_srv(int id,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){
	for(int i=0; i<srv_id; i++){
		if(srv[i].ID == id)
			return i;
	}
	return 0;
}

int pos_hb(int id,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){
	for(int i=0; i<hb_id; i++){
		if(hb[i].ID == id)
			return i;
	}
	return 0;
}

int returnNextPosition(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[],int type){
	
	int nextpos = 0;
	
	switch(type){
		case 1:
					
			for(int i = 0 ; i < ws_id ; i++){
				if(ws[i].ID == 0){
					nextpos = i;
					break;
				}
			}
			
			break;
		
		case 2:
					
			for(int i = 0 ; i < sw_id ; i++){
				if(sw[i].ID == 0){
					nextpos = i;
					break;
				}
			}
			
			break;
		
		case 3:
					
			for(int i = 0 ; i < hb_id ; i++){
				if(hb[i].ID == 0){
					nextpos = i;
					break;
				}
			}
			
			break;
		
		case 4:
					
			for(int i = 0 ; i < srv_id ; i++){
				if(srv[i].ID == 0){
					nextpos = i;
					break;
				}
			}
			
			break;
	
		default:
		
			break;
	}	
	
	return nextpos;						
}
