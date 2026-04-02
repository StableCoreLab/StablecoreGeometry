#include <cassert>

#include <cmath>

#include <gtest/gtest.h>

#include "sdk/Geometry.h"
#include "support/Fixtures3d.h"

using geometry::sdk::BuildSectionComponents;
using geometry::sdk::BuildSectionTopology;
using geometry::sdk::ClassifySectionContent;
using geometry::sdk::ConvertToBrepBody;
using geometry::sdk::BrepConversionIssue3d;
using geometry::sdk::Plane;
using geometry::sdk::Point3d;
using geometry::sdk::PolyhedronBody;
using geometry::sdk::PolyhedronFace3d;
using geometry::sdk::PolyhedronLoop3d;
using geometry::sdk::Section;
using geometry::sdk::SectionContentKind3d;
using geometry::sdk::Vector3d;

namespace
{
PolyhedronBody BuildAdjacentCoplanarFaceBody()
{
    return PolyhedronBody({
        PolyhedronFace3d(
            Plane::FromPointAndNormal(Point3d{0.0, 0.0, 0.0}, Vector3d{0.0, 0.0, 1.0}),
            PolyhedronLoop3d({
                Point3d{0.0, 0.0, 0.0},
                Point3d{1.0, 0.0, 0.0},
                Point3d{1.0, 1.0, 0.0},
                Point3d{0.0, 1.0, 0.0},
            })),
        PolyhedronFace3d(
            Plane::FromPointAndNormal(Point3d{1.0, 0.0, 0.0}, Vector3d{0.0, 0.0, 1.0}),
            PolyhedronLoop3d({
                Point3d{1.0, 0.0, 0.0},
                Point3d{2.0, 0.0, 0.0},
                Point3d{2.0, 1.0, 0.0},
                Point3d{1.0, 1.0, 0.0},
            }))});
}

PolyhedronLoop3d TranslateLoop(const PolyhedronLoop3d& loop, const Vector3d& delta)
{
    std::vector<Point3d> vertices;
    vertices.reserve(loop.VertexCount());
    for (std::size_t i = 0; i < loop.VertexCount(); ++i)
    {
        vertices.push_back(loop.VertexAt(i) + delta);
    }
    return PolyhedronLoop3d(std::move(vertices));
}

PolyhedronFace3d TranslateFace(const PolyhedronFace3d& face, const Vector3d& delta)
{
    const Plane translatedPlane = Plane::FromPointAndNormal(
        face.SupportPlane().origin + delta,
        face.SupportPlane().normal);

    PolyhedronLoop3d outer = TranslateLoop(face.OuterLoop(), delta);
    std::vector<PolyhedronLoop3d> holes;
    holes.reserve(face.HoleCount());
    for (std::size_t i = 0; i < face.HoleCount(); ++i)
    {
        holes.push_back(TranslateLoop(face.HoleAt(i), delta));
    }

    return PolyhedronFace3d(translatedPlane, std::move(outer), std::move(holes));
}

PolyhedronBody BuildTwoSeparatedUnitCubesBody()
{
    const PolyhedronBody first = geometry::test::BuildUnitCubeBody();
    std::vector<PolyhedronFace3d> faces = first.Faces();

    const Vector3d delta{3.0, 0.0, 0.0};
    for (const PolyhedronFace3d& face : first.Faces())
    {
        faces.push_back(TranslateFace(face, delta));
    }

    return PolyhedronBody(std::move(faces));
}
} // namespace

TEST(Section3dCapabilityTest, SlantedCubeSectionBuildsSingleAreaComponent)
{
    const PolyhedronBody cubeBody = geometry::test::BuildUnitCubeBody();
    assert(cubeBody.IsValid());

    const Plane slantedCut = Plane::FromPointAndNormal(
        Point3d{0.0, 0.0, 0.5},
        Vector3d{1.0, 0.0, 1.0});
    const auto section = Section(cubeBody, slantedCut);
    assert(section.success);
    assert(section.IsValid());
    assert(!section.segments.empty());
    assert(!section.polygons.empty());

    const auto topology = BuildSectionTopology(section);
    assert(topology.IsValid());
    assert(topology.Count() == 1);
    assert(topology.Roots().size() == 1);

    const auto components = BuildSectionComponents(section);
    assert(components.IsValid());
    assert(components.components.size() == 1);
    assert(components.components.front().polygonIndices.size() == 1);
    assert(components.components.front().faceIndices.size() == 1);

    assert(ClassifySectionContent(section) == SectionContentKind3d::Area);
}


// Demonstrates non-axis-aligned multi-face sectioning remains stable on a
// generic cube cut: contour/polygon counts stay deterministic.
TEST(Section3dCapabilityTest, NonAxisAlignedCubeSectionHasSingleHexLikeContour)
{
    const PolyhedronBody cubeBody = geometry::test::BuildUnitCubeBody();
    assert(cubeBody.IsValid());

    const Plane cut = Plane::FromPointAndNormal(
        Point3d{0.5, 0.5, 0.5},
        Vector3d{1.0, 1.0, 1.0});
    const auto section = Section(cubeBody, cut);
    assert(section.success);
    assert(section.IsValid());

    assert(section.polygons.size() == 1);
    assert(section.contours.size() == 1);
    assert(section.contours[0].closed);
    // For x+y+z=1.5 slicing a unit cube, the expected closed contour has 6 corners.
    assert(section.contours[0].points.size() == 6);

    const auto topology = BuildSectionTopology(section);
    assert(topology.IsValid());
    assert(topology.Roots().size() == 1);
    assert(ClassifySectionContent(section) == SectionContentKind3d::Area);
}

