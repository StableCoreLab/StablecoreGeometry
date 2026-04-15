#include <gtest/gtest.h>

#include <cmath>
#include <vector>

#include "sdk/BodyBoolean.h"
#include "sdk/BrepConversion.h"
#include "support/Fixtures3d.h"

using Geometry::Sdk::BodyBooleanIssue3d;
using Geometry::Sdk::BodyBooleanOptions3d;
using Geometry::Sdk::DifferenceBodies;
using Geometry::Sdk::IntersectBodies;
using Geometry::Sdk::PolyhedronBody;
using Geometry::Sdk::Point3d;
using Geometry::Sdk::PolyhedronFace3d;
using Geometry::Sdk::PolyhedronLoop3d;
using Geometry::Sdk::UnionBodies;
using Geometry::Sdk::Plane;
using Geometry::Sdk::Vector3d;

namespace
{
[[nodiscard]] PolyhedronBody BuildAxisAlignedBoxBody(
    double minX,
    double minY,
    double minZ,
    double maxX,
    double maxY,
    double maxZ)
{
    return PolyhedronBody(
        {
            PolyhedronFace3d(
                Plane::FromPointAndNormal(Point3d{minX, minY, minZ}, Vector3d{0.0, 0.0, -1.0}),
                PolyhedronLoop3d(
                    {
                        Point3d{minX, minY, minZ},
                        Point3d{minX, maxY, minZ},
                        Point3d{maxX, maxY, minZ},
                        Point3d{maxX, minY, minZ},
                    })),
            PolyhedronFace3d(
                Plane::FromPointAndNormal(Point3d{minX, minY, maxZ}, Vector3d{0.0, 0.0, 1.0}),
                PolyhedronLoop3d(
                    {
                        Point3d{minX, minY, maxZ},
                        Point3d{maxX, minY, maxZ},
                        Point3d{maxX, maxY, maxZ},
                        Point3d{minX, maxY, maxZ},
                    })),
            PolyhedronFace3d(
                Plane::FromPointAndNormal(Point3d{minX, minY, minZ}, Vector3d{0.0, -1.0, 0.0}),
                PolyhedronLoop3d(
                    {
                        Point3d{minX, minY, minZ},
                        Point3d{maxX, minY, minZ},
                        Point3d{maxX, minY, maxZ},
                        Point3d{minX, minY, maxZ},
                    })),
            PolyhedronFace3d(
                Plane::FromPointAndNormal(Point3d{maxX, minY, minZ}, Vector3d{1.0, 0.0, 0.0}),
                PolyhedronLoop3d(
                    {
                        Point3d{maxX, minY, minZ},
                        Point3d{maxX, maxY, minZ},
                        Point3d{maxX, maxY, maxZ},
                        Point3d{maxX, minY, maxZ},
                    })),
            PolyhedronFace3d(
                Plane::FromPointAndNormal(Point3d{minX, maxY, minZ}, Vector3d{0.0, 1.0, 0.0}),
                PolyhedronLoop3d(
                    {
                        Point3d{minX, maxY, minZ},
                        Point3d{minX, maxY, maxZ},
                        Point3d{maxX, maxY, maxZ},
                        Point3d{maxX, maxY, minZ},
                    })),
            PolyhedronFace3d(
                Plane::FromPointAndNormal(Point3d{minX, minY, minZ}, Vector3d{-1.0, 0.0, 0.0}),
                PolyhedronLoop3d(
                    {
                        Point3d{minX, minY, minZ},
                        Point3d{minX, minY, maxZ},
                        Point3d{minX, maxY, maxZ},
                        Point3d{minX, maxY, minZ},
                    })),
        });
}

[[nodiscard]] PolyhedronBody BuildTranslatedUnitCubeBody(double dx, double dy, double dz)
{
    return BuildAxisAlignedBoxBody(dx + 0.0, dy + 0.0, dz + 0.0, dx + 1.0, dy + 1.0, dz + 1.0);
}

[[nodiscard]] Point3d RotatePointAroundZ(const Point3d& point, const Point3d& origin, double angleRadians)
{
    const double cosine = std::cos(angleRadians);
    const double sine = std::sin(angleRadians);
    const double dx = point.x - origin.x;
    const double dy = point.y - origin.y;
    return Point3d{
        origin.x + dx * cosine - dy * sine,
        origin.y + dx * sine + dy * cosine,
        point.z};
}

[[nodiscard]] Vector3d RotateVectorAroundZ(const Vector3d& vector, double angleRadians)
{
    const double cosine = std::cos(angleRadians);
    const double sine = std::sin(angleRadians);
    return Vector3d{
        vector.x * cosine - vector.y * sine,
        vector.x * sine + vector.y * cosine,
        vector.z};
}

[[nodiscard]] PolyhedronLoop3d RotateLoopAroundZ(const PolyhedronLoop3d& loop, const Point3d& origin, double angleRadians)
{
    std::vector<Point3d> vertices;
    vertices.reserve(loop.VertexCount());
    for (std::size_t i = 0; i < loop.VertexCount(); ++i)
    {
        vertices.push_back(RotatePointAroundZ(loop.VertexAt(i), origin, angleRadians));
    }
    return PolyhedronLoop3d(std::move(vertices));
}

[[nodiscard]] PolyhedronFace3d RotateFaceAroundZ(const PolyhedronFace3d& face, const Point3d& origin, double angleRadians)
{
    const Plane rotatedPlane = Plane::FromPointAndNormal(
        RotatePointAroundZ(face.SupportPlane().origin, origin, angleRadians),
        RotateVectorAroundZ(face.SupportPlane().normal, angleRadians));

    PolyhedronLoop3d outer = RotateLoopAroundZ(face.OuterLoop(), origin, angleRadians);
    std::vector<PolyhedronLoop3d> holes;
    holes.reserve(face.HoleCount());
    for (std::size_t i = 0; i < face.HoleCount(); ++i)
    {
        holes.push_back(RotateLoopAroundZ(face.HoleAt(i), origin, angleRadians));
    }

    return PolyhedronFace3d(rotatedPlane, std::move(outer), std::move(holes));
}

[[nodiscard]] PolyhedronBody BuildRotatedOverlapBoxBody()
{
    const PolyhedronBody box = BuildAxisAlignedBoxBody(0.25, 0.0, 0.0, 1.25, 1.0, 1.0);
    std::vector<PolyhedronFace3d> faces;
    faces.reserve(box.FaceCount());

    const Point3d rotationOrigin{0.75, 0.5, 0.0};
    const double angleRadians = std::acos(-1.0) * 0.25;
    for (const PolyhedronFace3d& face : box.Faces())
    {
        faces.push_back(RotateFaceAroundZ(face, rotationOrigin, angleRadians));
    }

    return PolyhedronBody(std::move(faces));
}
} // namespace

