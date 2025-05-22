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
        /* Question */
    }

    void queryTrainScheduler(const TrainID &trainID) {
        /* Question */
    }

    void releaseTicket(const TrainScheduler &scheduler, const Date &date) {
        /* Question */
    }

    void expireTicket(const TrainID &trainID, const Date &date) {
        /* Question */
    }

    int queryRemainingTicket(const TrainID &trainID, const Date &date, const StationID &departureStation) {
        /* Question */
    }

    bool trySatisfyOrder() {
        /* Question */
    }

    void queryMyTicket() {
        /* Question */
    }

    void orderTicket(const TrainID &trainID, const Date &date, const StationID &departureStation) {
        /* Question */
    }

    void refundTicket(const TrainID &trainID, const Date &date, const StationID &departureStation) {
        /* Question */
    }

    void findAllRoute(const StationID departureID, const StationID arrivalID) {
        /* Question */
    }

    void findBestRoute(const StationID departureID, const StationID arrivalID, int preference) {
        /* Question */
    }

    void login(const UserID userID, const char *password) {
        /* Question */
    }

    void logout() {
        /* Question */
    }

    void addUser(const UserID userID, const char *username, const char* password) {
        if (userManager->existUser(userID)) {
            std::cout << "User ID existed." << std::endl;
            return ;
        }
        if (currentUser.userID == -1) {
            std::cout << "Permission denied." << std::endl;
            return ;
        }

        userManager->insertUser(userID, username, password, 0);
        std::cout << "User added." << std::endl;
    }

    void findUserInfoByUserID(const UserID userID) {
        /* Question */
    }

    void modifyUserPassword(const UserID userID, const char *newPassword) {
        /* Question */
    }

    void modifyUserPrivilege(const UserID userID, int newPrivilege) {
        /* Question */
    }
} // namespace trainsys
