#pragma once

#include <algorithm>
#include <cmath>
#include <type_traits>

#include "Geometry2d/SegmentTypes.h"
#include "Support/Epsilon.h"
#include "Types/Geometry2d/Box2.h"
#include "Types/Geometry2d/Point2.h"
#include "Types/Geometry2d/Vector2.h"

namespace Geometry
{
    namespace Detail
    {
        [[nodiscard]] inline double ClampDouble( double value, double low, double high )
        {
            return std::max( low, std::min( value, high ) );
        }

        [[nodiscard]] inline double NormalizeAngle( double angle )
        {
            angle = std::fmod( angle, Geometry::kTwoPi );
            if( angle < 0.0 )
            {
                angle += Geometry::kTwoPi;
            }
            return angle;
        }

        [[nodiscard]] inline double SignedSweep( double startAngle, double endAngle,
                                                 ArcDirection direction )
        {
            if( direction == ArcDirection::CounterClockwise )
            {
                return NormalizeAngle( endAngle - startAngle );
            }

            return -NormalizeAngle( startAngle - endAngle );
        }

        [[nodiscard]] inline bool IsAngleOnArc( double candidateAngle, double startAngle,
                                                double signedSweep, double eps = kDefaultEpsilon )
        {
            if( signedSweep >= 0.0 )
            {
                const double delta = NormalizeAngle( candidateAngle - startAngle );
                return delta <= signedSweep + eps;
            }

            const double delta = NormalizeAngle( startAngle - candidateAngle );
            return delta <= ( -signedSweep ) + eps;
        }

        template <typename T>
        [[nodiscard]] constexpr bool IsFiniteValue( T value )
        {
            if constexpr( std::is_floating_point_v<T> )
            {
                return std::isfinite( static_cast<double>( value ) );
            }
            else
            {
                return true;
            }
        }
    }  // namespace Detail

    template <typename T>
    class Segment2
    {
    public:
        using PointType = Point2<T>;
        using VectorType = Vector2<T>;
        using LengthType = typename VectorType::LengthType;

        virtual ~Segment2() = default;

        [[nodiscard]] virtual SegmentKind2 Kind() const = 0;
        [[nodiscard]] virtual PointType StartPoint() const = 0;
        [[nodiscard]] virtual PointType EndPoint() const = 0;
        [[nodiscard]] virtual LengthType Length() const = 0;
        [[nodiscard]] virtual Box2<T> Bounds() const = 0;
        [[nodiscard]] virtual PointType PointAt( double parameter ) const = 0;
        [[nodiscard]] virtual PointType PointAtLength( LengthType distanceFromStart,
                                                       bool clampToSegment = false ) const = 0;
        [[nodiscard]] virtual bool IsValid() const = 0;
    };

}  // namespace Geometry
