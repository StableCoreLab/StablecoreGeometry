#include "Geometry2d/Polyline2d.h"

#include <cassert>
#include <memory>
#include <sstream>
#include <utility>

#include "Geometry2d/ArcSegment2d.h"
#include "Geometry2d/LineSegment2d.h"
#include "Support/Geometry2d/Predicate2.h"

namespace Geometry
{
    namespace
    {
        [[nodiscard]] std::vector<std::shared_ptr<Segment2d>>
        CloneSegments( const std::vector<std::shared_ptr<Segment2d>> &segments )
        {
            std::vector<std::shared_ptr<Segment2d>> clones;
            clones.reserve( segments.size() );
            for( const auto &segment : segments )
            {
                if( segment == nullptr )
                {
                    clones.push_back( nullptr );
                    continue;
                }

                std::unique_ptr<Segment2d> clone = segment->Clone();
                clones.push_back( std::shared_ptr<Segment2d>( std::move( clone ) ) );
            }
            return clones;
        }

        [[nodiscard]] std::vector<std::shared_ptr<Segment2d>> MakeLineSegments(
            const std::vector<Point2d> &points, PolylineClosure closure )
        {
            std::vector<std::shared_ptr<Segment2d>> segments;
            if( points.size() < 2 )
            {
                return segments;
            }

            const std::size_t segmentCount =
                closure == PolylineClosure::Closed ? points.size() : points.size() - 1;
            segments.reserve( segmentCount );
            for( std::size_t i = 0; i < segmentCount; ++i )
            {
                segments.push_back(
                    std::make_shared<LineSegment2d>( points[i], points[( i + 1 ) % points.size()] ) );
            }
            return segments;
        }
    }  // namespace

    struct Polyline2d::Impl
    {
        std::vector<std::shared_ptr<Segment2d>> segments{};
        PolylineClosure closure{ PolylineClosure::Open };

        Impl() = default;

        explicit Impl( PolylineClosure closure ) : closure( closure ) {}

        Impl( std::vector<std::shared_ptr<Segment2d>> segments, PolylineClosure closure ) :
            segments( std::move( segments ) ),
            closure( closure )
        {
        }
    };

    Polyline2d::Polyline2d() : impl_( std::make_unique<Impl>() ) {}

    Polyline2d::Polyline2d( PolylineClosure closure ) : impl_( std::make_unique<Impl>( closure ) ) {}

    Polyline2d::Polyline2d( std::vector<Point2d> points, PolylineClosure closure ) :
        impl_( std::make_unique<Impl>( MakeLineSegments( points, closure ), closure ) )
    {
    }

    Polyline2d::Polyline2d( std::vector<std::shared_ptr<Segment2d>> segments, PolylineClosure closure ) :
        impl_( std::make_unique<Impl>( std::move( segments ), closure ) )
    {
    }

    Polyline2d::Polyline2d( std::unique_ptr<Impl> impl ) : impl_( std::move( impl ) ) {}

    Polyline2d::Polyline2d( const Polyline2d &other ) :
        impl_( std::make_unique<Impl>( CloneSegments( other.impl_->segments ), other.impl_->closure ) )
    {
    }

    Polyline2d::Polyline2d( Polyline2d &&other ) noexcept = default;

    Polyline2d &Polyline2d::operator=( const Polyline2d &other )
    {
        if( this != &other )
        {
            impl_ = std::make_unique<Impl>( CloneSegments( other.impl_->segments ),
                                            other.impl_->closure );
        }
        return *this;
    }

    Polyline2d &Polyline2d::operator=( Polyline2d &&other ) noexcept = default;

    Polyline2d::~Polyline2d() = default;

    bool Polyline2d::IsValid() const
    {
        if( impl_->segments.empty() )
        {
            return false;
        }

        double totalLength = 0.0;
        for( std::size_t i = 0; i < impl_->segments.size(); ++i )
        {
            const auto &segment = impl_->segments[i];
            if( segment == nullptr || !segment->IsValid() )
            {
                return false;
            }

            totalLength += segment->Length();
            if( i > 0 && !IsEqual( impl_->segments[i - 1]->EndPoint(), segment->StartPoint() ) )
            {
                return false;
            }
        }

        if( !( totalLength > 0.0 ) )
        {
            return false;
        }

        if( IsClosed() )
        {
            return IsEqual( impl_->segments.back()->EndPoint(), impl_->segments.front()->StartPoint() );
        }

        return true;
    }

    bool Polyline2d::IsClosed() const { return impl_->closure == PolylineClosure::Closed; }

