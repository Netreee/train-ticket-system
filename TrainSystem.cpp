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
            std::cout << "TrainID existed." << std::endl;
            return;
        }
        
        try {
            // 添加列车调度信息
            schedulerManager->addScheduler(trainID, seatNum, passingStationNumber, stations, duration, price);
            
            // 将路线信息添加到铁路图中
            for (int i = 0; i < passingStationNumber - 1; i++) {
                railwayGraph->addRoute(stations[i], stations[i + 1], duration[i], price[i], trainID);
            }
            
            std::cout << "Train added." << std::endl;
        } catch (const std::exception& e) {
            std::cout << "添加列车失败: " << e.what() << std::endl;
        }
    }

    void queryTrainScheduler(const TrainID &trainID) {
        // 检查列车是否存在
        if (!schedulerManager->existScheduler(trainID)) {
            std::cout << "Train not found." << std::endl;
            return;
        }
        
        // 获取并显示列车调度信息
        TrainScheduler scheduler = schedulerManager->getScheduler(trainID);
        //打印
        std::cout << "TrainID: " << scheduler.getTrainID() << std::endl;
        std::cout << "SeatNum: " << scheduler.getSeatNum() << std::endl;
        std::cout << "PassingStationNum: " << scheduler.getPassingStationNum() << std::endl;
        std::cout << "Stations: " ;
        for(int i=0; i<scheduler.getPassingStationNum();i++){
            std::cout << scheduler.getStation(i) << " ";
        }
        std::cout << std::endl;
        std::cout << "Duration: ";
        for(int i=0; i<scheduler.getPassingStationNum()-1;i++){
            std::cout << scheduler.getDuration(i) << " ";
        }
        std::cout << std::endl;
        std::cout << "Price: ";
        for(int i=0; i<scheduler.getPassingStationNum()-1;i++){
            std::cout << scheduler.getPrice(i) << " ";
        }
        std::cout << std::endl;
    }

    void releaseTicket(const TrainScheduler &scheduler, const Date &date) {
        // 检查权限
        if (currentUser.privilege < ADMIN_PRIVILEGE) {
            std::cout << "Permission denied." << std::endl;
            return;
        }
        
        try {
            ticketManager->releaseTicket(scheduler, date);
            std::cout << "Ticket released." << std::endl;
        } catch (const std::exception& e) {
            std::cout << "发布车票失败: " << e.what() << std::endl;
        }
    }

    void expireTicket(const TrainID &trainID, const Date &date) {
        // 检查权限
        if (currentUser.privilege < ADMIN_PRIVILEGE) {
            std::cout << "Permission denied." << std::endl;
            return;
        }
        
        try {
            ticketManager->expireTicket(trainID, date);
            std::cout << "Ticket expired." << std::endl;
        } catch (const std::exception& e) {
            std::cout << "车票过期操作失败: " << e.what() << std::endl;
        }
    }

    int queryRemainingTicket(const TrainID &trainID, const Date &date, const StationID &departureStation) {
        // 检查列车是否存在
        if (!schedulerManager->existScheduler(trainID)) {
            //std::cout << "列车不存在" << std::endl;
            return -1;
        }
        
        // 查询余票
        int remaining = ticketManager->querySeat(trainID, date, departureStation);
        if (remaining == -1) {
            //std::cout << "该日期该站点无票务信息" << std::endl;
            return -1;
        }
        
        return remaining;
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
        while (waitingList->isBusy()){
            PurchaseInfo purchaseInfo = waitingList->getFrontPurchaseInfo();
            waitingList->removeHeadFromWaitingList();
            std::cout << "Processing request from User " << purchaseInfo.userID << std::endl;
    
            if(purchaseInfo.isOrdering()){
            // 购票信息
            int remainingTickets = queryRemainingTicket(purchaseInfo.trainID, purchaseInfo.date, purchaseInfo.departureStation);
            if(remainingTickets < purchaseInfo.type){
                std::cout << "No enough tickets or scheduler not exists. Order failed." << std::endl; //任务失败
            }else{
                ticketManager->updateSeat(purchaseInfo.trainID, purchaseInfo.date, purchaseInfo.departureStation, -purchaseInfo.type);
                
                TrainScheduler schedule = schedulerManager->getScheduler(purchaseInfo.trainID);
                int id = schedule.findStation(purchaseInfo.departureStation);
                int duration = schedule.getDuration(id);
                int price = schedule.getPrice(id);
                StationID arrivalStation = schedule.getStation(id + 1);
                
                tripManager->addTrip(purchaseInfo.userID, TripInfo(
                    purchaseInfo.trainID, purchaseInfo.departureStation, arrivalStation, purchaseInfo.type, duration, price, purchaseInfo.date
                ));    
                std::cout << "Order succeeded." << std::endl;
                }
            }else{
            // 退票信息
            ticketManager->updateSeat(purchaseInfo.trainID, purchaseInfo.date, purchaseInfo.departureStation, -purchaseInfo.type);
            
            TrainScheduler schedule = schedulerManager->getScheduler(purchaseInfo.trainID);
            int id = schedule.findStation(purchaseInfo.departureStation);
            int duration = schedule.getDuration(id);
            int price = schedule.getPrice(id);
            StationID arrivalStation = schedule.getStation(id + 1);
    
            tripManager->removeTrip(purchaseInfo.userID, TripInfo(
                purchaseInfo.trainID, purchaseInfo.departureStation, arrivalStation, -purchaseInfo.type, duration, price, purchaseInfo.date
            ));
            std::cout << "Refund succeeded." << std::endl;
            }
        }
        //添加任务
        waitingList->addToWaitingList(PurchaseInfo(currentUser.userID, trainID, date, departureStation, +1));
        std::cout << "Ordering request has added to waiting list." << std::endl;
    }

    void refundTicket(const TrainID &trainID, const Date &date, const StationID &departureStation) {
        while (waitingList->isBusy()){
            PurchaseInfo purchaseInfo = waitingList->getFrontPurchaseInfo();
            waitingList->removeHeadFromWaitingList();
            std::cout << "Processing request from User " << purchaseInfo.userID << std::endl;
    
            if(purchaseInfo.isOrdering()){
            // 购票信息
            int remainingTickets = queryRemainingTicket(purchaseInfo.trainID, purchaseInfo.date, purchaseInfo.departureStation);
            if(remainingTickets < purchaseInfo.type){
                std::cout << "No enough tickets or scheduler not exists. Order failed." << std::endl; //任务失败
            }else{
                ticketManager->updateSeat(purchaseInfo.trainID, purchaseInfo.date, purchaseInfo.departureStation, -purchaseInfo.type);
                
                TrainScheduler schedule = schedulerManager->getScheduler(purchaseInfo.trainID);
                int id = schedule.findStation(purchaseInfo.departureStation);
                int duration = schedule.getDuration(id);
                int price = schedule.getPrice(id);
                StationID arrivalStation = schedule.getStation(id + 1);
                
                tripManager->addTrip(purchaseInfo.userID, TripInfo(
                    purchaseInfo.trainID, purchaseInfo.departureStation, arrivalStation, purchaseInfo.type, duration, price, purchaseInfo.date
                ));    
                std::cout << "Order succeeded." << std::endl;
                }
            }else{
            // 退票信息
            ticketManager->updateSeat(purchaseInfo.trainID, purchaseInfo.date, purchaseInfo.departureStation, -purchaseInfo.type);
            
            TrainScheduler schedule = schedulerManager->getScheduler(purchaseInfo.trainID);
            int id = schedule.findStation(purchaseInfo.departureStation);
            int duration = schedule.getDuration(id);
            int price = schedule.getPrice(id);
            StationID arrivalStation = schedule.getStation(id + 1);
    
            tripManager->removeTrip(purchaseInfo.userID, TripInfo(
                purchaseInfo.trainID, purchaseInfo.departureStation, arrivalStation, -purchaseInfo.type, duration, price, purchaseInfo.date
            ));
            std::cout << "Refund succeeded." << std::endl;
            }
        }
        //添加任务
        waitingList->addToWaitingList(PurchaseInfo(currentUser.userID, trainID, date, departureStation, -1));
        std::cout << "Refunding request has added to waiting list." << std::endl;
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
        // 检查用户是否已经登录
        if (currentUser.userID != -1) {
            std::cout << "Only one user can login in at the same time." << std::endl;
            return;
        }
        
        // 检查用户是否存在
        if (!userManager->existUser(userID)) {
            std::cout << "User not found. Login failed." << std::endl;
            return;
        }
        
        // 验证密码
        UserInfo user = userManager->findUser(userID);
        if (strcmp(user.password, password) != 0) {
            std::cout << "Wrong password. Login failed." << std::endl;
            return;
        }
        
        // 登录成功
        currentUser = user;
        std::cout << "Login succeeded." << std::endl;
        //std::cout << "Welcome, " << currentUser.username << "!" << std::endl;
    }

    void logout() {
        // 检查是否已登录
        if (currentUser.userID == -1) {
            std::cout << "No user logined." << std::endl;
            return;
        }
        
        //std::cout << "Goodbye, " << currentUser.username << "!" << std::endl;
        
        // 重置当前用户为未登录状态
        currentUser = UserInfo();
        currentUser.userID = -1;
        
        std::cout << "Logout succeeded." << std::endl;
    }

    void addUser(const UserID userID, const char *username, const char* password) {
        // 检查是否有用户登录
        if (currentUser.userID != -1) {
            std::cout << "Permission denied. Please logout first." << std::endl;
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
