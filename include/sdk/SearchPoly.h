#pragma once

#include <cstddef>
#include <optional>
#include <vector>

#include "export/GeometryExport.h"
#include "sdk/GeometryTypes.h"
#include "sdk/LineSegment2d.h"
#include "sdk/MultiPolygon2d.h"
#include "sdk/MultiPolyline2d.h"

namespace Geometry::Sdk
{
enum class SearchPolyIssue2d
{
    None,
    InvalidInput,
    NoClosedPolygonFound
};

enum class SearchPolyPenaltyKind2d
{
    None,
    SyntheticClosure,
    BranchPenalty,
    SyntheticBranchPenalty,
    Mixed
};

enum class SearchPolySyntheticEdgeKind2d
{
    Unknown,
    GapClosure,
    BranchCleanup,
    Mixed
};

enum class SearchPolySyntheticEdgeSource2d
{
    Unknown,
    SingleGapClose,
    BranchCleanup,
    MixedBridge
};

struct GEOMETRY_API SearchPolyDiagnostics2d
{
    std::size_t inputPolylineCount{0};
    std::size_t inputSegmentCount{0};
    std::size_t uniqueVertexCount{0};
    std::size_t danglingEndpointCount{0};
    std::size_t branchVertexCount{0};
    std::size_t inferredSyntheticEdgeCount{0};

    [[nodiscard]] bool RequiresRepair() const
    {
        return danglingEndpointCount > 0 || branchVertexCount > 0;
    }
};

struct GEOMETRY_API SearchPolyOptions2d
{
    double epsilon{1e-9};
    bool autoClose{true};
    bool autoExtend{true};
    bool allowFakeEdges{true};
    bool removeBranches{true};
};

struct GEOMETRY_API SearchPolyCandidate2d
{
    Polygon2d polygon{};
    double absoluteArea{0.0};
    double branchScore{0.0};
    double inferredSyntheticPerimeter{0.0};
    std::size_t holeCount{0};
    std::size_t inferredSyntheticEdgeCount{0};
    std::size_t branchVertexCount{0};
    std::size_t syntheticBranchVertexCount{0};
    SearchPolyPenaltyKind2d dominantPenaltyKind{SearchPolyPenaltyKind2d::None};
    SearchPolySyntheticEdgeKind2d dominantSyntheticEdgeKind{SearchPolySyntheticEdgeKind2d::Unknown};
    SearchPolySyntheticEdgeSource2d dominantSyntheticEdgeSource{SearchPolySyntheticEdgeSource2d::Unknown};
    std::vector<LineSegment2d> inferredSyntheticEdges{};
    std::vector<SearchPolySyntheticEdgeKind2d> inferredSyntheticEdgeKinds{};
    std::vector<SearchPolySyntheticEdgeSource2d> inferredSyntheticEdgeSources{};
    std::vector<std::size_t> inferredSyntheticEdgeStartVertexIndices{};
    std::vector<std::size_t> inferredSyntheticEdgeEndVertexIndices{};
    std::vector<std::size_t> inferredSyntheticEdgeStartDegrees{};
    std::vector<std::size_t> inferredSyntheticEdgeEndDegrees{};
    std::vector<std::size_t> inferredSyntheticEdgeDanglingTouchCounts{};
    std::vector<std::size_t> inferredSyntheticEdgeBranchTouchCounts{};
    std::vector<double> inferredSyntheticEdgeLengths{};
    std::size_t rank{0};

    [[nodiscard]] bool IsValid() const
    {
        return polygon.IsValid() && absoluteArea >= 0.0 && inferredSyntheticPerimeter >= 0.0;
    }
};

struct GEOMETRY_API SearchPolyResult2d
{
    SearchPolyIssue2d issue{SearchPolyIssue2d::None};
    MultiPolygon2d polygons{};
    std::vector<SearchPolyCandidate2d> candidates{};
    SearchPolyDiagnostics2d diagnostics{};
    double bestCandidateScoreMargin{0.0};
    double bestCandidateSyntheticPerimeter{0.0};
    std::size_t bestCandidateSyntheticEdgeCount{0};
    SearchPolySyntheticEdgeKind2d bestCandidateSyntheticEdgeKind{SearchPolySyntheticEdgeKind2d::Unknown};
    SearchPolySyntheticEdgeSource2d bestCandidateSyntheticEdgeSource{SearchPolySyntheticEdgeSource2d::Unknown};
    double runnerUpSyntheticPerimeter{0.0};
    std::size_t runnerUpSyntheticEdgeCount{0};
    SearchPolySyntheticEdgeKind2d runnerUpSyntheticEdgeKind{SearchPolySyntheticEdgeKind2d::Unknown};
    SearchPolySyntheticEdgeSource2d runnerUpSyntheticEdgeSource{SearchPolySyntheticEdgeSource2d::Unknown};
    std::size_t runnerUpBranchVertexCount{0};
    SearchPolyPenaltyKind2d bestCandidatePenaltyKind{SearchPolyPenaltyKind2d::None};
    SearchPolyPenaltyKind2d runnerUpPenaltyKind{SearchPolyPenaltyKind2d::None};
    SearchPolyPenaltyKind2d ambiguousTopPenaltyKind{SearchPolyPenaltyKind2d::None};
    SearchPolySyntheticEdgeKind2d ambiguousTopSyntheticEdgeKind{SearchPolySyntheticEdgeKind2d::Unknown};
    SearchPolySyntheticEdgeSource2d ambiguousTopSyntheticEdgeSource{SearchPolySyntheticEdgeSource2d::Unknown};
    std::size_t ambiguousTopCandidateCountWithSyntheticEdges{0};
    std::size_t ambiguousTopCandidateCountWithBranchPenalty{0};
    bool bestCandidateBeatsSyntheticRunnerUp{false};
    bool bestCandidateBeatsBranchRunnerUp{false};
    std::size_t candidateCountWithSyntheticEdges{0};
    std::size_t candidateCountWithBranchPenalty{0};
    std::size_t ambiguousTopCandidateCount{0};
    bool usedAutoClose{false};
    bool usedAutoExtend{false};
    bool usedSyntheticEdges{false};
    bool usedBranchScoring{false};

    [[nodiscard]] bool IsSuccess() const
    {
        return issue == SearchPolyIssue2d::None && polygons.Count() > 0;
    }
};

[[nodiscard]] GEOMETRY_API SearchPolyResult2d SearchPolygons(
    const MultiPolyline2d& lines,
    SearchPolyOptions2d options = {});

[[nodiscard]] GEOMETRY_API std::optional<SearchPolyCandidate2d> SearchPolygonContainingPoint(
    const MultiPolyline2d& lines,
    const Point2d& point,
    SearchPolyOptions2d options = {});
} // namespace Geometry::Sdk
