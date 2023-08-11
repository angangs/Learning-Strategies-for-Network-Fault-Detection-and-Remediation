#include "createSizes.h"
#include "topology.h"
#include "precsfr.h"
#include "simulator.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <list>
#include <limits.h>
#include <istream>
#include <iterator>
#include <time.h>
#include <math.h>

#define NANOS 1000000000LL

using namespace std;

int fault_node = 0;
int total_episodes=1;

void myInitialize(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){

	int i=0,j=0;

	for(i=0;i<sw_id;i++){
		sw[i].workFine=1;
		sw[i].visited=0;
		sw[i].ID=0;
		for(j=0;j<sw_lnk;j++)
			sw[i].linkerID[j]=0;
	}

	for(i=0;i<ws_id;i++){
		ws[i].workFine=1;
		ws[i].visited=0;
		ws[i].ID=0;
		for(j=0;j<ws_lnk;j++)
			ws[i].linkerID[j]=0;
	}

	for(i=0;i<srv_id;i++){
		srv[i].workFine=1;
		srv[i].visited=0;
		srv[i].ID=0;
		for(j=0;j<srv_lnk;j++)
			srv[i].linkerID[j]=0;
	}

	for(i=0;i<hb_id;i++){
		hb[i].workFine=1;
		hb[i].visited=0;
		hb[i].ID=0;
		for(j=0;j<hb_lnk;j++)
			hb[i].linkerID[j]=0;
	}
}


void insertFault(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[], int select_op,vector<int>&selF){

	int i=0;
	ofstream myfile;
	
	if(select_op==0){	
		fault_node = (rand()%total);
	
		while(selF[fault_node]==0)
			fault_node = (rand()%total);
		//fault_node++;
		int cum=0;
		int x = (rand()%100)+1;
		int j=0;
		for(j=0; j< total; j++){
			cum+=(PriorProb[j]*100);
			if(x<cum)
				break;
		}
		fault_node = j+1;
	}
	else{
		cout<<"\nselect fault node: ";
		scanf("%d",&fault_node);
	}

	if(is_ws(fault_node,ws,sw,srv,hb)){////workstations

		for(i=0;i<ws_id;i++){
			if(ws[i].ID==fault_node)
				ws[i].workFine=0;
		}
	}

	else if(is_sw(fault_node,ws,sw,srv,hb)){////switches

		for(i=0;i<sw_id;i++){
			if(sw[i].ID==fault_node)
				sw[i].workFine=0;
		}
	}

	else if(is_srv(fault_node,ws,sw,srv,hb)){////servers

		for(i=0;i<srv_id;i++){
			if(srv[i].ID==fault_node)
				srv[i].workFine=0;
		}
	}

	else{////hubs

		for(i=0;i<hb_id;i++){
			if(hb[i].ID==fault_node)
				hb[i].workFine=0;
		}
	}
}

void putInArray(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[],struct array ar[]){

	int i,j;

	for(i=0;i<total;i++)
		ar[i].ID=0;

	for(i=0;i<total;i++){
		for(j=0;j<arr_lnk;j++)
			ar[i].link[j]=0;
	}
	
	for(i=0;i<total;i++){
		if(is_ws(i+1,ws,sw,srv,hb)){
			ar[i].ID = i+1;
			ar[i].work = ws[pos_ws(i+1,ws,sw,srv,hb)].workFine;
			for(j=0;j<ws_lnk;j++)
				ar[i].link[j] = ws[pos_ws(i+1,ws,sw,srv,hb)].linkerID[j];
		}
		else if(is_sw(i+1,ws,sw,srv,hb)){
			ar[i].ID = i+1;
			ar[i].work = sw[pos_sw(i+1,ws,sw,srv,hb)].workFine;
			for(j=0;j<sw_lnk;j++)
				ar[i].link[j] = sw[pos_sw(i+1,ws,sw,srv,hb)].linkerID[j];
		}
		else if(is_srv(i+1,ws,sw,srv,hb)){
			ar[i].ID = i+1;
			ar[i].work = srv[pos_srv(i+1,ws,sw,srv,hb)].workFine;
			for(j=0;j<srv_lnk;j++)
				ar[i].link[j] = srv[pos_srv(i+1,ws,sw,srv,hb)].linkerID[j];
		}
		else{
			ar[i].ID = i+1;
			ar[i].work = hb[pos_hb(i+1,ws,sw,srv,hb)].workFine;
			for(j=0;j<hb_lnk;j++)
				ar[i].link[j] = hb[pos_hb(i+1,ws,sw,srv,hb)].linkerID[j];
		}
	}
}

