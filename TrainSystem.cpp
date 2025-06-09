#include "TrainSystem.h"

#include <iostream>
#include <cstring>

namespace trainsys {
    UserInfo currentUser;
    UserManager *userManager;
    RailwayGraph *railwayGraph;
    SchedulerManager *schedulerManager;
    TicketManager *ticketManager;
    PrioritizedWaitingList *waitingList;
    TripManager *tripManager;
    StationManager *stationManager;
    RedBlackTree<String, StationID> stationNameToIDMapping;

    void addTrainScheduler(const TrainID &trainID, int seatNum, int passingStationNumber,
                           const StationID *stations, const int *duration, const int *price) {
        // 检查权限
        if (currentUser.privilege < ADMIN_PRIVILEGE) {
            std::cout << "Permission denied." << std::endl;
            return;
        }
        
        // 检查列车是否已存在
        if (schedulerManager->existScheduler(trainID)) {
            std::cout << "列车已存在" << std::endl;
            return;
        }
        
        try {
            // 添加列车调度信息
            schedulerManager->addScheduler(trainID, seatNum, passingStationNumber, stations, duration, price);
            
            // 将路线信息添加到铁路图中
            for (int i = 0; i < passingStationNumber - 1; i++) {
                railwayGraph->addRoute(stations[i], stations[i + 1], duration[i], price[i], trainID);
            }
            
            std::cout << "添加列车成功" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "添加列车失败: " << e.what() << std::endl;
        }
    }

    void queryTrainScheduler(const TrainID &trainID) {
        // 检查列车是否存在
        if (!schedulerManager->existScheduler(trainID)) {
            std::cout << "列车不存在" << std::endl;
            return;
        }
        
        // 获取并显示列车调度信息
        TrainScheduler scheduler = schedulerManager->getScheduler(trainID);
        std::cout << scheduler << std::endl;
    }

    void releaseTicket(const TrainScheduler &scheduler, const Date &date) {
        // 检查权限
        if (currentUser.privilege < ADMIN_PRIVILEGE) {
            std::cout << "Permission denied." << std::endl;
            return;
        }
        
        try {
            ticketManager->releaseTicket(scheduler, date);
            std::cout << "发布车票成功" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "发布车票失败: " << e.what() << std::endl;
        }
    }

    void expireTicket(const TrainID &trainID, const Date &date) {
        /* Question */
    }

    int queryRemainingTicket(const TrainID &trainID, const Date &date, const StationID &departureStation) {
        /* Question */
    }

    bool trySatisfyOrder() {
        if (waitingList->isEmpty()) {
            return false;
        }

        PurchaseInfo purchaseInfo = waitingList->getFrontPurchaseInfo();
        TrainID trainID = purchaseInfo.trainID;
        Date date = purchaseInfo.date;
        StationID departureStation = purchaseInfo.departureStation;

        // 获取列车调度信息
        if (!schedulerManager->existScheduler(trainID)) {
            waitingList->removeHeadFromWaitingList();
            return false;
        }
        TrainScheduler scheduler = schedulerManager->getScheduler(trainID);

        // 找到出发站在列车路线中的位置
        int departureIdx = scheduler.findStation(departureStation);
        if (departureIdx == -1) {
            waitingList->removeHeadFromWaitingList();
            return false;
        }

        // 检查从出发站到后续所有站点的余票
        int minSeats = scheduler.getSeatNum();  // 初始化为最大座位数
        for (int i = departureIdx; i < scheduler.getPassingStationNum() - 1; i++) {
            int remainingSeats = ticketManager->querySeat(trainID, date, scheduler.getStation(i));
            if (remainingSeats == -1) {  // 该站点没有票务信息
                minSeats = 0;
                break;
            }
            minSeats = std::min(minSeats, remainingSeats);
        }

        // 如果有足够的座位，满足订单
        if (minSeats > 0) {
            // 更新所有相关站点的座位数
            for (int i = departureIdx; i < scheduler.getPassingStationNum() - 1; i++) {
                ticketManager->updateSeat(trainID, date, scheduler.getStation(i), -1);
            }

            // 创建行程信息
            TripInfo tripInfo(trainID, departureStation, 
                             scheduler.getStation(scheduler.getPassingStationNum() - 1),
                             1,  // 购买一张票
                             scheduler.getDuration(departureIdx),
                             scheduler.getPrice(departureIdx),
                             date);
            
            // 添加到用户的行程中
            tripManager->addTrip(purchaseInfo.userID, tripInfo);
            
            // 从等待列表中移除
            waitingList->removeHeadFromWaitingList();
            return true;
        }

        return false;
    }

