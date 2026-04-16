#pragma once

#include <cmath>
#include <sstream>
#include <string>

#include "Support/Epsilon.h"
#include "Types/Geometry3d/Direction3.h"
#include "Types/Geometry3d/Matrix3.h"

namespace Geometry
{
    struct Transform3d
    {
        Matrix3d linear{ Matrix3d::Identity() };
        Vector3d translation{};

        [[nodiscard]] static constexpr Transform3d Identity() { return Transform3d{}; }

        [[nodiscard]] static Transform3d Translation( const Vector3d &offset )
        {
            return Transform3d{ Matrix3d::Identity(), offset };
        }

        [[nodiscard]] static Transform3d Scale( const Point3d &origin, double ratio )
        {
            const Matrix3d scale{ ratio, 0.0, 0.0, 0.0, ratio, 0.0, 0.0, 0.0, ratio };
            const Point3d movedOrigin = scale * origin;
            return Transform3d{ scale, origin - movedOrigin };
        }

        [[nodiscard]] bool IsValid() const { return linear.IsValid() && translation.IsValid(); }

        [[nodiscard]] Point3d Apply( const Point3d &point ) const
        {
            return linear * point + translation;
        }

        [[nodiscard]] Vector3d Apply( const Vector3d &vector ) const { return linear * vector; }

        [[nodiscard]] Direction3d Apply( const Direction3d &direction,
                                         double eps = kDefaultEpsilon ) const
        {
            return Direction3d::FromVector( Apply( direction.Vector() ), eps );
        }

        [[nodiscard]] Transform3d Inverse( double eps = kDefaultEpsilon ) const
        {
            const Matrix3d inverseLinear = linear.Inverse( eps );
            if( !inverseLinear.IsInvertible( eps ) && !linear.IsInvertible( eps ) )
            {
                return {};
            }
            return Transform3d{ inverseLinear, -( inverseLinear * translation ) };
        }

        [[nodiscard]] std::string DebugString() const
        {
            std::ostringstream stream;
            stream << "Transform3d{linear=" << linear.DebugString()
                   << ", translation=" << translation.DebugString() << "}";
            return stream.str();
        }
    };

    [[nodiscard]] inline Transform3d operator*( const Transform3d &lhs, const Transform3d &rhs )
    {
        return Transform3d{ lhs.linear * rhs.linear, lhs.linear * rhs.translation + lhs.translation };
    }
}  // namespace Geometry
