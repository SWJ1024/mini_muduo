#ifndef MUDUO_BASE_DATE_H
#define MUDUO_BASE_DATE_H

#include <time.h>
#include <stdio.h>
#include <string>


struct YearMonthDay {
	int year, month, day;
};


class Date {
	public:
		static const int kDayPerWeek =7;
		static const int KJulianDayOf1970_01_01;

		Date() : julianDayNumber_(0) {}
		Date(int, int, int);

		explicit Date(int julianDayNum) : julianDayNumber_(julianDayNum) {}
		explicit Date(const struct tm&);

		void swap(Date &t) {
			std::swap(julianDayNumber_, t.julianDayNumber_);
		}

		bool valid() const {return julianDayNumber_ > 0;}

		std::string toIsoString() const;

		int year() const {
			return YearMonthDay().year;
		}

		int month() const {
			return YearMonthDay().month;
		}

		int day() const {
			return YearMonthDay().day;
		}

		int weekDay() const {
			return (julianDayNumber_ + 1) % kDayPerWeek;
		}

		int julianDayNumber() const {
			return julianDayNumber_;
		}

		struct YearMonthDay yearMonthDay() const;
	private:
		int julianDayNumber_;
};

inline bool operator < (Date x, Date y) {
	return x.julianDayNumber() < y.julianDayNumber(); 
}


inline bool operator == (Date x, Date y) {
	return x.julianDayNumber() == y.julianDayNumber(); 
}

#endif
