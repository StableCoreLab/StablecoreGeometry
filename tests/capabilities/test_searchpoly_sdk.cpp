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
using geometry::sdk::SearchPolyPenaltyKind2d;
using geometry::sdk::SearchPolySyntheticEdgeKind2d;

TEST(SearchPolySdkCapabilityTest, InvalidInputContractRejectsEmptyLineCollection)
{
    const MultiPolyline2d lines{};
    const SearchPolyOptions2d options{};
    const auto result = SearchPolygons(lines, options);

    EXPECT_EQ(result.issue, SearchPolyIssue2d::InvalidInput);
    EXPECT_FALSE(result.IsSuccess());
    EXPECT_EQ(result.polygons.Count(), 0U);
    EXPECT_TRUE(result.candidates.empty());
    EXPECT_EQ(result.diagnostics.inputPolylineCount, 0U);
    EXPECT_EQ(result.diagnostics.inputSegmentCount, 0U);
    EXPECT_EQ(result.diagnostics.uniqueVertexCount, 0U);
    EXPECT_EQ(result.diagnostics.danglingEndpointCount, 0U);
    EXPECT_EQ(result.diagnostics.branchVertexCount, 0U);
    EXPECT_EQ(result.diagnostics.inferredSyntheticEdgeCount, 0U);
    EXPECT_DOUBLE_EQ(result.bestCandidateScoreMargin, 0.0);
    EXPECT_DOUBLE_EQ(result.bestCandidateSyntheticPerimeter, 0.0);
    EXPECT_EQ(result.bestCandidateSyntheticEdgeCount, 0U);
    EXPECT_DOUBLE_EQ(result.runnerUpSyntheticPerimeter, 0.0);
    EXPECT_EQ(result.runnerUpSyntheticEdgeCount, 0U);
    EXPECT_EQ(result.runnerUpBranchVertexCount, 0U);
    EXPECT_EQ(result.bestCandidatePenaltyKind, SearchPolyPenaltyKind2d::None);
    EXPECT_EQ(result.runnerUpPenaltyKind, SearchPolyPenaltyKind2d::None);
    EXPECT_FALSE(result.bestCandidateBeatsSyntheticRunnerUp);
    EXPECT_FALSE(result.bestCandidateBeatsBranchRunnerUp);
    EXPECT_EQ(result.candidateCountWithSyntheticEdges, 0U);
    EXPECT_EQ(result.candidateCountWithBranchPenalty, 0U);
    EXPECT_EQ(result.ambiguousTopCandidateCount, 0U);
    EXPECT_FALSE(result.usedAutoClose);
    EXPECT_FALSE(result.usedAutoExtend);
    EXPECT_FALSE(result.usedSyntheticEdges);
    EXPECT_FALSE(result.usedBranchScoring);
}

