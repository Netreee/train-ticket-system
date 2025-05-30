#ifndef DATETIME_H_
#define DATETIME_H_

#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

namespace trainsys {
    const int mday_number_[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const int prefix_total_[13] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

    struct Date {
        int mon;
        int mday;

        Date() = default;

        Date(int mday_, int mon_);

        Date(const Date &o) = default;

        explicit Date(const char *str);

        ~Date() = default;

        Date &operator+=(int days);

        Date &operator-=(int days);

        Date operator+(int days);

        Date operator-(int days);

        operator std::string() const;

        friend std::ostream &operator<<(std::ostream &os, const Date &date);

        friend bool operator==(const Date &lhs, const Date &rhs);

        friend bool operator!=(const Date &lhs, const Date &rhs);

        friend bool operator<(const Date &lhs, const Date &rhs);

        friend bool operator>(const Date &lhs, const Date &rhs);

        friend bool operator<=(const Date &lhs, const Date &rhs);

        friend bool operator>=(const Date &lhs, const Date &rhs);

        friend int operator-(const Date &lhs, const Date &rhs);
    };

    struct Time {
        Date date;
        int min;
        int hour;

        Time() = default;

        Time(int min_, int hour_, int mday_ = 1, int mon_ = 1, int year_ = 1);

        Time(const Time &o);

        explicit Time(const char *str);

        ~Time() = default;

        operator std::string() const;

        friend std::ostream &operator<<(std::ostream &os, const Time &time);

        Time &operator+=(int mins);

        Time &operator-=(int mins);

        Time operator+(int mins) const;

        Time operator-(int mins) const;

        friend bool operator==(const Time &lhs, const Time &rhs);

        friend bool operator!=(const Time &lhs, const Time &rhs);

        friend bool operator<(const Time &lhs, const Time &rhs);

        friend bool operator>(const Time &lhs, const Time &rhs);

        friend bool operator<=(const Time &lhs, const Time &rhs);

        friend bool operator>=(const Time &lhs, const Time &rhs);

        friend int operator-(const Time &lhs, const Time &rhs);
    };
} // namespace trainsys

#endif  // LIBTRAIN_DATETIME_H_
