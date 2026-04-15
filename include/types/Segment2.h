#pragma once

#include "algorithm/Predicate2.h"
#include "types/Box2.h"
#include "types/Point2.h"
#include "types/Vector2.h"

namespace geometry
{
enum class SegmentKind2
{
    Line,
    Arc
};

enum class ArcDirection
{
    Clockwise,
    CounterClockwise
};

} // namespace geometry

#include "types/Segment2Detail.h"

namespace geometry
{
template <typename T>
class Segment2
{
public:
    using PointType = Point2<T>;
    using VectorType = Vector2<T>;
    using LengthType = typename VectorType::LengthType;

    virtual ~Segment2() = default;

    [[nodiscard]] virtual SegmentKind2 Kind() const = 0;
    [[nodiscard]] virtual PointType StartPoint() const = 0;
    [[nodiscard]] virtual PointType EndPoint() const = 0;
    [[nodiscard]] virtual LengthType Length() const = 0;
    [[nodiscard]] virtual Box2<T> Bounds() const = 0;
    [[nodiscard]] virtual PointType PointAt(double parameter) const = 0;
    [[nodiscard]] virtual PointType PointAtLength(LengthType distanceFromStart, bool clampToSegment = false) const = 0;
    [[nodiscard]] virtual bool IsValid() const = 0;
};

using Segment2d = Segment2<double>;
using Segment2i = Segment2<int>;
}