TEST(SearchPolySdkCapabilityTest, NoClosedPolygonFoundKeepsDiagnosticsAndCandidateSetConsistent)
{
    const MultiPolyline2d lines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{2.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{2.0, 0.0}, Point2d{2.0, 1.0}}, PolylineClosure::Open)};

    const auto result = SearchPolygons(lines);

    EXPECT_EQ(result.issue, SearchPolyIssue2d::NoClosedPolygonFound);
    EXPECT_FALSE(result.IsSuccess());
    EXPECT_EQ(result.polygons.Count(), 0U);
    EXPECT_TRUE(result.candidates.empty());
    EXPECT_EQ(result.diagnostics.inputPolylineCount, 2U);
    EXPECT_EQ(result.diagnostics.inputSegmentCount, 2U);
    EXPECT_EQ(result.diagnostics.uniqueVertexCount, 3U);
    EXPECT_EQ(result.diagnostics.danglingEndpointCount, 2U);
    EXPECT_EQ(result.diagnostics.branchVertexCount, 0U);
    EXPECT_EQ(result.diagnostics.inferredSyntheticEdgeCount, 1U);
    EXPECT_DOUBLE_EQ(result.bestCandidateScoreMargin, 0.0);
    EXPECT_DOUBLE_EQ(result.bestCandidateSyntheticPerimeter, 0.0);
    EXPECT_EQ(result.bestCandidateSyntheticEdgeCount, 0U);
    EXPECT_DOUBLE_EQ(result.runnerUpSyntheticPerimeter, 0.0);
    EXPECT_EQ(result.runnerUpSyntheticEdgeCount, 0U);
    EXPECT_EQ(result.runnerUpBranchVertexCount, 0U);
    EXPECT_EQ(result.bestCandidatePenaltyKind, SearchPolyPenaltyKind2d::None);
    EXPECT_EQ(result.runnerUpPenaltyKind, SearchPolyPenaltyKind2d::None);
    EXPECT_FALSE(result.bestCandidateBeatsSyntheticRunnerUp);
    EXPECT_FALSE(result.bestCandidateBeatsBranchRunnerUp);
    EXPECT_EQ(result.candidateCountWithSyntheticEdges, 0U);
    EXPECT_EQ(result.candidateCountWithBranchPenalty, 0U);
    EXPECT_EQ(result.ambiguousTopCandidateCount, 0U);
    EXPECT_FALSE(result.usedAutoClose);
    EXPECT_FALSE(result.usedAutoExtend);
    EXPECT_FALSE(result.usedSyntheticEdges);
    EXPECT_FALSE(result.usedBranchScoring);
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
    EXPECT_FALSE(result.usedBranchScoring);
    EXPECT_EQ(result.candidates.front().rank, 0U);
    EXPECT_DOUBLE_EQ(result.bestCandidateScoreMargin, 0.0);
    EXPECT_DOUBLE_EQ(result.bestCandidateSyntheticPerimeter, 0.0);
    EXPECT_EQ(result.bestCandidateSyntheticEdgeCount, 0U);
    EXPECT_DOUBLE_EQ(result.runnerUpSyntheticPerimeter, 0.0);
    EXPECT_EQ(result.runnerUpSyntheticEdgeCount, 0U);
    EXPECT_EQ(result.runnerUpBranchVertexCount, 0U);
    EXPECT_EQ(result.bestCandidatePenaltyKind, SearchPolyPenaltyKind2d::None);
    EXPECT_EQ(result.runnerUpPenaltyKind, SearchPolyPenaltyKind2d::None);
    EXPECT_FALSE(result.bestCandidateBeatsSyntheticRunnerUp);
    EXPECT_FALSE(result.bestCandidateBeatsBranchRunnerUp);
    EXPECT_EQ(result.candidateCountWithSyntheticEdges, 0U);
    EXPECT_EQ(result.candidateCountWithBranchPenalty, 0U);
    EXPECT_EQ(result.ambiguousTopCandidateCount, 1U);
    EXPECT_TRUE(result.candidates.front().polygon.IsValid());
    EXPECT_DOUBLE_EQ(result.candidates.front().absoluteArea, 16.0);
    EXPECT_DOUBLE_EQ(result.candidates.front().branchScore, 16.0);
    EXPECT_EQ(result.candidates.front().holeCount, 0U);
    EXPECT_EQ(result.candidates.front().inferredSyntheticEdgeCount, 0U);
    EXPECT_DOUBLE_EQ(result.candidates.front().inferredSyntheticPerimeter, 0.0);
    EXPECT_TRUE(result.candidates.front().inferredSyntheticEdges.empty());
    EXPECT_TRUE(result.candidates.front().inferredSyntheticEdgeKinds.empty());
    EXPECT_TRUE(result.candidates.front().inferredSyntheticEdgeLengths.empty());
    EXPECT_EQ(result.candidates.front().branchVertexCount, 0U);
    EXPECT_EQ(result.candidates.front().syntheticBranchVertexCount, 0U);
    EXPECT_EQ(result.candidates.front().dominantPenaltyKind, SearchPolyPenaltyKind2d::None);
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
    EXPECT_TRUE(result.usedBranchScoring);
    ASSERT_EQ(result.candidates.size(), 1U);
    EXPECT_GE(result.candidates.front().inferredSyntheticEdgeCount, 1U);
    EXPECT_GT(result.candidates.front().inferredSyntheticPerimeter, 0.0);
    ASSERT_EQ(result.candidates.front().inferredSyntheticEdges.size(), result.candidates.front().inferredSyntheticEdgeCount);
    ASSERT_EQ(result.candidates.front().inferredSyntheticEdgeKinds.size(), result.candidates.front().inferredSyntheticEdgeCount);
    for (const auto& syntheticEdge : result.candidates.front().inferredSyntheticEdges)
    {
        EXPECT_TRUE(syntheticEdge.IsValid());
        EXPECT_GT(syntheticEdge.Length(), 0.0);
    }
    for (const auto syntheticKind : result.candidates.front().inferredSyntheticEdgeKinds)
    {
        EXPECT_EQ(syntheticKind, SearchPolySyntheticEdgeKind2d::GapClosure);
    }
    ASSERT_EQ(result.candidates.front().inferredSyntheticEdgeLengths.size(), result.candidates.front().inferredSyntheticEdgeCount);
    for (double syntheticLength : result.candidates.front().inferredSyntheticEdgeLengths)
    {
        EXPECT_GT(syntheticLength, 0.0);
    }
    EXPECT_EQ(result.candidates.front().dominantPenaltyKind, SearchPolyPenaltyKind2d::SyntheticClosure);
    EXPECT_LT(result.candidates.front().branchScore, result.candidates.front().absoluteArea);
}

