#include "Core/Relation.h"

#include <cmath>
#include <memory>
#include <vector>

#include "Core/Intersection.h"
#include "Core/Projection.h"
#include "Core/ShapeOps.h"
#include "Support/Epsilon.h"
#include "Support/Geometry2d/Predicate2.h"

namespace Geometry
{
    namespace
    {
        [[nodiscard]] bool IsPointOnLineSegment( const Point2d &point, const LineSegment2d &segment,
                                                 double eps )
        {
            const auto projection = ProjectPointToLineSegment( point, segment, true );
            return projection.distanceSquared <= eps * eps;
        }

        [[nodiscard]] bool IsPointOnArcSegment( const Point2d &point, const ArcSegment2d &segment,
                                                double eps )
        {
            const auto projection = ProjectPointToArcSegment( point, segment, true );
            return projection.distanceSquared <= eps * eps;
        }

        [[nodiscard]] std::size_t CountLineSegmentRayCrossings( const LineSegment2d &segment,
                                                                const Point2d &point, double eps )
        {
            const double startY = segment.startPoint.y;
            const double endY = segment.endPoint.y;
            const bool straddles = ( startY <= point.y && endY > point.y ) ||
                                   ( endY <= point.y && startY > point.y );
            if( !straddles )
            {
                return 0;
            }

            const double x =
                segment.startPoint.x +
                ( segment.endPoint.x - segment.startPoint.x ) * ( point.y - startY ) / ( endY - startY );
            return x > point.x + eps ? 1U : 0U;
        }

        [[nodiscard]] std::size_t CountArcSegmentRayCrossings( const ArcSegment2d &segment,
                                                               const Point2d &point, double eps )
        {
            const double dy = point.y - segment.center.y;
            if( std::abs( dy ) >= segment.radius - eps )
            {
                return 0;
            }

            const double radialSquared = segment.radius * segment.radius - dy * dy;
            if( radialSquared <= 0.0 )
            {
                return 0;
            }

            const double dx = std::sqrt( radialSquared );
            const double candidateX[2] = { segment.center.x - dx, segment.center.x + dx };
            std::size_t count = 0;
            for( const double x : candidateX )
            {
                if( x <= point.x + eps )
                {
                    continue;
                }

                const Point2d candidate{ x, point.y };
                const SegmentProjection2d projection = ProjectPointToArcSegment( candidate, segment, false );
                if( projection.distanceSquared > eps * eps || !projection.isOnSegment )
                {
                    continue;
                }

                if( projection.parameter < -eps || projection.parameter >= 1.0 - eps )
                {
                    continue;
                }

                ++count;
            }

            return count;
        }

        [[nodiscard]] std::size_t CountSegmentRayCrossings( const Segment2d &segment,
                                                            const Point2d &point, double eps )
        {
            if( segment.Kind() == SegmentKind2::Line )
            {
                return CountLineSegmentRayCrossings( static_cast<const LineSegment2d &>( segment ), point,
                                                     eps );
            }

            return CountArcSegmentRayCrossings( static_cast<const ArcSegment2d &>( segment ), point, eps );
        }

        [[nodiscard]] PointContainment2d LocatePointInRing( const Point2d &point, const Polyline2d &ring,
                                                            double eps )
        {
            if( !ring.IsClosed() || !ring.IsValid() || ring.SegmentCount() == 0 )
            {
                return PointContainment2d::Outside;
            }

            std::size_t crossingCount = 0;
            for( std::size_t i = 0; i < ring.SegmentCount(); ++i )
            {
                std::unique_ptr<Segment2d> segment = ring.SegmentAt( i );
                if( segment == nullptr )
                {
                    continue;
                }

                if( LocatePoint( point, *segment, eps ) == PointContainment2d::OnBoundary )
                {
                    return PointContainment2d::OnBoundary;
                }

                crossingCount += CountSegmentRayCrossings( *segment, point, eps );
            }

            return ( crossingCount % 2U ) == 1U ? PointContainment2d::Inside
                                                : PointContainment2d::Outside;
        }
    }  // namespace

