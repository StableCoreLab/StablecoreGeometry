#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Export/GeometryExport.h"
#include "Geometry2d/Segment2d.h"

namespace Geometry
{
    enum class PolylineClosure
    {
        Open,
        Closed
    };

    class GEOMETRY_API Polyline2d
    {
    public:
        Polyline2d();
        explicit Polyline2d( PolylineClosure closure );
        Polyline2d( std::vector<Point2d> points, PolylineClosure closure = PolylineClosure::Open );
        Polyline2d( std::vector<std::shared_ptr<Segment2d>> segments, PolylineClosure closure );
        Polyline2d( const Polyline2d &other );
        Polyline2d( Polyline2d &&other ) noexcept;
        Polyline2d &operator=( const Polyline2d &other );
        Polyline2d &operator=( Polyline2d &&other ) noexcept;
        ~Polyline2d();

        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] bool IsClosed() const;
        [[nodiscard]] std::size_t PointCount() const;
        [[nodiscard]] std::size_t VertexCount() const;
        [[nodiscard]] std::size_t SegmentCount() const;
        [[nodiscard]] double Length() const;
        [[nodiscard]] Point2d PointAt( std::size_t index ) const;
        [[nodiscard]] Point2d PointAt( int index ) const
        {
            return PointAt( static_cast<std::size_t>( index ) );
        }
        [[nodiscard]] Point2d PointAt( double parameter ) const;
        [[nodiscard]] Point2d VertexAt( std::size_t index ) const;
        [[nodiscard]] Point2d StartPoint() const;
        [[nodiscard]] Point2d EndPoint() const;
        [[nodiscard]] double LengthAt( double parameter ) const;
        [[nodiscard]] double ParameterAtLength( double distanceFromStart,
                                                bool clampToPath = false ) const;
        [[nodiscard]] Point2d PointAtLength( double distanceFromStart, bool clampToPath = false ) const;
        [[nodiscard]] std::unique_ptr<Segment2d> SegmentAt( std::size_t index ) const;
        [[nodiscard]] Box2d Bounds() const;
        [[nodiscard]] std::string DebugString() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;

        explicit Polyline2d( std::unique_ptr<Impl> impl );
    };
}  // namespace Geometry