int searchSpec(int c1,int c2,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){

	ofstream myfile;
	ifstream fileArray;
	struct array ar[total];
	list<int> slist;
	
	putInArray(ws,sw,srv,hb,ar); //nodes ar with their links

	returnArray(ar,c1,0);		//all nodes with their values, as appeared in BFS search, start is c1

	printPath(ar,c1,c2, &slist, 0);
	
	if(slist.empty() || ar[c2-1].work==0)
		return 0;
	else
		return 1;
}

void printPath(struct array *nd, int x1, int x2, list<int> *pathlist, int print_operation){
	if(x1==x2){
		(*pathlist).push_back(x1);
		if(print_operation==1)
			cout<<""<<x1;
	}
	else if(nd[x2-1].p==0){
		if(print_operation==1)
			cout<<"No path exists!";
	}
	else{
		printPath(nd,x1,nd[x2-1].p, pathlist, print_operation);
		(*pathlist).push_back(x2);
		if(print_operation==1)
			cout<<"-->"<<x2;
	}
}

int search(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){

	int c1=0,c2=0;
	ofstream myfile;
	ifstream fileArray;
	//struct array ar[total];
	list<int> slist;
	

	for(c1=0;c1<ws_id+srv_id;c1++){

		for(c2=c1+1;c2<ws_id+srv_id;c2++){

			if(searchSpec(c1+1,c2+1,ws,sw,srv,hb)==0)
				return 0;
		}
	}

	return 1;
}

void returnArray(struct array ar[],int node,int count){

	/*
	 * 	 BFS Algorithm
	 * 
         for each u in V − {s}             			,for each vertex u in V[G] except s.
			 color[u] ← WHITE
             d[u] ← infinity
             π[u] ← NIL
         color[s] ← GRAY                 			, Source vertex discovered
         d[s] ← 0                           		, initialize
         π[s] ← NIL                         		, initialize
         Q ← {}                             		, Clear queue Q
         ENQUEUE(Q, s)
		 while Q is non-empty
				 u ← DEQUEUE(Q)              		, That is, u = head[Q]
                 for each v adjacent to u   		, for loop for every node along with edge.
						if color[v] == WHITE		, if color is white you've never seen it before
								color[v]← GRAY
                                d[v] ← d[u] + 1
                                π[v] ← u
                                ENQUEUE(Q, v)
                 DEQUEUE(Q)
         	 	 color[u] ← BLACK
     *
     *  
	 */

	ofstream myfile;
	struct array u;
	int v=0;
	 
	for(int i=0; i<total; i++){
		if(ar[i].ID != node){
			ar[i].color = "WHITE";
			ar[i].d = INT_MAX;
			ar[i].p = 0;
		}
	}
	/*
	 * node explore the whole network if a node is fault
	 */
	ar[node-1].color = "GRAY";
	ar[node-1].d = 0;
	ar[node-1].p = 0;
	
	std::list <array>Q;
	if(ar[node-1].work==1)
		Q.push_back(ar[node-1]);
	
	while(!Q.empty()){
		u = Q.front();
		for(int k=0; k<arr_lnk; k++){
			if(u.link[k]!=0){
				for(int h=0; h<total; h++){
					if(ar[h].ID == u.link[k]){
						v = h;
						break;
					}
				}
				if( ar[v].color.compare("WHITE")==0 ){
					ar[v].color = "GRAY";
					ar[v].d = u.d + 1;
					ar[v].p = u.ID;//parent of ID v is ID u
					if(ar[v].work==1)
						Q.push_back(ar[v]);
				}
			}
		}
		Q.pop_front();
	}
	u.color = "BLACK";
}

int findDepth(struct array ar[],int num,int level){
	for(int i=0; i<total; i++){
		if( (ar[i].ID) == num ){
			return ar[i].d;
		}
	}
	return 0;
}

void insertCustomizeFault(int fault_node,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){
	if(is_ws(fault_node,ws,sw,srv,hb))///workstations
		ws[pos_ws(fault_node,ws,sw,srv,hb)].workFine=0;
	else if(is_sw(fault_node,ws,sw,srv,hb))///switches
		sw[pos_sw(fault_node,ws,sw,srv,hb)].workFine=0;
	else if(is_srv(fault_node,ws,sw,srv,hb))///servers
		srv[pos_srv(fault_node,ws,sw,srv,hb)].workFine=0;
	else///hubs
		hb[pos_hb(fault_node,ws,sw,srv,hb)].workFine=0;
}

