#include "simulator.h"
#include "csfr.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <list>
#include <limits.h>
#include <istream>
#include <iterator>
#include <vector>
#include <string.h>
#include <ctime>

using namespace std;

int total_costM = 0;
list <int>Act;
std::vector< std::vector<int> > Actions;
std::vector<int> FaultState;
std::vector<double> PriorProb;
std::vector<double> PriorProb_rl;
std::vector< std::vector<int> > Observ;
std::vector< std::vector<int> > Cost;
std::vector< std::vector<int> > Observ_rl;
std::vector< std::vector<int> > Cost_rl;

using namespace std;

int cost_r_ws = 1800;
int cost_r_sw = 1400;
int cost_r_hb = 1000;
int cost_r_srv = 2200;
int cost_t_ping = 100;

double create_cost_rl(int action,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){
	struct array ar[total];
	list<int> slist;
	list<int> flist;
	int tmp_cost=0;
	if(action>total_actions-total){
		if(is_ws(Actions[action-1][0],ws,sw,srv,hb))
			tmp_cost = cost_r_ws;
		else if(is_sw(Actions[action-1][0],ws,sw,srv,hb))
			tmp_cost = cost_r_sw;
		else if(is_srv(Actions[action-1][0],ws,sw,srv,hb))
			tmp_cost = cost_r_srv;
		else if(is_hb(Actions[action-1][0],ws,sw,srv,hb))
			tmp_cost = cost_r_hb;
	}
	else{
		putInArray(ws,sw,srv,hb,ar); //nodes ar with their links

		returnArray(ar,Actions[action-1][0],0);		//all nodes with their values, as appeared in BFS search, start is c1

		slist.clear();

		printPath(ar,Actions[action-1][0],Actions[action-1][1], &slist, 0);

		if(!slist.empty()){
			tmp_cost = (slist.size()-1) * cost_t_ping;
			if(ar[Actions[action-1][1]-1].work==0){
				if(is_ws(fault_node,ws,sw,srv,hb))
					tmp_cost+=cost_r_ws;//1800
				else if(is_sw(fault_node,ws,sw,srv,hb))
					tmp_cost+=cost_r_sw;//1400
				else if(is_srv(fault_node,ws,sw,srv,hb))
					tmp_cost+=cost_r_srv;//2200
				else
					tmp_cost+=cost_r_hb;//1000
			}
		}
		else{//ping failed
			flist.clear();
			printPath(ar,Actions[action-1][0],fault_node, &flist, 0);
			tmp_cost = (flist.size()-1) * cost_t_ping;
			if(is_ws(fault_node,ws,sw,srv,hb))
				tmp_cost+=cost_r_ws;//1800
			else if(is_sw(fault_node,ws,sw,srv,hb))
				tmp_cost+=cost_r_sw;//1400
			else if(is_srv(fault_node,ws,sw,srv,hb))
				tmp_cost+= cost_r_srv;//2200
			else
				tmp_cost+= cost_r_hb;//1000
		}
	}
	return tmp_cost;
}

void computeObservAndCost(vector< vector<int> > &Observ, vector< vector<int> > &Cost, struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){
	struct array ar[total];
	list<int> slist;
	list<int> flist;

	for(int i=0;i<(total_actions)-total;i++){
		for(int j=0;j<total;j++){
			insertCustomizeFault(FaultState[j],ws,sw,srv,hb);

			putInArray(ws,sw,srv,hb,ar); //nodes ar with their links

			returnArray(ar,Actions[i][0],0);		//all nodes with their values, as appeared in BFS search, start is c1

			slist.clear();

			printPath(ar,Actions[i][0],Actions[i][1], &slist, 0);

			if(!slist.empty()){
				Cost[i][j] = (slist.size()-1) * cost_t_ping;
				if(ar[Actions[i][1]-1].work==0){
					if(is_ws(FaultState[j],ws,sw,srv,hb))
						Cost[i][j]+=cost_r_ws;//1800
					else if(is_sw(FaultState[j],ws,sw,srv,hb))
						Cost[i][j]+=cost_r_sw;//1400
					else if(is_srv(FaultState[j],ws,sw,srv,hb))
						Cost[i][j]+=cost_r_srv;//2200
					else
						Cost[i][j]+=cost_r_hb;//1000
					Observ[i][j] = 0;
				}
				else
					Observ[i][j] = 1;
			}
			else{//ping failed
				flist.clear();
				printPath(ar,Actions[i][0],FaultState[j], &flist, 0);
				Observ[i][j] = 0;
				Cost[i][j] = (flist.size()-1) * cost_t_ping;
				if(is_ws(FaultState[j],ws,sw,srv,hb))
					Cost[i][j]+=cost_r_ws;//1800
				else if(is_sw(FaultState[j],ws,sw,srv,hb))
					Cost[i][j]+=cost_r_sw;//1400
				else if(is_srv(FaultState[j],ws,sw,srv,hb))
					Cost[i][j] += cost_r_srv;//2200
				else
					Cost[i][j] += cost_r_hb;//1000
			}
			uninsertCustomizeFault(FaultState[j],ws,sw,srv,hb);
		}
	}

}

