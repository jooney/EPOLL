/*
 * Timestamp.h
 *
 *  Created on: 2017Äê9ÔÂ24ÈÕ
 *      Author: jjz
 */

#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_
#include <algorithm>
#include <string>

class Timestamp
{
public:
	Timestamp()
		:_microSecondsSinceEpoch(0){}
	Timestamp(long long microSecondsSinceEpochArg)
		:_microSecondsSinceEpoch(microSecondsSinceEpochArg){}

	void swap(Timestamp& that)
	{
		std::swap(_microSecondsSinceEpoch,that._microSecondsSinceEpoch);
	}
	std::string toString() const;
	std::string toFormatString(bool showMicroseconds = true)const;
	bool valid() const {return _microSecondsSinceEpoch > 0;}
	long long microSecondsSinceEpoch() const {return _microSecondsSinceEpoch;}
	time_t secondsSinceEpoch() const
	{
		return static_cast<time_t>(_microSecondsSinceEpoch / kMicroSecondsPerSecond);
	}

	static Timestamp now();
	static Timestamp invalid()
	{
		return Timestamp();
	}
	static Timestamp fromUnixTime(time_t t)
	{
		return fromUnixTime(t,0);
	}
	static Timestamp fromUnixTime(time_t t, int microseconds) //time_t return seconds
	{
		return Timestamp(static_cast<long long>(t) * kMicroSecondsPerSecond + microseconds);
	}
	static const int kMicroSecondsPerSecond = 1000 * 1000;

	bool operator< (const Timestamp& ref) const
	{
		return this->microSecondsSinceEpoch() < ref.microSecondsSinceEpoch();
	}
	bool operator== (const Timestamp& ref) const
	{
		return this->microSecondsSinceEpoch() == ref.microSecondsSinceEpoch();
	}
private:
	long long _microSecondsSinceEpoch;
};

//return in seconds
/*double timeDifference(const Timestamp& high, const Timestamp& low)
{
	long long diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
	return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

Timestamp addTime(const Timestamp& timestamp, double seconds)
{
	long long delta = static_cast<long long>(seconds * Timestamp::kMicroSecondsPerSecond);
	return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}*/
#endif /* TIMESTAMP_H_ */
