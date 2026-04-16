#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "Export/GeometryExport.h"
#include "Types/Geometry2d/Box2.h"
#include "Types/Geometry2d/Point2.h"

namespace Geometry
{
    struct GEOMETRY_API KDTreeEntry2d
    {
        std::size_t id{ 0 };
        Point2d point{};

        [[nodiscard]] bool IsValid() const { return point.IsValid(); }
    };

    struct GEOMETRY_API KDTreeHit2d
    {
        std::size_t id{ 0 };
        Point2d point{};
        double distanceSquared{ 0.0 };

        [[nodiscard]] bool IsValid() const { return point.IsValid() && distanceSquared >= 0.0; }
    };

    class GEOMETRY_API KDTree2d
    {
    public:
        KDTree2d() = default;
        explicit KDTree2d( std::vector<KDTreeEntry2d> entries );

        void Clear();
        void Add( std::size_t id, const Point2d &point );
        bool Remove( std::size_t id );
        void Update( std::size_t id, const Point2d &point );

        [[nodiscard]] std::size_t Size() const;
        [[nodiscard]] bool IsEmpty() const;
        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] bool Contains( std::size_t id ) const;
        [[nodiscard]] const KDTreeEntry2d *Find( std::size_t id ) const;
        [[nodiscard]] std::vector<std::size_t> Query( const Point2d &point, double eps = 1e-9 ) const;
        [[nodiscard]] std::vector<std::size_t> QueryWithinDistance( const Point2d &point,
                                                                    double maxDistance ) const;
        [[nodiscard]] std::optional<KDTreeHit2d> Nearest( const Point2d &point ) const;
        [[nodiscard]] std::string DebugString() const;

        [[nodiscard]] const std::vector<KDTreeEntry2d> &Entries() const;
        [[nodiscard]] std::vector<KDTreeEntry2d> &Entries();
        [[nodiscard]] const std::vector<KDTreeEntry2d> &Data() const;
        [[nodiscard]] std::vector<KDTreeEntry2d> &Data();

    private:
        std::vector<KDTreeEntry2d> entries_{};
    };

}  // namespace Geometry
