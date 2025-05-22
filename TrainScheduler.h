#ifndef TRAIN_SCHEDULER_H_
#define TRAIN_SCHEDULER_H_

#include "Utils.h"
#include "DataStructure/List.h"

namespace trainsys {
    class TrainScheduler {
    private:
        TrainID trainID;
        int seatNum;
        int passingStationNum;
        StationID stations[MAX_PASSING_STATION_NUMBER];
        int duration[MAX_PASSING_STATION_NUMBER];
        int price[MAX_PASSING_STATION_NUMBER];

    public:
        TrainScheduler();

        ~TrainScheduler();

        void addStation(const StationID &station);

        void insertStation(int i, const StationID &station);

        void removeStation(int i);

        int findStation(const StationID &station);

        void traverseStation();

        void setTrainID(const TrainID &id);

        void setPrice(const int price[]);

        void setDuration(const int duration[]);

        void setSeatNumber(int seatNum);

        TrainID getTrainID() const;

        int getSeatNum() const;

        int getPassingStationNum() const;

        void getStations(StationID *stations) const;

        void getDuration(int *duration) const;

        void getPrice(int *price) const;

        StationID getStation(int i) const;

        int getDuration(int i) const;

        int getPrice(int i) const;

        bool operator ==(const TrainScheduler &rhs) const;

        bool operator !=(const TrainScheduler &rhs) const;

        bool operator <(const TrainScheduler &rhs) const;

        friend std::ostream &operator <<(std::ostream &os, const TrainScheduler &trainScheduler);
    };
}

#endif