TEST(SearchPolySdkCapabilityTest, AutoFlagsRespectOptionsWhileDiagnosticsRemainStable)
{
    const MultiPolyline2d syntheticLines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 0.3}}, PolylineClosure::Open)};

    SearchPolyOptions2d autoCloseDisabled{};
    autoCloseDisabled.autoClose = false;
    const auto syntheticResult = SearchPolygons(syntheticLines, autoCloseDisabled);

    ASSERT_TRUE(syntheticResult.IsSuccess());
    EXPECT_EQ(syntheticResult.diagnostics.danglingEndpointCount, 2U);
    EXPECT_TRUE(syntheticResult.usedSyntheticEdges);
    EXPECT_TRUE(syntheticResult.usedBranchScoring);
    EXPECT_FALSE(syntheticResult.usedAutoClose);
    EXPECT_FALSE(syntheticResult.usedAutoExtend);

    const MultiPolyline2d branchingLines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 2.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 2.0}, Point2d{0.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 2.0}, Point2d{-1.0, 2.0}}, PolylineClosure::Open)};

    SearchPolyOptions2d autoExtendDisabled{};
    autoExtendDisabled.autoExtend = false;
    const auto branchingResult = SearchPolygons(branchingLines, autoExtendDisabled);

    ASSERT_TRUE(branchingResult.IsSuccess());
    EXPECT_EQ(branchingResult.diagnostics.branchVertexCount, 1U);
    EXPECT_TRUE(branchingResult.usedBranchScoring);
    EXPECT_TRUE(branchingResult.usedAutoClose);
    EXPECT_FALSE(branchingResult.usedAutoExtend);
}

