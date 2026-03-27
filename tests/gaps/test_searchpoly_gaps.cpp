#include <gtest/gtest.h>

#include "sdk/GeometryPathOps.h"
#include "sdk/MultiPolyline2d.h"

using geometry::sdk::MultiPolyline2d;
using geometry::sdk::Point2d;
using geometry::sdk::Polyline2d;
using geometry::sdk::PolylineClosure;

TEST(SearchPolyGapTest, BranchScoringAndFakeEdgeRankingRemainOpen)
{
    const MultiPolyline2d branchedAmbiguous{
        Polyline2d({Point2d{0.0, 0.0}, Point2d{4.0, 0.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 0.0}, Point2d{4.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{4.0, 4.0}, Point2d{0.0, 4.0}}, PolylineClosure::Open),
        Polyline2d({Point2d{0.0, 4.0}, Point2d{0.0, 0.4}}, PolylineClosure::Open),
        Polyline2d({Point2d{-0.2, 0.4}, Point2d{-0.2, -0.2}}, PolylineClosure::Open),
        Polyline2d({Point2d{-0.2, -0.2}, Point2d{1.0, -0.2}}, PolylineClosure::Open),
        Polyline2d({Point2d{2.0, -1.0}, Point2d{2.0, 2.0}}, PolylineClosure::Open)};
    (void)branchedAmbiguous;
    GTEST_SKIP() << "Known gap: Delphi-like branch scoring and fake-edge candidate ranking are still below target.";
}
