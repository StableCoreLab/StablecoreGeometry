#include "Geometry2d/ISCPolygon2d.h"

#include <memory>
#include <stdexcept>
#include <sstream>
#include <utility>
#include <vector>

#include "Types/Geometry2d/LineSegment2.h"
#include "Types/Geometry2d/Polygon2.h"

namespace Geometry::Sdk
{
namespace
{
[[nodiscard]] Geometry::PolylineClosure ToInternalClosure(ISCPolylineClosure closure)
{
    return closure == ISCPolylineClosure::Closed ? Geometry::PolylineClosure::Closed
                                                 : Geometry::PolylineClosure::Open;
}

[[nodiscard]] Geometry::Polyline2d BuildInternalPolylineFromSdk(const ISCPolyline2d& polyline)
{
    std::vector<SCPoint2d> points;
    points.reserve(polyline.PointCount());
    for (std::size_t i = 0; i < polyline.PointCount(); ++i)
    {
        points.push_back(polyline.PointAt(i));
    }

    std::vector<std::shared_ptr<Geometry::Segment2d>> segments;
    if (points.size() >= 2)
    {
        const ISCPolylineClosure closure =
            polyline.IsClosed() ? ISCPolylineClosure::Closed : ISCPolylineClosure::Open;
        const std::size_t segmentCount = closure == ISCPolylineClosure::Closed ? points.size()
                                                                            : points.size() - 1;
        segments.reserve(segmentCount);
        for (std::size_t i = 0; i < segmentCount; ++i)
        {
            segments.push_back(std::make_shared<Geometry::LineSegment2d>(
                points[i],
                points[(i + 1) % points.size()]));
        }
        return Geometry::Polyline2d(std::move(segments), ToInternalClosure(closure));
    }

    return Geometry::Polyline2d();
}
} // namespace

struct ISCPolygon2d::SCImpl
{
    Geometry::Polygon2d polygon;

    SCImpl() = default;

    explicit SCImpl(Geometry::Polygon2d value)
        : polygon(std::move(value))
    {
    }
};

ISCPolygon2d::ISCPolygon2d() : impl_(std::make_unique<SCImpl>()) {}

ISCPolygon2d::ISCPolygon2d(ISCPolyline2d outerRing)
    : impl_(std::make_unique<SCImpl>(Geometry::Polygon2d(BuildInternalPolylineFromSdk(outerRing))))
{
}

ISCPolygon2d::ISCPolygon2d(ISCPolyline2d outerRing, std::vector<ISCPolyline2d> holes)
{
    std::vector<Geometry::Polyline2d> internalHoles;
    internalHoles.reserve(holes.size());
    for (const ISCPolyline2d& hole : holes)
    {
        std::vector<SCPoint2d> points;
        points.reserve(hole.PointCount());
        for (std::size_t i = 0; i < hole.PointCount(); ++i)
        {
            points.push_back(hole.PointAt(i));
        }

        std::vector<std::shared_ptr<Geometry::Segment2d>> segments;
        if (points.size() >= 2)
        {
            const std::size_t segmentCount = points.size();
            segments.reserve(segmentCount);
            for (std::size_t i = 0; i < segmentCount; ++i)
            {
                segments.push_back(std::make_shared<Geometry::LineSegment2d>(
                    points[i],
                    points[(i + 1) % points.size()]));
            }
        }

        internalHoles.push_back(Geometry::Polyline2d(std::move(segments), Geometry::PolylineClosure::Closed));
    }

    impl_ = std::make_unique<SCImpl>(
        Geometry::Polygon2d(BuildInternalPolylineFromSdk(outerRing), std::move(internalHoles)));
}

ISCPolygon2d::ISCPolygon2d(const ISCPolygon2d& other)
    : impl_(std::make_unique<SCImpl>(other.impl_->polygon))
{
}

ISCPolygon2d::ISCPolygon2d(ISCPolygon2d&& other) noexcept = default;

ISCPolygon2d& ISCPolygon2d::operator=(const ISCPolygon2d& other)
{
    if (this != &other)
    {
        impl_ = std::make_unique<SCImpl>(other.impl_->polygon);
    }
    return *this;
}

ISCPolygon2d& ISCPolygon2d::operator=(ISCPolygon2d&& other) noexcept = default;

ISCPolygon2d::~ISCPolygon2d() = default;

bool ISCPolygon2d::IsValid() const
{
    return impl_->polygon.IsValid();
}

std::size_t ISCPolygon2d::PointCount() const
{
    std::size_t count = impl_->polygon.OuterRing().VertexCount();
    for (std::size_t i = 0; i < impl_->polygon.HoleCount(); ++i)
    {
        count += impl_->polygon.HoleAt(i).VertexCount();
    }
    return count;
}

std::size_t ISCPolygon2d::SegmentCount() const
{
    std::size_t count = impl_->polygon.OuterRing().SegmentCount();
    for (std::size_t i = 0; i < impl_->polygon.HoleCount(); ++i)
    {
        count += impl_->polygon.HoleAt(i).SegmentCount();
    }
    return count;
}

std::size_t ISCPolygon2d::HoleCount() const
{
    return impl_->polygon.HoleCount();
}

ISCPolyline2d ISCPolygon2d::OuterRing() const
{
    std::vector<SCPoint2d> points;
    const auto& ring = impl_->polygon.OuterRing();
    points.reserve(ring.VertexCount());
    for (std::size_t i = 0; i < ring.VertexCount(); ++i)
    {
        points.push_back(ring.VertexAt(i));
    }

    return ISCPolyline2d(std::move(points), ISCPolylineClosure::Closed);
}

ISCPolyline2d ISCPolygon2d::HoleAt(std::size_t index) const
{
    if (index >= impl_->polygon.HoleCount())
    {
        throw std::out_of_range("ISCPolygon2d::HoleAt index out of range");
    }

    std::vector<SCPoint2d> points;
    const auto& ring = impl_->polygon.HoleAt(index);
    points.reserve(ring.VertexCount());
    for (std::size_t i = 0; i < ring.VertexCount(); ++i)
    {
        points.push_back(ring.VertexAt(i));
    }

    return ISCPolyline2d(std::move(points), ISCPolylineClosure::Closed);
}

double ISCPolygon2d::Area() const
{
    return impl_->polygon.Area();
}

SCBox2d ISCPolygon2d::Bounds() const
{
    return impl_->polygon.Bounds();
}

std::string ISCPolygon2d::DebugString() const
{
    std::ostringstream stream;
    stream << "ISCPolygon2d{holeCount=" << HoleCount()
           << ", pointCount=" << PointCount()
           << ", segmentCount=" << SegmentCount()
           << ", valid=" << (IsValid() ? "true" : "false")
           << ", bounds=" << Bounds().DebugString() << "}";
    return stream.str();
}
} // namespace Geometry::Sdk
