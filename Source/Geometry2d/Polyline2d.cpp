#include "Geometry2d/Polyline2d.h"

#include <memory>
#include <sstream>
#include <utility>

#include "Geometry2d/ArcSegment2d.h"
#include "Geometry2d/LineSegment2d.h"
#include "Types/Geometry2d/ArcSegment2.h"
#include "Types/Geometry2d/LineSegment2.h"
#include "Types/Geometry2d/Polyline2.h"

namespace Geometry
{
namespace
{
using InternalPolyline = Geometry::Polyline2<double>;
using InternalSegment = Geometry::Segment2<double>;
using InternalLineSegment = Geometry::LineSegment2<double>;
using InternalArcSegment = Geometry::ArcSegment2<double>;

[[nodiscard]] Geometry::PolylineClosure2 ToInternalClosure(PolylineClosure closure)
{
    return closure == PolylineClosure::Closed ? Geometry::PolylineClosure2::Closed
                                              : Geometry::PolylineClosure2::Open;
}

[[nodiscard]] InternalPolyline MakeInternalPolyline(
    const std::vector<Point2d>& points,
    PolylineClosure closure)
{
    std::vector<std::shared_ptr<InternalSegment>> segments;
    if (points.size() < 2)
    {
        return InternalPolyline(std::move(segments), ToInternalClosure(closure));
    }

    const std::size_t segmentCount =
        closure == PolylineClosure::Closed ? points.size() : points.size() - 1;
    segments.reserve(segmentCount);
    for (std::size_t i = 0; i < segmentCount; ++i)
    {
        segments.push_back(std::make_shared<InternalLineSegment>(
            points[i],
            points[(i + 1) % points.size()]));
    }

    return InternalPolyline(std::move(segments), ToInternalClosure(closure));
}

[[nodiscard]] InternalPolyline MakeInternalPolyline(
    const std::vector<std::shared_ptr<Segment2d>>& segments,
    PolylineClosure closure)
{
    std::vector<std::shared_ptr<InternalSegment>> internalSegments;
    internalSegments.reserve(segments.size());

    for (const auto& segment : segments)
    {
        if (segment == nullptr)
        {
            return InternalPolyline();
        }

        if (const auto* line = dynamic_cast<const LineSegment2d*>(segment.get()))
        {
            internalSegments.push_back(std::make_shared<InternalLineSegment>(
                line->startPoint,
                line->endPoint));
            continue;
        }

        if (const auto* arc = dynamic_cast<const ArcSegment2d*>(segment.get()))
        {
            internalSegments.push_back(std::make_shared<InternalArcSegment>(
                arc->center,
                arc->radius,
                arc->startAngle,
                arc->EndAngle(),
                arc->Direction()));
            continue;
        }

        return InternalPolyline();
    }

    return InternalPolyline(std::move(internalSegments), ToInternalClosure(closure));
}
} // namespace

struct Polyline2d::Impl
{
    InternalPolyline polyline;

    Impl() = default;

    explicit Impl(PolylineClosure closure)
        : polyline(ToInternalClosure(closure))
    {
    }

    explicit Impl(InternalPolyline value)
        : polyline(std::move(value))
    {
    }
};

Polyline2d::Polyline2d() : impl_(std::make_unique<Impl>()) {}

Polyline2d::Polyline2d(PolylineClosure closure) : impl_(std::make_unique<Impl>(closure)) {}

Polyline2d::Polyline2d(std::vector<Point2d> points, PolylineClosure closure)
    : impl_(std::make_unique<Impl>(MakeInternalPolyline(points, closure)))
{
}

Polyline2d::Polyline2d(std::vector<std::shared_ptr<Segment2d>> segments, PolylineClosure closure)
    : impl_(std::make_unique<Impl>(MakeInternalPolyline(segments, closure)))
{
}

Polyline2d::Polyline2d(std::unique_ptr<Impl> impl) : impl_(std::move(impl)) {}

Polyline2d::Polyline2d(const Polyline2d& other)
    : impl_(std::make_unique<Impl>(other.impl_->polyline))
{
}

Polyline2d::Polyline2d(Polyline2d&& other) noexcept = default;

Polyline2d& Polyline2d::operator=(const Polyline2d& other)
{
    if (this != &other)
    {
        impl_ = std::make_unique<Impl>(other.impl_->polyline);
    }
    return *this;
}

Polyline2d& Polyline2d::operator=(Polyline2d&& other) noexcept = default;

Polyline2d::~Polyline2d() = default;

bool Polyline2d::IsValid() const
{
    return impl_->polyline.IsValid();
}

bool Polyline2d::IsClosed() const
{
    return impl_->polyline.IsClosed();
}

double Polyline2d::Length() const
{
    return impl_->polyline.Length();
}

std::size_t Polyline2d::PointCount() const
{
    return impl_->polyline.VertexCount();
}

std::size_t Polyline2d::VertexCount() const
{
    return PointCount();
}

std::size_t Polyline2d::SegmentCount() const
{
    return impl_->polyline.SegmentCount();
}

Point2d Polyline2d::PointAt(double parameter) const
{
    return impl_->polyline.PointAt(parameter);
}

Point2d Polyline2d::PointAt(std::size_t index) const
{
    return impl_->polyline.VertexAt(index);
}

Point2d Polyline2d::VertexAt(std::size_t index) const
{
    return PointAt(index);
}

Point2d Polyline2d::StartPoint() const
{
    return impl_->polyline.StartPoint();
}

Point2d Polyline2d::EndPoint() const
{
    return impl_->polyline.EndPoint();
}

double Polyline2d::LengthAt(double parameter) const
{
    return impl_->polyline.LengthAt(parameter);
}

double Polyline2d::ParameterAtLength(double distanceFromStart, bool clampToPath) const
{
    return impl_->polyline.ParameterAtLength(distanceFromStart, clampToPath);
}

Point2d Polyline2d::PointAtLength(double distanceFromStart, bool clampToPath) const
{
    return impl_->polyline.PointAtLength(distanceFromStart, clampToPath);
}

std::unique_ptr<Segment2d> Polyline2d::SegmentAt(std::size_t index) const
{
    const auto& segment = impl_->polyline.SegmentAt(index);
    if (const auto* line = dynamic_cast<const InternalLineSegment*>(&segment))
    {
        return std::make_unique<LineSegment2d>(line->StartPoint(), line->EndPoint());
    }

    if (const auto* arc = dynamic_cast<const InternalArcSegment*>(&segment))
    {
        return std::make_unique<ArcSegment2d>(
            arc->Center(),
            static_cast<double>(arc->Radius()),
            static_cast<double>(arc->StartAngle()),
            static_cast<double>(arc->EndAngle()),
            arc->Direction());
    }

    return nullptr;
}

Box2d Polyline2d::Bounds() const
{
    return impl_->polyline.Bounds();
}

std::string Polyline2d::DebugString() const
{
    std::ostringstream stream;
    stream << "Polyline2d{closure=" << (IsClosed() ? "Closed" : "Open")
           << ", pointCount=" << PointCount()
           << ", segmentCount=" << SegmentCount()
           << ", valid=" << (IsValid() ? "true" : "false")
           << ", bounds=" << Bounds().DebugString() << "}";
    return stream.str();
}
} // namespace Geometry
