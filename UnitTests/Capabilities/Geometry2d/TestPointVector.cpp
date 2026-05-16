#include <gtest/gtest.h>
#include <cmath>
#include <concepts>

#include "Geometry2d/ArcSegment2d.h"
#include "Geometry2d/LineSegment2d.h"
#include "Support/Geometry2d/Normalize2.h"
#include "Support/GeometryTestSupport.h"
#include "Types/Geometry2d/Box2.h"

using Geometry::ArcDirection;
using Geometry::ArcSegment2d;
using Geometry::Box2d;
using Geometry::Box2i;
using Geometry::Cross;
using Geometry::Dot;
using Geometry::IsEqual;
using Geometry::IsZero;
using Geometry::LineSegment2d;
using Geometry::Point2d;
using Geometry::Point2i;
using Geometry::Segment2d;
using Geometry::SegmentKind2;
using Geometry::TryNormalize;
using Geometry::Vector2d;
using Geometry::Vector2i;

template <typename T>
concept SupportsPointPlusPoint = requires( T a, T b ) { a + b; };

template <typename T>
concept SupportsPointScalarMul = requires( T a ) { a * 2; };

template <typename T>
concept SupportsPointNegate = requires( T a ) { -a; };

namespace
{
    constexpr double kPi = 3.141592653589793238462643383279502884;
}

