#include "serialize/GeometryText.h"

#include <iomanip>
#include <istream>
#include <limits>
#include <sstream>
#include <string>

namespace geometry::serialize
{
namespace
{
template <typename... Ts>
[[nodiscard]] std::string MakeText(const char* tag, Ts... values)
{
    std::ostringstream oss;
    oss.setf(std::ios::fmtflags(0), std::ios::floatfield);
    oss << std::setprecision(std::numeric_limits<double>::max_digits10) << tag;
    ((oss << ' ' << values), ...);
    return oss.str();
}

[[nodiscard]] bool ExpectTag(std::istringstream& iss, const char* tag)
{
    std::string actualTag;
    if (!(iss >> actualTag))
    {
        return false;
    }

    return actualTag == tag;
}

[[nodiscard]] bool ReadDouble(std::istringstream& iss, double& value)
{
    return static_cast<bool>(iss >> value);
}

[[nodiscard]] bool ReadBool(std::istringstream& iss, bool& value)
{
    int raw = 0;
    if (!(iss >> raw))
    {
        return false;
    }

    if (raw != 0 && raw != 1)
    {
        return false;
    }

    value = raw != 0;
    return true;
}

[[nodiscard]] bool ExpectEnd(std::istringstream& iss)
{
    iss >> std::ws;
    return iss.eof();
}
} // namespace

std::string ToText(const sdk::Point2d& value)
{
    return MakeText("Point2d", value.x, value.y);
}

bool FromText(std::string_view text, sdk::Point2d& value)
{
    std::istringstream iss{std::string(text)};
    if (!ExpectTag(iss, "Point2d"))
    {
        return false;
    }

    if (!ReadDouble(iss, value.x) || !ReadDouble(iss, value.y))
    {
        return false;
    }

    return ExpectEnd(iss);
}

std::string ToText(const sdk::Vector2d& value)
{
    return MakeText("Vector2d", value.x, value.y);
}

bool FromText(std::string_view text, sdk::Vector2d& value)
{
    std::istringstream iss{std::string(text)};
    if (!ExpectTag(iss, "Vector2d"))
    {
        return false;
    }

    if (!ReadDouble(iss, value.x) || !ReadDouble(iss, value.y))
    {
        return false;
    }

    return ExpectEnd(iss);
}

std::string ToText(const sdk::Box2d& value)
{
    const sdk::Point2d minPoint = value.GetMinPoint();
    const sdk::Point2d maxPoint = value.GetMaxPoint();
    return MakeText("Box2d", minPoint.x, minPoint.y, maxPoint.x, maxPoint.y);
}

bool FromText(std::string_view text, sdk::Box2d& value)
{
    std::istringstream iss{std::string(text)};
    if (!ExpectTag(iss, "Box2d"))
    {
        return false;
    }

    sdk::Point2d minPoint{};
    sdk::Point2d maxPoint{};
    if (!ReadDouble(iss, minPoint.x) ||
        !ReadDouble(iss, minPoint.y) ||
        !ReadDouble(iss, maxPoint.x) ||
        !ReadDouble(iss, maxPoint.y))
    {
        return false;
    }

    value = sdk::Box2d::FromMinMax(minPoint, maxPoint);
    return ExpectEnd(iss);
}

std::string ToText(const sdk::SegmentProjection2d& value)
{
    return MakeText(
        "SegmentProjection2d",
        value.point.x,
        value.point.y,
        value.parameter,
        value.distanceSquared,
        value.isOnSegment ? 1 : 0);
}

bool FromText(std::string_view text, sdk::SegmentProjection2d& value)
{
    std::istringstream iss{std::string(text)};
    if (!ExpectTag(iss, "SegmentProjection2d"))
    {
        return false;
    }

    if (!ReadDouble(iss, value.point.x) ||
        !ReadDouble(iss, value.point.y) ||
        !ReadDouble(iss, value.parameter) ||
        !ReadDouble(iss, value.distanceSquared) ||
        !ReadBool(iss, value.isOnSegment))
    {
        return false;
    }

    return ExpectEnd(iss);
}
} // namespace geometry::serialize
