#pragma once

#include "export/GeometryExport.h"
#include "sdk/ArcSegment2d.h"
#include "sdk/LineSegment2d.h"
#include "sdk/MultiPolygon2d.h"
#include "sdk/MultiPolyline2d.h"
#include "sdk/Polygon2d.h"
#include "sdk/Polyline2d.h"

namespace geometry::sdk
{
[[nodiscard]] GEOMETRY_API Point2d Translate(const Point2d& point, const Vector2d& offset);
[[nodiscard]] GEOMETRY_API LineSegment2d Translate(const LineSegment2d& segment, const Vector2d& offset);
[[nodiscard]] GEOMETRY_API ArcSegment2d Translate(const ArcSegment2d& segment, const Vector2d& offset);
[[nodiscard]] GEOMETRY_API Polyline2d Translate(const Polyline2d& polyline, const Vector2d& offset);
[[nodiscard]] GEOMETRY_API Polygon2d Translate(const Polygon2d& polygon, const Vector2d& offset);
[[nodiscard]] GEOMETRY_API MultiPolyline2d Translate(const MultiPolyline2d& polylines, const Vector2d& offset);
[[nodiscard]] GEOMETRY_API MultiPolygon2d Translate(const MultiPolygon2d& polygons, const Vector2d& offset);

[[nodiscard]] GEOMETRY_API Point2d Rotate(const Point2d& point, const Point2d& origin, double angleRadians);
[[nodiscard]] GEOMETRY_API LineSegment2d Rotate(
    const LineSegment2d& segment,
    const Point2d& origin,
    double angleRadians);
[[nodiscard]] GEOMETRY_API ArcSegment2d Rotate(
    const ArcSegment2d& segment,
    const Point2d& origin,
    double angleRadians);
[[nodiscard]] GEOMETRY_API Polyline2d Rotate(
    const Polyline2d& polyline,
    const Point2d& origin,
    double angleRadians);
[[nodiscard]] GEOMETRY_API Polygon2d Rotate(
    const Polygon2d& polygon,
    const Point2d& origin,
    double angleRadians);

[[nodiscard]] GEOMETRY_API Point2d Mirror(const Point2d& point, const Point2d& linePoint, const Vector2d& lineDir);
[[nodiscard]] GEOMETRY_API LineSegment2d Mirror(
    const LineSegment2d& segment,
    const Point2d& linePoint,
    const Vector2d& lineDir);
[[nodiscard]] GEOMETRY_API ArcSegment2d Mirror(
    const ArcSegment2d& segment,
    const Point2d& linePoint,
    const Vector2d& lineDir);
[[nodiscard]] GEOMETRY_API Polyline2d Mirror(
    const Polyline2d& polyline,
    const Point2d& linePoint,
    const Vector2d& lineDir);
[[nodiscard]] GEOMETRY_API Polygon2d Mirror(
    const Polygon2d& polygon,
    const Point2d& linePoint,
    const Vector2d& lineDir);

[[nodiscard]] GEOMETRY_API Point2d Stretch(
    const Point2d& point,
    const Box2d& region,
    const Vector2d& offset);
[[nodiscard]] GEOMETRY_API LineSegment2d Stretch(
    const LineSegment2d& segment,
    const Box2d& region,
    const Vector2d& offset);
[[nodiscard]] GEOMETRY_API ArcSegment2d Stretch(
    const ArcSegment2d& segment,
    const Box2d& region,
    const Vector2d& offset);
[[nodiscard]] GEOMETRY_API Polyline2d Stretch(
    const Polyline2d& polyline,
    const Box2d& region,
    const Vector2d& offset);
[[nodiscard]] GEOMETRY_API Polygon2d Stretch(
    const Polygon2d& polygon,
    const Box2d& region,
    const Vector2d& offset);
} // namespace geometry::sdk
