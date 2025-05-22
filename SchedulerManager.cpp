#include "SchedulerManager.h"


namespace trainsys {
    SchedulerManager::SchedulerManager(const std::string &filename)
        : schedulerInfo(filename) {
    }

    void SchedulerManager::addScheduler(const TrainID &trainID, int seatNum,
                                        int passingStationNumber, const StationID *stations, const int *duration,
                                        const int *price) {
        /* Question */
    }

    bool SchedulerManager::existScheduler(const TrainID &trainID) {
        /* Question */
    }

    TrainScheduler SchedulerManager::getScheduler(const TrainID &trainID) {
        /* Question */
    }

    void SchedulerManager::removeScheduler(const TrainID &trainID) {
        /* Question */
    }
}
