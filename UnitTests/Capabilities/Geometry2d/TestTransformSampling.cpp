#include <gtest/gtest.h>
#include <cmath>
#include <memory>

#include "Core/Editing.h"
#include "Core/Sampling.h"
#include "Core/Transform.h"
#include "Support/GeometryTestSupport.h"

using Geometry::ArcSegment2d;
using Geometry::Box2d;
using Geometry::InsertPoint;
using Geometry::LineSegment2d;
using Geometry::Mirror;
using Geometry::Normalize;
using Geometry::Point2d;
using Geometry::Polygon2d;
using Geometry::Polyline2d;
using Geometry::PolylineClosure;
using Geometry::Rotate;
using Geometry::SampleByMaxAngle;
using Geometry::Stretch;
using Geometry::Translate;
using Geometry::Vector2d;

TEST( TransformSamplingTest, CoversCurrentCapabilities )
{
    const LineSegment2d line( Point2d{ 0.0, 0.0 }, Point2d{ 2.0, 0.0 } );
    const auto moved = Translate( line, Vector2d{ 1.0, 2.0 } );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( moved.startPoint, ( Point2d{ 1.0, 2.0 } ), 1e-12 );

    const auto rotated = Rotate( Point2d{ 1.0, 0.0 }, Point2d{ 0.0, 0.0 }, std::acos( -1.0 ) * 0.5 );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( rotated, ( Point2d{ 0.0, 1.0 } ), 1e-12 );

    const auto mirrored = Mirror( Point2d{ 1.0, 2.0 }, Point2d{ 0.0, 0.0 }, Vector2d{ 1.0, 0.0 } );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( mirrored, ( Point2d{ 1.0, -2.0 } ), 1e-12 );

    const auto stretched =
        Stretch( Point2d{ 1.0, 1.0 }, Box2d::FromMinMax( Point2d{ 0.0, 0.0 }, Point2d{ 2.0, 2.0 } ),
                 Vector2d{ 3.0, 0.0 } );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( stretched, ( Point2d{ 4.0, 1.0 } ), 1e-12 );

    const ArcSegment2d arc( Point2d{ 0.0, 0.0 }, 1.0, 0.0, std::acos( -1.0 ) * 0.5 );
    const auto arcSamples = SampleByMaxAngle( arc, std::acos( -1.0 ) * 0.25 );
    ASSERT_EQ( arcSamples.size(), 3 );

    const Polyline2d path(
        { Point2d{ 0.0, 0.0 }, Point2d{ 2.0, 0.0 }, Point2d{ 2.0, 0.0 }, Point2d{ 2.0, 2.0 } },
        PolylineClosure::Open );
    const auto normalized = Normalize( path );
    ASSERT_EQ( normalized.PointCount(), 3 );

    const auto inserted =
        InsertPoint( Polyline2d( { Point2d{ 0.0, 0.0 }, Point2d{ 4.0, 0.0 } }, PolylineClosure::Open ),
                     Point2d{ 2.0, 0.0 } );
    ASSERT_EQ( inserted.PointCount(), 3 );

    const Polyline2d mixedPath(
        { std::make_shared<LineSegment2d>( Point2d{ 0.0, 0.0 }, Point2d{ 1.0, 0.0 } ),
          std::make_shared<ArcSegment2d>( Point2d{ 1.0, 1.0 }, 1.0, -std::acos( -1.0 ) * 0.5, 0.0,
                                          Geometry::ArcDirection::CounterClockwise ) },
        PolylineClosure::Open );
    const Polyline2d movedMixedPath = Translate( mixedPath, Vector2d{ 2.0, 3.0 } );
    ASSERT_TRUE( movedMixedPath.IsValid() );
    ASSERT_EQ( movedMixedPath.SegmentAt( 0 )->Kind(), Geometry::SegmentKind2::Line );
    ASSERT_EQ( movedMixedPath.SegmentAt( 1 )->Kind(), Geometry::SegmentKind2::Arc );
    const Polyline2d rotatedMixedPath =
        Rotate( mixedPath, Point2d{ 0.0, 0.0 }, std::acos( -1.0 ) * 0.5 );
    ASSERT_TRUE( rotatedMixedPath.IsValid() );
    ASSERT_EQ( rotatedMixedPath.SegmentAt( 0 )->Kind(), Geometry::SegmentKind2::Line );
    ASSERT_EQ( rotatedMixedPath.SegmentAt( 1 )->Kind(), Geometry::SegmentKind2::Arc );

    const Polygon2d polygon( Polyline2d(
        { Point2d{ 0.0, 0.0 }, Point2d{ 4.0, 0.0 }, Point2d{ 4.0, 4.0 }, Point2d{ 0.0, 4.0 } },
        PolylineClosure::Closed ) );
    const auto movedPolygon = Translate( polygon, Vector2d{ 1.0, 0.0 } );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( movedPolygon.OuterRing().PointAt( 0 ), ( Point2d{ 1.0, 0.0 } ),
                                     1e-12 );
}
