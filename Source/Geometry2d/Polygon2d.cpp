#include "Geometry2d/Polygon2d.h"

#include <cmath>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "Geometry2d/ArcSegment2d.h"
#include "Support/Geometry2d/Predicate2.h"

namespace Geometry
{
    namespace
    {
        struct RingMoment
        {
            double signedArea{ 0.0 };
            double firstMomentX{ 0.0 };
            double firstMomentY{ 0.0 };
        };

        [[nodiscard]] RingMoment RingMomentIntegral( const Polyline2d &ring );

        [[nodiscard]] double RingSignedArea( const Polyline2d &ring )
        {
            return RingMomentIntegral( ring ).signedArea;
        }

        [[nodiscard]] std::pair<Point2d, double> RingCentroidAndArea( const Polyline2d &ring )
        {
            const RingMoment moment = RingMomentIntegral( ring );
            if( IsZero( moment.signedArea ) )
            {
                return { Point2d{}, 0.0 };
            }

            return { Point2d( moment.firstMomentX / moment.signedArea,
                              moment.firstMomentY / moment.signedArea ),
                     moment.signedArea };
        }

        void AccumulateLineSegmentMoment( const Point2d &startPoint, const Point2d &endPoint,
                                          RingMoment &total )
        {
            const double cross = startPoint.x * endPoint.y - endPoint.x * startPoint.y;

            total.signedArea += cross / 2.0;
            total.firstMomentX +=
                ( endPoint.y - startPoint.y ) *
                ( startPoint.x * startPoint.x + startPoint.x * endPoint.x +
                  endPoint.x * endPoint.x ) /
                6.0;
            total.firstMomentY +=
                ( startPoint.x - endPoint.x ) *
                ( startPoint.y * startPoint.y + startPoint.y * endPoint.y +
                  endPoint.y * endPoint.y ) /
                6.0;
        }

        [[nodiscard]] double ArcFirstMomentXPrimitive( double centerX, double radius, double angle )
        {
            const double sinAngle = std::sin( angle );
            const double cosAngle = std::cos( angle );
            return ( centerX * centerX * radius * sinAngle +
                     centerX * radius * radius * ( angle + sinAngle * cosAngle ) +
                     radius * radius * radius * ( sinAngle - sinAngle * sinAngle * sinAngle / 3.0 ) ) /
                   2.0;
        }

        [[nodiscard]] double ArcFirstMomentYPrimitive( double centerY, double radius, double angle )
        {
            const double sinAngle = std::sin( angle );
            const double cosAngle = std::cos( angle );
            return ( -centerY * centerY * radius * cosAngle +
                     centerY * radius * radius * ( angle - sinAngle * cosAngle ) +
                     radius * radius * radius * ( -cosAngle + cosAngle * cosAngle * cosAngle / 3.0 ) ) /
                   2.0;
        }

        void AccumulateArcSegmentMoment( const ArcSegment2d &arc, RingMoment &total )
        {
            const double cx = arc.center.x;
            const double cy = arc.center.y;
            const double radius = arc.radius;
            const double startAngle = arc.startAngle;
            const double sweep = arc.sweepAngle;
            const double endAngle = startAngle + sweep;

            const double sinStart = std::sin( startAngle );
            const double cosStart = std::cos( startAngle );
            const double sinEnd = std::sin( endAngle );
            const double cosEnd = std::cos( endAngle );

            total.signedArea +=
                ( cx * radius * ( sinEnd - sinStart ) - cy * radius * ( cosEnd - cosStart ) +
                  radius * radius * sweep ) /
                2.0;

            total.firstMomentX +=
                ArcFirstMomentXPrimitive( cx, radius, endAngle ) -
                ArcFirstMomentXPrimitive( cx, radius, startAngle );
            total.firstMomentY +=
                ArcFirstMomentYPrimitive( cy, radius, endAngle ) -
                ArcFirstMomentYPrimitive( cy, radius, startAngle );
        }

        [[nodiscard]] RingMoment RingMomentIntegral( const Polyline2d &ring )
        {
            RingMoment total{};
            for( std::size_t i = 0; i < ring.SegmentCount(); ++i )
            {
                std::unique_ptr<Segment2d> segment = ring.SegmentAt( i );
                if( segment == nullptr )
                {
                    continue;
                }

                switch( segment->Kind() )
                {
                case SegmentKind2::Line:
                    AccumulateLineSegmentMoment( segment->StartPoint(), segment->EndPoint(), total );
                    break;
                case SegmentKind2::Arc:
                {
                    const auto *arc = dynamic_cast<const ArcSegment2d *>( segment.get() );
                    if( arc != nullptr )
                    {
                        AccumulateArcSegmentMoment( *arc, total );
                    }
                    break;
                }
                default:
                    break;
                }
            }

            return total;
        }
    }  // namespace

    struct Polygon2d::Impl
    {
        Polyline2d outerRing{};
        std::vector<Polyline2d> holes{};

        Impl() = default;