TEST(BodyBooleanSdkCapabilityTest, InvalidInputContractIsStableForEmptyBodies)
{
    const PolyhedronBody first;
    const PolyhedronBody second;
    const BodyBooleanOptions3d options{};

    const auto intersection = IntersectBodies(first, second, options);
    const auto united = UnionBodies(first, second, options);
    const auto difference = DifferenceBodies(first, second, options);

    EXPECT_EQ(intersection.issue, BodyBooleanIssue3d::InvalidInput);
    EXPECT_EQ(united.issue, BodyBooleanIssue3d::InvalidInput);
    EXPECT_EQ(difference.issue, BodyBooleanIssue3d::InvalidInput);
    EXPECT_FALSE(intersection.IsSuccess());
    EXPECT_FALSE(united.IsSuccess());
    EXPECT_FALSE(difference.IsSuccess());
}

TEST(BodyBooleanSdkCapabilityTest, IdenticalPolyhedronIntersectionReturnsClosedBody)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = Geometry::Test::BuildUnitCubeBody();

    const auto result = IntersectBodies(first, second);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_EQ(result.body.VertexCount(), 8U);
    EXPECT_EQ(result.body.EdgeCount(), 12U);
    EXPECT_EQ(result.body.ShellCount(), 1U);
    EXPECT_TRUE(result.body.ShellAt(0).IsClosed());
}

TEST(BodyBooleanSdkCapabilityTest, IdenticalPolyhedronUnionReturnsSingleClosedBody)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = Geometry::Test::BuildUnitCubeBody();

    const auto result = UnionBodies(first, second);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_TRUE(result.body.ShellAt(0).IsClosed());
    EXPECT_TRUE(result.bodies.empty());
}

TEST(BodyBooleanSdkCapabilityTest, IdenticalPolyhedronDifferenceReturnsDeterministicEmptyResult)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = Geometry::Test::BuildUnitCubeBody();

    const auto result = DifferenceBodies(first, second);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    EXPECT_TRUE(result.producedEmptyResult);
    EXPECT_EQ(result.body.FaceCount(), 0U);
    EXPECT_TRUE(result.bodies.empty());
}

