#include <gtest/gtest.h>

#include "sdk/GeometrySearchPoly.h"

using geometry::sdk::MultiPolyline2d;
using geometry::sdk::Point2d;
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
    EXPECT_EQ(result.diagnostics.inputPolylineCount, 4U);
    EXPECT_EQ(result.diagnostics.inputSegmentCount, 4U);
    EXPECT_EQ(result.diagnostics.danglingEndpointCount, 0U);
    EXPECT_EQ(result.diagnostics.branchVertexCount, 0U);
    EXPECT_FALSE(result.usedAutoClose);
    EXPECT_FALSE(result.usedAutoExtend);
    EXPECT_FALSE(result.usedSyntheticEdges);
    EXPECT_EQ(result.candidates.front().rank, 0U);
    EXPECT_TRUE(result.candidates.front().polygon.IsValid());
    EXPECT_DOUBLE_EQ(result.candidates.front().absoluteArea, 16.0);
    EXPECT_EQ(result.candidates.front().holeCount, 0U);
}

TEST(SearchPolySdkCapabilityTest, SearchPolygonsRanksLargerCandidateFirst)
{
    const MultiPolyline2d lines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{8.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{8.0, 0.0}, Point2d{8.0, 8.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{8.0, 8.0}, Point2d{0.0, 8.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 8.0}, Point2d{0.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{12.0, 0.0}, Point2d{14.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{14.0, 0.0}, Point2d{14.0, 2.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{14.0, 2.0}, Point2d{12.0, 2.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{12.0, 2.0}, Point2d{12.0, 0.0}}, PolylineClosure::Open)};

    const auto result = SearchPolygons(lines);

    ASSERT_TRUE(result.IsSuccess());
    ASSERT_EQ(result.candidates.size(), 2U);
    EXPECT_DOUBLE_EQ(result.candidates[0].absoluteArea, 64.0);
    EXPECT_DOUBLE_EQ(result.candidates[1].absoluteArea, 4.0);
    EXPECT_EQ(result.candidates[0].rank, 0U);
    EXPECT_EQ(result.candidates[1].rank, 1U);
}

TEST(SearchPolySdkCapabilityTest, SearchPolygonsReportsRepairDiagnosticsForNearClosedLoop)
{
    const MultiPolyline2d lines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 0.3}}, PolylineClosure::Open)};

    const auto result = SearchPolygons(lines);

    ASSERT_TRUE(result.IsSuccess());
    EXPECT_EQ(result.diagnostics.danglingEndpointCount, 2U);
    EXPECT_GE(result.diagnostics.inferredSyntheticEdgeCount, 1U);
    EXPECT_TRUE(result.usedAutoClose);
    EXPECT_TRUE(result.usedSyntheticEdges);
}

TEST(SearchPolySdkCapabilityTest, SearchPolygonContainingPointReturnsSmallestContainingCandidate)
{
    const MultiPolyline2d lines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{10.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{10.0, 0.0}, Point2d{10.0, 10.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{10.0, 10.0}, Point2d{0.0, 10.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 10.0}, Point2d{0.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{20.0, 0.0}, Point2d{24.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{24.0, 0.0}, Point2d{24.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{24.0, 4.0}, Point2d{20.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{20.0, 4.0}, Point2d{20.0, 0.0}}, PolylineClosure::Open)};

    const auto candidate = SearchPolygonContainingPoint(lines, Point2d{22.0, 2.0});

    ASSERT_TRUE(candidate.has_value());
    EXPECT_TRUE(candidate->IsValid());
    EXPECT_DOUBLE_EQ(candidate->absoluteArea, 16.0);
    EXPECT_EQ(candidate->rank, 1U);
}