    PointContainment2d LocatePoint( const Point2d &point, const LineSegment2d &segment, double eps )
    {
        if( !segment.IsValid() )
        {
            return PointContainment2d::Outside;
        }

        if( IsPointOnLineSegment( point, segment, eps ) )
        {
            return PointContainment2d::OnBoundary;
        }

        return PointContainment2d::Outside;
    }

    PointContainment2d LocatePoint( const Point2d &point, const ArcSegment2d &segment, double eps )
    {
        if( !segment.IsValid() )
        {
            return PointContainment2d::Outside;
        }

        if( IsPointOnArcSegment( point, segment, eps ) )
        {
            return PointContainment2d::OnBoundary;
        }

        return PointContainment2d::Outside;
    }

    PointContainment2d LocatePoint( const Point2d &point, const Segment2d &segment, double eps )
    {
        if( segment.Kind() == Geometry::SegmentKind2::Line )
        {
            return LocatePoint( point, static_cast<const LineSegment2d &>( segment ), eps );
        }
        if( segment.Kind() == Geometry::SegmentKind2::Arc )
        {
            return LocatePoint( point, static_cast<const ArcSegment2d &>( segment ), eps );
        }
        return PointContainment2d::Outside;
    }

    PointContainment2d LocatePoint( const Point2d &point, const Polyline2d &polyline, double eps )
    {
        if( !polyline.IsValid() )
        {
            return PointContainment2d::Outside;
        }

        if( !polyline.IsClosed() )
        {
            for( std::size_t i = 0; i < polyline.SegmentCount(); ++i )
            {
                std::unique_ptr<Segment2d> segment = polyline.SegmentAt( i );
                if( segment != nullptr && LocatePoint( point, *segment, eps ) == PointContainment2d::OnBoundary )
                {
                    return PointContainment2d::OnBoundary;
                }
            }
            return PointContainment2d::Outside;
        }

        return LocatePointInRing( point, polyline, eps );
    }

    PointContainment2d LocatePoint( const Point2d &point, const Polygon2d &polygon, double eps )
    {
        if( !polygon.IsValid() )
        {
            return PointContainment2d::Outside;
        }

        const PointContainment2d outer = LocatePointInRing( point, polygon.OuterRing(), eps );
        if( outer == PointContainment2d::Outside )
        {
            return PointContainment2d::Outside;
        }
        if( outer == PointContainment2d::OnBoundary )
        {
            return PointContainment2d::OnBoundary;
        }

        for( std::size_t i = 0; i < polygon.HoleCount(); ++i )
        {
            const PointContainment2d hole = LocatePointInRing( point, polygon.HoleAt( i ), eps );
            if( hole == PointContainment2d::OnBoundary )
            {
                return PointContainment2d::OnBoundary;
            }
            if( hole == PointContainment2d::Inside )
            {
                return PointContainment2d::Outside;
            }
        }

        return PointContainment2d::Inside;
    }

    bool IsParallel( const LineSegment2d &first, const LineSegment2d &second, double eps )
    {
        if( !first.IsValid() || !second.IsValid() )
        {
            return false;
        }

        return std::abs( Cross( first.endPoint - first.startPoint,
                                second.endPoint - second.startPoint ) ) <= eps;
    }

    bool IsAntiParallel( const LineSegment2d &first, const LineSegment2d &second, double eps )
    {
        if( !IsParallel( first, second, eps ) )
        {
            return false;
        }

        return Dot( first.endPoint - first.startPoint, second.endPoint - second.startPoint ) < 0.0;
    }

    bool IsSameDirection( const LineSegment2d &first, const LineSegment2d &second, double eps )
    {
        if( !IsParallel( first, second, eps ) )
        {
            return false;
        }

        return Dot( first.endPoint - first.startPoint, second.endPoint - second.startPoint ) > 0.0;
    }

    bool IsEqual( const LineSegment2d &first, const LineSegment2d &second, bool ignoreDirection,
                  double eps )
    {
        if( first.startPoint.AlmostEquals( second.startPoint, eps ) &&
            first.endPoint.AlmostEquals( second.endPoint, eps ) )
        {
            return true;
        }

        return ignoreDirection && first.startPoint.AlmostEquals( second.endPoint, eps ) &&
               first.endPoint.AlmostEquals( second.startPoint, eps );
    }
}  // namespace Geometry