TEST(BodyBooleanSdkCapabilityTest, DisjointPolyhedronUnionReturnsTwoBodies)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = BuildTranslatedUnitCubeBody(3.0, 0.0, 0.0);

    const auto result = UnionBodies(first, second);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    EXPECT_EQ(result.bodies.size(), 2U);
    EXPECT_EQ(result.body.FaceCount(), 0U);
    EXPECT_EQ(result.bodies[0].FaceCount(), 6U);
    EXPECT_EQ(result.bodies[1].FaceCount(), 6U);
}

TEST(BodyBooleanSdkCapabilityTest, DisjointPolyhedronUnionNormalizesBodyOrder)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = BuildTranslatedUnitCubeBody(3.0, 0.0, 0.0);

    const auto result = UnionBodies(second, first);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_EQ(result.bodies.size(), 2U);
    EXPECT_EQ(result.bodies[0].FaceCount(), 6U);
    EXPECT_EQ(result.bodies[1].FaceCount(), 6U);
    const auto firstBounds = result.bodies[0].Bounds();
    const auto secondBounds = result.bodies[1].Bounds();
    EXPECT_NEAR(firstBounds.MinPoint().x, 0.0, 1e-12);
    EXPECT_NEAR(firstBounds.MaxPoint().x, 1.0, 1e-12);
    EXPECT_NEAR(secondBounds.MinPoint().x, 3.0, 1e-12);
    EXPECT_NEAR(secondBounds.MaxPoint().x, 4.0, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, DisjointPolyhedronDifferenceReturnsOriginalBody)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = BuildTranslatedUnitCubeBody(3.0, 0.0, 0.0);

    const auto result = DifferenceBodies(first, second);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_EQ(result.body.VertexCount(), 8U);
    EXPECT_EQ(result.body.EdgeCount(), 12U);
}

TEST(BodyBooleanSdkCapabilityTest, OverlappingPolyhedronIntersectionReturnsSingleOverlapBox)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = BuildAxisAlignedBoxBody(0.5, 0.0, 0.0, 1.5, 1.0, 1.0);

    const auto result = IntersectBodies(first, second);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_TRUE(result.bodies.empty());
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_TRUE(result.body.ShellAt(0).IsClosed());
    const auto bounds = result.body.Bounds();
    EXPECT_NEAR(bounds.MinPoint().x, 0.5, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().y, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().z, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().x, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().y, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().z, 1.0, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, OverlappingPolyhedronUnionReturnsSingleAxisAlignedBox)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = BuildAxisAlignedBoxBody(0.5, 0.0, 0.0, 1.5, 1.0, 1.0);

    const auto result = UnionBodies(first, second);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_TRUE(result.bodies.empty());
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_TRUE(result.body.ShellAt(0).IsClosed());
    const auto bounds = result.body.Bounds();
    EXPECT_NEAR(bounds.MinPoint().x, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().y, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().z, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().x, 1.5, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().y, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().z, 1.0, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, TouchingPolyhedronUnionReturnsSingleAxisAlignedBox)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = BuildAxisAlignedBoxBody(1.0, 0.0, 0.0, 2.0, 1.0, 1.0);

    const auto result = UnionBodies(first, second);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_TRUE(result.bodies.empty());
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_EQ(result.body.ShellCount(), 1U);
    EXPECT_TRUE(result.body.ShellAt(0).IsClosed());
    const auto bounds = result.body.Bounds();
    EXPECT_NEAR(bounds.MinPoint().x, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().x, 2.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().y, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().y, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().z, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().z, 1.0, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, TouchingPolyhedronIntersectionReturnsDeterministicEmptyResult)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = BuildAxisAlignedBoxBody(1.0, 0.0, 0.0, 2.0, 1.0, 1.0);

    const auto intersection = IntersectBodies(first, second);

    EXPECT_EQ(intersection.issue, BodyBooleanIssue3d::None);
    EXPECT_TRUE(intersection.IsSuccess());
    EXPECT_TRUE(intersection.producedEmptyResult);
    EXPECT_EQ(intersection.body.FaceCount(), 0U);
    EXPECT_TRUE(intersection.bodies.empty());
}

