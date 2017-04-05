#pragma once

namespace openbbg {

typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimeValue;

inline void GetTime(TimeValue &timeValue)
{
	timeValue = std::chrono::high_resolution_clock::now();
}

inline float GetTimeDurationMS(TimeValue &begin, TimeValue &end)
{
	return std::chrono::duration<float> { end - begin }.count() * 1000.f;
}


typedef struct AverageLog AverageLog;

#if 1
// Time based average log
struct AverageLog
{
	std::vector<float> log;

	float trackLength;

	float total;

	float average;

	AverageLog(float trackLength)
		: trackLength { trackLength }
		, total { 0.f }
		, average { 0.f }
	{
		log.reserve(256);
	}

	inline bool Push(float in)
	{
		log.push_back(in);
		total += in;
		if (total >= trackLength) {
			average = total / (float)log.size();
			log.clear();
			total = 0.f;
			return true;
		}
		return false;
	}
};
#else
// Fixed length average log
struct AverageLog
{
	std::vector<float> log;

	uint32_t currentIdx;

	uint32_t trackLength;

	float invTrackLength;

	float total;

	float average;

	AverageLog(uint32_t trackLength)
		: trackLength { trackLength }
		, currentIdx { 0 }
		, total { 0.f }
		, average { 0.f }
	{
		log.resize(trackLength);
		invTrackLength = 1.f / (float)trackLength;
	}

	inline bool Push(float in)
	{
		log[currentIdx] = in;
		total += in;
		if (++currentIdx == trackLength) {
			currentIdx = 0;
			average = total * invTrackLength;
			total = 0.f;
			return true;
		}
		return false;
	}
};
#endif

}