// Demonstrates Section(BrepBody, Plane) remains stable on an oblique cut:
// after Polyhedron->Brep conversion, sectioning still yields one closed area
// contour with deterministic vertex count.
TEST(Section3dCapabilityTest, BrepBodyObliqueSectionHasSingleHexLikeContour)
{
    const PolyhedronBody cubeBody = geometry::test::BuildUnitCubeBody();
    assert(cubeBody.IsValid());

    const auto converted = ConvertToBrepBody(cubeBody);
    assert(converted.success);
    assert(converted.issue == BrepConversionIssue3d::None);
    assert(converted.body.IsValid());

    const Plane cut = Plane::FromPointAndNormal(
        Point3d{0.5, 0.5, 0.5},
        Vector3d{1.0, 1.0, 1.0});
    const auto section = Section(converted.body, cut);
    assert(section.success);
    assert(section.IsValid());

    assert(section.polygons.size() == 1);
    assert(section.contours.size() == 1);
    assert(section.contours[0].closed);
    assert(section.contours[0].points.size() == 6);

    const auto topology = BuildSectionTopology(section);
    assert(topology.IsValid());
    assert(topology.Roots().size() == 1);

    const auto components = BuildSectionComponents(section);
    assert(components.IsValid());
    assert(components.components.size() == 1);
    assert(ClassifySectionContent(section) == SectionContentKind3d::Area);
}

// Demonstrates coplanar fragment merge also holds for Section(BrepBody,
// Plane): after Polyhedron->Brep conversion, adjacent coplanar faces are still
// merged into one area polygon instead of staying split.
TEST(Section3dCapabilityTest, BrepBodyAdjacentCoplanarFacesMergeIntoSinglePolygon)
{
    const PolyhedronBody polyBody = BuildAdjacentCoplanarFaceBody();
    assert(polyBody.IsValid());
    assert(polyBody.FaceCount() == 2);

    const auto converted = ConvertToBrepBody(polyBody);
    assert(converted.success);
    assert(converted.issue == BrepConversionIssue3d::None);
    assert(converted.body.IsValid());

    const Plane cut = Plane::FromPointAndNormal(
        Point3d{0.0, 0.0, 0.0},
        Vector3d{0.0, 0.0, 1.0});
    const auto section = Section(converted.body, cut);
    assert(section.success);
    assert(section.IsValid());
    assert(section.polygons.size() == 1);
    assert(section.contours.size() == 1);
    assert(section.contours[0].closed);
    assert(section.contours[0].points.size() == 4);

    const auto topology = BuildSectionTopology(section);
    assert(topology.IsValid());
    assert(topology.Roots().size() == 1);

    const auto components = BuildSectionComponents(section);
    assert(components.IsValid());
    assert(components.components.size() == 1);
    assert(ClassifySectionContent(section) == SectionContentKind3d::Area);
    assert(std::abs(geometry::sdk::Area(section.polygons[0]) - 2.0) < 1e-12);
}

// Demonstrates Section(BrepBody, Plane) also supports multi-component area
// output: two separated cubes sectioned at z=0.5 produce two independent
// section polygons/components.
TEST(Section3dCapabilityTest, BrepBodySectionBuildsTwoAreaComponents)
{
    const PolyhedronBody polyBody = BuildTwoSeparatedUnitCubesBody();
    assert(polyBody.IsValid());
    assert(polyBody.FaceCount() == 12);

    const auto converted = ConvertToBrepBody(polyBody);
    assert(converted.success);
    assert(converted.issue == BrepConversionIssue3d::None);
    assert(converted.body.IsValid());

    const Plane cut = Plane::FromPointAndNormal(
        Point3d{0.0, 0.0, 0.5},
        Vector3d{0.0, 0.0, 1.0});
    const auto section = Section(converted.body, cut);
    assert(section.success);
    assert(section.IsValid());
    assert(section.polygons.size() == 2);
    assert(section.contours.size() == 2);
    assert(section.contours[0].closed);
    assert(section.contours[1].closed);

    const auto topology = BuildSectionTopology(section);
    assert(topology.IsValid());
    assert(topology.Roots().size() == 2);

    const auto components = BuildSectionComponents(section);
    assert(components.IsValid());
    assert(components.components.size() == 2);
    assert(ClassifySectionContent(section) == SectionContentKind3d::Area);
}

// Demonstrates coplanar adjacent face fragments are merged into one area
// polygon instead of remaining as two disjoint coplanar pieces.
TEST(Section3dCapabilityTest, AdjacentCoplanarFacesMergeIntoSingleSectionPolygon)
{
    const PolyhedronBody body = BuildAdjacentCoplanarFaceBody();
    assert(body.IsValid());
    assert(body.FaceCount() == 2);

    const Plane cut = Plane::FromPointAndNormal(
        Point3d{0.0, 0.0, 0.0},
        Vector3d{0.0, 0.0, 1.0});
    const auto section = Section(body, cut);
    assert(section.success);
    assert(section.IsValid());
    assert(section.polygons.size() == 1);
    assert(section.contours.size() == 1);
    assert(section.contours[0].closed);
    assert(section.contours[0].points.size() == 4);

    const auto topology = BuildSectionTopology(section);
    assert(topology.IsValid());
    assert(topology.Roots().size() == 1);

    const auto components = BuildSectionComponents(section);
    assert(components.IsValid());
    assert(components.components.size() == 1);
    assert(components.components[0].faceIndices.size() == 1);
    assert(ClassifySectionContent(section) == SectionContentKind3d::Area);
    assert(std::abs(geometry::sdk::Area(section.polygons[0]) - 2.0) < 1e-12);
}