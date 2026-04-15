#include "common/Epsilon.h"

namespace Geometry
{
const double kSharedTopologyMatchEpsilon = 1e-6;
const double kLoopCleanupEpsilon = kDefaultEpsilon;
const double kRepresentativeMatchEpsilon = 1e-6;
const double kRepresentativeGroupingEpsilon = kRepresentativeMatchEpsilon;
const double kBrepVertexDedupEpsilon = kRepresentativeMatchEpsilon;
const double kPlaneProjectionEpsilon = kDefaultEpsilon;
const double kBodyBooleanDefaultEpsilon = kDefaultEpsilon;
const double kIntersectionDefaultEpsilon = kDefaultEpsilon;
const double kProjectionDefaultEpsilon = kDefaultEpsilon;
const double kPathOpsDefaultEpsilon = kDefaultEpsilon;
const double kOffsetDefaultEpsilon = kDefaultEpsilon;
const double kAxisOpsDefaultEpsilon = kDefaultEpsilon;
const double kAlgorithmsDefaultEpsilon = kDefaultEpsilon;
const double kArcSegmentDefaultEpsilon = kDefaultEpsilon;
const double kHealingDefaultEpsilon = kDefaultEpsilon;
const double kMeshConversionDefaultEpsilon = kDefaultEpsilon;
const double kShapeOpsDefaultEpsilon = kDefaultEpsilon;
const double kTransformDefaultEpsilon = kDefaultEpsilon;
const double kSectionDefaultEpsilon = kDefaultEpsilon;
const double kSearchPolyDefaultEpsilon = kDefaultEpsilon;
const double kBooleanComparisonEpsilon = 1e-12;
const double kBooleanAreaEpsilon = 1e-10;
const double kBooleanRebuildFallbackEpsilon = 1e-8;
const double kPathOpsComparisonEpsilon = 1e-12;
const double kPathOpsAreaEpsilon = 1e-10;
const double kPathOpsRebuildFallbackEpsilon = 1e-8;
const double kSearchPolyComparisonEpsilon = 1e-12;
const double kOffsetRebuildFallbackEpsilon = 1e-8;
const double kSupportPlaneHoleDistanceWeight = 4.0;
const double kSupportPlaneOnPlaneEpsilon = 1e-10;
} // namespace Geometry
