#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include "export/GeometryExport.h"
#include "sdk/GeometryMetrics.h"

namespace geometry::sdk
{
struct GEOMETRY_API BoxTreeEntry2d
{
    std::size_t id{0};
    Box2d box{};

    [[nodiscard]] bool IsValid() const
    {
        return box.IsValid();
    }
};

struct GEOMETRY_API BoxTreeHit2d
{
    std::size_t id{0};
    Box2d box{};
};

class GEOMETRY_API BoxTree2d
{
public:
    BoxTree2d() = default;
    explicit BoxTree2d(std::vector<BoxTreeEntry2d> entries);

    void Clear();
    void Add(std::size_t id, const Box2d& box);
    bool Remove(std::size_t id);
    void Update(std::size_t id, const Box2d& box);

    [[nodiscard]] std::size_t Size() const;
    [[nodiscard]] bool IsEmpty() const;
    [[nodiscard]] bool IsValid() const;
    [[nodiscard]] bool Contains(std::size_t id) const;
    [[nodiscard]] const BoxTreeEntry2d* Find(std::size_t id) const;
    [[nodiscard]] std::vector<std::size_t> Query(const Box2d& box, double eps = 1e-9) const;
    [[nodiscard]] std::vector<std::size_t> QueryContaining(const Point2d& point, double eps = 1e-9) const;
    [[nodiscard]] std::string DebugString() const;

    [[nodiscard]] const std::vector<BoxTreeEntry2d>& Data() const;
    [[nodiscard]] std::vector<BoxTreeEntry2d>& Data();

private:
    std::vector<BoxTreeEntry2d> entries_{};
};

using GeometryBoxTree2d = BoxTree2d;
} // namespace geometry::sdk