TEST(SearchPolySdkCapabilityTest, SearchPolygonsRanksCleanCandidateAheadOfSyntheticPeer)
{
    const MultiPolyline2d lines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{10.0, 0.0}, Point2d{14.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{14.0, 0.0}, Point2d{14.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{14.0, 4.0}, Point2d{10.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{10.0, 4.0}, Point2d{10.0, 0.3}}, PolylineClosure::Open)};

    const auto result = SearchPolygons(lines);

    ASSERT_TRUE(result.IsSuccess());
    ASSERT_EQ(result.candidates.size(), 2U);
    EXPECT_TRUE(result.usedAutoClose);
    EXPECT_TRUE(result.usedSyntheticEdges);
    EXPECT_TRUE(result.usedBranchScoring);

    const auto& first = result.candidates[0];
    const auto& second = result.candidates[1];
    EXPECT_LT(first.polygon.Bounds().MinPoint().x, 1.0);
    EXPECT_GT(second.polygon.Bounds().MinPoint().x, 9.0);
    EXPECT_EQ(first.inferredSyntheticEdgeCount, 0U);
    EXPECT_GE(second.inferredSyntheticEdgeCount, 1U);
    EXPECT_TRUE(first.inferredSyntheticEdges.empty());
    EXPECT_TRUE(first.inferredSyntheticEdgeKinds.empty());
    ASSERT_EQ(second.inferredSyntheticEdges.size(), second.inferredSyntheticEdgeCount);
    ASSERT_EQ(second.inferredSyntheticEdgeKinds.size(), second.inferredSyntheticEdgeCount);
    EXPECT_GT(first.branchScore, second.branchScore);
    EXPECT_EQ(first.rank, 0U);
    EXPECT_EQ(second.rank, 1U);
    EXPECT_DOUBLE_EQ(result.bestCandidateSyntheticPerimeter, 0.0);
    EXPECT_EQ(result.bestCandidateSyntheticEdgeCount, 0U);
    EXPECT_GT(result.runnerUpSyntheticPerimeter, 0.0);
    EXPECT_GE(result.runnerUpSyntheticEdgeCount, 1U);
    EXPECT_EQ(result.runnerUpBranchVertexCount, 0U);
    EXPECT_EQ(result.bestCandidatePenaltyKind, SearchPolyPenaltyKind2d::None);
    EXPECT_EQ(result.runnerUpPenaltyKind, SearchPolyPenaltyKind2d::SyntheticClosure);
    EXPECT_TRUE(result.bestCandidateBeatsSyntheticRunnerUp);
    EXPECT_FALSE(result.bestCandidateBeatsBranchRunnerUp);
    EXPECT_EQ(result.candidateCountWithSyntheticEdges, 1U);
    EXPECT_EQ(result.candidateCountWithBranchPenalty, 0U);
    EXPECT_EQ(result.ambiguousTopCandidateCount, 1U);
    EXPECT_GT(result.bestCandidateScoreMargin, 0.0);
}

TEST(SearchPolySdkCapabilityTest, SearchPolygonsReportsFakeEdgeDiagnosticsForAmbiguousCandidate)
{
    const MultiPolyline2d ambiguousFakeLines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 0.25}}, PolylineClosure::Open),
        Polyline2d({Point2d{-0.15, 0.25}, Point2d{-0.15, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{-0.15, 0.0}, Point2d{0.15, 0.0}}, PolylineClosure::Open)};

    const auto result = SearchPolygons(ambiguousFakeLines);

    ASSERT_TRUE(result.IsSuccess());
    ASSERT_EQ(result.candidates.size(), 1U);
    EXPECT_TRUE(result.usedSyntheticEdges);
    EXPECT_TRUE(result.usedBranchScoring);
    EXPECT_GE(result.candidates.front().inferredSyntheticEdgeCount, 1U);
    EXPECT_GT(result.candidates.front().inferredSyntheticPerimeter, 0.0);
    ASSERT_EQ(result.candidates.front().inferredSyntheticEdges.size(), result.candidates.front().inferredSyntheticEdgeCount);
    ASSERT_EQ(result.candidates.front().inferredSyntheticEdgeKinds.size(), result.candidates.front().inferredSyntheticEdgeCount);
    for (const auto& syntheticEdge : result.candidates.front().inferredSyntheticEdges)
    {
        EXPECT_TRUE(syntheticEdge.IsValid());
        EXPECT_GT(syntheticEdge.Length(), 0.0);
    }
    for (const auto syntheticKind : result.candidates.front().inferredSyntheticEdgeKinds)
    {
        EXPECT_EQ(syntheticKind, SearchPolySyntheticEdgeKind2d::GapClosure);
    }
    EXPECT_LT(result.candidates.front().branchScore, result.candidates.front().absoluteArea);
    EXPECT_EQ(result.bestCandidateSyntheticEdgeCount, result.candidates.front().inferredSyntheticEdgeCount);
    EXPECT_DOUBLE_EQ(result.bestCandidateSyntheticPerimeter, result.candidates.front().inferredSyntheticPerimeter);
    EXPECT_DOUBLE_EQ(result.runnerUpSyntheticPerimeter, 0.0);
    EXPECT_EQ(result.runnerUpSyntheticEdgeCount, 0U);
    EXPECT_EQ(result.runnerUpBranchVertexCount, 0U);
    EXPECT_EQ(result.bestCandidatePenaltyKind, SearchPolyPenaltyKind2d::SyntheticClosure);
    EXPECT_EQ(result.runnerUpPenaltyKind, SearchPolyPenaltyKind2d::None);
    EXPECT_FALSE(result.bestCandidateBeatsSyntheticRunnerUp);
    EXPECT_FALSE(result.bestCandidateBeatsBranchRunnerUp);
    EXPECT_EQ(result.candidateCountWithSyntheticEdges, 1U);
    EXPECT_EQ(result.ambiguousTopCandidateCount, 1U);
}

