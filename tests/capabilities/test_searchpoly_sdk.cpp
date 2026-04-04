#include <gtest/gtest.h>

#include "sdk/GeometrySearchPoly.h"

using geometry::sdk::MultiPolyline2d;
using geometry::sdk::Point2d;
using geometry::sdk::Polygon2d;
using geometry::sdk::Polyline2d;
using geometry::sdk::PolylineClosure;
using geometry::sdk::SearchPolygonContainingPoint;
using geometry::sdk::SearchPolygons;
using geometry::sdk::SearchPolyIssue2d;
using geometry::sdk::SearchPolyOptions2d;

TEST(SearchPolySdkCapabilityTest, InvalidInputContractRejectsEmptyLineCollection)
{
    const MultiPolyline2d lines{};
    const SearchPolyOptions2d options{};
    const auto result = SearchPolygons(lines, options);

    EXPECT_EQ(result.issue, SearchPolyIssue2d::InvalidInput);
    EXPECT_FALSE(result.IsSuccess());
    EXPECT_EQ(result.polygons.Count(), 0U);
    EXPECT_TRUE(result.candidates.empty());
}

TEST(SearchPolySdkCapabilityTest, SearchPolygonsBuildsRepresentativeClosedCandidate)
{
    const MultiPolyline2d lines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 0.0}}, PolylineClosure::Open)};

    const SearchPolyOptions2d options{};
    const auto result = SearchPolygons(lines, options);

    ASSERT_EQ(result.issue, SearchPolyIssue2d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_EQ(result.polygons.Count(), 1U);
    ASSERT_EQ(result.candidates.size(), 1U);
    EXPECT_TRUE(result.usedAutoClose);
    EXPECT_TRUE(result.usedAutoExtend);
    EXPECT_TRUE(result.usedSyntheticEdges);
    EXPECT_TRUE(result.candidates.front().polygon.IsValid());
    EXPECT_DOUBLE_EQ(result.candidates.front().absoluteArea, 16.0);
    EXPECT_EQ(result.candidates.front().holeCount, 0U);
}

TEST(SearchPolySdkCapabilityTest, SearchPolygonContainingPointReturnsContainingCandidate)
{
    const MultiPolyline2d lines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{6.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{6.0, 0.0}, Point2d{6.0, 6.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{6.0, 6.0}, Point2d{0.0, 6.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 6.0}, Point2d{0.0, 0.0}}, PolylineClosure::Open)};

    const auto candidate = SearchPolygonContainingPoint(lines, Point2d{3.0, 3.0});

    ASSERT_TRUE(candidate.has_value());
    EXPECT_TRUE(candidate->IsValid());
    EXPECT_DOUBLE_EQ(candidate->absoluteArea, 36.0);
}
