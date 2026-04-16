#pragma once

#include <cmath>
#include <type_traits>

#include "Types/Geometry2d/Point2.h"

namespace Geometry
{
    template <typename T>
    using VectorLengthType = std::conditional_t<std::is_floating_point_v<T>, T, double>;

    template <typename T>
    struct Vector2 : Vec2Storage<T>
    {
        using Vec2Storage<T>::Vec2Storage;
        using LengthType = VectorLengthType<T>;

        [[nodiscard]] static constexpr Vector2 FromXY( T xValue, T yValue )
        {
            return Vector2( xValue, yValue );
        }

        [[nodiscard]] constexpr auto LengthSquared() const -> LengthType
        {
            return static_cast<LengthType>( this->x ) * static_cast<LengthType>( this->x ) +
                   static_cast<LengthType>( this->y ) * static_cast<LengthType>( this->y );
        }

        [[nodiscard]] auto Length() const -> LengthType
        {
            using std::sqrt;
            return sqrt( LengthSquared() );
        }

        [[nodiscard]] std::string DebugString() const
        {
            return this->Vec2Storage<T>::DebugString( "Vector2" );
        }
    };

    template <typename T>
    [[nodiscard]] constexpr Vector2<T> operator+( const Vector2<T> &lhs, const Vector2<T> &rhs )
    {
        return Vector2<T>( lhs.x + rhs.x, lhs.y + rhs.y );
    }

    template <typename T>
    [[nodiscard]] constexpr Vector2<T> operator-( const Vector2<T> &lhs, const Vector2<T> &rhs )
    {
        return Vector2<T>( lhs.x - rhs.x, lhs.y - rhs.y );
    }

    template <typename T>
    [[nodiscard]] constexpr Vector2<T> operator-( const Vector2<T> &vector )
    {
        return Vector2<T>( -vector.x, -vector.y );
    }

    template <typename T, typename Scalar, typename = std::enable_if_t<std::is_arithmetic_v<Scalar>>>
    [[nodiscard]] constexpr Vector2<T> operator*( const Vector2<T> &vector, Scalar scalar )
    {
        return Vector2<T>( static_cast<T>( vector.x * scalar ), static_cast<T>( vector.y * scalar ) );
    }

    template <typename Scalar, typename T, typename = std::enable_if_t<std::is_arithmetic_v<Scalar>>>
    [[nodiscard]] constexpr Vector2<T> operator*( Scalar scalar, const Vector2<T> &vector )
    {
        return vector * scalar;
    }

    template <typename T, typename Scalar, typename = std::enable_if_t<std::is_arithmetic_v<Scalar>>>
    [[nodiscard]] constexpr Vector2<T> operator/( const Vector2<T> &vector, Scalar scalar )
    {
        return Vector2<T>( static_cast<T>( vector.x / scalar ), static_cast<T>( vector.y / scalar ) );
    }

    template <typename T>
    [[nodiscard]] constexpr Vector2<T> operator-( const Point2<T> &lhs, const Point2<T> &rhs )
    {
        return Vector2<T>( lhs.x - rhs.x, lhs.y - rhs.y );
    }

    using Vector2d = Vector2<double>;
    using Vector2i = Vector2<int>;
}  // namespace Geometry
