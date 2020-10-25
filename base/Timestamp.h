#ifndef MUDUO_BASE_TIMESTAMP_H
#define MUDUO_BASE_TIMESTAMP_H

#include <boost/operators.hpp>
#include <string>

using std::string;

class Timestamp : public boost::equality_comparable<Timestamp>, 
                  public boost::less_than_comparable<Timestamp> {
public:
    Timestamp();
    explicit Timestamp(int64_t);
    void swap(Timestamp &);
    string toString() const;
    static Timestamp now();
    string toFormattedString(bool showMicroseconds = true) const;
    int64_t getmicroSecond() const;
    time_t getSecond() const;
	bool valid() const {return microSeconds > 0;}
	static Timestamp invalid() {return Timestamp();}
    static const int M = 1000*1000;
private:
    int64_t microSeconds;
};


inline bool operator < (Timestamp lhs, Timestamp rhs) {
    return lhs.getmicroSecond() < rhs.getmicroSecond();
}

inline bool operator == (Timestamp lhs, Timestamp rhs) {
    return lhs.getmicroSecond() == rhs.getmicroSecond();
}

inline Timestamp addTime(Timestamp timestamp, double t) {
    int64_t del = static_cast<int64_t> (t*Timestamp::M);
    return Timestamp(timestamp.getmicroSecond() + del);
}


inline double timeDifference(Timestamp a, Timestamp b) {
    double del = a.getmicroSecond() - b.getmicroSecond();
    return static_cast<double> (del/Timestamp::M);
}




#endif    //MUDUO_ABSE_TIMESTAMP_H
