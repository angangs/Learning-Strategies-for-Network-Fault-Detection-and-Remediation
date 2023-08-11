#include "createSizes.h"
#include "simulator.h"
#include "csfr.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <list>
#include <limits.h>
#include <istream>
#include <iterator>
#include <iostream>

void setArrays(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){
	ofstream observFile;
	ofstream costFile;
	ifstream fileArray;
	int repairAction=0;
	int i=0,j=0, cnt=0,cnt_=0;
	int test_array[ws_id+srv_id];
	
	
	for(int t=0;t<ws_id;t++){
		test_array[t] = ws[t].ID;
	}
	
	for(int t=ws_id;t<ws_id+srv_id;t++){
		test_array[t] = srv[cnt].ID;
		cnt++;
	}
	/**
	 * 
	 * create test actions
	 * 
	 */ 
	for(int t=0;t<ws_id+srv_id;t++){
		for(int i=t;i<ws_id+srv_id;i++){
			if(i==ws_id+srv_id-1)
				break;
			Actions[cnt_][0] = test_array[t];
			Actions[cnt_][1] = test_array[i+1];
			cnt_++;
		}
	}
	/**
	 * 
	 * create repair actions
	 * 
	 */ 
	for(j=(total_actions)-total;j<(total_actions);j++){
		repairAction++;
		Actions[j][0] = repairAction;
		Actions[j][1] = 0;
	}
	
	ofstream actionFile;
	actionFile.open("Action.txt");
	for(i=0;i<(total_actions);i++){
		actionFile<<""<<Actions[i][0]<<"--"<<Actions[i][1]<<"\n";
	}
	
	actionFile.close();
	
	for(i=0;i<total;i++){

		FaultState[i]=i+1;
	}
	
	/*
	 * KNOWN DISTRIBUTION !!
	 */ 
	for(i=0;i<total;i++){
		//PriorProb[i] = 1.0 / (sw_id + ws_id + hb_id + srv_id);
		if(i==0)//id=1
			PriorProb[i] = 0.3;
		else if(i==2)//id=3
			PriorProb[i] = 0.1;
		else if(i==6)//id=7
			PriorProb[i] = 0.1;
		else if(i==7)//id=8
			PriorProb[i] = 0.5;
		else
			PriorProb[i] = 0;
	}
	/**
	 *
	 * create observations
	 */ 
	for(i=0;i<(total_actions)-total;i++){
		for(j=0;j<total;j++){
			Observ[i][j] = 0;
			Cost[i][j] = 0;
		}
	}
	
	 ////////////////////////////////////////
	//repair actions @ Observ & Cost array//
   ////////////////////////////////////////
	int counter=0;

	for(i=(total_actions)-total;i<(total_actions);i++){

		if(is_ws(counter+1,ws,sw,srv,hb)){
			for(j=0;j<total;j++)
				Cost[i][j]=1800;
		}
		else if(is_sw(counter+1,ws,sw,srv,hb)){
			for(j=0;j<total;j++)
				Cost[i][j]=1400;
		}
		else if(is_srv(counter+1,ws,sw,srv,hb)){
			for(j=0;j<total;j++)
				Cost[i][j]=2200;
		}
		else if(is_hb(counter+1,ws,sw,srv,hb)){
			for(j=0;j<total;j++)
				Cost[i][j]=1000;
		}

		counter++;

		for(int t=0;t<total;t++){

			if(i-((total_actions)-total)==t)
				Observ[i][t]=1;
			else
				Observ[i][t]=0;
		}
	}

	 ////////////////////////////////////////
	////test actions @ Observ & Cost array//
   ////////////////////////////////////////
	computeObservAndCost( Observ, Cost, ws, sw, srv, hb);
	/*
	 * Write @ observation.txt file
	 */
	observFile.open("observation.txt");

	for(i=0;i<(total_actions);i++){
		observFile << "\n";

		for(j=0;j<total;j++)
			observFile << "\t" << Observ[i][j];
		observFile<<"\t"<<Actions[i][0]<<" to ";
		observFile<<Actions[i][1];
	}
	observFile.close();
	/*
	 * Write @ cost.txt file
	 */
	costFile.open("cost.txt");

	for(i=0;i<(total_actions);i++){
		costFile << "\n";

		for(j=0;j<total;j++)
			costFile << "\t" << Cost[i][j];
		costFile<<"\t"<<Actions[i][0]<<" to ";
		costFile<<Actions[i][1];
	}
	costFile.close();
}
