#include "Geometry2d/Polygon2d.h"

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
[[nodiscard]] Geometry::PolylineClosure ToInternalClosure(PolylineClosure closure)
{
    return closure == PolylineClosure::Closed ? Geometry::PolylineClosure::Closed
                                              : Geometry::PolylineClosure::Open;
}

[[nodiscard]] Geometry::Polyline2d BuildInternalPolylineFromSdk(const Polyline2d& polyline)
{
    std::vector<Point2d> points;
    points.reserve(polyline.PointCount());
    for (std::size_t i = 0; i < polyline.PointCount(); ++i)
    {
        points.push_back(polyline.PointAt(i));
    }

    std::vector<std::shared_ptr<Geometry::Segment2d>> segments;
    if (points.size() >= 2)
    {
        const PolylineClosure closure =
            polyline.IsClosed() ? PolylineClosure::Closed : PolylineClosure::Open;
        const std::size_t segmentCount = closure == PolylineClosure::Closed ? points.size()
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

struct Polygon2d::Impl
{
    Geometry::Polygon2d polygon;

    Impl() = default;

    explicit Impl(Geometry::Polygon2d value)
        : polygon(std::move(value))
    {
    }
};

Polygon2d::Polygon2d() : impl_(std::make_unique<Impl>()) {}

Polygon2d::Polygon2d(Polyline2d outerRing)
    : impl_(std::make_unique<Impl>(Geometry::Polygon2d(BuildInternalPolylineFromSdk(outerRing))))
{
}

Polygon2d::Polygon2d(Polyline2d outerRing, std::vector<Polyline2d> holes)
{
    std::vector<Geometry::Polyline2d> internalHoles;
    internalHoles.reserve(holes.size());
    for (const Polyline2d& hole : holes)
    {
        internalHoles.push_back(BuildInternalPolylineFromSdk(hole));
    }

    impl_ = std::make_unique<Impl>(
        Geometry::Polygon2d(BuildInternalPolylineFromSdk(outerRing), std::move(internalHoles)));
}

Polygon2d::Polygon2d(const Polygon2d& other)
    : impl_(std::make_unique<Impl>(other.impl_->polygon))
{
}

Polygon2d::Polygon2d(Polygon2d&& other) noexcept = default;

Polygon2d& Polygon2d::operator=(const Polygon2d& other)
{
    if (this != &other)
    {
        impl_ = std::make_unique<Impl>(other.impl_->polygon);
    }
    return *this;
}

Polygon2d& Polygon2d::operator=(Polygon2d&& other) noexcept = default;

Polygon2d::~Polygon2d() = default;

bool Polygon2d::IsValid() const
{
    return impl_->polygon.IsValid();
}

std::size_t Polygon2d::PointCount() const
{
    std::size_t count = impl_->polygon.OuterRing().VertexCount();
    for (std::size_t i = 0; i < impl_->polygon.HoleCount(); ++i)
    {
        count += impl_->polygon.HoleAt(i).VertexCount();
    }
    return count;
}

std::size_t Polygon2d::SegmentCount() const
{
    std::size_t count = impl_->polygon.OuterRing().SegmentCount();
    for (std::size_t i = 0; i < impl_->polygon.HoleCount(); ++i)
    {
        count += impl_->polygon.HoleAt(i).SegmentCount();
    }
    return count;
}

std::size_t Polygon2d::HoleCount() const
{
    return impl_->polygon.HoleCount();
}

Polyline2d Polygon2d::OuterRing() const
{
    std::vector<Point2d> points;
    const auto& ring = impl_->polygon.OuterRing();
    points.reserve(ring.VertexCount());
    for (std::size_t i = 0; i < ring.VertexCount(); ++i)
    {
        points.push_back(ring.VertexAt(i));
    }

    return Polyline2d(std::move(points), PolylineClosure::Closed);
}

Polyline2d Polygon2d::HoleAt(std::size_t index) const
{
    if (index >= impl_->polygon.HoleCount())
    {
        throw std::out_of_range("Polygon2d::HoleAt index out of range");
    }

    std::vector<Point2d> points;
    const auto& ring = impl_->polygon.HoleAt(index);
    points.reserve(ring.VertexCount());
    for (std::size_t i = 0; i < ring.VertexCount(); ++i)
    {
        points.push_back(ring.VertexAt(i));
    }

    return Polyline2d(std::move(points), PolylineClosure::Closed);
}

double Polygon2d::Area() const
{
    return impl_->polygon.Area();
}

Box2d Polygon2d::Bounds() const
{
    return impl_->polygon.Bounds();
}

std::string Polygon2d::DebugString() const
{
    std::ostringstream stream;
    stream << "Polygon2d{holeCount=" << HoleCount()
           << ", pointCount=" << PointCount()
           << ", segmentCount=" << SegmentCount()
           << ", valid=" << (IsValid() ? "true" : "false")
           << ", bounds=" << Bounds().DebugString() << "}";
    return stream.str();
}
} // namespace Geometry::Sdk

