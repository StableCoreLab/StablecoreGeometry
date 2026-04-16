#pragma once

#include "Brep/BrepBody.h"
#include "Brep/BrepFace.h"
#include "Brep/PolyhedronBody.h"
#include "Brep/PolyhedronFace3d.h"
#include "Core/GeometryTypes.h"
#include "Export/GeometryExport.h"
#include "Geometry2d/ArcSegment2d.h"
#include "Geometry2d/LineSegment2d.h"
#include "Geometry2d/Polygon2d.h"
#include "Geometry2d/Polyline2d.h"
#include "Geometry2d/Segment2d.h"
#include "Geometry3d/Curve3d.h"
#include "Geometry3d/CurveOnSurface.h"
#include "Geometry3d/TriangleMesh.h"

namespace Geometry
{
    enum class PointContainment2d
    {
        Outside,
        OnBoundary,
        Inside
    };

    enum class PointPlaneSide3d
    {
        Below,
        OnPlane,
        Above
    };

    [[nodiscard]] GEOMETRY_API PointContainment2d LocatePoint( const Point2d &point,
                                                               const LineSegment2d &segment,
                                                               double eps = 1e-9 );
    [[nodiscard]] GEOMETRY_API PointContainment2d LocatePoint( const Point2d &point,
                                                               const ArcSegment2d &segment,
                                                               double eps = 1e-9 );
    [[nodiscard]] GEOMETRY_API PointContainment2d LocatePoint( const Point2d &point,
                                                               const Segment2d &segment,
                                                               double eps = 1e-9 );
    [[nodiscard]] GEOMETRY_API PointContainment2d LocatePoint( const Point2d &point,
                                                               const Polyline2d &polyline,
                                                               double eps = 1e-9 );
    [[nodiscard]] GEOMETRY_API PointContainment2d LocatePoint( const Point2d &point,
                                                               const Polygon2d &polygon,
                                                               double eps = 1e-9 );

    [[nodiscard]] GEOMETRY_API PointPlaneSide3d LocatePoint( const Point3d &point, const Plane &plane,
                                                             const GeometryTolerance3d &tolerance = {} );
    [[nodiscard]] GEOMETRY_API PointContainment2d
    LocatePoint( const Point3d &point, const Curve3d &curve, const GeometryTolerance3d &tolerance = {} );
    [[nodiscard]] GEOMETRY_API PointContainment2d
    LocatePoint( const Point3d &point, const CurveOnSurface &curveOnSurface,
                 const GeometryTolerance3d &tolerance = {} );
    [[nodiscard]] GEOMETRY_API PointContainment2d LocatePoint(
        const Point3d &point, const PolyhedronFace3d &face, const GeometryTolerance3d &tolerance = {} );
    [[nodiscard]] GEOMETRY_API PointContainment2d LocatePoint(
        const Point3d &point, const PolyhedronBody &body, const GeometryTolerance3d &tolerance = {} );
    [[nodiscard]] GEOMETRY_API PointContainment2d
    LocatePoint( const Point3d &point, const BrepFace &face, const GeometryTolerance3d &tolerance = {} );
    [[nodiscard]] GEOMETRY_API PointContainment2d
    LocatePoint( const Point3d &point, const BrepBody &body, const GeometryTolerance3d &tolerance = {} );
    [[nodiscard]] GEOMETRY_API PointContainment2d LocatePoint(
        const Point3d &point, const TriangleMesh &mesh, const GeometryTolerance3d &tolerance = {} );

    [[nodiscard]] GEOMETRY_API bool IsParallel( const LineSegment2d &first, const LineSegment2d &second,
                                                double eps = 1e-9 );
    [[nodiscard]] GEOMETRY_API bool IsAntiParallel( const LineSegment2d &first,
                                                    const LineSegment2d &second, double eps = 1e-9 );
    [[nodiscard]] GEOMETRY_API bool IsSameDirection( const LineSegment2d &first,
                                                     const LineSegment2d &second, double eps = 1e-9 );
    [[nodiscard]] GEOMETRY_API bool IsEqual( const LineSegment2d &first, const LineSegment2d &second,
                                             bool ignoreDirection = true, double eps = 1e-9 );

    [[nodiscard]] GEOMETRY_API bool IsParallel( const Vector3d &first, const Vector3d &second,
                                                const GeometryTolerance3d &tolerance = {} );
    [[nodiscard]] GEOMETRY_API bool IsPerpendicular( const Vector3d &first, const Vector3d &second,
                                                     const GeometryTolerance3d &tolerance = {} );
}  // namespace Geometry