TEST(BodyBooleanSdkCapabilityTest, ContainedPolyhedronIntersectionReturnsInnerBody)
{
    const PolyhedronBody first = BuildAxisAlignedBoxBody(-0.5, -0.5, -0.5, 1.5, 1.5, 1.5);
    const PolyhedronBody second = Geometry::Test::BuildUnitCubeBody();

    const auto result = IntersectBodies(first, second);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_TRUE(result.bodies.empty());
    EXPECT_FALSE(result.producedEmptyResult);
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_EQ(result.body.ShellCount(), 1U);
    EXPECT_TRUE(result.body.ShellAt(0).IsClosed());
    const auto bounds = result.body.Bounds();
    EXPECT_NEAR(bounds.MinPoint().x, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().y, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().z, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().x, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().y, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().z, 1.0, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, ContainedPolyhedronUnionReturnsOuterBody)
{
    const PolyhedronBody first = BuildAxisAlignedBoxBody(-0.5, -0.5, -0.5, 1.5, 1.5, 1.5);
    const PolyhedronBody second = Geometry::Test::BuildUnitCubeBody();

    const auto result = UnionBodies(first, second);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_TRUE(result.bodies.empty());
    EXPECT_FALSE(result.producedEmptyResult);
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_EQ(result.body.ShellCount(), 1U);
    EXPECT_TRUE(result.body.ShellAt(0).IsClosed());
    const auto bounds = result.body.Bounds();
    EXPECT_NEAR(bounds.MinPoint().x, -0.5, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().y, -0.5, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().z, -0.5, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().x, 1.5, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().y, 1.5, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().z, 1.5, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, DisjointPolyhedronIntersectionReturnsDeterministicEmptyResult)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = BuildAxisAlignedBoxBody(3.0, 0.0, 0.0, 4.0, 1.0, 1.0);

    const auto result = IntersectBodies(first, second);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    EXPECT_TRUE(result.producedEmptyResult);
    EXPECT_EQ(result.body.FaceCount(), 0U);
    EXPECT_TRUE(result.bodies.empty());
}

TEST(BodyBooleanSdkCapabilityTest, EdgeTouchingPolyhedronIntersectionReturnsDeterministicEmptyResult)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = BuildAxisAlignedBoxBody(1.0, 1.0, 0.0, 2.0, 2.0, 1.0);

    const auto intersection = IntersectBodies(first, second);

    EXPECT_EQ(intersection.issue, BodyBooleanIssue3d::None);
    EXPECT_TRUE(intersection.IsSuccess());
    EXPECT_TRUE(intersection.producedEmptyResult);
    EXPECT_EQ(intersection.body.FaceCount(), 0U);
    EXPECT_TRUE(intersection.bodies.empty());
}

TEST(BodyBooleanSdkCapabilityTest, VertexTouchingPolyhedronIntersectionReturnsDeterministicEmptyResult)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = BuildAxisAlignedBoxBody(1.0, 1.0, 1.0, 2.0, 2.0, 2.0);

    const auto intersection = IntersectBodies(first, second);

    EXPECT_EQ(intersection.issue, BodyBooleanIssue3d::None);
    EXPECT_TRUE(intersection.IsSuccess());
    EXPECT_TRUE(intersection.producedEmptyResult);
    EXPECT_EQ(intersection.body.FaceCount(), 0U);
    EXPECT_TRUE(intersection.bodies.empty());
}

TEST(BodyBooleanSdkCapabilityTest, EdgeTouchingPolyhedronUnionReturnsDeterministicOrderedMultiBodyResult)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = BuildAxisAlignedBoxBody(1.0, 1.0, 0.0, 2.0, 2.0, 1.0);

    const auto result = UnionBodies(second, first);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_EQ(result.bodies.size(), 2U);
    EXPECT_EQ(result.body.FaceCount(), 0U);
    EXPECT_FALSE(result.producedEmptyResult);
    const auto firstBounds = result.bodies[0].Bounds();
    const auto secondBounds = result.bodies[1].Bounds();
    EXPECT_NEAR(firstBounds.MinPoint().x, 0.0, 1e-12);
    EXPECT_NEAR(firstBounds.MaxPoint().x, 1.0, 1e-12);
    EXPECT_NEAR(secondBounds.MinPoint().x, 1.0, 1e-12);
    EXPECT_NEAR(secondBounds.MaxPoint().x, 2.0, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, VertexTouchingPolyhedronDifferenceReturnsOriginalBody)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = BuildAxisAlignedBoxBody(1.0, 1.0, 1.0, 2.0, 2.0, 2.0);

    const auto result = DifferenceBodies(first, second);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_TRUE(result.bodies.empty());
    EXPECT_FALSE(result.producedEmptyResult);
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_EQ(result.body.ShellCount(), 1U);
    EXPECT_TRUE(result.body.ShellAt(0).IsClosed());
    const auto bounds = result.body.Bounds();
    EXPECT_NEAR(bounds.MinPoint().x, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().x, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().y, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().y, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().z, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().z, 1.0, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, TouchingPolyhedronDifferenceReturnsOriginalBody)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = BuildAxisAlignedBoxBody(1.0, 0.0, 0.0, 2.0, 1.0, 1.0);

    const auto result = DifferenceBodies(first, second);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_TRUE(result.bodies.empty());
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_EQ(result.body.ShellCount(), 1U);
    EXPECT_TRUE(result.body.ShellAt(0).IsClosed());
    const auto bounds = result.body.Bounds();
    EXPECT_NEAR(bounds.MinPoint().x, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().x, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().y, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().y, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().z, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().z, 1.0, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, OverlappingPolyhedronDifferenceReturnsRemainingSingleBox)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = BuildAxisAlignedBoxBody(0.5, 0.0, 0.0, 1.0, 1.0, 1.0);

    const auto result = DifferenceBodies(first, second);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_TRUE(result.bodies.empty());
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_TRUE(result.body.ShellAt(0).IsClosed());
    const auto bounds = result.body.Bounds();
    EXPECT_NEAR(bounds.MinPoint().x, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().y, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().z, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().x, 0.5, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().y, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().z, 1.0, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, ContainedPolyhedronDifferenceReturnsDeterministicEmptyResult)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = BuildAxisAlignedBoxBody(-0.5, -0.5, -0.5, 1.5, 1.5, 1.5);

    const auto result = DifferenceBodies(first, second);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    EXPECT_TRUE(result.producedEmptyResult);
    EXPECT_EQ(result.body.FaceCount(), 0U);
    EXPECT_TRUE(result.bodies.empty());
}

