#include <gtest/gtest.h>

#include "sdk/GeometryBodyBoolean.h"
#include "sdk/PolyhedronBody.h"

using geometry::sdk::BodyBooleanIssue3d;
using geometry::sdk::IntersectBodies;
using geometry::sdk::PolyhedronBody;

TEST(BodyBoolean3dGapTest, DelphiGradeBodyAndShellBooleanRemainOpen)
{
    const PolyhedronBody first;
    const PolyhedronBody second;
    const auto result = IntersectBodies(first, second);
    (void)result;

    GTEST_SKIP() << "Known 3D gap: Delphi-grade body/shell boolean workflow remains open. "
                    "Current fast-track now covers invalid-input contract, deterministic "
                    "identical/disjoint closed-body subsets, axis-aligned single-box overlap "
                    "subsets, face-touching axis-aligned box union subsets whose result remains "
                    "one closed box, face-touching external difference subsets that preserve "
                    "the original closed box, axis-aligned contained difference-empty subsets, "
                    "and axis-aligned face/edge/vertex-touching empty "
                    "intersection subsets. Non-box overlap, non-axis-aligned or richer touching "
                    "intersection semantics, "
                    "shell-policy variants, and topology-preserving healing integration are still "
                    "pending.";
}
