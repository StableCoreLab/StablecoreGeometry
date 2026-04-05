#include <gtest/gtest.h>

#include "sdk/Geometry.h"

using geometry::sdk::LineSegment3d;
using geometry::sdk::Plane;
using geometry::sdk::Point3d;
using geometry::sdk::PolyhedronBody;
using geometry::sdk::Section;
using geometry::sdk::Vector3d;

TEST(Section3dGapTest, NonPlanarDominantSectionGraphRemainsOpen)
{
    const PolyhedronBody body;
    const Plane cuttingPlane = Plane::FromPointAndNormal(
        Point3d{0.0, 0.0, 0.1},
        Vector3d{0.0, 0.0, 1.0});

    const auto section = Section(body, cuttingPlane);
    (void)section;
    GTEST_SKIP() << "Known 3D gap: non-planar dominant section graph and ambiguous non-manifold contour stitching semantics in non-planar-dominant sections are still open. "
                    "Covered subsets: convex-body oblique-face-to-face mid-plane stitching (four-segment closed-contour determinism on unit-cube y=0.5 cut), "
                    "triangular-prism mid-section (three-segment closed-contour determinism, perimeter~=3), "
                    "triangular-prism mid-section on BrepBody path (three-segment closed-contour determinism, perimeter~=3), "
                    "unit-cube x-axis section (four-segment 1x1 square contour, perimeter=4), "
                    "unit-cube x-axis section on BrepBody path (four-segment 1x1 square contour, perimeter=4), "
                    "rectangular-prism z-axis section (four-segment 2x2 square contour, perimeter=8), "
                    "rectangular-prism z-axis section on BrepBody path (four-segment 2x2 square contour, perimeter=8), "
                    "rectangular-prism x-axis section on PolyhedronBody path (four-segment 2x1 rectangle contour, perimeter=6), "
                    "rectangular-prism x-axis section on BrepBody path (four-segment 2x1 rectangle contour, perimeter=6), "
                    "unit-cube y-axis section on BrepBody path (four-segment 1x1 square contour, perimeter=4), "
                    "rectangular-prism y-axis section on PolyhedronBody path (four-segment 2x1 rectangle contour, perimeter=6), "
                    "rectangular-prism y-axis section on BrepBody path (four-segment 2x1 rectangle contour, perimeter=6), "
                    "unit-cube z-axis section on PolyhedronBody path (four-segment 1x1 square contour, perimeter=4), "
                    "unit-cube z-axis section on BrepBody path (four-segment 1x1 square contour, perimeter=4), "
                    "mixed coplanar-frame + non-planar cube section on PolyhedronBody path (2 polygons / 3 closed contours / total area=9), "
                    "mixed coplanar-frame + non-planar cube section on BrepBody path (2 polygons / 3 closed contours / total area=9), "
                    "mixed closed-area + detached open-contour section on PolyhedronBody path (1 polygon + 1 open contour / mixed content), "
                    "and mixed closed-area + detached open-contour section on BrepBody path (1 polygon + 1 open contour / mixed content), "
                    "plus vertex-attached and edge-attached open-contour mixed-content on PolyhedronBody / BrepBody paths "
                    "(1 polygon + 1 open contour / mixed content), "
                    "and edge-adjacent mixed coplanar + non-planar area merge on PolyhedronBody / BrepBody paths "
                    "(single merged polygon / area=2).";
}

TEST(Section3dGapTest, FaceMergeSemanticsAfterSectionRemainsOpen)
{
    const PolyhedronBody body;
    const Plane cuttingPlane = Plane::FromPointAndNormal(
        Point3d{0.0, 0.0, 0.0},
        Vector3d{0.0, 0.0, 1.0});

    const auto section = Section(body, cuttingPlane);
    (void)section;
    GTEST_SKIP() << "Known 3D gap: section face-merge policy for ambiguous coplanar fragments beyond adjacent coplanar union subsets is not closed yet. "
                    "Covered subsets: two-face adjacent coplanar Polyhedron merge, two-face adjacent coplanar BrepBody merge, "
                    "three-face coplanar horizontal strip merge (3x1 rectangle, area=3.0), three-face coplanar horizontal strip merge on BrepBody path (area=3.0), "
                    "and four-face coplanar frame merge into a single polygon-with-hole (area=8.0). "
                    "Remaining open: non-adjacent fragment merge across convex-hull gaps, mixed open-curve/area edge-adjacency arbitration beyond representative detached / vertex-attached / edge-attached subsets, "
                    "and more general mixed coplanar/non-planar adjacency arbitration beyond the representative edge-adjacent single-merge subset.";
}