void planning_manager(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[],int strategy){
	ofstream myfile;
	int beliefstate[total],minAction,result=0, cost=0, total_cost=0,last_id=0;
	double V=0;
	list<int> s_list,slist;
	struct array ar[total];

	for(int i=0;i<total;i++){
		if (is_sw(i+1,ws,sw,srv,hb)||is_ws(i+1,ws,sw,srv,hb)||is_hb(i+1,ws,sw,srv,hb)||is_srv(i+1,ws,sw,srv,hb))
			beliefstate[i]=1;
		else
			beliefstate[i]=0;
	}
	cout<<"\n************************************************************";
	cout<<"\nPlanning manager started to repair... (fault node : "<<fault_node<<")";
	cout<<"\n************************************************************\n";
	/*
	 *
	 * planning manager started repairing
	 *  fault network
	 *
	 */
	//clock_t begin = clock();
	while(checkUpdatebelief(beliefstate,total)>=1){
		cout<<"\nSize of Belief State is : "<<checkUpdatebelief(beliefstate,total);
		cout<<"\nBelief State is : {";
		for(int i=0;i<total;i++){
			if(beliefstate[i]==1){
				cout<<" "<<i+1;
			}
		}
		cout<<" }";

		V=getVvalue(beliefstate,0,&minAction,ws,sw,srv,hb,strategy);

		if(minAction!=-10 && minAction!=-1)
			cout<<"\nmin action is : "<<minAction+1;
		else
			cout<<"\nmin action is : "<<minAction;

		if(minAction >= 0 && minAction < (total_actions-total)){
			result = Observ[minAction][fault_node-1];
			cost = Cost[minAction][fault_node-1];
			total_cost += cost;
			cout<<"\nexpected value: "<<V<<" Ping from: "<<Actions[minAction][0]<<" to: "<<Actions[minAction][1]<<" {";

			putInArray(ws,sw,srv,hb,ar); //nodes ar with their links

			returnArray(ar,Actions[minAction][0],0);		//all nodes with their values, as appeared in BFS search, start is c1

			slist.clear();

			printPath(ar,Actions[minAction][0],Actions[minAction][1], &slist, 1);

			cout<<"}";
			cout<<", result was: "<<result;
			cout<<"\ncost was : "<<cost;
			cout<<"\ntotal cost was : "<<total_cost;
			defineSubbelief(minAction,beliefstate,result,beliefstate);
			cout<<endl;
		}
		else if(minAction >= (total_actions-total) && minAction<total_actions){
			cost = Cost[minAction][0];
			total_cost += cost;
			cout<<"\nexpected value: "<<V<<", Action : 'Cheapest repair of the remaining nodes' { Repair node "<<Actions[minAction][0]<<" }";
			defineSubbelief(minAction,beliefstate,0,beliefstate);
			uninsertCustomizeFault(Actions[minAction][0],ws,sw,srv,hb);
			cout<<"\ncost was : "<<cost;
			cout<<"\ntotal cost was : "<<total_cost;
			cout<<endl;

			cout<<"\nwait while the system is trying to check if fault remains...";

			if(checkFine(ws,sw,srv,hb)==1){
				clearBelief(beliefstate,total);
				last_id = Actions[minAction][0];
				cout<<"\nfault repaired ( "<<last_id<<" ), network works fine";
				cout<<endl;
				cout<<"prior probability of fault node is: "<<PriorProb[Actions[minAction][0]-1];
			}
			else
				cout<<"\nfault remains, system goes on repairing...";

			cout<<endl;
		}
		else{
			cost=0;
			for(int i = 0; i < total; i++){
				if(beliefstate[i] == 1){
					if(is_hb(i+1,ws,sw,srv,hb)){
						cost += cost_r_hb;
						uninsertCustomizeFault(i+1,ws,sw,srv,hb);
					}
					else if(is_sw(i+1,ws,sw,srv,hb)){
						cost += cost_r_sw;
						uninsertCustomizeFault(i+1,ws,sw,srv,hb);
					}
					else if(is_ws(i+1,ws,sw,srv,hb)){
						cost += cost_r_ws;
						uninsertCustomizeFault(i+1,ws,sw,srv,hb);
					}
					else if(is_srv(i+1,ws,sw,srv,hb)){
						cost += cost_r_srv;
						uninsertCustomizeFault(i+1,ws,sw,srv,hb);
					}
					if(checkFine(ws,sw,srv,hb)==1){
						clearBelief(beliefstate,total);
						cout<<"\nfault repaired, network works fine";
						last_id = i+1;
					}
					else
						cout<<"\nfault remains, system goes on repairing...";
				}
			}

			total_cost += cost;
			cout<<"\nexpected value: "<<V<<", Action : 'Repair remaining nodes'";
			cout<<"\ncost was : "<<cost;
			cout<<"\ntotal cost was : "<<total_cost;
			cout<<endl;
			total_costM = total_cost;
			cout<<endl;
		}
	}
	//clock_t end = clock();
	
	cout<<endl;
	cout<<"\nFinally Belief State is : {";
	for(int i=0;i<total;i++){
		if(beliefstate[i]==1){
			cout<<" "<<i+1;
		}
	}
	cout<<" }";
	cout<<endl;
	//cout<<"Running time is: "<<((float)(end - begin) / CLOCKS_PER_SEC) * 1000<<"milli-sec";
}

