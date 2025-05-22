#ifndef TICKET_MANAGER_H_
#define TICKET_MANAGER_H_

#include "Utils.h"
#include "TicketInfo.h"
#include "TrainScheduler.h"
#include "DataStructure/BPlusTree.h"

namespace trainsys {
    class TicketManager {
    private:
        BPlusTree<TrainID, TicketInfo> ticketInfo;

    public:
        TicketManager(const std::string &filename);

        ~TicketManager();

        int querySeat(const TrainID &trainID, const Date &date, const StationID &stationID);

        int updateSeat(const TrainID &trainID, const Date &date, const StationID &stationID, int delta);

        void releaseTicket(const TrainScheduler &scheduler, const Date &date);

        void expireTicket(const TrainID &trainID, const Date &date);
    };
}

#endif // TICKET_MANAGER_H_