TEST(BodyBooleanSdkCapabilityTest, NonBoxOverlapUnionAndDifferenceRemainUnsupported)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = BuildAxisAlignedBoxBody(0.5, 0.5, 0.0, 1.5, 1.5, 1.0);

    const auto united = UnionBodies(first, second);
    const auto difference = DifferenceBodies(first, second);

    EXPECT_EQ(united.issue, BodyBooleanIssue3d::UnsupportedOperation);
    EXPECT_EQ(difference.issue, BodyBooleanIssue3d::UnsupportedOperation);
    EXPECT_FALSE(united.IsSuccess());
    EXPECT_FALSE(difference.IsSuccess());
}

TEST(BodyBooleanSdkCapabilityTest, FaceTouchingLShapeUnionReturnsExplicitUnsupportedOperation)
{
    const PolyhedronBody first = BuildAxisAlignedBoxBody(0.0, 0.0, 0.0, 2.0, 1.0, 1.0);
    const PolyhedronBody second = BuildAxisAlignedBoxBody(0.0, 1.0, 0.0, 1.0, 2.0, 1.0);

    const auto result = UnionBodies(first, second);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::UnsupportedOperation);
    EXPECT_FALSE(result.IsSuccess());
    EXPECT_EQ(result.body.FaceCount(), 0U);
    EXPECT_TRUE(result.bodies.empty());
}

TEST(BodyBooleanSdkCapabilityTest, BrepFaceTouchingLShapeUnionReturnsExplicitUnsupportedOperation)
{
    const auto first = Geometry::Sdk::ConvertToBrepBody(BuildAxisAlignedBoxBody(0.0, 0.0, 0.0, 2.0, 1.0, 1.0));
    const auto second = Geometry::Sdk::ConvertToBrepBody(BuildAxisAlignedBoxBody(0.0, 1.0, 0.0, 1.0, 2.0, 1.0));
    ASSERT_TRUE(first.success);
    ASSERT_TRUE(second.success);

    const auto result = UnionBodies(first.body, second.body);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::UnsupportedOperation);
    EXPECT_FALSE(result.IsSuccess());
    EXPECT_EQ(result.body.FaceCount(), 0U);
    EXPECT_TRUE(result.bodies.empty());
}

TEST(BodyBooleanSdkCapabilityTest, RotatedBoxIntersectionReturnsExplicitUnsupportedOperation)
{
    const PolyhedronBody first = Geometry::Test::BuildUnitCubeBody();
    const PolyhedronBody second = BuildRotatedOverlapBoxBody();

    const auto result = IntersectBodies(first, second);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::UnsupportedOperation);
    EXPECT_FALSE(result.IsSuccess());
    EXPECT_EQ(result.body.FaceCount(), 0U);
    EXPECT_TRUE(result.bodies.empty());
}