void create_rl_observation_and_cost(int count_episodes,vector<int> actions,vector<int> observations,vector<int> costs){
	bool found = false;

	for(int i=0;i<total_actions;i++){
		for(uint k=0;k<actions.size();k++){
			if(actions[k]==i+1){
				Observ_rl[i][count_episodes]=observations[k];
				Cost_rl[i][count_episodes]=costs[k];
				found = true;
			}
		}
		if(found==false){
			Observ_rl[i][count_episodes]=-1;
			Cost_rl[i][count_episodes]=-1;
		}
		found=false;
	}
}

void create_episode(vector<int> *actions,vector<int> *costs,vector<int> *observations, struct workstation ws[], struct switches sw[],struct server srv[],struct hub hb[]){
	int count = 0;

	(*actions).resize(0);
	(*observations).resize(0);
	(*costs).resize(0);
	while(checkFine(ws, sw, srv, hb)==0){
		(*actions).resize(count + 1, (rand() % total_actions) + 1);
		(*costs).resize(count + 1, create_cost_rl((*actions)[count],ws,sw,srv,hb));

		if((*actions)[count]<=total_actions-total)
			(*observations).resize(count+1, searchSpec(Actions[(*actions)[count]-1][0],Actions[(*actions)[count]-1][1],ws,sw,srv,hb));
		else{
			if(Actions[(*actions)[count]-1][0]==fault_node)//is this a repair action for fault node?
				(*observations).resize(count+1,1);
			else
				(*observations).resize(count+1,0);
		}

		if( Actions[ (*actions)[count] - 1 ][ 1 ] == 0 )
			uninsertCustomizeFault( Actions[ (*actions)[ count ] - 1 ][ 0 ], ws, sw, srv, hb);

		count++;
	}
}

