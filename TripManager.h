#ifndef TRIP_MANAGER_H_
#define TRIP_MANAGER_H_

#include "Utils.h"
#include "DataStructure/BPlusTree.h"
#include "TripInfo.h"

namespace trainsys {
    class TripManager {
    private:
        BPlusTree<UserID, TripInfo> tripInfo;

    public:
        TripManager(const std::string &filename);

        ~TripManager() {
        }

        void addTrip(const UserID &userID, const TripInfo &trip);

        seqList<TripInfo> queryTrip(const UserID &userID);

        void removeTrip(const UserID &userID, const TripInfo &trip);
    };
} // namespace trainsys 

#endif