TEST(BodyBooleanSdkCapabilityTest, BrepRotatedBoxIntersectionReturnsExplicitUnsupportedOperation)
{
    const auto first = Geometry::Sdk::ConvertToBrepBody(Geometry::Test::BuildUnitCubeBody());
    const auto second = Geometry::Sdk::ConvertToBrepBody(BuildRotatedOverlapBoxBody());
    ASSERT_TRUE(first.success);
    ASSERT_TRUE(second.success);

    const auto result = IntersectBodies(first.body, second.body);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::UnsupportedOperation);
    EXPECT_FALSE(result.IsSuccess());
    EXPECT_EQ(result.body.FaceCount(), 0U);
    EXPECT_TRUE(result.bodies.empty());
}

TEST(BodyBooleanSdkCapabilityTest, IdenticalBrepIntersectionReturnsClosedBody)
{
    const auto converted = Geometry::Sdk::ConvertToBrepBody(Geometry::Test::BuildUnitCubeBody());
    ASSERT_TRUE(converted.success);

    const auto result = IntersectBodies(converted.body, converted.body);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_EQ(result.body.ShellCount(), 1U);
    EXPECT_TRUE(result.body.ShellAt(0).IsClosed());
}

TEST(BodyBooleanSdkCapabilityTest, IdenticalBrepDifferenceReturnsDeterministicEmptyResult)
{
    const auto converted = Geometry::Sdk::ConvertToBrepBody(Geometry::Test::BuildUnitCubeBody());
    ASSERT_TRUE(converted.success);

    const auto result = DifferenceBodies(converted.body, converted.body);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    EXPECT_TRUE(result.producedEmptyResult);
    EXPECT_EQ(result.body.FaceCount(), 0U);
    EXPECT_TRUE(result.bodies.empty());
}

