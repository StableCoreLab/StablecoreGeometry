#include "Serialize/GeometryText.h"

#include <iomanip>
#include <istream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace Geometry::Serialize
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

[[nodiscard]] std::string ClosureToken(Geometry::Sdk::PolylineClosure closure)
{
    return closure == Geometry::Sdk::PolylineClosure::Closed ? "closed" : "open";
}

[[nodiscard]] bool ReadClosure(std::istringstream& iss, Geometry::Sdk::PolylineClosure& closure)
{
    std::string token;
    if (!(iss >> token))
    {
        return false;
    }

    if (token == "open")
    {
        closure = Geometry::Sdk::PolylineClosure::Open;
        return true;
    }

    if (token == "closed")
    {
        closure = Geometry::Sdk::PolylineClosure::Closed;
        return true;
    }

    return false;
}
} // namespace

std::string ToText(const Geometry::Sdk::Point2d& value)
{
    return MakeText("Point2d", value.x, value.y);
}

bool FromText(std::string_view text, Geometry::Sdk::Point2d& value)
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

std::string ToText(const Geometry::Sdk::Vector2d& value)
{
    return MakeText("Vector2d", value.x, value.y);
}

bool FromText(std::string_view text, Geometry::Sdk::Vector2d& value)
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

std::string ToText(const Geometry::Sdk::Box2d& value)
{
    const Geometry::Sdk::Point2d minPoint = value.MinPoint();
    const Geometry::Sdk::Point2d maxPoint = value.MaxPoint();
    return MakeText("Box2d", minPoint.x, minPoint.y, maxPoint.x, maxPoint.y);
}

bool FromText(std::string_view text, Geometry::Sdk::Box2d& value)
{
    std::istringstream iss{std::string(text)};
    if (!ExpectTag(iss, "Box2d"))
    {
        return false;
    }

    Geometry::Sdk::Point2d minPoint{};
    Geometry::Sdk::Point2d maxPoint{};
    if (!ReadDouble(iss, minPoint.x) ||
        !ReadDouble(iss, minPoint.y) ||
        !ReadDouble(iss, maxPoint.x) ||
        !ReadDouble(iss, maxPoint.y))
    {
        return false;
    }

    value = Geometry::Sdk::Box2d::FromMinMax(minPoint, maxPoint);
    return ExpectEnd(iss);
}

std::string ToText(const Geometry::Sdk::SegmentProjection2d& value)
{
    return MakeText(
        "SegmentProjection2d",
        value.point.x,
        value.point.y,
        value.parameter,
        value.distanceSquared,
        value.isOnSegment ? 1 : 0);
}

bool FromText(std::string_view text, Geometry::Sdk::SegmentProjection2d& value)
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

std::string ToText(const Geometry::Sdk::ArcSegment2d& value)
{
    return MakeText(
        "ArcSegment2d",
        value.center.x,
        value.center.y,
        value.radius,
        value.startAngle,
        value.sweepAngle);
}

bool FromText(std::string_view text, Geometry::Sdk::ArcSegment2d& value)
{
    std::istringstream iss{std::string(text)};
    if (!ExpectTag(iss, "ArcSegment2d"))
    {
        return false;
    }

    if (!ReadDouble(iss, value.center.x) ||
        !ReadDouble(iss, value.center.y) ||
        !ReadDouble(iss, value.radius) ||
        !ReadDouble(iss, value.startAngle) ||
        !ReadDouble(iss, value.sweepAngle))
    {
        return false;
    }

    return ExpectEnd(iss) && value.IsValid();
}

std::string ToText(const Geometry::Sdk::Polyline2d& value)
{
    std::ostringstream oss;
    oss.setf(std::ios::fmtflags(0), std::ios::floatfield);
    oss << std::setprecision(std::numeric_limits<double>::max_digits10)
        << "Polyline2d " << ClosureToken(value.IsClosed() ? Geometry::Sdk::PolylineClosure::Closed
                                                          : Geometry::Sdk::PolylineClosure::Open)
        << ' ' << value.PointCount();
    for (std::size_t i = 0; i < value.PointCount(); ++i)
    {
        const Geometry::Sdk::Point2d point = value.PointAt(i);
        oss << ' ' << point.x << ' ' << point.y;
    }
    return oss.str();
}

