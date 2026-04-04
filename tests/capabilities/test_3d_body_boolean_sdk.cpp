#include <gtest/gtest.h>

#include "sdk/GeometryBodyBoolean.h"

using geometry::sdk::BodyBooleanIssue3d;
using geometry::sdk::BodyBooleanOptions3d;
using geometry::sdk::DifferenceBodies;
using geometry::sdk::IntersectBodies;
using geometry::sdk::PolyhedronBody;
using geometry::sdk::UnionBodies;

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