    void queryMyTicket() {
        // 检查用户是否登录
        if (currentUser.userID == -1) {
            std::cout << "请先登录" << std::endl;
            return;
        }

        // 获取用户的所有行程
        seqList<TripInfo> trips = tripManager->queryTrip(currentUser.userID);
        
        if (trips.length() == 0) {
            std::cout << "您当前没有行程" << std::endl;
            return;
        }

        // 显示所有行程信息
        std::cout << "您的行程信息：" << std::endl;
        for (int i = 0; i < trips.length(); i++) {
            TripInfo trip = trips.visit(i);
            String departureName = stationManager->getStationName(trip.departureStation);
            String arrivalName = stationManager->getStationName(trip.arrivalStation);
            
            std::cout << "行程 " << (i + 1) << ":" << std::endl;
            std::cout << "  列车: " << trip.trainID << std::endl;
            std::cout << "  出发站: " << departureName << std::endl;
            std::cout << "  到达站: " << arrivalName << std::endl;
            std::cout << "  日期: " << trip.date << std::endl;
            std::cout << "  票价: " << trip.price << " 元" << std::endl;
            std::cout << "  时长: " << trip.duration << " 分钟" << std::endl;
            std::cout << "  票数: " << trip.ticketNumber << std::endl;
            
            // 显示该行程的路线信息
            std::cout << "  路线详情:" << std::endl;
            railwayGraph->displayRoute(trip.departureStation, trip.arrivalStation);
            std::cout << std::endl;
        }
    }

    void orderTicket(const TrainID &trainID, const Date &date, const StationID &departureStation) {
        /* Question */
    }

    void refundTicket(const TrainID &trainID, const Date &date, const StationID &departureStation) {
        /* Question */
    }

    void findAllRoute(const StationID departureID, const StationID arrivalID) {
        // 检查站点是否存在
        try {
            String departureName = stationManager->getStationName(departureID);
            String arrivalName = stationManager->getStationName(arrivalID);
        } catch (const std::exception& e) {
            std::cout << "站点不存在" << std::endl;
            return;
        }
        
        // 检查站点是否可达
        if (!railwayGraph->checkStationAccessibility(departureID, arrivalID)) {
            std::cout << "站点之间不可达" << std::endl;
            return;
        }
        
        // 显示所有路线
        railwayGraph->displayRoute(departureID, arrivalID);
    }

    void findBestRoute(const StationID departureID, const StationID arrivalID, int preference) {
        // 检查站点是否存在
        try {
            String departureName = stationManager->getStationName(departureID);
            String arrivalName = stationManager->getStationName(arrivalID);
            // 如果能获取到站点名称，说明站点存在
        } catch (const std::exception& e) {
            std::cout << "站点不存在" << std::endl;
            return;
        }
        
        // 检查站点是否可达
        if (!railwayGraph->checkStationAccessibility(departureID, arrivalID)) {
            std::cout << "站点之间不可达" << std::endl;
            return;
        }
        
        // 根据偏好查找最佳路线
        // preference: 0表示按时间最短，1表示按价格最低
        if (preference != 0 && preference != 1) {
            std::cout << "无效的偏好设置" << std::endl;
            return;
        }
        
        railwayGraph->shortestPath(departureID, arrivalID, preference);
    }

    void login(const UserID userID, const char *password) {
        /* Question */
    }

    void logout() {
        /* Question */
    }

    void addUser(const UserID userID, const char *username, const char* password) {
        // 检查权限
        if (currentUser.privilege < ADMIN_PRIVILEGE) {
            std::cout << "Permission denied." << std::endl;
            return;
        }
        
        if (userManager->existUser(userID)) {
            std::cout << "User ID existed." << std::endl;
            return;
        }

        userManager->insertUser(userID, username, password, 0);
        std::cout << "User added." << std::endl;
    }

    void findUserInfoByUserID(const UserID userID) {
        if (!userManager->existUser(userID)) {
            std::cout << "User not found." << std::endl;
            return;
        }
   
        UserInfo res = userManager->findUser(userID);
        if (currentUser.privilege <= res.privilege) {
            std::cout << "Permission denied" << std::endl;
            return;
        }
        std::cout << "UserID: " << res.userID << std::endl;
        std::cout << "UserName: " << res.username << std::endl;
        std::cout << "Password: " << res.password << std::endl;
        std::cout << "Privilege: " << res.privilege << std::endl;
    }

    void modifyUserPassword(const UserID userID, const char *newPassword) {
        if (!userManager->existUser(userID)) {
            std::cout << "User not found." << std::endl;
            return;
        }

        UserInfo res = userManager->findUser(userID);
        if (currentUser.privilege <= res.privilege) {
            std::cout << "Modification forbidden." << std::endl;
            return;
        }
        userManager->modifyUserPassword(userID, newPassword);
        std::cout << "Modification succeeded." << std::endl;
    }

    void modifyUserPrivilege(const UserID userID, int newPrivilege) {
        if (!userManager->existUser(userID)) {
            std::cout << "User not found." << std::endl;
            return;
        }

        UserInfo res = userManager->findUser(userID);
        if (currentUser.privilege <= res.privilege) {
            std::cout << "Modification forbidden." << std::endl;
            return;
        }
        userManager->modifyUserPrivilege(userID, newPrivilege);
        std::cout << "Modification succeeded." << std::endl;
    }
} // namespace trainsys
