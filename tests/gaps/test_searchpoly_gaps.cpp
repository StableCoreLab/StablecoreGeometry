#include <gtest/gtest.h>

#include "sdk/GeometrySearchPoly.h"

TEST(SearchPolyGapTest, DelphiGradeAmbiguousBranchScoringRemainsOpen)
{
    GTEST_SKIP() << "Known 2D gap: GeometrySearchPoly now exposes stable SDK entry points, "
                    "candidate ranking, branch scoring, candidate-level fake-edge diagnostics, "
                    "edge-level synthetic explanation (kind/source/vertex-identity/touch-mapping), "
                    "top-candidate / runner-up / ambiguous-top synthetic-source summary, "
                    "ambiguous-top summary explanation/counts, clean-winner-vs-synthetic-runner-up causal explanation, result/diagnostics consistency, and auto-flag gating, but Delphi-grade "
                    "ambiguous recovery, richer fake-edge explanation, and full smart-search "
                    "parity remain open.";
}

TEST(SearchPolyGapTest, SearchPolygonsReportsAmbiguousRecoveryWhenTwoCandidatesTieAfterSyntheticPenaltyNormalization)
{
    GTEST_SKIP() << "Known 2D gap: ambiguous recovery for tied candidates after synthetic penalty normalization is still not a complete Delphi-grade contract. "
                    "Representative unresolved scenario: two top-ranked candidates tie on normalized branch score, both require synthetic edges, but their dominant synthetic sources differ "
                    "(for example SingleGapClose vs MixedBridge / BranchCleanup). Current stable summaries can already report ambiguous-top counts and a mixed synthetic-source summary, "
                    "but they still do not directly identify which tied candidate carried which competing recovery profile. "
                    "Expected future capability: return a stable ambiguous-recovery explanation that names the tied candidates and makes their competing synthetic-source profile directly readable.";
}
