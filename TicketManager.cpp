#include "TicketManager.h"
#include "TrainScheduler.h"
#include "DataStructure/List.h"

namespace trainsys {
    TicketManager::TicketManager(const std::string &filename) : ticketInfo(filename) {
    }

    TicketManager::~TicketManager() {
    }

    int TicketManager::querySeat(const TrainID &trainID, const Date &date, const StationID &stationID) {
        seqList<TicketInfo> relatedInfo = ticketInfo.find(trainID);
        for (int i = 0; i < relatedInfo.length(); ++i) {
            if (relatedInfo.visit(i).date == date && relatedInfo.visit(i).departureStation == stationID) {
                return relatedInfo.visit(i).seatNum;
            }
        }
    return -1; //没有找到符合条件的车票
    }

    int TicketManager::updateSeat(const TrainID &trainID, const Date &date, const StationID &stationID, int delta) {
        return 0;
    }

    void TicketManager::releaseTicket(const TrainScheduler &scheduler, const Date &date) {
        /* Question */
        int passingStationNum = scheduler.getPassingStationNum();
        for (int i = 0; i + 1 < passingStationNum; ++i) {
            TicketInfo newTicket;
            newTicket.trainID = scheduler.getTrainID();
            newTicket.departureStation = scheduler.getStation(i);
            newTicket.arrivalStation = scheduler.getStation(i + 1);
            newTicket.seatNum = scheduler.getSeatNum();
            newTicket.price = scheduler.getPrice(i);
            newTicket.duration = scheduler.getDuration(i);
            newTicket.date = date;
            ticketInfo.insert(newTicket.trainID, newTicket);
        }
    }

    void TicketManager::expireTicket(const TrainID &trainID, const Date &date) {
        seqList<TicketInfo> relatedInfo = ticketInfo.find(trainID);
        if(relatedInfo.length() == 0){
            std::cout << "Train not found." << std::endl;  //没有找到trainID
        }else{
            for (int i = 0; i < relatedInfo.length(); ++i) {
                if (relatedInfo.visit(i).date == date) {
                    ticketInfo.remove(trainID, relatedInfo.visit(i));
                }
            }
            std::cout << "Ticket expired." << std::endl;
        }
    }
}