void write_to_file(int count_episodes,vector<int> actions,vector<int> observations,vector<int> costs){
	ofstream observFileRL;
	ofstream costFileRL;
	/*
	 * Write @ observation.txt file
	 */
	count_episodes++;
	observFileRL.open("observationRL.txt");

	for(int i=0;i<(total_actions);i++){
		for(int j=0;j<count_episodes;j++)
			observFileRL << "\t" << Observ_rl[i][j];
		observFileRL<<"\t"<<Actions[i][0]<<" to ";
		observFileRL<<Actions[i][1];
		observFileRL << "\n";
	}
	observFileRL.close();
	/*
	 * Write @ cost.txt file
	 */
	costFileRL.open("costRL.txt");

	for(int i=0;i<(total_actions);i++){
		for(int j=0;j<count_episodes;j++)
			costFileRL <<"\t"<< Cost_rl[i][j];
		costFileRL<<"\t"<<Actions[i][0]<<" to ";
		costFileRL<<Actions[i][1];
		costFileRL << "\n";
	}
	costFileRL.close();
}

void training_rl_manager(int count_episodes,struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[]){
	vector<int> actions;
	vector<int> costs;
	vector<int> observations;

	create_episode(&actions, &costs, &observations, ws, sw, srv, hb);
	create_rl_observation_and_cost(count_episodes,actions,observations,costs);
	write_to_file(count_episodes,actions,observations,costs);
}

