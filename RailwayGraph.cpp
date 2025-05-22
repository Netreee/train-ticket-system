#include "RailwayGraph.h"
#include "DataStructure/Queue.h"
#include "DataStructure/List.h"

#include <iostream>

namespace trainsys {
    RailwayGraph::RailwayGraph()
        : routeGraph(MAX_STATIONID), stationSet(MAX_STATIONID), routeSectionPool() {
    }

    RailwayGraph::~RailwayGraph() {
        for (int i = 0; i < routeSectionPool.length(); ++i) {
            delete routeSectionPool.visit(i);
        }
    }

    void RailwayGraph::addRoute(StationID departureStationID, StationID arrivalStationID, int duration, int price,
                                TrainID trainID) {
        /* Question */
    }

    bool RailwayGraph::checkStationAccessibility(StationID departureStationID, StationID arrivalStationID) {
        /* Question */
    }

    void RailwayGraph::routeDfs(int x, int t, seqList<StationID> &prev, bool *visited) {
        /* Question */
    }

    void RailwayGraph::displayRoute(StationID departureStationID, StationID arrivalStationID) {
        /* Question */
    }

    void RailwayGraph::shortestPath(StationID departureStationID, StationID arrivalStationID, int type) {
        /* Question */
    }
}
