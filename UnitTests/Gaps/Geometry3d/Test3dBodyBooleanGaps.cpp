#include <gtest/gtest.h>

#include "Brep/BodyBoolean.h"
#include "Brep/PolyhedronBody.h"



using Geometry::BodyBooleanIssue3d;
using Geometry::IntersectBodies;
using Geometry::PolyhedronBody;

TEST(BodyBoolean3dGapTest, DelphiGradeBodyAndShellBooleanRemainOpen)
{
    const PolyhedronBody first;
    const PolyhedronBody second;
    const auto result = IntersectBodies(first, second);
    (void)result;

    GTEST_SKIP() << "Known 3D gap: Delphi-grade body/shell boolean workflow remains open. "
                    "Current fast-track now covers invalid-input contract, deterministic "
                    "identical/disjoint closed-body subsets including identical difference-empty, axis-aligned contained-body "
                    "intersection/union subsets, axis-aligned single-box overlap "
                    "subsets, face-touching axis-aligned box union subsets whose result remains "
                    "one closed box, face-touching external difference subsets that preserve "
                    "the original closed box, axis-aligned contained difference-empty subsets, "
                    "axis-aligned edge/vertex-touching ordered multi-body union subsets, axis-aligned edge/vertex-touching external difference subsets, "
                    "explicit unsupported contract for face-touching L-shaped non-box unions, explicit unsupported contract for rotated-box positive-volume intersection, "
                    "and deterministic disjoint ordered-multi-body union / disjoint / axis-aligned face/edge/vertex-touching empty "
                    "intersection subsets. Non-box overlap, non-axis-aligned or richer touching "
                    "intersection semantics, "
                    "shell-policy variants, and topology-preserving healing integration are still "
                    "pending.";
}

TEST(BodyBoolean3dGapTest, ContainedShellPolicyOptionStillHasNoEffectAndStaysGap)
{
    GTEST_SKIP() << "Known 3D gap: shell-policy options are still not part of the public boolean contract. "
                    "Representative unresolved scenario: a contained or face-touching axis-aligned pair evaluated with operateOnShells=true still follows the closed-body fast path, "
                    "so the option does not yet advertise or prove any deterministic shell-level semantic effect. "
                    "Expected future capability: either prove the option changes result selection/diagnostics deterministically or keep it explicitly unsupported.";
}






