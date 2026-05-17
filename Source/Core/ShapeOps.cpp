#include "Core/ShapeOps.h"

#include <algorithm>
#include <cmath>
#include <memory>
#include <utility>
#include <vector>

#include "RingIntegral2d.h"
#include "Core/Algorithms.h"
#include "Support/Epsilon.h"

namespace Geometry
{
    namespace
    {
        [[nodiscard]] std::shared_ptr<Segment2d> CloneSharedSegment( const Segment2d &segment )
        {
            return std::shared_ptr<Segment2d>( segment.Clone().release() );
        }

        [[nodiscard]] double SignedArea( const Polyline2d &ring )
        {
            return Detail::ComputeSignedArea( ring );
        }

        [[nodiscard]] Point2d RingCentroid( const Polyline2d &ring )
        {
            const Detail::RingMoment2d moment = Detail::ComputeRingMoment( ring );
            if( std::abs( moment.signedArea ) <= Geometry::kShapeOpsDefaultEpsilon )
            {
                return Point2d{};
            }

            return Point2d{ moment.firstMomentX / moment.signedArea,
                            moment.firstMomentY / moment.signedArea };
        }

    }  // namespace

    double Perimeter( const Polygon2d &polygon ) { return polygon.Perimeter(); }

    RingOrientation2d Orientation( const Polyline2d &ring )
    {
        const double signedArea = SignedArea( ring );
        if( std::abs( signedArea ) <= Geometry::kShapeOpsDefaultEpsilon )
        {
            return RingOrientation2d::Unknown;
        }

        return signedArea > 0.0 ? RingOrientation2d::CounterClockwise : RingOrientation2d::Clockwise;
    }

    bool IsClockwise( const Polyline2d &ring )
    {
        return Orientation( ring ) == RingOrientation2d::Clockwise;
    }

    bool IsCounterClockwise( const Polyline2d &ring )
    {
        return Orientation( ring ) == RingOrientation2d::CounterClockwise;
    }

    Point2d Centroid( const Polygon2d &polygon ) { return polygon.Centroid(); }

    LineSegment2d Reverse( const LineSegment2d &segment )
    {
        return LineSegment2d( segment.endPoint, segment.startPoint );
    }

    ArcSegment2d Reverse( const ArcSegment2d &segment )
    {
        return ArcSegment2d( segment.center, segment.radius, segment.EndAngle(), -segment.sweepAngle );
    }

    Polyline2d Reverse( const Polyline2d &polyline )
    {
        std::vector<std::shared_ptr<Segment2d>> segments;
        segments.reserve( polyline.SegmentCount() );
        for( std::size_t i = polyline.SegmentCount(); i > 0; --i )
        {
            std::unique_ptr<Segment2d> segment = polyline.SegmentAt( i - 1 );
            if( segment == nullptr )
            {
                segments.push_back( nullptr );
                continue;
            }

            switch( segment->Kind() )
            {
            case SegmentKind2::Line:
                segments.push_back( std::make_shared<LineSegment2d>(
                    Reverse( static_cast<const LineSegment2d &>( *segment ) ) ) );
                break;
            case SegmentKind2::Arc:
                segments.push_back( std::make_shared<ArcSegment2d>(
                    Reverse( static_cast<const ArcSegment2d &>( *segment ) ) ) );
                break;
            default:
                segments.push_back( std::shared_ptr<Segment2d>( std::move( segment ) ) );
                break;
            }
        }

        return Polyline2d( std::move( segments ),
                           polyline.IsClosed() ? PolylineClosure::Closed : PolylineClosure::Open );
    }

    Polyline2d Close( const Polyline2d &polyline )
    {
        if( polyline.IsClosed() )
        {
            return polyline;
        }

        if( polyline.SegmentCount() == 0 )
        {
            return Polyline2d( PolylineClosure::Closed );
        }

        std::vector<std::shared_ptr<Segment2d>> segments;
        segments.reserve( polyline.SegmentCount() + 1 );
        for( std::size_t i = 0; i < polyline.SegmentCount(); ++i )
        {
            std::unique_ptr<Segment2d> segment = polyline.SegmentAt( i );
            if( segment == nullptr )
            {
                continue;
            }
            segments.push_back( CloneSharedSegment( *segment ) );
        }

        if( segments.empty() )
        {
            return Polyline2d( PolylineClosure::Closed );
        }

        if( !segments.back()->EndPoint().AlmostEquals( segments.front()->StartPoint() ) )
        {
            segments.push_back(
                std::make_shared<LineSegment2d>( segments.back()->EndPoint(), segments.front()->StartPoint() ) );
        }

        return Polyline2d( std::move( segments ), PolylineClosure::Closed );
    }
}  // namespace Geometry