void uninsertCustomizeFault(int fault_node,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){
	if(is_ws(fault_node,ws,sw,srv,hb))///workstations
		ws[pos_ws(fault_node,ws,sw,srv,hb)].workFine=1;
	else if(is_sw(fault_node,ws,sw,srv,hb))///switches
		sw[pos_sw(fault_node,ws,sw,srv,hb)].workFine=1;
	else if(is_srv(fault_node,ws,sw,srv,hb))///servers
		srv[pos_srv(fault_node,ws,sw,srv,hb)].workFine=1;
	else///hubs
		hb[pos_hb(fault_node,ws,sw,srv,hb)].workFine=1;
}

int checkFine(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){
	int networkOK=0;

	for(int i=0; i<total; i++){
		if(is_hb(i+1,ws,sw,srv,hb) && hb[pos_hb(i+1,ws,sw,srv,hb)].workFine==0){
			networkOK = 0;
			break;
		}
		else if(is_sw(i+1,ws,sw,srv,hb) && sw[pos_sw(i+1,ws,sw,srv,hb)].workFine==0){
			networkOK = 0;
			break;
		}
		else if(is_ws(i+1,ws,sw,srv,hb) && ws[pos_ws(i+1,ws,sw,srv,hb)].workFine==0){
			networkOK = 0;
			break;
		}
		else if(is_srv(i+1,ws,sw,srv,hb) && srv[pos_srv(i+1,ws,sw,srv,hb)].workFine==0){
			networkOK = 0;
			break;
		}
		else{
			networkOK = 1;
		}
	}
	return networkOK;
}

