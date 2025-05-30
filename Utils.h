#ifndef UTILS_H_
#define UTILS_H_

#include <cstring>
#include "DateTime.h"
#include "DataStructure/List.h"

namespace trainsys {
    const int MAX_TRAINID_LEN = 20;
    const int MAX_USERNAME_LEN = 20;
    const int MAX_PASSWORD_LEN = 30;

    const int MAX_PASSING_STATION_NUMBER = 30;

    const int MAX_STATIONID = 1000;
    const int MAX_STATIONNAME_LEN = 30;

    const int ADMIN_PRIVILEGE = 10;

    const int BUSY_STATE_TRESHOLD = 1;

    struct String;

    using UserID = long long;
    using StationID = int;
    using TrainID = String;
    using StationName = String;

    const int MAX_STRING_LENGTH = 50;

    struct String {
        char index[MAX_STRING_LENGTH];

        String() = default;

        explicit String(const char *str) {
            strcpy(index, str);
        }

        String(const String &other) {
            for (int i = 0; i < MAX_STRING_LENGTH; i++) {
                index[i] = other.index[i];
            }
        }

        String &operator=(const String &other) {
            for (int i = 0; i < MAX_STRING_LENGTH; i++) {
                index[i] = other.index[i];
            }
            return *this;
        }

        friend bool operator>(const String &lhs, const String &rhs) {
            return std::string(lhs.index) > std::string(rhs.index);
        }

        friend bool operator>=(const String &lhs, const String &rhs) {
            return std::string(lhs.index) >= std::string(rhs.index);
        }

        friend bool operator<(const String &lhs, const String &rhs) {
            return std::string(lhs.index) < std::string(rhs.index);
        }

        friend bool operator<=(const String &lhs, const String &rhs) {
            return std::string(lhs.index) <= std::string(rhs.index);
        }

        friend bool operator==(const String &lhs, const String &rhs) {
            return std::string(lhs.index) == std::string(rhs.index);
        }

        friend bool operator!=(const String &lhs, const String &rhs) {
            return std::string(lhs.index) != std::string(rhs.index);
        }

        friend std::ostream &operator<<(std::ostream &os, const String &obj) {
            os << obj.index;
            return os;
        }
    };

    template<class elemType>
    int binarySearch(const list<elemType> &data, const elemType &x) {
        int low = 0, high = data.length() - 1, mid;
        while (low <= high) {
            mid = (low + high) / 2;
            if (x == data.visit(mid)) return mid;
            else if (x < data.visit(mid)) high = mid - 1;
            else low = mid + 1;
        }
        return 0;
    }
}

#endif
