#include "TicketManager.h"
#include "TrainScheduler.h"
#include "DataStructure/List.h"

namespace trainsys {
    TicketManager::TicketManager(const std::string &filename) : ticketInfo(filename) {
    }

    TicketManager::~TicketManager() {
    }

    int TicketManager::querySeat(const TrainID &trainID, const Date &date, const StationID &stationID) {
        /* Question */
    }

    int TicketManager::updateSeat(const TrainID &trainID, const Date &date, const StationID &stationID, int delta) {
        /* Question */
    }

    void TicketManager::releaseTicket(const TrainScheduler &scheduler, const Date &date) {
        /* Question */
    }

    void TicketManager::expireTicket(const TrainID &trainID, const Date &date) {
        /* Question */
    }
}