TEST(SearchPolySdkCapabilityTest, SearchPolygonsAppliesBranchPenaltyAtExplicitBranchVertex)
{
    const MultiPolyline2d lines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 2.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 2.0}, Point2d{0.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 2.0}, Point2d{-1.0, 2.0}}, PolylineClosure::Open)};

    const auto result = SearchPolygons(lines);

    ASSERT_TRUE(result.IsSuccess());
    ASSERT_EQ(result.candidates.size(), 1U);
    EXPECT_EQ(result.diagnostics.branchVertexCount, 1U);
    EXPECT_TRUE(result.usedBranchScoring);
    EXPECT_TRUE(result.usedAutoExtend);
    EXPECT_EQ(result.candidates.front().inferredSyntheticEdgeCount, 0U);
    EXPECT_TRUE(result.candidates.front().inferredSyntheticEdges.empty());
    EXPECT_TRUE(result.candidates.front().inferredSyntheticEdgeKinds.empty());
    EXPECT_GE(result.candidates.front().branchVertexCount, 1U);
    EXPECT_TRUE(result.candidates.front().inferredSyntheticEdgeLengths.empty());
    EXPECT_EQ(result.candidates.front().dominantPenaltyKind, SearchPolyPenaltyKind2d::BranchPenalty);
    EXPECT_LT(result.candidates.front().branchScore, result.candidates.front().absoluteArea);
    EXPECT_EQ(result.bestCandidateSyntheticEdgeCount, 0U);
    EXPECT_DOUBLE_EQ(result.bestCandidateSyntheticPerimeter, 0.0);
    EXPECT_DOUBLE_EQ(result.runnerUpSyntheticPerimeter, 0.0);
    EXPECT_EQ(result.runnerUpSyntheticEdgeCount, 0U);
    EXPECT_EQ(result.runnerUpBranchVertexCount, 0U);
    EXPECT_EQ(result.bestCandidatePenaltyKind, SearchPolyPenaltyKind2d::BranchPenalty);
    EXPECT_EQ(result.runnerUpPenaltyKind, SearchPolyPenaltyKind2d::None);
    EXPECT_FALSE(result.bestCandidateBeatsSyntheticRunnerUp);
    EXPECT_FALSE(result.bestCandidateBeatsBranchRunnerUp);
    EXPECT_EQ(result.candidateCountWithSyntheticEdges, 0U);
    EXPECT_EQ(result.candidateCountWithBranchPenalty, 1U);
    EXPECT_EQ(result.ambiguousTopCandidateCount, 1U);
}

TEST(SearchPolySdkCapabilityTest, SearchPolygonsReportsTopCandidateExplanationAgainstRunnerUp)
{
    const MultiPolyline2d lines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{10.0, 0.0}, Point2d{14.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{14.0, 0.0}, Point2d{14.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{14.0, 4.0}, Point2d{10.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{10.0, 4.0}, Point2d{10.0, 0.3}}, PolylineClosure::Open)};

    const auto result = SearchPolygons(lines);

    ASSERT_TRUE(result.IsSuccess());
    ASSERT_EQ(result.candidates.size(), 2U);
    EXPECT_EQ(result.ambiguousTopCandidateCount, 1U);
    EXPECT_EQ(result.bestCandidateSyntheticEdgeCount, 0U);
    EXPECT_DOUBLE_EQ(result.bestCandidateSyntheticPerimeter, 0.0);
    EXPECT_GT(result.runnerUpSyntheticPerimeter, 0.0);
    EXPECT_GE(result.runnerUpSyntheticEdgeCount, 1U);
    EXPECT_EQ(result.runnerUpBranchVertexCount, 0U);
    EXPECT_TRUE(result.bestCandidateBeatsSyntheticRunnerUp);
    EXPECT_FALSE(result.bestCandidateBeatsBranchRunnerUp);
    EXPECT_EQ(result.candidateCountWithSyntheticEdges, 1U);
    EXPECT_GT(result.bestCandidateScoreMargin, 0.0);
    EXPECT_DOUBLE_EQ(
        result.bestCandidateScoreMargin,
        result.candidates[0].branchScore - result.candidates[1].branchScore);
}

