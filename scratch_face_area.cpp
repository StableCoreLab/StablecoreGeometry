#include <iostream>

#include "sdk/GeometryMeasure.h"
#include "sdk/GeometryProjection.h"
#include "sdk/GeometryShapeOps.h"
#include "sdk/PolyhedronFace3d.h"

int main()
{
    using namespace Geometry;
    using namespace Geometry::Sdk;

    const PolyhedronLoop3d outerLoop(
        {
            Point3d{0.0, 0.0, 0.0},
            Point3d{2.0, 0.0, 0.0},
            Point3d{2.0, 2.0, 0.0},
            Point3d{0.0, 2.0, 0.0},
        });
    const PolyhedronFace3d face(
        Plane::FromPointAndNormal(Point3d{0.0, 0.0, 0.0}, Vector3d{0.0, 0.0, 1.0}),
        outerLoop);

    const FaceProjection3d projected = ProjectFaceToPolygon2d(face);
    std::cout << "face valid: " << face.IsValid() << "\n";
    std::cout << "projection success: " << projected.success << "\n";
    std::cout << "polygon valid: " << projected.polygon.IsValid() << "\n";
    std::cout << "polygon area: " << Area(projected.polygon) << "\n";
    std::cout << "face area: " << Area(face) << "\n";
    return 0;
}