void connectionSim(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[],int depth,int *sel_str){

	int networkOK=0,counter=0,r=1,sel_op=0,sel_mng=0,temp;
	string filename,pref,pref_sel_op,manager;
	ofstream myfile;
	vector<int>selF;
	vector<double>counter_episodes;
	double statistics_rl[2][TSIM/fault_time];
	double statistics_pl[2][TSIM/fault_time];
	int experimental_mode=0,count_col=0;
	
	cout<<"Select experimental mode by pressing (0) to turn off or (1) to turn on: ";
	cin>>experimental_mode;
	while( experimental_mode!=0 && experimental_mode!=1 ){
		cout<<"\npress (0) to turn off or (1) to turn on experimental mode: ";
		cin>>experimental_mode;
	}
	
	if(experimental_mode==0){
		/*
		 * select manager to run
		 */ 
		cout<<endl;
		cout<<"Select (p) to use Planning manager or (r) to user RL manager: ";
		cin>>manager;

		if(manager.compare("r")==0 || manager.compare("R")==0)
			sel_mng=0;
		else if(manager.compare("p")==0 || manager.compare("P")==0)
			sel_mng=1;
		else{
			while( manager.compare("r")!=0 && manager.compare("R")!=0 && manager.compare("p")!=0 && manager.compare("P")!=0 ){
				cout<<"\npress 'r' for RL or 'p' for Planning: ";
				cin>>manager;
			}
			if(manager.compare("r")==0 || manager.compare("R")==0)
				sel_mng=0;
			else if(manager.compare("p")==0 || manager.compare("P")==0)
				sel_mng=1;
		}
		
		
		/*
		 * select type of faults
		 */
		cout<<"\ndo you want random or manual faults (r/m)?: ";
		cin>>pref_sel_op;

		if(pref_sel_op.compare("r")==0 || pref_sel_op.compare("R")==0)
			sel_op=0;
		else if(pref_sel_op.compare("m")==0 || pref_sel_op.compare("M")==0)
			sel_op=1;
		else{
			while( pref_sel_op.compare("r")!=0 && pref_sel_op.compare("R")!=0 && pref_sel_op.compare("m")!=0 && pref_sel_op.compare("M")!=0 ){
				cout<<"\npress 'r' for random or 'm' for manual faults: ";
				cin>>pref_sel_op;
			}
			if(pref_sel_op.compare("r")==0 || pref_sel_op.compare("R")==0)
				sel_op=0;
			else if(pref_sel_op.compare("m")==0 || pref_sel_op.compare("M")==0)
				sel_op=1;
		}
		/*
		 * select strategy
		 */
		cout<<"\nselect strategy, press: \n(1) for repair all\n(2) for cheapest repair\n(3) for most probable repair";
		cout<<"\nchosen strategy: ";
		cin>>(*sel_str);
		while((*sel_str)!=1 && (*sel_str)!=2 && (*sel_str)!=3){
			cout<<"\npress '1' for repair all or '2' for cheapest repair or '3' for most probable repair: ";
			cin>>(*sel_str);
		}
		
		/*
		 * select among specific faults for random fault-type-system
		 */
		selF.reserve(total);
		for(int i=0; i<total; i++)
			selF[i]=0;
		
		if(sel_op==0){
			cout<<"\nselect node from 1-"<<total<<" to break down, else press '0' to exit: ";
			cin>>temp;
			while(temp!=0){
				
				while(!cin.good() || temp<0 || temp>total){
					cin.clear();
					cin.ignore();
					cout<<"\nchoose numbers between 1-"<<total<<" or '0' to exit: ";
					cin>>temp;
				}
				if(temp==0)
					break;
				//cn++;			
				selF[temp-1]=1;
				cout<<"\nselect node from 1-"<<total<<" to break down, else press '0' to exit: ";
				cin>>temp;
			}
		}
		
		if(sel_mng==0){
			for (int i = 0; i < total_actions; i++) {
				Observ_rl.push_back(vector<int>()); // Add an empty row
				Cost_rl.push_back(vector<int>()); // Add an empty row
			}
			
			cout<<"\ngive rl manager the number of episodes you want to run: ";
			cin>>total_episodes;
			while(!cin.good()){
				cout<<"\nonly integer number is valid, number of episodes? : ";
				cin.clear();
				cin.ignore();
				cin>>total_episodes;
			}
			cout<<"\ntotal episodes: "<<total_episodes;
			/*
			 * reserve space for PriorProb_rl array
			 */ 
			PriorProb_rl.reserve(total_episodes);
			
			if(total_episodes!=0){
				for(int i=0;i<total_episodes;i++)
					PriorProb_rl[i] = 1.0 / total_episodes;
			}

			
			/*
			 * training rl manager
			 */
			for(int t=0; t<total_episodes; t++){
				insertFault(ws,sw,srv,hb,sel_op,selF);
				/*
				 * building proper vector each time
				 */
				
				for (std::vector<int>::size_type i = 0; i < Observ_rl.size(); i++)
					Observ_rl[i].push_back(i * t); // Add column to all rows
				for (std::vector<int>::size_type i = 0; i < Cost_rl.size(); i++)
					Cost_rl[i].push_back(i * t); // Add column to all rows

				/*
				 * training of each episode begins here
				 */
				training_rl_manager(t,ws,sw,srv,hb);
				cout<<"\nEnd of episode : "<<t+1;
				cout<<endl;
			}
			cout<<"\nEnd of training, let see what RLmanager learned...";

			/*
			 * simulating rl manager
			 */
			 
			counter_episodes.reserve(total);
			for(int i=0;i<total;i++)
				counter_episodes[i]=0;
			 
			for(r = 1; r <= TSIM; r++){
				counter++;
				if(counter==fault_time){
					counter=0;
					myfile.close();
					insertFault(ws,sw,srv,hb,sel_op,selF);
				}
				
				networkOK = checkFine(ws,sw,srv,hb);

				if(networkOK == 0)
					rl_manager(ws,sw,srv,hb,(*sel_str),counter_episodes);
			}
		}
		else{
			/*
			 * simulating planning
			 */
			for(r = 1; r <= TSIM; r++){
				counter++;
				if(counter==fault_time){
					counter=0;
					myfile.close();
					insertFault(ws,sw,srv,hb,sel_op,selF);
				}
				
				networkOK = checkFine(ws,sw,srv,hb);

				if(networkOK == 0)
					planning_manager(ws,sw,srv,hb,(*sel_str));
			}
		}
	}
	else{
		total_episodes=0;
		/*
		 * select strategy
		 */
		cout<<"\nselect strategy, press: \n(1) for repair all\n(2) for cheapest repair";
		cout<<"\nchosen strategy: ";
		cin>>(*sel_str);
		while((*sel_str)!=1 && (*sel_str)!=2){
			cout<<"\npress '1' for repair all or '2' for cheapest repair: ";
			cin>>(*sel_str);
		}
		
		cout<<"\nselect node from 1-"<<total<<" to break down, else press '0' to exit: ";
		cin>>temp;
		
		selF.reserve(total);
		for(int i=0; i<total; i++)
			selF[i]=0;
			
		counter_episodes.reserve(total);
		for(int i=0;i<total;i++)
			counter_episodes[i]=0;

		while(temp!=0){
			
			while(!cin.good() || temp<0 || temp>total){
				cin.clear();
				cin.ignore();
				cout<<"\nchoose numbers between 1-"<<total<<" or '0' to exit: ";
				cin>>temp;
			}
			if(temp==0)
				break;
			//cn++;			
			selF[temp-1]=1;
			cout<<"\nselect node from 1-"<<total<<" to break down, else press '0' to exit: ";
			cin>>temp;
		}
		
		/*
		 **********************   M 	A	 N	 A	 G	 E	 R	 S    **********************
		 */ 
		
		for (int i = 0; i < total_actions; i++) {
			Observ_rl.push_back(vector<int>()); // Add an empty row
			Cost_rl.push_back(vector<int>()); // Add an empty row
		}
		
		
		/*
		 * reserve space for PriorProb_rl array
		 */ 
		PriorProb_rl.reserve(0);

		counter=0;
		int iteration = 0;
		for(r = 1; r <= TSIM; r++){
			
			counter++;
			if(counter==fault_time){
				counter=0;
				myfile.close();
				insertFault(ws,sw,srv,hb,0,selF);
			}
			
			networkOK = checkFine(ws,sw,srv,hb);
						
			if(networkOK == 0){
				iteration++;
				cout<<"\nITERATION : "<<iteration;
				/*
				* RL MANGER
				*/
				
				struct timespec start1, stop1, start2, stop2;
				double accum1, accum2;
				
				clock_gettime( CLOCK_REALTIME, &start1);
				rl_manager(ws,sw,srv,hb,(*sel_str),counter_episodes);
				clock_gettime( CLOCK_REALTIME, &stop1);
				
				accum1 = (double)((stop1.tv_sec)*NANOS + stop1.tv_nsec) - (double)((start1.tv_sec)*NANOS + start1.tv_nsec);
								
				statistics_rl[0][count_col] = accum1;
				
				statistics_rl[1][count_col] = (float)total_costM;
				
				cout<<"\n\n\nEND OF RL MANAGER";
			
				cout<<"\n\n***************[STATICTICS]***************";
				cout<<"\nRunning time is : "<<accum1<<" nsec";
				cout<<"\nTotal cost is : "<<total_costM<<" sec";
				cout<<"\n******************************************";
				cout<<endl;
								
				insertCustomizeFault(fault_node,ws,sw,srv,hb);
			
				/*
				 * PLANNING MANGER
				 */ 
			
				clock_gettime( CLOCK_REALTIME, &start2);
				planning_manager(ws,sw,srv,hb,(*sel_str));
				clock_gettime( CLOCK_REALTIME, &stop2);
				
				accum2 = (double)((stop2.tv_sec)*NANOS + stop2.tv_nsec) - (double)((start2.tv_sec)*NANOS + start2.tv_nsec);
				
				statistics_pl[0][count_col] = accum2;
				
				statistics_pl[1][count_col] = (float)total_costM;
				
				cout<<"\n\n\nEND OF PLANNING MANAGER";
			
				cout<<"\n\n***************[STATICTICS]***************";
				cout<<"\nRunning time is : "<<accum2<<" nsec";
				cout<<"\nTotal cost is : "<<total_costM<<" sec";
				cout<<"\n******************************************";
				cout<<endl;
				
				count_col++;
			}
		}
		float mean_tot_cost_rl = 0.0;
		float mean_tot_time_rl = 0.0;
		float mean_tot_cost_pl = 0.0;
		float mean_tot_time_pl = 0.0;
		
		for(int i=0; i<(TSIM/fault_time); i++){
			mean_tot_cost_rl += statistics_rl[1][i];
			mean_tot_time_rl += statistics_rl[0][i];
			mean_tot_cost_pl += statistics_pl[1][i];
			mean_tot_time_pl += statistics_pl[0][i];
		}
		mean_tot_cost_rl = mean_tot_cost_rl/(TSIM/fault_time);
		mean_tot_time_rl = mean_tot_time_rl/(TSIM/fault_time);
		mean_tot_cost_pl = mean_tot_cost_pl/(TSIM/fault_time);
		mean_tot_time_pl = mean_tot_time_pl/(TSIM/fault_time);
		cout<<"\n\n***************[TOTAL - STATICTICS]***************";
		for(int i=0; i<(TSIM/fault_time);i++){
			cout<<"\nrl time : "<<statistics_rl[0][i]<<" nsec"<<"  planning time : "<<statistics_pl[0][i]<<" nsec";
			cout<<"\nrl cost : "<<statistics_rl[1][i]<<" sec"<<"  planning cost : "<<statistics_pl[1][i]<<" sec";
			cout<<endl;
		}
		cout<<"\n**************************************************";
		cout<<"\n--------------------------------------------------";
		cout<<"\nmean of total time rl : "<<mean_tot_time_rl<<" nsec"<<"  mean of total time planning : "<<mean_tot_time_pl<<" nsec";
		cout<<"\nmean of total cost rl : "<<mean_tot_cost_rl<<" sec"<<"  mean of total cost planning : "<<mean_tot_cost_pl<<" sec";
	}
}