bool FromText(std::string_view text, Geometry::Sdk::Polyline2d& value)
{
    std::istringstream iss{std::string(text)};
    if (!ExpectTag(iss, "Polyline2d"))
    {
        return false;
    }

    Geometry::Sdk::PolylineClosure closure{};
    std::size_t pointCount = 0;
    if (!ReadClosure(iss, closure) || !(iss >> pointCount))
    {
        return false;
    }

    std::vector<Geometry::Sdk::Point2d> points(pointCount);
    for (std::size_t i = 0; i < pointCount; ++i)
    {
        if (!ReadDouble(iss, points[i].x) || !ReadDouble(iss, points[i].y))
        {
            return false;
        }
    }

    if (!ExpectEnd(iss))
    {
        return false;
    }

    value = Geometry::Sdk::Polyline2d(std::move(points), closure);
    return true;
}

std::string ToText(const Geometry::Sdk::Polygon2d& value)
{
    std::ostringstream oss;
    oss.setf(std::ios::fmtflags(0), std::ios::floatfield);
    oss << std::setprecision(std::numeric_limits<double>::max_digits10)
        << "Polygon2d " << ToText(value.OuterRing()) << ' ' << value.HoleCount();
    for (std::size_t i = 0; i < value.HoleCount(); ++i)
    {
        oss << ' ' << ToText(value.HoleAt(i));
    }
    return oss.str();
}

bool FromText(std::string_view text, Geometry::Sdk::Polygon2d& value)
{
    std::istringstream iss{std::string(text)};
    if (!ExpectTag(iss, "Polygon2d"))
    {
        return false;
    }

    std::string outerTag;
    if (!(iss >> outerTag) || outerTag != "Polyline2d")
    {
        return false;
    }

    std::string outerText = outerTag;
    std::string token;
    std::size_t outerPointCount = 0;
    if (!(iss >> token))
    {
        return false;
    }
    outerText += " " + token;
    if (token != "open" && token != "closed")
    {
        return false;
    }
    if (!(iss >> outerPointCount))
    {
        return false;
    }
    outerText += " " + std::to_string(outerPointCount);
    for (std::size_t i = 0; i < outerPointCount * 2; ++i)
    {
        if (!(iss >> token))
        {
            return false;
        }
        outerText += " " + token;
    }

    Geometry::Sdk::Polyline2d outerRing;
    if (!FromText(outerText, outerRing))
    {
        return false;
    }
    if (!outerRing.IsClosed())
    {
        return false;
    }

    std::size_t holeCount = 0;
    if (!(iss >> holeCount))
    {
        return false;
    }

    std::vector<Geometry::Sdk::Polyline2d> holes;
    holes.reserve(holeCount);
    for (std::size_t holeIndex = 0; holeIndex < holeCount; ++holeIndex)
    {
        std::string holeTag;
        if (!(iss >> holeTag) || holeTag != "Polyline2d")
        {
            return false;
        }

        std::string holeText = holeTag;
        if (!(iss >> token))
        {
            return false;
        }
        holeText += " " + token;
        if (token != "open" && token != "closed")
        {
            return false;
        }

        std::size_t holePointCount = 0;
        if (!(iss >> holePointCount))
        {
            return false;
        }
        holeText += " " + std::to_string(holePointCount);
        for (std::size_t i = 0; i < holePointCount * 2; ++i)
        {
            if (!(iss >> token))
            {
                return false;
            }
            holeText += " " + token;
        }

        Geometry::Sdk::Polyline2d hole;
        if (!FromText(holeText, hole))
        {
            return false;
        }
        if (!hole.IsClosed())
        {
            return false;
        }
        holes.push_back(std::move(hole));
    }

    if (!ExpectEnd(iss))
    {
        return false;
    }

    value = Geometry::Sdk::Polygon2d(std::move(outerRing), std::move(holes));
    return value.IsValid();
}
} // namespace Geometry::Serialize