TEST(SearchPolySdkCapabilityTest, SearchPolygonsReportsRunnerUpBranchPenaltyExplanation)
{
    const MultiPolyline2d lines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{10.0, 0.0}, Point2d{14.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{14.0, 0.0}, Point2d{14.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{14.0, 4.0}, Point2d{10.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{10.0, 4.0}, Point2d{10.0, 2.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{10.0, 2.0}, Point2d{10.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{10.0, 2.0}, Point2d{9.0, 2.0}}, PolylineClosure::Open)};

    const auto result = SearchPolygons(lines);

    ASSERT_TRUE(result.IsSuccess());
    ASSERT_EQ(result.candidates.size(), 2U);
    EXPECT_EQ(result.bestCandidateSyntheticEdgeCount, 0U);
    EXPECT_DOUBLE_EQ(result.bestCandidateSyntheticPerimeter, 0.0);
    EXPECT_DOUBLE_EQ(result.runnerUpSyntheticPerimeter, 0.0);
    EXPECT_EQ(result.runnerUpSyntheticEdgeCount, 0U);
    EXPECT_GE(result.runnerUpBranchVertexCount, 1U);
    EXPECT_EQ(result.bestCandidatePenaltyKind, SearchPolyPenaltyKind2d::None);
    EXPECT_EQ(result.runnerUpPenaltyKind, SearchPolyPenaltyKind2d::BranchPenalty);
    EXPECT_FALSE(result.bestCandidateBeatsSyntheticRunnerUp);
    EXPECT_TRUE(result.bestCandidateBeatsBranchRunnerUp);
    EXPECT_EQ(result.candidateCountWithSyntheticEdges, 0U);
    EXPECT_EQ(result.candidateCountWithBranchPenalty, 1U);
}

TEST(SearchPolySdkCapabilityTest, SearchPolygonsClassifiesBranchCleanupSyntheticEdges)
{
    const MultiPolyline2d lines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{-1.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 5.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 0.0}, Point2d{-1.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 0.0}, Point2d{0.0, -1.0}}, PolylineClosure::Open)};

    const auto result = SearchPolygons(lines);

    ASSERT_TRUE(result.IsSuccess());
    ASSERT_EQ(result.candidates.size(), 1U);
    const auto& candidate = result.candidates.front();
    ASSERT_EQ(candidate.inferredSyntheticEdgeKinds.size(), candidate.inferredSyntheticEdgeCount);
    ASSERT_GE(candidate.inferredSyntheticEdgeKinds.size(), 1U);
    for (const auto syntheticKind : candidate.inferredSyntheticEdgeKinds)
    {
        EXPECT_EQ(syntheticKind, SearchPolySyntheticEdgeKind2d::BranchCleanup);
    }
}

TEST(SearchPolySdkCapabilityTest, SearchPolygonsClassifiesMixedSyntheticEdges)
{
    const MultiPolyline2d lines{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{-1.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 5.0}}, PolylineClosure::Open)};

    const auto result = SearchPolygons(lines);

    ASSERT_TRUE(result.IsSuccess());
    ASSERT_EQ(result.candidates.size(), 1U);
    const auto& candidate = result.candidates.front();
    ASSERT_EQ(candidate.inferredSyntheticEdgeKinds.size(), candidate.inferredSyntheticEdgeCount);
    ASSERT_GE(candidate.inferredSyntheticEdgeKinds.size(), 1U);
    for (const auto syntheticKind : candidate.inferredSyntheticEdgeKinds)
    {
        EXPECT_EQ(syntheticKind, SearchPolySyntheticEdgeKind2d::Mixed);
    }
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