void rl_manager(struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[],int strategy,vector<double> &counter_episodes){
	ofstream myfile;
	
	int beliefstate[total_episodes],minAction,result=0, cost=0, total_cost=0,last_id=0;
	double V=0;
	double sum=0.0;
	
	for(int i=0;i<total_episodes;i++)
		sum += counter_episodes[i];
		
	for(int i=0;i<total_episodes;i++)
		cout<<"\nprobability of id "<<i+1<<" : "<<counter_episodes[i]/sum;
	
	for(int i=0;i<total_episodes;i++)
		beliefstate[i]=1;

	cout<<"\n************************************************************";
	cout<<"\nRL manager started to repair... (fault node : "<<fault_node<<")";
	cout<<"\n************************************************************\n";
	/*
	 *
	 * planning manager started repairing
	 *  fault network
	 *
	 */
	//clock_t begin = clock();
	cout<<"total episodes are: "<<total_episodes;
	cout<<"\ncheck Belief: "<<checkUpdatebelief(beliefstate,total_episodes);
	if(checkUpdatebelief(beliefstate,total_episodes)==0){
		/*
		 * if an episode does not exit yet => add the episode
		 */
		total_episodes++;

		for (std::vector<int>::size_type i = 0; i < Observ_rl.size(); i++){
			Observ_rl[i].push_back(i); // Add column to all rows
			Cost_rl[i].push_back(i); // Add column to all rows
		}
			
		training_rl_manager(0,ws,sw,srv,hb);
		cout<<"\nEnd of episode : "<<total_episodes;
		cout<<endl;
		
		PriorProb_rl.reserve(total_episodes);
		//counter_episodes[0]=1;
		PriorProb_rl[0] = 1.0;
		beliefstate[0]=1;
	}
	while(checkUpdatebelief(beliefstate,total_episodes)>=1){

		cout<<"\nSize of Belief State is : "<<checkUpdatebelief(beliefstate,total_episodes);
		cout<<"\nBelief State is : {";
		
		for(int i=0;i<total_episodes;i++){
			if(beliefstate[i]==1){
				cout<<" "<<i+1;
			}
		}
		cout<<" }";
		
		V=getVvalue_rl(beliefstate,0,&minAction,ws,sw,srv,hb,strategy);
		
		if(minAction!=-10 && minAction!=-1)
			cout<<"\nmin action is : "<<minAction+1;
		else
			cout<<"\nmin action is : "<<minAction;
		
		if(minAction >= 0 && minAction < (total_actions-total)){			
			result = Observ[minAction][fault_node-1];
			cost = Cost[minAction][fault_node-1];
			total_cost += cost;
			cout<<"\nexpected value: "<<V<<" Ping from: "<<Actions[minAction][0]<<" to: "<<Actions[minAction][1];
			cout<<", result was: "<<result;
			cout<<"\ncost was : "<<cost;
			cout<<"\ntotal cost was : "<<total_cost;
			defineSubbelief_rlq(minAction,beliefstate,result,beliefstate);
			cout<<endl;
		}
		else if(minAction >= (total_actions-total) && minAction<total_actions){
			cost = Cost[minAction][0];
			total_cost += cost;
			cout<<"\nexpected value: "<<V<<", Action : 'Cheapest repair of the remaining nodes' { Repair node "<<Actions[minAction][0]<<" }";
			defineSubbelief_rlq(minAction,beliefstate,0,beliefstate);
			uninsertCustomizeFault(Actions[minAction][0],ws,sw,srv,hb);\
			int ep=0;
			for(ep=0;ep<total_episodes;ep++){
				if(Observ_rl[minAction][ep]==1)
					break;
			}
			counter_episodes[ep]+=1;
			int summ=0;
			for(int i=0;i<total_episodes;i++)
				summ += counter_episodes[i];
			
			for(int i=0;i<total_episodes;i++)
				PriorProb_rl[i] = counter_episodes[i]/summ;
				
			cout<<"\ncost was : "<<cost;
			cout<<"\ntotal cost was : "<<total_cost;
			cout<<endl;
			cout<<"\nwait while the system is trying to check if fault remains...";

			if(checkFine(ws,sw,srv,hb)==1){
				clearBelief(beliefstate,total_episodes);
				last_id = Actions[minAction][0];
				cout<<"\nfault repaired ( "<<last_id<<" ), network works fine";
			}
			else
				cout<<"\nfault remains, system goes on repairing...";

			cout<<endl;
		}
		else{
			cost=0;
			for(int i = 0; i < total_episodes; i++){
				if(beliefstate[i] == 1){
					for(int h=total_actions-total; h<total_actions; h++){
						if(is_hb(Actions[h][0],ws,sw,srv,hb) && Observ_rl[h][i]==1){
							cost += cost_r_hb;
							uninsertCustomizeFault(Actions[h][0],ws,sw,srv,hb);
							beliefstate[i]=0;
							counter_episodes[i]+=1;
							int summ=0;
							for(int i=0;i<total_episodes;i++)
								summ += counter_episodes[i];
							
							for(int i=0;i<total_episodes;i++)
								PriorProb_rl[i] = counter_episodes[i]/summ;
						}
						else if(is_sw(Actions[h][0],ws,sw,srv,hb) && Observ_rl[h][i]==1){
							cost += cost_r_sw;
							uninsertCustomizeFault(Actions[h][0],ws,sw,srv,hb);
							beliefstate[i]=0;
							counter_episodes[i]+=1;
							int summ=0;
							for(int i=0;i<total_episodes;i++)
								summ += counter_episodes[i];
							
							for(int i=0;i<total_episodes;i++)
								PriorProb_rl[i] = counter_episodes[i]/summ;
						}
						else if(is_ws(Actions[h][0],ws,sw,srv,hb) && Observ_rl[h][i]==1){
							cost += cost_r_ws;
							uninsertCustomizeFault(Actions[h][0],ws,sw,srv,hb);
							beliefstate[i]=0;
							counter_episodes[i]+=1;
							int summ=0;
							for(int i=0;i<total_episodes;i++)
								summ += counter_episodes[i];
							
							for(int i=0;i<total_episodes;i++)
								PriorProb_rl[i] = counter_episodes[i]/summ;
						}
						else if(is_srv(Actions[h][0],ws,sw,srv,hb) && Observ_rl[h][i]==1){
							cost += cost_r_srv;
							uninsertCustomizeFault(Actions[h][0],ws,sw,srv,hb);
							beliefstate[i]=0;
							counter_episodes[i]+=1;
							int summ=0;
							for(int i=0;i<total_episodes;i++)
								summ += counter_episodes[i];
							
							for(int i=0;i<total_episodes;i++)
								PriorProb_rl[i] = counter_episodes[i]/summ;
						}
					}
				}
			}
			
			total_cost += cost;
			cout<<"\nexpected value: "<<V<<", Action : 'Repair remaining nodes'";
			cout<<"\ncost was : "<<cost;
			cout<<"\ntotal cost was : "<<total_cost;
			total_costM = total_cost;
			cout<<endl;
		}
		/*
		 * if an episode does not exit yet => add the episode
		 */
		if(checkUpdatebelief(beliefstate,total_episodes)==0 && checkFine(ws,sw,srv,hb)!=1){
			total_episodes++;
			
			for (std::vector<int>::size_type i = 0; i < Observ_rl.size(); i++){
				Observ_rl[i].push_back(i); // Add column to all rows
				Cost_rl[i].push_back(i); // Add column to all rows
			}
			
			training_rl_manager(total_episodes-1,ws,sw,srv,hb);
			cout<<"\nEnd of episode : "<<total_episodes;
			cout<<endl;
			
			PriorProb_rl.reserve(total_episodes);
			counter_episodes[total_episodes-1] = 1;
			int summ=0;
			for(int i=0;i<total_episodes;i++)
				summ += counter_episodes[i];
			
			for(int i=0;i<total_episodes;i++)
				PriorProb_rl[i] = counter_episodes[i]/summ;

			beliefstate[total_episodes-1]=1;
		}
	}
	//clock_t end = clock();
	
	cout<<endl;
	cout<<"\nFinally Belief State is : {";
	for(int i=0;i<total_episodes;i++){
		if(beliefstate[i]==1){
			cout<<" "<<i+1;
		}
	}
	cout<<" }";
	cout<<endl;
	//cout<<"Running time is: "<<((float)(end - begin) / CLOCKS_PER_SEC) * 1000<<"milli-sec";
	cout<<endl;
}

