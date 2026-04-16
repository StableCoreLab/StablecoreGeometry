#include "Geometry2d/Polygon2d.h"

#include <memory>
#include <stdexcept>
#include <sstream>
#include <utility>
#include <vector>

#include "Geometry2d/LineSegment2d.h"
#include "Geometry2d/ArcSegment2d.h"
#include "Geometry2d/Polyline2d.h"
#include "Types/Geometry2d/LineSegment2.h"
#include "Types/Geometry2d/ArcSegment2.h"
#include "Types/Geometry2d/Polygon2.h"

namespace Geometry
{
namespace
{
using InternalPolyline = Geometry::Polyline2<double>;
using InternalPolygon = Geometry::Polygon2<double>;
using InternalSegment = Geometry::Segment2<double>;
using InternalLineSegment = Geometry::LineSegment2<double>;

[[nodiscard]] Geometry::PolylineClosure2 ToInternalClosure(PolylineClosure closure)
{
    return closure == PolylineClosure::Closed ? Geometry::PolylineClosure2::Closed
                                              : Geometry::PolylineClosure2::Open;
}

[[nodiscard]] InternalPolyline BuildInternalPolylineFromWrapper(const Polyline2d& polyline)
{
    std::vector<std::shared_ptr<InternalSegment>> segments;
    segments.reserve(polyline.SegmentCount());
    for (std::size_t i = 0; i < polyline.SegmentCount(); ++i)
    {
        const std::unique_ptr<Segment2d> segment = polyline.SegmentAt(i);
        if (segment == nullptr)
        {
            return InternalPolyline();
        }

        if (const auto* line = dynamic_cast<const LineSegment2d*>(segment.get()))
        {
            segments.push_back(std::make_shared<InternalLineSegment>(
                line->startPoint,
                line->endPoint));
            continue;
        }

        if (const auto* arc = dynamic_cast<const ArcSegment2d*>(segment.get()))
        {
            segments.push_back(std::make_shared<Geometry::ArcSegment2<double>>(
                arc->center,
                arc->radius,
                arc->startAngle,
                arc->EndAngle(),
                arc->Direction()));
            continue;
        }

        return InternalPolyline();
    }

    return InternalPolyline(std::move(segments), ToInternalClosure(polyline.IsClosed() ? PolylineClosure::Closed
                                                                                         : PolylineClosure::Open));
}
} // namespace

struct Polygon2d::Impl
{
    InternalPolygon polygon;

    Impl() = default;

    explicit Impl(InternalPolygon value)
        : polygon(std::move(value))
    {
    }
};

Polygon2d::Polygon2d() : impl_(std::make_unique<Impl>()) {}

Polygon2d::Polygon2d(Polyline2d outerRing)
    : impl_(std::make_unique<Impl>(InternalPolygon(BuildInternalPolylineFromWrapper(outerRing))))
{
}

Polygon2d::Polygon2d(Polyline2d outerRing, std::vector<Polyline2d> holes)
{
    std::vector<InternalPolyline> internalHoles;
    internalHoles.reserve(holes.size());
    for (const Polyline2d& hole : holes)
    {
        internalHoles.push_back(BuildInternalPolylineFromWrapper(hole));
    }

    impl_ = std::make_unique<Impl>(
        InternalPolygon(BuildInternalPolylineFromWrapper(outerRing), std::move(internalHoles)));
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

double Polygon2d::Perimeter() const
{
    return impl_->polygon.Perimeter();
}

Point2d Polygon2d::Centroid() const
{
    return impl_->polygon.Centroid();
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
} // namespace Geometry
