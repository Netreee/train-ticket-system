#include <vector>
#include <queue>
#include <algorithm>
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
        // 添加一条新的线路（双向）
        RouteSectionInfo *info1 = new RouteSectionInfo(trainID, arrivalStationID, price, duration);
        routeGraph.insert(departureStationID, arrivalStationID, info1);
        routeSectionPool.pushBack(info1);

        // 由于铁路线通常是双向的，这里同时插入反向路段
        RouteSectionInfo *info2 = new RouteSectionInfo(trainID, departureStationID, price, duration);
        routeGraph.insert(arrivalStationID, departureStationID, info2);
        routeSectionPool.pushBack(info2);

        // 使用并查集维护连通性
        int root1 = stationSet.find(departureStationID);
        int root2 = stationSet.find(arrivalStationID);
        stationSet.join(root1, root2);
    }

    bool RailwayGraph::checkStationAccessibility(StationID departureStationID, StationID arrivalStationID) {
        return stationSet.find(departureStationID) == stationSet.find(arrivalStationID);
    }

    void RailwayGraph::routeDfs(int x, int t, seqList<StationID> &prev, bool *visited) {
        visited[x] = true;
        prev.pushBack(x);
        if (x == t) {
            // 打印找到的路径
            for (int i = 0; i < prev.length(); ++i) {
                std::cout << prev.visit(i);
                if (i + 1 < prev.length()) std::cout << " -> ";
            }
            std::cout << std::endl;
        } else {
            // 深度优先遍历相邻站点
            for (auto p = routeGraph.verList[x]; p != nullptr; p = p->next) {
                int y = p->end;
                if (!visited[y]) {
                    routeDfs(y, t, prev, visited);
                }
            }
        }
        // 回溯
        prev.popBack();
        visited[x] = false;
    }

    void RailwayGraph::displayRoute(StationID departureStationID, StationID arrivalStationID) {
        if (!checkStationAccessibility(departureStationID, arrivalStationID)) {
            std::cout << "两个车站之间没有可达路径。" << std::endl;
            return;
        }
        bool visited[MAX_STATIONID] = {false};
        seqList<StationID> path; // 存储当前路径
        std::cout << "列出所有可达路径: " << std::endl;
        routeDfs(departureStationID, arrivalStationID, path, visited);
    }

    void RailwayGraph::shortestPath(StationID departureStationID, StationID arrivalStationID, int type) {
        if (!checkStationAccessibility(departureStationID, arrivalStationID)) {
            std::cout << "两个车站之间没有可达路径。" << std::endl;
            return;
        }
        const int INF = 0x3f3f3f3f;
        std::vector<int> dist(MAX_STATIONID, INF);
        std::vector<int> prevStation(MAX_STATIONID, -1);
        struct Node {
            int station;
            int dist;
            bool operator<(const Node &rhs) const { return dist > rhs.dist; } // 反转以便priority_queue取最小值
        };
        std::priority_queue<Node> pq;
        dist[departureStationID] = 0;
        pq.push({departureStationID, 0});
        while (!pq.empty()) {
            Node cur = pq.top(); pq.pop();
            if (cur.dist != dist[cur.station]) continue; // 过期
            if (cur.station == arrivalStationID) break;
            for (auto p = routeGraph.verList[cur.station]; p != nullptr; p = p->next) {
                int v = p->end;
                RouteSectionInfo *info = p->weight;
                int w = (type == 0 ? info->duration : info->price);
                if (dist[v] > dist[cur.station] + w) {
                    dist[v] = dist[cur.station] + w;
                    prevStation[v] = cur.station;
                    pq.push({v, dist[v]});
                }
            }
        }
        if (dist[arrivalStationID] == INF) {
            std::cout << "未找到路径。" << std::endl;
            return;
        }
        // 还原路径
        std::vector<StationID> route;
        for (int v = arrivalStationID; v != -1; v = prevStation[v]) {
            route.push_back(v);
        }
        std::reverse(route.begin(), route.end());
        std::cout << "最短路径(" << (type == 0 ? "按时间" : "按价格") << ") 总" << (type == 0 ? "时间" : "花费") << ": " << dist[arrivalStationID] << std::endl;
        for (size_t i = 0; i < route.size(); ++i) {
            std::cout << route[i];
            if (i + 1 < route.size()) std::cout << " -> ";
        }
        std::cout << std::endl;
    }
}