    std::size_t Polyline2d::PointCount() const { return VertexCount(); }

    std::size_t Polyline2d::VertexCount() const
    {
        if( impl_->segments.empty() )
        {
            return 0;
        }

        return IsClosed() ? impl_->segments.size() : impl_->segments.size() + 1;
    }

    std::size_t Polyline2d::SegmentCount() const { return impl_->segments.size(); }

    double Polyline2d::Length() const
    {
        double total = 0.0;
        for( const auto &segment : impl_->segments )
        {
            if( segment != nullptr )
            {
                total += segment->Length();
            }
        }
        return total;
    }

    Point2d Polyline2d::PointAt( std::size_t index ) const
    {
        return VertexAt( index );
    }

    Point2d Polyline2d::PointAt( double parameter ) const
    {
        return PointAtLength( LengthAt( parameter ), false );
    }

    Point2d Polyline2d::VertexAt( std::size_t index ) const
    {
        assert( index < VertexCount() );
        if( impl_->segments.empty() )
        {
            return Point2d{};
        }

        if( !IsClosed() && index == impl_->segments.size() )
        {
            return impl_->segments.back()->EndPoint();
        }

        return impl_->segments.at( index )->StartPoint();
    }

    Point2d Polyline2d::StartPoint() const
    {
        if( impl_->segments.empty() || impl_->segments.front() == nullptr )
        {
            return Point2d{};
        }
        return impl_->segments.front()->StartPoint();
    }

    Point2d Polyline2d::EndPoint() const
    {
        if( impl_->segments.empty() || impl_->segments.back() == nullptr )
        {
            return Point2d{};
        }
        return impl_->segments.back()->EndPoint();
    }

    double Polyline2d::LengthAt( double parameter ) const { return parameter * Length(); }

    double Polyline2d::ParameterAtLength( double distanceFromStart, bool clampToPath ) const
    {
        const double totalLength = Length();
        if( !( totalLength > 0.0 ) )
        {
            return 0.0;
        }

        if( clampToPath )
        {
            if( distanceFromStart < 0.0 )
            {
                distanceFromStart = 0.0;
            }
            else if( distanceFromStart > totalLength )
            {
                distanceFromStart = totalLength;
            }
        }

        return distanceFromStart / totalLength;
    }

    Point2d Polyline2d::PointAtLength( double distanceFromStart, bool clampToPath ) const
    {
        if( impl_->segments.empty() )
        {
            return Point2d{};
        }

        if( !IsValid() )
        {
            return StartPoint();
        }

        const double totalLength = Length();
        if( !( totalLength > 0.0 ) )
        {
            return StartPoint();
        }

        if( clampToPath )
        {
            if( distanceFromStart < 0.0 )
            {
                distanceFromStart = 0.0;
            }
            else if( distanceFromStart > totalLength )
            {
                distanceFromStart = totalLength;
            }
        }

        if( distanceFromStart < 0.0 )
        {
            return impl_->segments.front()->PointAtLength( distanceFromStart, false );
        }

        double prefixLength = 0.0;
        for( std::size_t i = 0; i < impl_->segments.size(); ++i )
        {
            const double segmentLength = impl_->segments[i]->Length();
            const double segmentEnd = prefixLength + segmentLength;
            if( distanceFromStart <= segmentEnd || i + 1 == impl_->segments.size() )
            {
                return impl_->segments[i]->PointAtLength( distanceFromStart - prefixLength, false );
            }
            prefixLength = segmentEnd;
        }

        return EndPoint();
    }

    std::unique_ptr<Segment2d> Polyline2d::SegmentAt( std::size_t index ) const
    {
        assert( index < impl_->segments.size() );
        if( index >= impl_->segments.size() || impl_->segments[index] == nullptr )
        {
            return nullptr;
        }

        return impl_->segments[index]->Clone();
    }

    Box2d Polyline2d::Bounds() const
    {
        if( !IsValid() )
        {
            return Box2d{};
        }

        Box2d box;
        for( const auto &segment : impl_->segments )
        {
            box.ExpandToInclude( segment->Bounds() );
        }
        return box;
    }

    std::string Polyline2d::DebugString() const
    {
        std::ostringstream stream;
        stream << "Polyline2d{closure=" << ( IsClosed() ? "Closed" : "Open" )
               << ", pointCount=" << PointCount() << ", segmentCount=" << SegmentCount()
               << ", valid=" << ( IsValid() ? "true" : "false" ) << ", bounds=" << Bounds().DebugString()
               << "}";
        return stream.str();
    }
}  // namespace Geometry