        explicit Impl( Polyline2d outerRing ) : outerRing( std::move( outerRing ) ) {}

        Impl( Polyline2d outerRing, std::vector<Polyline2d> holes ) :
            outerRing( std::move( outerRing ) ),
            holes( std::move( holes ) )
        {
        }
    };

    Polygon2d::Polygon2d() : impl_( std::make_unique<Impl>() ) {}

    Polygon2d::Polygon2d( Polyline2d outerRing ) :
        impl_( std::make_unique<Impl>( std::move( outerRing ) ) )
    {
    }

    Polygon2d::Polygon2d( Polyline2d outerRing, std::vector<Polyline2d> holes ) :
        impl_( std::make_unique<Impl>( std::move( outerRing ), std::move( holes ) ) )
    {
    }

    Polygon2d::Polygon2d( const Polygon2d &other ) :
        impl_( std::make_unique<Impl>( other.impl_->outerRing, other.impl_->holes ) )
    {
    }

    Polygon2d::Polygon2d( Polygon2d &&other ) noexcept = default;

    Polygon2d &Polygon2d::operator=( const Polygon2d &other )
    {
        if( this != &other )
        {
            impl_ = std::make_unique<Impl>( other.impl_->outerRing, other.impl_->holes );
        }
        return *this;
    }

    Polygon2d &Polygon2d::operator=( Polygon2d &&other ) noexcept = default;

    Polygon2d::~Polygon2d() = default;

    bool Polygon2d::IsValid() const
    {
        if( !impl_->outerRing.IsValid() || !impl_->outerRing.IsClosed() )
        {
            return false;
        }

        const double outerArea = RingSignedArea( impl_->outerRing );
        if( !( outerArea > 0.0 ) )
        {
            return false;
        }

        for( const auto &hole : impl_->holes )
        {
            if( !hole.IsValid() || !hole.IsClosed() )
            {
                return false;
            }

            const double holeArea = RingSignedArea( hole );
            if( !( holeArea < 0.0 ) )
            {
                return false;
            }
        }

        return true;
    }

    std::size_t Polygon2d::PointCount() const
    {
        std::size_t count = impl_->outerRing.VertexCount();
        for( const auto &hole : impl_->holes )
        {
            count += hole.VertexCount();
        }
        return count;
    }

    std::size_t Polygon2d::SegmentCount() const
    {
        std::size_t count = impl_->outerRing.SegmentCount();
        for( const auto &hole : impl_->holes )
        {
            count += hole.SegmentCount();
        }
        return count;
    }

    std::size_t Polygon2d::HoleCount() const { return impl_->holes.size(); }

    Polyline2d Polygon2d::OuterRing() const { return impl_->outerRing; }

    Polyline2d Polygon2d::HoleAt( std::size_t index ) const
    {
        if( index >= impl_->holes.size() )
        {
            throw std::out_of_range( "Polygon2d::HoleAt index out of range" );
        }

        return impl_->holes[index];
    }

    double Polygon2d::Area() const
    {
        if( !IsValid() )
        {
            return 0.0;
        }

        double total = RingSignedArea( impl_->outerRing );
        for( const auto &hole : impl_->holes )
        {
            total += RingSignedArea( hole );
        }
        return std::abs( total );
    }

    double Polygon2d::Perimeter() const
    {
        if( !IsValid() )
        {
            return 0.0;
        }

        double total = impl_->outerRing.Length();
        for( const auto &hole : impl_->holes )
        {
            total += hole.Length();
        }
        return total;
    }

    Point2d Polygon2d::Centroid() const
    {
        if( !IsValid() )
        {
            return Point2d{};
        }

        const auto outer = RingCentroidAndArea( impl_->outerRing );
        double weightedX = outer.first.x * outer.second;
        double weightedY = outer.first.y * outer.second;
        double signedArea = outer.second;

        for( const auto &hole : impl_->holes )
        {
            const auto ring = RingCentroidAndArea( hole );
            weightedX += ring.first.x * ring.second;
            weightedY += ring.first.y * ring.second;
            signedArea += ring.second;
        }

        if( IsZero( signedArea ) )
        {
            return Point2d{};
        }

        return Point2d( weightedX / signedArea, weightedY / signedArea );
    }

    Box2d Polygon2d::Bounds() const
    {
        if( !IsValid() )
        {
            return Box2d{};
        }

        Box2d box = impl_->outerRing.Bounds();
        for( const auto &hole : impl_->holes )
        {
            box.ExpandToInclude( hole.Bounds() );
        }
        return box;
    }

    std::string Polygon2d::DebugString() const
    {
        std::ostringstream stream;
        stream << "Polygon2d{holeCount=" << HoleCount() << ", pointCount=" << PointCount()
               << ", segmentCount=" << SegmentCount() << ", valid=" << ( IsValid() ? "true" : "false" )
               << ", bounds=" << Bounds().DebugString() << "}";
        return stream.str();
    }
}  // namespace Geometry
