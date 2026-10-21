#pragma once

#include <string>
#include <sstream>
#include <type_traits>
#include <rapidjson/document.h>
#include <boost/unordered_set.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "FastLog.hpp"
#include "RbxFormat.hpp"

DYNAMIC_FASTFLAG(InfluxDb09Enabled)

namespace Aya
{

namespace Analytics
{

void setUserId(int id);
void setPlaceId(int id);
void setAppVersion(const std::string& version);
void setLocation(const std::string& loc);
void setReporter(const std::string& rep);

namespace EphemeralCounter
{
void reportStats(const std::string& category, float value, bool blocking = false);
void reportCountersCSV(const std::string& counterNamesCSV, bool blocking = false);
void reportCounter(const std::string& counterName, int amount, bool blocking = false);
} // namespace EphemeralCounter

namespace GoogleAnalytics
{
// Allow for easy initialization based on a lottery number.
// Calls setCanUseAnalytics and init.
void lotteryInit(const std::string& accountPropertyID, int lotteryThreshold, const std::string& productName = "", int robloxAnalyticsLottery = -1,
    const std::string& sessionKey = "sessionID=");

// Must be called before using the singleton.
void init(const std::string& accountPropertyID, const std::string& productName = "");

bool getCanUse();
void setCanUse();

void sendEventRoblox(const char* category, const char* action = "custom", const char* label = "none", int value = 0, bool sync = false);

void trackEvent(const char* category, const char* action = "custom", const char* label = "none", int value = 0, bool sync = false);
void trackEventWithoutThrottling(const char* category, const char* action = "custom", const char* label = "none", int value = 0, bool sync = false);
void trackUserTiming(const char* category, const char* variable, int milliseconds, const char* label = "none", bool sync = false);

const std::string& getSessionId();

} // namespace GoogleAnalytics


namespace InfluxDb
{

struct Point
{
    Point() = default;

    std::string name;
    std::string json;

    Point(const std::string& name_, const rapidjson::Value& value)
        : name(name_)
    {
        using namespace rapidjson;

        if (value.IsNull())
        {
            json = "null";
        }
        else if (value.IsBool())
        {
            json = value.GetBool() ? "true" : "false";
        }
        else if (value.IsObject() || value.IsArray())
        {
            throw std::runtime_error("Arrays and objects are not valid value types.");
        }
        else if (value.IsString())
        {
            std::string sval = value.GetString();
            if (DFFlag::InfluxDb09Enabled)
            {
                // escape double quotes
                boost::replace_all(sval, "\"", "\\\"");
            }
            json = "\"" + sval + "\"";
        }
        else if (value.IsNumber())
        {
            std::stringstream ss;
            if (value.IsDouble())
            {
                ss << value.GetDouble();
            }
            else if (value.IsInt())
            {
                ss << value.GetInt();
                if (DFFlag::InfluxDb09Enabled)
                {
                    ss << 'i'; // signals integer type
                }
            }
            else if (value.IsInt64())
            {
                ss << value.GetInt64();
                if (DFFlag::InfluxDb09Enabled)
                {
                    ss << 'i'; // signals integer type
                }
            }
            else if (value.IsUint())
            {
                ss << value.GetUint();
                if (DFFlag::InfluxDb09Enabled)
                {
                    ss << 'i'; // signals integer type
                }
            }
            else if (value.IsUint64())
            {
                ss << value.GetUint64();
                if (DFFlag::InfluxDb09Enabled)
                {
                    ss << 'i'; // signals integer type
                }
            }
            else
            {
                throw std::runtime_error("Unknown number type.");
            }
            json = ss.str();
        }
        else
        {
            throw std::runtime_error("Unknown rapidjson value type.");
        }
    }

    bool operator==(const Point& other) const
    {
        return this->name == other.name;
    }
};
std::size_t hash_value(const Point& p);

void init();
void reportPoints(const std::string& resource, const boost::unordered_set<Point>& points, int throttleHundredthsPercentage, bool blocking = false,
    const std::string& userIdOverride = "");
void reportPointsV2(const std::string& resource, const boost::unordered_set<Point>& points, int throttleHundredthsPercentage, bool blocking = false,
    const std::string& userIdOverride = "");

class Points
{
    boost::unordered_set<Point> pointList;
    std::string userIdOverride;

public:
    Points() {}
    ~Points() {}

    void setUserIdOverride(const int id)
    {
        userIdOverride = Aya::format("%d", id);
    }

    template<typename T>
    void addPoint(const std::string& name, T value, bool override = false)
    {
        rapidjson::Document doc; // Create a temporary rapidjson document
        rapidjson::Value val;    // Declare a rapidjson value

        if constexpr (std::is_same_v<T, int>)
            val.SetInt(value);
        else if constexpr (std::is_same_v<T, double>)
            val.SetDouble(value);
        // Add more type checks as needed...

        Point newPoint = Point(name, val);
        std::pair<boost::unordered_set<Point>::iterator, bool> res = pointList.insert(newPoint);

        if (override && !res.second)
        {
            pointList.erase(res.first);
            pointList.insert(newPoint);
        }
    }

    void report(const std::string& resource, int throttleHundredthsPercentage, bool blocking = false)
    {
        if (!pointList.empty())
        {
            if (DFFlag::InfluxDb09Enabled)
                reportPointsV2(resource, pointList, throttleHundredthsPercentage, blocking, userIdOverride);
            else
                reportPoints(resource, pointList, throttleHundredthsPercentage, blocking, userIdOverride);
            pointList.clear();
        }
    }

    const boost::unordered_set<Point>& getPoints()
    {
        return pointList;
    }
};


} // namespace InfluxDb

} // namespace Analytics

} // namespace Aya