double getVvalue(int *beliefstate, int depth, int *action, struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[],int strategy){

	int blf0[total],blf1[total],blf[total];
	double prb=0,prb0=0,prb1=0;
	double cst0=0,cst1=0;
	int i=0;
	double Q=0.0,minQ=0.0;
	double tmpQ1=0.0,tmpQ0=0.0;
	int minAction=0,action1=0, action2=0;
	
	if(checkUpdatebelief(beliefstate,total)==1){
		for(int i=0; i<total; i++){
			if(beliefstate[i]==1){
				if(is_ws(i+1,ws,sw,srv,hb))
					minQ = cost_r_ws;
				else if(is_sw(i+1,ws,sw,srv,hb))
					minQ = cost_r_sw;
				else if(is_srv(i+1,ws,sw,srv,hb))
					minQ = cost_r_srv;
				else if(is_hb(i+1,ws,sw,srv,hb))
					minQ = cost_r_hb;
			}
		}
		minAction = -1;
	}
	else{
		minQ = INT_MAX;
		for(i=0;i<(total_actions)-total;i++){//(total_actions) - total: just test actions
			defineSubbelief(i,beliefstate,1,blf1);
			defineSubbelief(i,beliefstate,0,blf0);

			prb0=getProb(blf0);
			prb1=getProb(blf1);

			if (prb0>0.0 && prb1>0.0) {
				prb=getProb(beliefstate);
				cst0=getCost(blf0,i);
				cst1=getCost(blf1,i);
				tmpQ0=getVvalue(blf0,depth+1,&action1,ws,sw,srv,hb,strategy);
				tmpQ1=getVvalue(blf1,depth+1,&action2,ws,sw,srv,hb,strategy);
				Q = ( (prb0/prb)*(cst0+tmpQ0) )+ ( (prb1/prb)*(cst1+tmpQ1) );
			}
			else
				Q = INT_MAX;

			if( Q < minQ){
				minQ = Q;
				minAction=i;
			}
		}
		/*
		 * Repair all Strategy #1
		 */
		if(minQ == INT_MAX && strategy == 1){
			double total_cost=0.0;
			for(int i = 0; i < total; i++){
				if(beliefstate[i] == 1){
					if(is_hb(i+1,ws,sw,srv,hb))
						total_cost = total_cost + cost_r_hb;
					else if(is_ws(i+1,ws,sw,srv,hb))
						total_cost = total_cost + cost_r_ws;
					else if(is_sw(i+1,ws,sw,srv,hb))
						total_cost = total_cost + cost_r_sw;
					else if(is_srv(i+1,ws,sw,srv,hb))
						total_cost = total_cost + cost_r_srv;
				}
			}
			minAction = -10;
			minQ = total_cost;
		}
		/*
		 * Cheapest repair Strategy #2
		 */
		else if(minQ == INT_MAX && strategy == 2){
			for(int i=total_actions-total;i<(total_actions);i++){
				if(beliefstate[i-(total_actions-total)]==1){
					if(Cost[i][0]<minQ){
						minQ = Cost[i][0];
						minAction = i;
					}
				}
			}
			defineSubbelief(minAction,beliefstate,0,blf);
			minQ += getVvalue(blf,depth+1,&action1,ws,sw,srv,hb,strategy);
		}
		/*
		 * Most probable repair Strategy #3
		 */
		 
		else if(minQ == INT_MAX && strategy == 3){
			double maxP=INT_MIN;
			for(int i=total_actions-total;i<(total_actions);i++){
				if(beliefstate[i-(total_actions-total)]==1){
					if(PriorProb[i-(total_actions-total)]>maxP){
						maxP = PriorProb[i-(total_actions-total)];
						minQ = Cost[i][0];
						minAction = i;
					}
				}
			}
			defineSubbelief(minAction,beliefstate,0,blf);
			minQ += getVvalue(blf,depth+1,&action1,ws,sw,srv,hb,strategy);
		}
	}
	*action = minAction;
	return minQ;
}