TEST(BodyBooleanSdkCapabilityTest, ContainedBrepIntersectionReturnsInnerBody)
{
    const auto first = Geometry::Sdk::ConvertToBrepBody(BuildAxisAlignedBoxBody(-0.5, -0.5, -0.5, 1.5, 1.5, 1.5));
    const auto second = Geometry::Sdk::ConvertToBrepBody(Geometry::Test::BuildUnitCubeBody());
    ASSERT_TRUE(first.success);
    ASSERT_TRUE(second.success);

    const auto result = IntersectBodies(first.body, second.body);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_TRUE(result.bodies.empty());
    EXPECT_FALSE(result.producedEmptyResult);
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_EQ(result.body.ShellCount(), 1U);
    EXPECT_TRUE(result.body.ShellAt(0).IsClosed());
    const auto bounds = result.body.Bounds();
    EXPECT_NEAR(bounds.MinPoint().x, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().y, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().z, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().x, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().y, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().z, 1.0, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, ContainedBrepUnionReturnsOuterBody)
{
    const auto first = Geometry::Sdk::ConvertToBrepBody(BuildAxisAlignedBoxBody(-0.5, -0.5, -0.5, 1.5, 1.5, 1.5));
    const auto second = Geometry::Sdk::ConvertToBrepBody(Geometry::Test::BuildUnitCubeBody());
    ASSERT_TRUE(first.success);
    ASSERT_TRUE(second.success);

    const auto result = UnionBodies(first.body, second.body);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_TRUE(result.bodies.empty());
    EXPECT_FALSE(result.producedEmptyResult);
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_EQ(result.body.ShellCount(), 1U);
    EXPECT_TRUE(result.body.ShellAt(0).IsClosed());
    const auto bounds = result.body.Bounds();
    EXPECT_NEAR(bounds.MinPoint().x, -0.5, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().y, -0.5, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().z, -0.5, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().x, 1.5, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().y, 1.5, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().z, 1.5, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, DisjointBrepIntersectionReturnsDeterministicEmptyResult)
{
    const auto first = Geometry::Sdk::ConvertToBrepBody(Geometry::Test::BuildUnitCubeBody());
    const auto second = Geometry::Sdk::ConvertToBrepBody(BuildAxisAlignedBoxBody(3.0, 0.0, 0.0, 4.0, 1.0, 1.0));
    ASSERT_TRUE(first.success);
    ASSERT_TRUE(second.success);

    const auto result = IntersectBodies(first.body, second.body);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    EXPECT_TRUE(result.producedEmptyResult);
    EXPECT_EQ(result.body.FaceCount(), 0U);
    EXPECT_TRUE(result.bodies.empty());
}

TEST(BodyBooleanSdkCapabilityTest, DisjointBrepUnionNormalizesBodyOrder)
{
    const auto first = Geometry::Sdk::ConvertToBrepBody(Geometry::Test::BuildUnitCubeBody());
    const auto second = Geometry::Sdk::ConvertToBrepBody(BuildTranslatedUnitCubeBody(3.0, 0.0, 0.0));
    ASSERT_TRUE(first.success);
    ASSERT_TRUE(second.success);

    const auto result = UnionBodies(second.body, first.body);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_EQ(result.bodies.size(), 2U);
    EXPECT_EQ(result.bodies[0].FaceCount(), 6U);
    EXPECT_EQ(result.bodies[1].FaceCount(), 6U);
    const auto firstBounds = result.bodies[0].Bounds();
    const auto secondBounds = result.bodies[1].Bounds();
    EXPECT_NEAR(firstBounds.MinPoint().x, 0.0, 1e-12);
    EXPECT_NEAR(firstBounds.MaxPoint().x, 1.0, 1e-12);
    EXPECT_NEAR(secondBounds.MinPoint().x, 3.0, 1e-12);
    EXPECT_NEAR(secondBounds.MaxPoint().x, 4.0, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, OverlappingBrepUnionReturnsSingleAxisAlignedBox)
{
    const auto first = Geometry::Sdk::ConvertToBrepBody(Geometry::Test::BuildUnitCubeBody());
    const auto second = Geometry::Sdk::ConvertToBrepBody(BuildAxisAlignedBoxBody(0.5, 0.0, 0.0, 1.5, 1.0, 1.0));
    ASSERT_TRUE(first.success);
    ASSERT_TRUE(second.success);

    const auto result = UnionBodies(first.body, second.body);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_TRUE(result.bodies.empty());
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_TRUE(result.body.ShellAt(0).IsClosed());
    const auto bounds = result.body.Bounds();
    EXPECT_NEAR(bounds.MinPoint().x, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().x, 1.5, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().y, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().y, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().z, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().z, 1.0, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, TouchingBrepUnionReturnsSingleAxisAlignedBox)
{
    const auto first = Geometry::Sdk::ConvertToBrepBody(Geometry::Test::BuildUnitCubeBody());
    const auto second = Geometry::Sdk::ConvertToBrepBody(BuildAxisAlignedBoxBody(1.0, 0.0, 0.0, 2.0, 1.0, 1.0));
    ASSERT_TRUE(first.success);
    ASSERT_TRUE(second.success);

    const auto result = UnionBodies(first.body, second.body);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_TRUE(result.bodies.empty());
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_EQ(result.body.ShellCount(), 1U);
    EXPECT_TRUE(result.body.ShellAt(0).IsClosed());
    const auto bounds = result.body.Bounds();
    EXPECT_NEAR(bounds.MinPoint().x, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().x, 2.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().y, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().y, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().z, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().z, 1.0, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, TouchingBrepDifferenceReturnsOriginalBody)
{
    const auto first = Geometry::Sdk::ConvertToBrepBody(Geometry::Test::BuildUnitCubeBody());
    const auto second = Geometry::Sdk::ConvertToBrepBody(BuildAxisAlignedBoxBody(1.0, 0.0, 0.0, 2.0, 1.0, 1.0));
    ASSERT_TRUE(first.success);
    ASSERT_TRUE(second.success);

    const auto result = DifferenceBodies(first.body, second.body);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_TRUE(result.bodies.empty());
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_EQ(result.body.ShellCount(), 1U);
    EXPECT_TRUE(result.body.ShellAt(0).IsClosed());
    const auto bounds = result.body.Bounds();
    EXPECT_NEAR(bounds.MinPoint().x, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().x, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().y, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().y, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().z, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().z, 1.0, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, TouchingBrepIntersectionReturnsDeterministicEmptyResult)
{
    const auto first = Geometry::Sdk::ConvertToBrepBody(Geometry::Test::BuildUnitCubeBody());
    const auto second = Geometry::Sdk::ConvertToBrepBody(BuildAxisAlignedBoxBody(1.0, 0.0, 0.0, 2.0, 1.0, 1.0));
    ASSERT_TRUE(first.success);
    ASSERT_TRUE(second.success);

    const auto result = IntersectBodies(first.body, second.body);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    EXPECT_TRUE(result.producedEmptyResult);
    EXPECT_EQ(result.body.FaceCount(), 0U);
    EXPECT_TRUE(result.bodies.empty());
}

TEST(BodyBooleanSdkCapabilityTest, EdgeTouchingBrepIntersectionReturnsDeterministicEmptyResult)
{
    const auto first = Geometry::Sdk::ConvertToBrepBody(Geometry::Test::BuildUnitCubeBody());
    const auto second = Geometry::Sdk::ConvertToBrepBody(BuildAxisAlignedBoxBody(1.0, 1.0, 0.0, 2.0, 2.0, 1.0));
    ASSERT_TRUE(first.success);
    ASSERT_TRUE(second.success);

    const auto result = IntersectBodies(first.body, second.body);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    EXPECT_TRUE(result.producedEmptyResult);
    EXPECT_EQ(result.body.FaceCount(), 0U);
    EXPECT_TRUE(result.bodies.empty());
}

TEST(BodyBooleanSdkCapabilityTest, VertexTouchingBrepIntersectionReturnsDeterministicEmptyResult)
{
    const auto first = Geometry::Sdk::ConvertToBrepBody(Geometry::Test::BuildUnitCubeBody());
    const auto second = Geometry::Sdk::ConvertToBrepBody(BuildAxisAlignedBoxBody(1.0, 1.0, 1.0, 2.0, 2.0, 2.0));
    ASSERT_TRUE(first.success);
    ASSERT_TRUE(second.success);

    const auto result = IntersectBodies(first.body, second.body);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    EXPECT_TRUE(result.producedEmptyResult);
    EXPECT_EQ(result.body.FaceCount(), 0U);
    EXPECT_TRUE(result.bodies.empty());
}

TEST(BodyBooleanSdkCapabilityTest, EdgeTouchingBrepUnionReturnsDeterministicOrderedMultiBodyResult)
{
    const auto first = Geometry::Sdk::ConvertToBrepBody(Geometry::Test::BuildUnitCubeBody());
    const auto second = Geometry::Sdk::ConvertToBrepBody(BuildAxisAlignedBoxBody(1.0, 1.0, 0.0, 2.0, 2.0, 1.0));
    ASSERT_TRUE(first.success);
    ASSERT_TRUE(second.success);

    const auto result = UnionBodies(second.body, first.body);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_EQ(result.bodies.size(), 2U);
    EXPECT_EQ(result.body.FaceCount(), 0U);
    EXPECT_FALSE(result.producedEmptyResult);
    const auto firstBounds = result.bodies[0].Bounds();
    const auto secondBounds = result.bodies[1].Bounds();
    EXPECT_NEAR(firstBounds.MinPoint().x, 0.0, 1e-12);
    EXPECT_NEAR(firstBounds.MaxPoint().x, 1.0, 1e-12);
    EXPECT_NEAR(secondBounds.MinPoint().x, 1.0, 1e-12);
    EXPECT_NEAR(secondBounds.MaxPoint().x, 2.0, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, VertexTouchingBrepDifferenceReturnsOriginalBody)
{
    const auto first = Geometry::Sdk::ConvertToBrepBody(Geometry::Test::BuildUnitCubeBody());
    const auto second = Geometry::Sdk::ConvertToBrepBody(BuildAxisAlignedBoxBody(1.0, 1.0, 1.0, 2.0, 2.0, 2.0));
    ASSERT_TRUE(first.success);
    ASSERT_TRUE(second.success);

    const auto result = DifferenceBodies(first.body, second.body);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    ASSERT_TRUE(result.bodies.empty());
    EXPECT_FALSE(result.producedEmptyResult);
    EXPECT_EQ(result.body.FaceCount(), 6U);
    EXPECT_EQ(result.body.ShellCount(), 1U);
    EXPECT_TRUE(result.body.ShellAt(0).IsClosed());
    const auto bounds = result.body.Bounds();
    EXPECT_NEAR(bounds.MinPoint().x, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().x, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().y, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().y, 1.0, 1e-12);
    EXPECT_NEAR(bounds.MinPoint().z, 0.0, 1e-12);
    EXPECT_NEAR(bounds.MaxPoint().z, 1.0, 1e-12);
}

TEST(BodyBooleanSdkCapabilityTest, ContainedBrepDifferenceReturnsDeterministicEmptyResult)
{
    const auto first = Geometry::Sdk::ConvertToBrepBody(Geometry::Test::BuildUnitCubeBody());
    const auto second = Geometry::Sdk::ConvertToBrepBody(BuildAxisAlignedBoxBody(-0.5, -0.5, -0.5, 1.5, 1.5, 1.5));
    ASSERT_TRUE(first.success);
    ASSERT_TRUE(second.success);

    const auto result = DifferenceBodies(first.body, second.body);

    ASSERT_EQ(result.issue, BodyBooleanIssue3d::None);
    ASSERT_TRUE(result.IsSuccess());
    EXPECT_TRUE(result.producedEmptyResult);
    EXPECT_EQ(result.body.FaceCount(), 0U);
    EXPECT_TRUE(result.bodies.empty());
}
