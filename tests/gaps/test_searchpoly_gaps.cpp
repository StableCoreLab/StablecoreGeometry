#include <gtest/gtest.h>

#include "sdk/GeometrySearchPoly.h"

TEST(SearchPolyGapTest, DelphiGradeAmbiguousBranchScoringRemainsOpen)
{
    GTEST_SKIP() << "Known 2D gap: GeometrySearchPoly now exposes stable SDK entry points, "
                    "candidate ranking, branch scoring, candidate-level fake-edge diagnostics, "
                    "edge-level synthetic explanation (kind/source/vertex-identity/touch-mapping), "
                    "result/diagnostics consistency, and auto-flag gating, but Delphi-grade "
                    "ambiguous recovery, richer fake-edge explanation, and full smart-search "
                    "parity remain open.";
}