double getVvalue_rl(int *beliefstate, int depth, int *action, struct workstation ws[],struct switches sw[],struct server srv[],struct hub hb[],int strategy){

	int blf0[total_episodes],blf1[total_episodes], blf0q[total_episodes],blf1q[total_episodes],blfq[total_episodes],blf[total_episodes];
	double prb=0,prb0=0,prb1=0,prbq=0;
	double cst0=0,cst1=0,cstq=0;

	double Q=0.0,minQ=0.0;
	double tmpQ1=0.0,tmpQ0=0.0;
	int minAction=0,action1=0, action0=0;

	if(checkUpdatebelief(beliefstate,total_episodes)==0){
		minAction=-2;
		return 0;
	}
	if(checkUpdatebelief(beliefstate,total_episodes)==1){
		for(int i=0; i<total_episodes; i++){
			if(beliefstate[i]==1){
				for(int k=total_actions-total;k<total;k++){
					if(is_ws(Actions[k][0],ws,sw,srv,hb))
						minQ = cost_r_ws;
					else if(is_sw(Actions[k][0],ws,sw,srv,hb))
						minQ = cost_r_sw;
					else if(is_srv(Actions[k][0],ws,sw,srv,hb))
						minQ = cost_r_srv;
					else if(is_hb(Actions[k][0],ws,sw,srv,hb))
						minQ = cost_r_hb;
				}
			}
		}
		minAction = -1;
	}
	else{
		minQ = INT_MAX;
		for(int i=0;i<(total_actions)-total;i++){//(total_actions) - total: just test actions
			defineSubbelief_rl(i,beliefstate,1,blf1);
			defineSubbelief_rl(i,beliefstate,0,blf0);
			defineSubbelief_rl(i,beliefstate,-1,blfq);

			prb0=getProb_rl(blf0);
			prb1=getProb_rl(blf1);
			prbq=getProb_rl(blfq);

			if (prb0>0.0 && prb1>0.0) {
				prb=getProb_rl(beliefstate);

				cst0=getCost_rl(blf0,i);
				cst1=getCost_rl(blf1,i);
				cstq=getCost_rl(blfq,i);

				defineSubbelief_rlq(i,beliefstate,1,blf1q);
				defineSubbelief_rlq(i,beliefstate,0,blf0q);

				tmpQ0=getVvalue_rl(blf0q,depth+1,&action0,ws,sw,srv,hb,strategy);
				tmpQ1=getVvalue_rl(blf1q,depth+1,&action1,ws,sw,srv,hb,strategy);

				Q = ( (prb0/prb)*(cst0+tmpQ0) ) + ( (prb1/prb)*(cst1+tmpQ1) ) + ( (prbq/prb)*(cstq) );
			}
			else
				Q = INT_MAX;

			if( Q < minQ){
				minQ = Q;
				minAction=i;
			}
		}
		/*
		 * Repair all Strategy #1
		 */
		if(minQ == INT_MAX && strategy == 1){
			double total_cost=0.0;
			for(int i = 0; i < total_episodes; i++){
				if(beliefstate[i] == 1){
					for(int h=total_actions-total; h<total_actions; h++){
						if(is_hb(Actions[h][0],ws,sw,srv,hb))
							total_cost = total_cost + cost_r_hb;
						else if(is_ws(Actions[h][0],ws,sw,srv,hb))
							total_cost = total_cost + cost_r_ws;
						else if(is_sw(Actions[h][0],ws,sw,srv,hb))
							total_cost = total_cost + cost_r_sw;
						else if(is_srv(Actions[h][0],ws,sw,srv,hb))
							total_cost = total_cost + cost_r_srv;
					}
				}
			}
			minAction = -10;
			minQ = total_cost;
		}
		/*
		 * Cheapest repair Strategy #2
		 */
		else if(minQ == INT_MAX && strategy == 2){
			for(int i=0; i<total_episodes;i++){
				if(beliefstate[i]==1){
					for(int h=total_actions-total; h<total_actions; h++){
						if(Observ_rl[h][i]==1 && Cost_rl[h][i]<minQ){
							minQ = Cost_rl[h][i];
							minAction = h;
						}
					}
				}
			}
			defineSubbelief_rlq(minAction,beliefstate,0,blf);
			minQ += getVvalue_rl(blf,depth+1,&action1,ws,sw,srv,hb,strategy);
		}
		/*
		 * Smart repair Strategy #3
		 */
		else if(minQ == INT_MAX && strategy == 3){
			double maxP=INT_MIN;
			for(int i=0; i<total_episodes;i++){
				if(beliefstate[i]==1){
					for(int h=total_actions-total; h<total_actions; h++){
						if(Observ_rl[h][i]==1 && maxP<PriorProb_rl[i]){
							maxP = PriorProb_rl[i];
							minQ = Cost_rl[h][i];
							minAction = h;
						}
					}
				}
			}
			defineSubbelief_rlq(minAction,beliefstate,0,blf);
			minQ += getVvalue_rl(blf,depth+1,&action1,ws,sw,srv,hb,strategy);
		}
	}
	*action = minAction;
	return minQ;
}


