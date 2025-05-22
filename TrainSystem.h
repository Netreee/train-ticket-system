#ifndef TRAIN_SYSTEM_H_
#define TRAIN_SYSTEM_H_

#include "Utils.h"
#include "UserManager.h"
#include "TripManager.h"
#include "TicketManager.h"
#include "TrainScheduler.h"
#include "RailwayGraph.h"
#include "SchedulerManager.h"
#include "StationManager.h"
#include "WaitingList.h"
#include "DataStructure/RedBlackTree.h"

namespace trainsys {
    void addTrainScheduler(const TrainID &trainID, int seatNum, int passingStationNumber,
                           const StationID *stations, const int *duration, const int *price);

    void queryTrainScheduler(const TrainID &trainID);

    void releaseTicket(const TrainScheduler &scheduler, const Date &date);

    void expireTicket(const TrainID &trainID, const Date &date);

    int queryRemainingTicket(const TrainID &trainID, const Date &date, const StationID &departureStation);

    void queryMyTicket();

    void orderTicket(const TrainID &trainID, const Date &date, const StationID &departureStation);

    void refundTicket(const TrainID &trainID, const Date &date, const StationID &departureStation);

    void findAllRoute(const StationID departureID, const StationID arrivalID);

    void findBestRoute(const StationID departureID, const StationID arrivalID, int preference);

    void login(const UserID userID, const char *password);

    void logout();

    void addUser(const UserID userID, const char *username, const char *password);

    void findUserInfoByUserID(const UserID userID);

    void modifyUserPassword(const UserID userID, const char *password);

    void modifyUserPrivilege(const UserID userID, int newPrivilege);
}

#endif
