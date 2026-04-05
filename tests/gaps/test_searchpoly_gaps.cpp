#include <gtest/gtest.h>

#include "sdk/GeometrySearchPoly.h"

TEST(SearchPolyGapTest, DelphiGradeAmbiguousBranchScoringRemainsOpen)
{
    GTEST_SKIP() << "Known 2D gap: GeometrySearchPoly now exposes stable SDK entry points, "
                    "candidate ranking, branch scoring, candidate-level fake-edge diagnostics, "
                    "edge-level synthetic explanation (kind/source/vertex-identity/touch-mapping), "
                    "top-candidate / runner-up / ambiguous-top synthetic-source summary, "
                    "ambiguous-top summary explanation/counts, result/diagnostics consistency, and auto-flag gating, but Delphi-grade "
                    "ambiguous recovery, richer fake-edge explanation, and full smart-search "
                    "parity remain open.";
}

TEST(SearchPolyGapTest, SearchPolygonsExplainsWhySyntheticRunnerUpLostToCleanWinner)
{
    GTEST_SKIP() << "Known 2D gap: top-candidate explanation still does not fully narrate why a synthetic runner-up lost to a clean winner in a way that matches Delphi-grade smart-search output. "
                    "Expected future capability: explain the exact penalty delta and synthetic-source cause without requiring a caller to rescan candidates.";
}

TEST(SearchPolyGapTest, SearchPolygonsReportsAmbiguousRecoveryWhenTwoCandidatesTieAfterSyntheticPenaltyNormalization)
{
    GTEST_SKIP() << "Known 2D gap: ambiguous recovery for tied candidates after synthetic penalty normalization is still not a complete Delphi-grade contract. "
                    "Expected future capability: return a stable ambiguous-recovery explanation that names the tied candidates and their synthetic-source profile.";
}
