#pragma once

#include "Geometry.h"

namespace Geometry::Test
{
inline Geometry::PolyhedronBody BuildUnitCubeBody()
{
    using Geometry::Plane;
    using Geometry::Point3d;
    using Geometry::PolyhedronBody;
    using Geometry::PolyhedronFace3d;
    using Geometry::PolyhedronLoop3d;
    using Geometry::Vector3d;

    return Geometry::PolyhedronBody(
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





