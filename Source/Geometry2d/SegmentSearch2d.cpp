#include "Geometry2d/SegmentSearch2d.h"

#include <algorithm>
#include <sstream>
#include <utility>

namespace Geometry
{
    SegmentSearch2d::SegmentSearch2d( std::vector<SegmentSearchEntry2d> entries ) :
        entries_( std::move( entries ) )
    {
        for( const auto &entry : entries_ )
        {
            nextId_ = std::max( nextId_, entry.id + 1 );
        }
    }

    void SegmentSearch2d::Clear()
    {
        entries_.clear();
        nextId_ = 0;
    }

    std::size_t SegmentSearch2d::Add( std::shared_ptr<const Segment2d> segment )
    {
        const std::size_t id = nextId_++;
        entries_.push_back( SegmentSearchEntry2d{ id, std::move( segment ),
                                                  entries_.empty() ? Box2d{} : entries_.back().box } );
        entries_.back().box = entries_.back().segment->Bounds();
        return id;
    }

    std::size_t SegmentSearch2d::Add( const Segment2d &segment ) { return Add( segment.Clone() ); }
    std::size_t SegmentSearch2d::Add( const LineSegment2d &segment ) { return Add( segment.Clone() ); }
    std::size_t SegmentSearch2d::Add( const ArcSegment2d &segment ) { return Add( segment.Clone() ); }

    bool SegmentSearch2d::Remove( std::size_t id )
    {
        const auto it =
            std::remove_if( entries_.begin(), entries_.end(),
                            [id]( const SegmentSearchEntry2d &entry ) { return entry.id == id; } );
        if( it == entries_.end() )
        {
            return false;
        }
        entries_.erase( it, entries_.end() );
        return true;
    }

    std::size_t SegmentSearch2d::Size() const { return entries_.size(); }
    bool SegmentSearch2d::IsEmpty() const { return entries_.empty(); }
    bool SegmentSearch2d::IsValid() const
    {
        for( const auto &entry : entries_ )
        {
            if( !entry.IsValid() )
            {
                return false;
            }
        }
        return true;
    }

    bool SegmentSearch2d::Contains( std::size_t id ) const { return Find( id ) != nullptr; }

    const SegmentSearchEntry2d *SegmentSearch2d::Find( std::size_t id ) const
    {
        const auto it =
            std::find_if( entries_.begin(), entries_.end(),
                          [id]( const SegmentSearchEntry2d &entry ) { return entry.id == id; } );
        return it == entries_.end() ? nullptr : &*it;
    }

    std::vector<std::size_t> SegmentSearch2d::QueryIntersecting( const Box2d &box, double eps ) const
    {
        std::vector<std::size_t> result;
        for( const auto &entry : entries_ )
        {
            if( Intersects( entry.box, box, eps ) )
            {
                result.push_back( entry.id );
            }
        }
        return result;
    }

    std::vector<std::size_t> SegmentSearch2d::QueryIntersecting( const Segment2d &segment,
                                                                 double eps ) const
    {
        std::vector<std::size_t> result;
        for( const auto &entry : entries_ )
        {
            if( HasIntersection( *entry.segment, segment, eps ) )
            {
                result.push_back( entry.id );
            }
        }
        return result;
    }

    std::vector<SegmentSearchHit2d> SegmentSearch2d::QueryWithinDistance( const Point2d &point,
                                                                          double maxDistance ) const
    {
        std::vector<SegmentSearchHit2d> result;
        const double maxDistanceSquared = maxDistance * maxDistance;
        for( const auto &entry : entries_ )
        {
            const auto projection = ProjectPointToSegment( point, *entry.segment, true );
            if( projection.distanceSquared <= maxDistanceSquared )
            {
                result.push_back( SegmentSearchHit2d{ entry.id, projection.point,
                                                      projection.distanceSquared, projection.parameter,
                                                      projection.isOnSegment } );
            }
        }
        return result;
    }

    std::optional<SegmentSearchHit2d> SegmentSearch2d::Nearest( const Point2d &point ) const
    {
        std::optional<SegmentSearchHit2d> best;
        for( const auto &entry : entries_ )
        {
            const auto projection = ProjectPointToSegment( point, *entry.segment, true );
            if( !best || projection.distanceSquared < best->distanceSquared )
            {
                best = SegmentSearchHit2d{ entry.id, projection.point, projection.distanceSquared,
                                           projection.parameter, projection.isOnSegment };
            }
        }
        return best;
    }

    std::string SegmentSearch2d::DebugString() const
    {
        std::ostringstream stream;
        stream << "SegmentSearch2d{size=" << Size() << ", nextId=" << nextId_
               << ", valid=" << ( IsValid() ? "true" : "false" ) << "}";
        return stream.str();
    }

    const std::vector<SegmentSearchEntry2d> &SegmentSearch2d::Entries() const { return entries_; }
    std::vector<SegmentSearchEntry2d> &SegmentSearch2d::Entries() { return entries_; }
    const std::vector<SegmentSearchEntry2d> &SegmentSearch2d::Data() const { return Entries(); }
    std::vector<SegmentSearchEntry2d> &SegmentSearch2d::Data() { return Entries(); }
}  // namespace Geometry
