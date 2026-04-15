#pragma once

#include "sdk/Geometry.h"

namespace Geometry::Test
{
inline Geometry::Sdk::PolyhedronBody BuildUnitCubeBody()
{
    using Geometry::Sdk::Plane;
    using Geometry::Sdk::Point3d;
    using Geometry::Sdk::PolyhedronBody;
    using Geometry::Sdk::PolyhedronFace3d;
    using Geometry::Sdk::PolyhedronLoop3d;
    using Geometry::Sdk::Vector3d;

    return PolyhedronBody(
        {
            PolyhedronFace3d(
                Plane::FromPointAndNormal(Point3d{0.0, 0.0, 0.0}, Vector3d{0.0, 0.0, -1.0}),
                PolyhedronLoop3d(
                    {
                        Point3d{0.0, 0.0, 0.0},
                        Point3d{0.0, 1.0, 0.0},
                        Point3d{1.0, 1.0, 0.0},
                        Point3d{1.0, 0.0, 0.0},
                    })),
            PolyhedronFace3d(
                Plane::FromPointAndNormal(Point3d{0.0, 0.0, 1.0}, Vector3d{0.0, 0.0, 1.0}),
                PolyhedronLoop3d(
                    {
                        Point3d{0.0, 0.0, 1.0},
                        Point3d{1.0, 0.0, 1.0},
                        Point3d{1.0, 1.0, 1.0},
                        Point3d{0.0, 1.0, 1.0},
                    })),
            PolyhedronFace3d(
                Plane::FromPointAndNormal(Point3d{0.0, 0.0, 0.0}, Vector3d{0.0, -1.0, 0.0}),
                PolyhedronLoop3d(
                    {
                        Point3d{0.0, 0.0, 0.0},
                        Point3d{1.0, 0.0, 0.0},
                        Point3d{1.0, 0.0, 1.0},
                        Point3d{0.0, 0.0, 1.0},
                    })),
            PolyhedronFace3d(
                Plane::FromPointAndNormal(Point3d{1.0, 0.0, 0.0}, Vector3d{1.0, 0.0, 0.0}),
                PolyhedronLoop3d(
                    {
                        Point3d{1.0, 0.0, 0.0},
                        Point3d{1.0, 1.0, 0.0},
                        Point3d{1.0, 1.0, 1.0},
                        Point3d{1.0, 0.0, 1.0},
                    })),
            PolyhedronFace3d(
                Plane::FromPointAndNormal(Point3d{0.0, 1.0, 0.0}, Vector3d{0.0, 1.0, 0.0}),
                PolyhedronLoop3d(
                    {
                        Point3d{0.0, 1.0, 0.0},
                        Point3d{0.0, 1.0, 1.0},
                        Point3d{1.0, 1.0, 1.0},
                        Point3d{1.0, 1.0, 0.0},
                    })),
            PolyhedronFace3d(
                Plane::FromPointAndNormal(Point3d{0.0, 0.0, 0.0}, Vector3d{-1.0, 0.0, 0.0}),
                PolyhedronLoop3d(
                    {
                        Point3d{0.0, 0.0, 0.0},
                        Point3d{0.0, 0.0, 1.0},
                        Point3d{0.0, 1.0, 1.0},
                        Point3d{0.0, 1.0, 0.0},
                    })),
        });
}
} // namespace Geometry::Test
