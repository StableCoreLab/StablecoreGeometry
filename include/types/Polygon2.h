#pragma once

#include <cassert>
#include <cmath>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <vector>

#include "algorithm/Predicate2.h"
#include "types/Box2.h"
#include "types/Polyline2.h"

namespace geometry
{
template <typename T>
using PolygonMeasureType = std::conditional_t<std::is_floating_point_v<T>, T, double>;

template <typename T>
class Polygon2
{
public:
    using ValueType = T;
    using PolylineType = Polyline2<T>;
    using PointType = Point2<T>;
    using MeasureType = PolygonMeasureType<T>;

    Polygon2() = default;

    explicit Polygon2(PolylineType outerRing)
        : outerRing_(std::move(outerRing))
    {
    }

    Polygon2(PolylineType outerRing, std::vector<PolylineType> holes)
        : outerRing_(std::move(outerRing)), holes_(std::move(holes))
    {
    }

    [[nodiscard]] const PolylineType& GetOuterRing() const
    {
        return outerRing_;
    }

    [[nodiscard]] std::size_t GetHoleCount() const
    {
        return holes_.size();
    }

    [[nodiscard]] const PolylineType& GetHole(std::size_t index) const
    {
        assert(index < holes_.size());
        return holes_[index];
    }

    [[nodiscard]] MeasureType Area() const
    {
        if (!IsValid())
        {
            return MeasureType{};
        }

        const MeasureType signedArea = RingSignedArea(outerRing_);
        MeasureType total = signedArea;
        for (const auto& hole : holes_)
        {
            total += RingSignedArea(hole);
        }

        return std::abs(total);
    }

    [[nodiscard]] MeasureType Perimeter() const
    {
        if (!IsValid())
        {
            return MeasureType{};
        }

        MeasureType total = static_cast<MeasureType>(outerRing_.Length());
        for (const auto& hole : holes_)
        {
            total += static_cast<MeasureType>(hole.Length());
        }

        return total;
    }

    [[nodiscard]] Point2<MeasureType> Centroid() const
    {
        if (!IsValid())
        {
            return Point2<MeasureType>{};
        }

        const auto outer = RingCentroidAndArea(outerRing_);
        MeasureType weightedX = outer.first.x * outer.second;
        MeasureType weightedY = outer.first.y * outer.second;
        MeasureType signedArea = outer.second;

        for (const auto& hole : holes_)
        {
            const auto ring = RingCentroidAndArea(hole);
            weightedX += ring.first.x * ring.second;
            weightedY += ring.first.y * ring.second;
            signedArea += ring.second;
        }

        if (IsZero(signedArea))
        {
            return Point2<MeasureType>{};
        }

        return Point2<MeasureType>(weightedX / signedArea, weightedY / signedArea);
    }

    [[nodiscard]] Box2<T> GetBoundingBox() const
    {
        if (!IsValid())
        {
            return Box2<T>();
        }

        Box2<T> box = outerRing_.GetBoundingBox();
        for (const auto& hole : holes_)
        {
            box.ExpandToInclude(hole.GetBoundingBox());
        }

        return box;
    }

    [[nodiscard]] bool IsValid() const
    {
        if (!outerRing_.IsValid() || !outerRing_.IsClosed())
        {
            return false;
        }

        const MeasureType outerArea = RingSignedArea(outerRing_);
        if (!(outerArea > MeasureType{}))
        {
            return false;
        }

        for (const auto& hole : holes_)
        {
            if (!hole.IsValid() || !hole.IsClosed())
            {
                return false;
            }

            const MeasureType holeArea = RingSignedArea(hole);
            if (!(holeArea < MeasureType{}))
            {
                return false;
            }
        }

        return true;
    }

private:
    [[nodiscard]] static MeasureType RingSignedArea(const PolylineType& ring)
    {
        const std::size_t vertexCount = ring.GetVertexCount();
        if (vertexCount < 3)
        {
            return MeasureType{};
        }

        MeasureType twiceArea{};
        for (std::size_t i = 0; i < vertexCount; ++i)
        {
            const PointType current = ring.GetVertex(i);
            const PointType next = ring.GetVertex((i + 1) % vertexCount);
            twiceArea += static_cast<MeasureType>(current.x) * static_cast<MeasureType>(next.y) -
                         static_cast<MeasureType>(next.x) * static_cast<MeasureType>(current.y);
        }

        return twiceArea / static_cast<MeasureType>(2);
    }

    [[nodiscard]] static std::pair<Point2<MeasureType>, MeasureType> RingCentroidAndArea(
        const PolylineType& ring)
    {
        const std::size_t vertexCount = ring.GetVertexCount();
        if (vertexCount < 3)
        {
            return {Point2<MeasureType>{}, MeasureType{}};
        }

        MeasureType twiceArea{};
        MeasureType cx{};
        MeasureType cy{};
        for (std::size_t i = 0; i < vertexCount; ++i)
        {
            const PointType current = ring.GetVertex(i);
            const PointType next = ring.GetVertex((i + 1) % vertexCount);
            const MeasureType cross = static_cast<MeasureType>(current.x) * static_cast<MeasureType>(next.y) -
                                      static_cast<MeasureType>(next.x) * static_cast<MeasureType>(current.y);
            twiceArea += cross;
            cx += (static_cast<MeasureType>(current.x) + static_cast<MeasureType>(next.x)) * cross;
            cy += (static_cast<MeasureType>(current.y) + static_cast<MeasureType>(next.y)) * cross;
        }

        const MeasureType area = twiceArea / static_cast<MeasureType>(2);
        if (IsZero(area))
        {
            return {Point2<MeasureType>{}, MeasureType{}};
        }

        const MeasureType factor = static_cast<MeasureType>(1) / (static_cast<MeasureType>(3) * twiceArea);
        return {Point2<MeasureType>(cx * factor, cy * factor), area};
    }

    PolylineType outerRing_{};
    std::vector<PolylineType> holes_{};
};

using Polygon2d = Polygon2<double>;
using Polygon2i = Polygon2<int>;
}
