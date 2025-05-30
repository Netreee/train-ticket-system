#include "TripManager.h"
#include "DataStructure/List.h"

namespace trainsys {
    TripManager::TripManager(const std::string &filename)
        : tripInfo(filename) {
    }

    void TripManager::addTrip(const UserID &userID, const TripInfo &trip) {
        /* Question */
    }

    seqList<TripInfo> TripManager::queryTrip(const UserID &userID) {
        /* Question */
    }

    void TripManager::removeTrip(const UserID &userID, const TripInfo &trip) {
        /* Question */
    }
}
