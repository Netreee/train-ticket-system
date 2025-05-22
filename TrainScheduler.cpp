#include "TrainScheduler.h"


namespace trainsys {
    TrainScheduler::TrainScheduler() {
        passingStationNum = 0;
    }

    TrainScheduler::~TrainScheduler() {
    }

    void TrainScheduler::addStation(const StationID &station) {
        /* Question */
    }

    void TrainScheduler::insertStation(int i, const StationID &station) {
        /* Question */
    }

    void TrainScheduler::removeStation(int i) {
        /* Question */
    }

    int TrainScheduler::findStation(const StationID &stationID) {
        /* Question */
    }

    void TrainScheduler::traverseStation() {
    }

    void TrainScheduler::setTrainID(const TrainID &id) {
        /* Question */
    }

    void TrainScheduler::setPrice(const int price[]) {
        /* Question */
    }

    void TrainScheduler::setDuration(const int duration[]) {
        /* Question */
    }

    void TrainScheduler::setSeatNumber(int seatNum) {
        /* Question */
    }

    TrainID TrainScheduler::getTrainID() const {
        /* Question */
    }

    int TrainScheduler::getSeatNum() const {
        /* Question */
    }

    int TrainScheduler::getPassingStationNum() const {
        /* Question */
    }

    StationID TrainScheduler::getStation(int i) const {
        /* Question */
    }

    int TrainScheduler::getDuration(int i) const {
        /* Question */
    }

    int TrainScheduler::getPrice(int i) const {
        /* Question */
    }

    void TrainScheduler::getStations(StationID *stations) const {
        /* Question */
    }

    void TrainScheduler::getDuration(int *duration) const {
        /* Question */
    }

    void TrainScheduler::getPrice(int *price) const {
        /* Question */
    }

    bool TrainScheduler::operator ==(const TrainScheduler &rhs) const {
        /* Question */
    }

    bool TrainScheduler::operator !=(const TrainScheduler &rhs) const {
        /* Question */
    }

    bool TrainScheduler::operator <(const TrainScheduler &rhs) const {
        /* Question */
    }

    std::ostream &operator <<(std::ostream &os, const TrainScheduler &trainScheduler) {
        /* Question */
    }
}
