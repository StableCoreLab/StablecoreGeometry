#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "Export/GeometryExport.h"
#include "Core/Intersection.h"
#include "Core/Metrics.h"
#include "Core/Projection.h"

namespace Geometry::Sdk
{
struct GEOMETRY_API SegmentSearchEntry2d
{
    std::size_t id{0};
    std::shared_ptr<const Segment2d> segment{};
    Box2d box{};

    [[nodiscard]] bool IsValid() const
    {
        return segment != nullptr && segment->IsValid() && box.IsValid();
    }
};

struct GEOMETRY_API SegmentSearchHit2d
{
    std::size_t id{0};
    Point2d point{};
    double distanceSquared{0.0};
    double parameter{0.0};
    bool isOnSegment{false};

    [[nodiscard]] bool IsValid() const
    {
        return point.IsValid() && distanceSquared >= 0.0;
    }
};

class GEOMETRY_API SegmentSearch2d
{
public:
    SegmentSearch2d() = default;
    explicit SegmentSearch2d(std::vector<SegmentSearchEntry2d> entries);

    void Clear();
    std::size_t Add(std::shared_ptr<const Segment2d> segment);
    std::size_t Add(const Segment2d& segment);
    std::size_t Add(const LineSegment2d& segment);
    std::size_t Add(const ArcSegment2d& segment);
    bool Remove(std::size_t id);

    [[nodiscard]] std::size_t Size() const;
    [[nodiscard]] bool IsEmpty() const;
    [[nodiscard]] bool IsValid() const;
    [[nodiscard]] bool Contains(std::size_t id) const;
    [[nodiscard]] const SegmentSearchEntry2d* Find(std::size_t id) const;
    [[nodiscard]] std::vector<std::size_t> QueryIntersecting(const Box2d& box, double eps = 1e-9) const;
    [[nodiscard]] std::vector<std::size_t> QueryIntersecting(const Segment2d& segment, double eps = 1e-9) const;
    [[nodiscard]] std::vector<SegmentSearchHit2d> QueryWithinDistance(
        const Point2d& point,
        double maxDistance) const;
    [[nodiscard]] std::optional<SegmentSearchHit2d> Nearest(const Point2d& point) const;
    [[nodiscard]] std::string DebugString() const;

    [[nodiscard]] const std::vector<SegmentSearchEntry2d>& Entries() const;
    [[nodiscard]] std::vector<SegmentSearchEntry2d>& Entries();
    [[nodiscard]] const std::vector<SegmentSearchEntry2d>& Data() const;
    [[nodiscard]] std::vector<SegmentSearchEntry2d>& Data();

private:
    std::size_t nextId_{0};
    std::vector<SegmentSearchEntry2d> entries_{};
};

} // namespace Geometry::Sdk
