#include "sdk/ISCPolyline2d.h"

#include <memory>
#include <sstream>
#include <utility>

#include "types/LineSegment2.h"
#include "types/Polyline2.h"

namespace Geometry::Sdk
{
namespace
{
[[nodiscard]] Geometry::PolylineClosure ToInternalClosure(ISCPolylineClosure closure)
{
    return closure == ISCPolylineClosure::Closed ? Geometry::PolylineClosure::Closed
                                                 : Geometry::PolylineClosure::Open;
}

[[nodiscard]] Geometry::Polyline2d MakeInternalPolyline(
    const std::vector<SCPoint2d>& points,
    ISCPolylineClosure closure)
{
    std::vector<std::shared_ptr<Geometry::Segment2d>> segments;
    if (points.size() < 2)
    {
        return Geometry::Polyline2d(std::move(segments), ToInternalClosure(closure));
    }

    const std::size_t segmentCount =
        closure == ISCPolylineClosure::Closed ? points.size() : points.size() - 1;
    segments.reserve(segmentCount);
    for (std::size_t i = 0; i < segmentCount; ++i)
    {
        const SCPoint2d& start = points[i];
        const SCPoint2d& end = points[(i + 1) % points.size()];
        segments.push_back(std::make_shared<Geometry::LineSegment2d>(start, end));
    }

    return Geometry::Polyline2d(std::move(segments), ToInternalClosure(closure));
}
} // namespace

struct ISCPolyline2d::SCImpl
{
    Geometry::Polyline2d polyline;

    SCImpl() = default;

    explicit SCImpl(ISCPolylineClosure closure)
        : polyline(ToInternalClosure(closure))
    {
    }

    explicit SCImpl(Geometry::Polyline2d value)
        : polyline(std::move(value))
    {
    }
};

ISCPolyline2d::ISCPolyline2d() : impl_(std::make_unique<SCImpl>()) {}

ISCPolyline2d::ISCPolyline2d(ISCPolylineClosure closure) : impl_(std::make_unique<SCImpl>(closure)) {}

ISCPolyline2d::ISCPolyline2d(std::vector<SCPoint2d> points, ISCPolylineClosure closure)
    : impl_(std::make_unique<SCImpl>(MakeInternalPolyline(points, closure)))
{
}

ISCPolyline2d::ISCPolyline2d(std::unique_ptr<SCImpl> impl) : impl_(std::move(impl)) {}

ISCPolyline2d::ISCPolyline2d(const ISCPolyline2d& other)
    : impl_(std::make_unique<SCImpl>(other.impl_->polyline))
{
}

ISCPolyline2d::ISCPolyline2d(ISCPolyline2d&& other) noexcept = default;

ISCPolyline2d& ISCPolyline2d::operator=(const ISCPolyline2d& other)
{
    if (this != &other)
    {
        impl_ = std::make_unique<SCImpl>(other.impl_->polyline);
    }
    return *this;
}

ISCPolyline2d& ISCPolyline2d::operator=(ISCPolyline2d&& other) noexcept = default;

ISCPolyline2d::~ISCPolyline2d() = default;

bool ISCPolyline2d::IsValid() const
{
    return impl_->polyline.IsValid();
}

bool ISCPolyline2d::IsClosed() const
{
    return impl_->polyline.IsClosed();
}

double ISCPolyline2d::Length() const
{
    return impl_->polyline.Length();
}

std::size_t ISCPolyline2d::PointCount() const
{
    return impl_->polyline.VertexCount();
}

std::size_t ISCPolyline2d::SegmentCount() const
{
    return impl_->polyline.SegmentCount();
}

SCPoint2d ISCPolyline2d::PointAt(std::size_t index) const
{
    return impl_->polyline.VertexAt(index);
}

SCBox2d ISCPolyline2d::Bounds() const
{
    return impl_->polyline.Bounds();
}

std::string ISCPolyline2d::DebugString() const
{
    std::ostringstream stream;
    stream << "ISCPolyline2d{closure=" << (IsClosed() ? "Closed" : "Open")
           << ", pointCount=" << PointCount()
           << ", segmentCount=" << SegmentCount()
           << ", valid=" << (IsValid() ? "true" : "false")
           << ", bounds=" << Bounds().DebugString() << "}";
    return stream.str();
}
} // namespace Geometry::Sdk
