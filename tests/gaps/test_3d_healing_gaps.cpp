#include <gtest/gtest.h>

#include "sdk/Geometry.h"

using geometry::sdk::BrepBody;
using geometry::sdk::Heal;
using geometry::sdk::MeshHealing3d;
using geometry::sdk::TriangleMesh;

TEST(Healing3dGapTest, AggressiveShellRepairPolicyRemainsOpen)
{
    const BrepBody body({});
    const auto healing = Heal(body);
    (void)healing;
    GTEST_SKIP() << "Known 3D gap: general topology-changing aggressive shell repair policy remains open beyond the current deterministic planar subsets: single/multi-face open shells, holed shells, multi-shell closure, mixed closed/open-shell repair, partial mixed-shell repair, coplanar shared-edge boundary-cap closure (including standalone single-shell subsets, support-mismatch + trim-backfill + holed representative subset, mixed-body closed-shell + eligible-shared-edge-shell subset, conservative competing-shared-boundary-edge arbitration, duplicated-topology geometrically coincident shared-boundary-loop arbitration, vertex-touching eligible multi-shell closure without shared boundary edges, the representative competing-pair-plus-vertex-touch shell arbitration subset, the representative independent-plus-competing-pair-plus-vertex-touch four-shell subset, and the representative mixed closed-shell + competing-pair + vertex-touch shell subset), three-shell deterministic mixed behavior, three-shell+trim-backfill interplay, three-shell with eligible multi-face open-sheet, three-shell with eligible holed-shell missing trims, three-shell with eligible multi-face holed+missing-trims, three-shell with eligible multi-face both-trims-missing, three-shell with eligible support-plane mismatch composition, support-mismatch eligible + ineligible multiface coexistence, support-mismatch+trim-backfill+ineligible-multiface composition, support-mismatch holed-eligible + ineligible-multiface composition, support-mismatch eligible-multiface-missing-trims + ineligible-multiface composition, and four-shell two-eligible-plus-one-ineligible deterministic behavior. Non-horizontal trim backfill now covers z=0 (horizontal), y=0 (+y normal), x=0 (+x normal), and oblique planar orientations.";
}

TEST(Healing3dGapTest, MultiStepMeshBodyJointHealingRemainsOpen)
{
    const TriangleMesh mesh({}, {});
    const MeshHealing3d meshHealing = Heal(mesh);
    (void)meshHealing;
    GTEST_SKIP() << "Known 3D gap: multi-step mesh/body joint healing workflow is still open.";
}

TEST(Healing3dGapTest, AggressiveHealingSkipsPartiallyOverlappedBoundaryLoopShells)
{
    GTEST_SKIP() << "Known 3D gap: two eligible open shells whose boundary loops only partially overlap still need explicit competing-shell arbitration. "
                    "Expected future capability: keep both shells open instead of boundary-capping them into overlapping or self-intersecting closures.";
}

TEST(Healing3dGapTest, AggressiveHealingClosesIndependentShellWhileSkippingPartialOverlapPair)
{
    GTEST_SKIP() << "Known 3D gap: a mixed body containing one independent eligible shell plus a partially overlapped competing pair still needs local arbitration coverage. "
                    "Expected future capability: close the independent shell while preserving the partial-overlap pair as open.";
}

TEST(Healing3dGapTest, AggressiveHealingRejectsNonPlanarSharedEdgeShellForBoundaryCap)
{
    GTEST_SKIP() << "Known 3D gap: a shell that shares boundary edges but is not globally planar should not be boundary-capped by the planar aggressive path. "
                    "Expected future capability: keep the shell open and surface the gap explicitly instead of silent fallback.";
}