TEST( PointVectorTest, CoversCurrentCapabilities )
{
    static_assert( std::is_abstract_v<Segment2d> );
    static_assert( !std::same_as<Point2d, Vector2d> );
    static_assert( !SupportsPointPlusPoint<Point2d> );
    static_assert( !SupportsPointScalarMul<Point2d> );
    static_assert( !SupportsPointNegate<Point2d> );

    constexpr Point2i pointA( 1, 2 );
    constexpr Point2i pointB( 4, 6 );
    constexpr Vector2i displacement = pointB - pointA;
    static_assert( pointA.IsValid() );
    static_assert( displacement == Vector2i( 3, 4 ) );
    static_assert( pointA + displacement == pointB );
    static_assert( pointB - displacement == pointA );

    constexpr Vector2i vectorA( 3, 4 );
    constexpr Vector2i vectorB( -1, 2 );
    static_assert( vectorA + vectorB == Vector2i( 2, 6 ) );
    static_assert( vectorA - vectorB == Vector2i( 4, 2 ) );
    static_assert( -vectorB == Vector2i( 1, -2 ) );
    static_assert( vectorA * 2 == Vector2i( 6, 8 ) );
    static_assert( 2 * vectorA == Vector2i( 6, 8 ) );
    static_assert( vectorA / 2 == Vector2i( 1, 2 ) );
    static_assert( vectorA.LengthSquared() == 25.0 );
    static_assert( vectorA.IsValid() );
    ASSERT_LT( std::abs( vectorA.Length() - 5.0 ), 1e-12 );

    static_assert( Dot( vectorA, vectorB ) == 5 );
    static_assert( Cross( vectorA, vectorB ) == 10 );
    static_assert( IsZero( 0 ) );
    static_assert( !IsZero( 1 ) );
    static_assert( IsZero( Vector2i{} ) );
    static_assert( !IsZero( vectorA ) );
    static_assert( IsEqual( Point2d( 1.0, 2.0 ), Point2d( 1.0 + 1e-10, 2.0 - 1e-10 ) ) );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( Point2d( 1.0, 2.0 ), Point2d( 1.0 + 1e-10, 2.0 - 1e-10 ), 1e-9 );
    GEOMETRY_TEST_ASSERT_VECTOR_NEAR( Vector2d( 3.0, 4.0 ), Vector2d( 3.0 + 1e-10, 4.0 - 1e-10 ), 1e-9 );

    Vector2d normalized;
    const bool normalizedOk = TryNormalize( Vector2d( 3.0, 4.0 ), normalized );
    ASSERT_TRUE( normalizedOk );
    ASSERT_LT( std::abs( normalized.x - 0.6 ), 1e-12 );
    ASSERT_LT( std::abs( normalized.y - 0.8 ), 1e-12 );
    ASSERT_LT( std::abs( normalized.Length() - 1.0 ), 1e-12 );

    Vector2d zeroNormalized( 7.0, 9.0 );
    const bool zeroOk = TryNormalize( Vector2d( 1e-12, 0.0 ), zeroNormalized );
    ASSERT_FALSE( zeroOk );
    ASSERT_EQ( zeroNormalized, Vector2d( 7.0, 9.0 ) );

    Box2i emptyBox;
    ASSERT_FALSE( emptyBox.IsValid() );

    emptyBox.ExpandToInclude( Point2i( 2, 3 ) );
    ASSERT_TRUE( emptyBox.IsValid() );
    ASSERT_EQ( emptyBox.MinPoint(), Point2i( 2, 3 ) );
    ASSERT_EQ( emptyBox.MaxPoint(), Point2i( 2, 3 ) );
    ASSERT_EQ( emptyBox.Width(), 0.0 );
    ASSERT_EQ( emptyBox.Height(), 0.0 );

    const Box2d boxA( Point2d( 1.0, 2.0 ), Point2d( 4.0, 6.0 ) );
    ASSERT_TRUE( boxA.IsValid() );
    ASSERT_LT( std::abs( boxA.Width() - 3.0 ), 1e-12 );
    ASSERT_LT( std::abs( boxA.Height() - 4.0 ), 1e-12 );
    ASSERT_LT( std::abs( boxA.Area() - 12.0 ), 1e-12 );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( boxA.Center(), Point2d( 2.5, 4.0 ), 1e-12 );

    const LineSegment2d line( Point2d( 1.0, 2.0 ), Point2d( 4.0, 6.0 ) );
    ASSERT_EQ( line.Kind(), SegmentKind2::Line );
    ASSERT_TRUE( line.IsValid() );
    ASSERT_LT( std::abs( line.Length() - 5.0 ), 1e-12 );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( line.PointAt( 0.5 ), Point2d( 2.5, 4.0 ), 1e-12 );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( line.PointAtLength( 2.5 ), Point2d( 2.5, 4.0 ), 1e-12 );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( line.PointAtLength( -2.5, true ), Point2d( 1.0, 2.0 ), 1e-12 );

    const Box2d lineBox = line.Bounds();
    ASSERT_TRUE( lineBox.IsValid() );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( lineBox.MinPoint(), Point2d( 1.0, 2.0 ), 1e-12 );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( lineBox.MaxPoint(), Point2d( 4.0, 6.0 ), 1e-12 );

    const Segment2d &lineAsSegment = line;
    ASSERT_EQ( lineAsSegment.Kind(), SegmentKind2::Line );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( lineAsSegment.PointAt( 0.5 ), Point2d( 2.5, 4.0 ), 1e-12 );

    const ArcSegment2d arc( Point2d( 0.0, 0.0 ), 1.0, 0.0, kPi / 2.0, ArcDirection::CounterClockwise );
    ASSERT_EQ( arc.Kind(), SegmentKind2::Arc );
    ASSERT_TRUE( arc.IsValid() );
    ASSERT_LT( std::abs( arc.Length() - ( kPi / 2.0 ) ), 1e-12 );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( arc.StartPoint(), Point2d( 1.0, 0.0 ), 1e-12 );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( arc.EndPoint(), Point2d( 0.0, 1.0 ), 1e-12 );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( arc.PointAt( 0.5 ),
                                     Point2d( std::sqrt( 2.0 ) / 2.0, std::sqrt( 2.0 ) / 2.0 ), 1e-12 );

    const Box2d arcBox = arc.Bounds();
    ASSERT_TRUE( arcBox.IsValid() );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( arcBox.MinPoint(), Point2d( 0.0, 0.0 ), 1e-12 );
    GEOMETRY_TEST_ASSERT_POINT_NEAR( arcBox.MaxPoint(), Point2d( 1.0, 1.0 ), 1e-12 );
}
