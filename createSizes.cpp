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

int total=0;
int ws_id;
int sw_id;
int hb_id;
int srv_id;
int ws_lnk;
int sw_lnk;
int hb_lnk;
int srv_lnk;
int arr_lnk;
int test_actions;
int total_actions;
int max_ws_lnk=0;
int max_sw_lnk=0;
int max_hb_lnk=0;
int max_srv_lnk=0;

void findSize(){
	
	std::string line;
	ifstream infile_("SIM.dta");
	ifstream infile_2("SIM.dta");
	
	while ( getline(infile_, line) ){
		total++;
	}
	
	while ( getline(infile_2, line) ){
		readPopulation(line);
	}
	
	ws_lnk = max_ws_lnk;
	sw_lnk = max_sw_lnk;
	hb_lnk = max_hb_lnk;
	srv_lnk = max_srv_lnk;

	int max_lnk=0;
	
	if(ws_lnk>sw_lnk)
		max_lnk = ws_lnk;
	else
		max_lnk = sw_lnk;
		
	if(max_lnk<hb_lnk)
		max_lnk = hb_lnk;
		
	if(max_lnk<srv_lnk)
		max_lnk = srv_lnk;
		
	arr_lnk = max_lnk;

	array();
	workstation();
	server();
	switches();
	hub();

	int j=0,k=0,oldl=0,l=1,old=0,val=0;
	
	for(int i=0;i<srv_id+ws_id-1;i++){

		old=val;

		val=val+srv_id+ws_id-1-i;

		k++;

		oldl=l;

		for(j=old;j<val;j++)
			l++;

		l=oldl+1;
	}
	test_actions=j;

	total_actions = test_actions + total;
	
	make_actions_array();
	make_fault_state_array();
	make_prior_prob_array();
	make_observ_array();
	make_cost_array();
	
	cout<<"\n\n-------------------------------------";
	cout<<"\n[INFORMATION OF TOPOLOGY]";
	cout<<"\n\ntotal: "<<total;
	cout<<"\nworkstations : "<<ws_id;
	cout<<"\nswitches : "<<sw_id;
	cout<<"\nhubs : "<<hb_id;
	cout<<"\nservers : "<<srv_id;
	cout<<"\nworkstation's link : "<<ws_lnk;
	cout<<"\nswitches's link : "<<sw_lnk;
	cout<<"\nhub's link : "<<hb_lnk;
	cout<<"\nserver's link : "<<srv_lnk;
	cout<<"\nmaximum number of links : "<<arr_lnk;
	cout<<"\nnumber of test actions : "<<test_actions;
	cout<<"\nnumber of repair actions : "<<total;
	cout<<"\nnumber of total actions : "<<total_actions;
	cout<<"\n-------------------------------------";
}

array::array()
{link.resize(arr_lnk);};

workstation::workstation()
{linkerID.resize(ws_lnk);};

server::server()
{linkerID.resize(srv_lnk);};

hub::hub()
{linkerID.resize(hb_lnk);};

switches::switches()
{linkerID.resize(sw_lnk);};

void make_fault_state_array(){
	FaultState.reserve(total);
}

void make_prior_prob_array(){
	PriorProb.reserve(total);
}

void make_observ_array(){
	int map1[(total_actions)][total];
	for(int i=0; i<(total_actions); i++){
		for(int j=0; j<total; j++)
			map1[i][j] = 0;
	}
	Observ.reserve((total_actions));
	for(int i=0; i<(total_actions); i++){
		Observ.push_back(std::vector<int>(map1[i], map1[i] + total));
	}
}

void make_cost_array(){
	int map2[(total_actions)][total];
	for(int i=0; i<(total_actions); i++){
		for(int j=0; j<total; j++)
			map2[i][j] = 0;
	}
	Cost.reserve((total_actions));
	for(int i=0; i<(total_actions); i++){
		Cost.push_back(std::vector<int>(map2[i], map2[i] + total));
	}
}

void make_actions_array(){
	int map3[total_actions][2];
	for(int i=0; i<total_actions; i++){
		for(int j=0; j<2; j++)
			map3[i][j] = 0;
	}
	Actions.reserve(total_actions);
	for(int i=0; i<total_actions; i++){
		Actions.push_back(std::vector<int>(map3[i], map3[i] + 2));
	}
}

void readPopulation(std::string line){
	
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
		ws_id++;
		ws_lnk=0;
		for(int i = 0 ; i < total-1 ; i++){

			if(0 < atoi(words[2+i].c_str()) && atoi(words[2+i].c_str()) <= total)
				ws_lnk++;
		}
		if(max_ws_lnk<ws_lnk)
			max_ws_lnk = ws_lnk;
	}
	else if( "sw" == words[0] ){
		sw_id++;
		sw_lnk=0;
		for(int i = 0 ; i < total-1 ; i++){
			
			if(0 < atoi(words[2+i].c_str()) && atoi(words[2+i].c_str()) <= total)
				sw_lnk++;
		}
		if(max_sw_lnk<sw_lnk)
			max_sw_lnk = sw_lnk;
	}
	
	else if( "hb" == words[0] ){
		hb_id++;
		hb_lnk=0;
		for(int i = 0 ; i < total-1 ; i++){
			
			if(0 < atoi(words[2+i].c_str()) && atoi(words[2+i].c_str()) <= total)
				hb_lnk++;
		}
		if(max_hb_lnk<hb_lnk)
			max_hb_lnk = hb_lnk;
	}
	
	else if( "srv" == words[0] ){
		srv_id++;
		srv_lnk=0;
		for(int i = 0 ; i < total-1 ; i++){

			if(0 < atoi(words[2+i].c_str()) && atoi(words[2+i].c_str()) <= total)
				srv_lnk++;
		}
		if(max_srv_lnk<srv_lnk)
			max_srv_lnk = srv_lnk;
	}
}