void defineSubbelief(int rowAction,int *beliefstate,int selection,int *b){
	int i=0;
	for(i=0;i<total;i++){
		if( (beliefstate[i]==1) && (Observ[rowAction][i]==selection) ){
			b[i]=1;
		}
		else{
			b[i]=0;
		}
	}
}

void defineSubbelief_rl(int rowAction,int *beliefstate,int selection,int *b){
	int i=0;
	for(i=0;i<total_episodes;i++){
		if( (beliefstate[i]==1) && (Observ_rl[rowAction][i]==selection) ){
			b[i]=1;
		}
		else{
			b[i]=0;
		}
	}
}

void defineSubbelief_rlq(int rowAction,int *beliefstate,int selection,int *b){// minAction = test action (-1 must be included!)
	int i=0;
	for(i=0;i<total_episodes;i++){
		if( (beliefstate[i]==1) && ((Observ_rl[rowAction][i]==selection) || (Observ_rl[rowAction][i]==-1) ) ){
			b[i]=1;
		}
		else{
			b[i]=0;
		}
	}
}

int checkUpdatebelief(int *beliefstate,int tot){
	int i=0,sum=0;

	for(i=0;i<tot;i++){
		sum=sum+beliefstate[i];
	}

	return sum;
}

void clearBelief(int *beliefstate,int tot){
	for(int i=0;i<tot;i++){
		if(beliefstate[i]==1)
			beliefstate[i]=0;
	}
}

double getCost(int *beliefstate,int rowAction){

	int i=0;
	double cst=0.0;

	for(i=0;i<total;i++){
		if(beliefstate[i]==1){
			cst=cst+(PriorProb[i]*Cost[rowAction][i]);
		}
	}
	return cst;
}

double getCost_rl(int *beliefstate,int rowAction){

	int i=0;
	double cst=0.0;

	for(i=0;i<total_episodes;i++){
		if(beliefstate[i]==1 && Cost_rl[rowAction][i]!=-1){
			cst=cst+(PriorProb_rl[i]*Cost_rl[rowAction][i]);
		}
	}
	return cst;
}

double getProb(int *beliefstate){

	int i=0;
	double prb=0.0;

	for(i=0;i<total;i++){
		if(beliefstate[i]==1){
			prb=prb+PriorProb[i];
		}
	}
	return prb;
}

double getProb_rl(int *beliefstate){

	int i=0;

	double prb=0.0;

	for(i=0;i<total_episodes;i++){
		if(beliefstate[i]==1){
			prb=prb+PriorProb_rl[i];
		}
	}

	return prb;
}
