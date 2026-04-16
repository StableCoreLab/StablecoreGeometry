#include <gtest/gtest.h>
#include <cmath>

#include "Types/Geometry2d/Box2.h"
#include "Support/GeometryTestSupport.h"



using Geometry::Box2d;
using Geometry::Box2i;
using Geometry::Point2d;
using Geometry::Point2i;

TEST(BoxTest, CoversCurrentCapabilities)
{
    Box2i emptyBox;
    ASSERT_FALSE(emptyBox.IsValid());

    const Box2i invalidBox(Point2i(3, 4), Point2i(1, 2));
    ASSERT_FALSE(invalidBox.IsValid());

    const Box2i pointBox(Point2i(2, 3), Point2i(2, 3));
    ASSERT_TRUE(pointBox.IsValid());
    ASSERT_EQ(Box2i::FromMinMax(Point2i(2, 3), Point2i(2, 3)), pointBox);
    ASSERT_EQ(pointBox.Width(), 0.0);
    ASSERT_EQ(pointBox.Height(), 0.0);
    ASSERT_EQ(pointBox.Area(), 0.0);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(pointBox.Center(), Point2d(2.0, 3.0), 1e-12);

    const Box2i boxA(Point2i(1, 2), Point2i(4, 6));
    ASSERT_TRUE(boxA.IsValid());
    ASSERT_EQ(boxA.Width(), 3.0);
    ASSERT_EQ(boxA.Height(), 4.0);
    ASSERT_EQ(boxA.Area(), 12.0);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(boxA.Center(), Point2d(2.5, 4.0), 1e-12);

    Box2i expanded;
    expanded.ExpandToInclude(Point2i(5, 7));
    ASSERT_TRUE(expanded.IsValid());
    ASSERT_EQ(expanded.MinPoint(), Point2i(5, 7));
    ASSERT_EQ(expanded.MaxPoint(), Point2i(5, 7));

    expanded.ExpandToInclude(Point2i(2, 9));
    ASSERT_EQ(expanded.MinPoint(), Point2i(2, 7));
    ASSERT_EQ(expanded.MaxPoint(), Point2i(5, 9));

    expanded.ExpandToInclude(Box2i(Point2i(0, 1), Point2i(3, 8)));
    ASSERT_EQ(expanded.MinPoint(), Point2i(0, 1));
    ASSERT_EQ(expanded.MaxPoint(), Point2i(5, 9));

    const Box2i beforeIgnored = expanded;
    expanded.ExpandToInclude(Box2i(Point2i(10, 10), Point2i(4, 12)));
    ASSERT_EQ(expanded, beforeIgnored);

    Box2d floatingBox;
    floatingBox.ExpandToInclude(Point2d(1.5, 2.5));
    floatingBox.ExpandToInclude(Point2d(-0.5, 3.0));
    ASSERT_TRUE(floatingBox.IsValid());
    ASSERT_LT(std::abs(floatingBox.Width() - 2.0), 1e-12);
    ASSERT_LT(std::abs(floatingBox.Height() - 0.5), 1e-12);
    ASSERT_LT(std::abs(floatingBox.Area() - 1.0), 1e-12);
    GEOMETRY_TEST_ASSERT_POINT_NEAR(floatingBox.Center(), Point2d(0.5, 2.75), 1e-12);
    GEOMETRY_TEST_ASSERT_BOX_NEAR(
        floatingBox,
        Box2d::FromMinMax(Point2d(-0.5, 2.5), Point2d(1.5, 3.0)),
        1e-12);

    const Box2i sameA(Point2i(1, 2), Point2i(3, 4));
    const Box2i sameB(Point2i(1, 2), Point2i(3, 4));
    const Box2i different(Point2i(1, 2), Point2i(3, 5));
    ASSERT_EQ(sameA, sameB);
    ASSERT_NE(sameA, different);
    GEOMETRY_TEST_ASSERT_BOX_NEAR(sameA, sameB, 0.0);
}







