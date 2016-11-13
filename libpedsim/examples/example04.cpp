/// pedsim - A microscopic pedestrian simulation system.
/// Copyright (c) by Christian Gloor

#include "ped_includes.h"
#include <iostream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <cmath>
#include <algorithm>

using namespace std;


int main(int argc, char *argv[]) {
    cout << "PedSim Example using libpedsim version " << Ped::LIBPEDSIM_VERSION << endl;

    Ped::OutputWriter *ow = new Ped::TCPOutputWriter();
    ow->setScenarioName("Example 04");

    // setup
    Ped::Tscene *pedscene = new Ped::Tscene();
    pedscene->setOutputWriter(ow);

    Ped::Elevation *e = new Ped::Elevation("/home/chgloor/src/pedsim-3dvis/ecosystem/3dvis/ground2.asc");
    e->SetMeta(-125, -75, 1.0);
    pedscene->SetElevation(e);
    
    // outer boudaries
    pedscene->addObstacle(new Ped::Tobstacle(-125, 70,  125, 70));
    pedscene->addObstacle(new Ped::Tobstacle(-125, -70,  125, -70));
    pedscene->addObstacle(new Ped::Tobstacle(-125, 70,  -125, -70));
    pedscene->addObstacle(new Ped::Tobstacle( 125, 70,   125, -70));

    // bottleneck
    double w = 2;
    //    pedscene->addObstacle(new Ped::Tobstacle(  30,   -w,   30,  w));

    pedscene->addObstacle(new Ped::Tobstacle( -30,   w,   30,  w));
    pedscene->addObstacle(new Ped::Tobstacle( -30,  -w,   30, -w));

    pedscene->addObstacle(new Ped::Tobstacle( -30,  -w,   -100, -70));
    pedscene->addObstacle(new Ped::Tobstacle( -30,   w,   -100,  70));
    pedscene->addObstacle(new Ped::Tobstacle(  30,  -w,    100, -70));
    pedscene->addObstacle(new Ped::Tobstacle(  30,   w,    100,  70));


    int nagents = 250;

    Ped::Twaypoint *w1 = new Ped::Twaypoint(100, 0, 24);
    pedscene->addWaypoint(w1);

    Ped::Tagent *cam_agent;
    for (int i = 0; i<nagents; i++) {
      Ped::Tagent *a = new Ped::Tagent();
        a->setPosition(0.0 + rand()/(RAND_MAX/40.0)-100.0, 0.0 + rand()/(RAND_MAX/30.0)-15.0, 0.0);
        a->addWaypoint(w1);
	a->setWaypointBehavior(Ped::Tagent::BEHAVIOR_ONCE);
	a->setfactorsocialforce(10.0);
        pedscene->addAgent(a);
	if (i == 100) cam_agent = a;
    }

    Ped::Tobstacle o1(Ped::Tvector(-20, -10, e->GetHeight(-20, -10)), Ped::Tvector(0, 0, 0));
    ow->drawPoint(o1, "tree");
    Ped::Tobstacle o2(Ped::Tvector(-10, -10, e->GetHeight(-10, -10)), Ped::Tvector(0, 0, 0));
    ow->drawPoint(o2, "tree");
    Ped::Tobstacle o3(Ped::Tvector(0, -10, e->GetHeight(0, -10)), Ped::Tvector(0, 0, 0));
    ow->drawPoint(o3, "tree");
    Ped::Tobstacle o4(Ped::Tvector(10, -10, e->GetHeight(10, -10)), Ped::Tvector(0, 0, 0));
    ow->drawPoint(o4, "tree");
    Ped::Tobstacle o5(Ped::Tvector(20, -10, e->GetHeight(20, -10)), Ped::Tvector(0, 0, 0));
    ow->drawPoint(o5, "tree");
    
    // move all agents for a few steps
    long timestep = 0;
    Ped::Tvector p = cam_agent->getPosition();
    //    Ped::Tvector v = cam_agent->getVelocity().normalized();
    Ped::Tvector v(1, 0, 0);
    for (int i=0; i<10000; ++i) {
        pedscene->moveAgents(0.4);

	p = 0.2 * cam_agent->getPosition() + 0.8 * p;
	v = 0.005 * cam_agent->getVelocity().normalized() + 0.995 * v;
	//	p.z = p.z + 2.2; // cam location 2.2m above ground = 0.2m above head.
	ow->setCamera(Ped::Tvector(p.x, p.y, p.z+2.2), v.normalized(), "Agent 1");
	ow->setCamera(Ped::Tvector(p.x-20, p.y+40, p.z+20), Ped::Tvector(1, -2, -1).normalized(), "Cam 1");

	std::this_thread::sleep_for(std::chrono::milliseconds(1000/50));
    }

    // cleanup
    for (auto a : pedscene->getAllAgents()) { delete a; };
    for (auto o : pedscene->getAllObstacles()) { delete o; };
    delete pedscene;

    return EXIT_SUCCESS;
}
