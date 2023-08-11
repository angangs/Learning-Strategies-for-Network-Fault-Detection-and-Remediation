#include "createSizes.h"
#include "simulator.h"
#include "csfr.h"
#include "precsfr.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <list>
#include <limits.h>
#include <istream>
#include <iterator>

using namespace std;

int main(){	
	findSize();

	int dep=0,sel_str=0;
	struct workstation ws[ws_id];
	struct server srv[srv_id];
	struct hub hb[hb_id];
	struct switches sw[sw_id];

	myInitialize(ws,sw,srv,hb);
	
	readMyTopology(ws,sw,srv,hb);
	
	myPrint(ws,sw,srv,hb);
	
	setArrays(ws,sw,srv,hb);
	cout<<endl;
	cout<<"\nStarting Simulation in a few milli-seconds. . ."<< endl;

	/**
	 * Connection Simulator starts here!
	 */
	srand((unsigned)time(NULL));
	connectionSim(ws,sw,srv,hb,dep,&sel_str);

	cout<<"\nEnd of Simulation. . ."<< endl;
	return 0;
}
