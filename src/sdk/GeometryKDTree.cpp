#include "sdk/GeometryKDTree.h"

#include <algorithm>
#include <limits>
#include <sstream>
#include <utility>

#include "algorithm/Predicate2.h"
#include "sdk/GeometryMetrics.h"

namespace geometry::sdk
{
GeometryKDTree2d::GeometryKDTree2d(std::vector<KDTreeEntry2d> entries)
    : entries_(std::move(entries))
{
}

void GeometryKDTree2d::Clear()
{
    entries_.clear();
}

void GeometryKDTree2d::Add(std::size_t id, const Point2d& point)
{
    entries_.push_back(KDTreeEntry2d{id, point});
}

bool GeometryKDTree2d::Remove(std::size_t id)
{
    const auto it = std::remove_if(entries_.begin(), entries_.end(), [id](const KDTreeEntry2d& entry) {
        return entry.id == id;
    });
    if (it == entries_.end())
    {
        return false;
    }

    entries_.erase(it, entries_.end());
    return true;
}

void GeometryKDTree2d::Update(std::size_t id, const Point2d& point)
{
    if (auto* entry = const_cast<KDTreeEntry2d*>(Find(id)); entry != nullptr)
    {
        entry->point = point;
        return;
    }

    Add(id, point);
}

std::size_t GeometryKDTree2d::Size() const
{
    return entries_.size();
}

bool GeometryKDTree2d::IsEmpty() const
{
    return entries_.empty();
}

bool GeometryKDTree2d::IsValid() const
{
    for (const auto& entry : entries_)
    {
        if (!entry.IsValid())
        {
            return false;
        }
    }
    return true;
}

bool GeometryKDTree2d::Contains(std::size_t id) const
{
    return Find(id) != nullptr;
}

const KDTreeEntry2d* GeometryKDTree2d::Find(std::size_t id) const
{
    const auto it = std::find_if(entries_.begin(), entries_.end(), [id](const KDTreeEntry2d& entry) {
        return entry.id == id;
    });
    return it == entries_.end() ? nullptr : &*it;
}

std::vector<std::size_t> GeometryKDTree2d::Query(const Point2d& point, double eps) const
{
    std::vector<std::size_t> result;
    for (const auto& entry : entries_)
    {
        if (IsEqual(entry.point, point, eps))
        {
            result.push_back(entry.id);
        }
    }
    return result;
}

std::vector<std::size_t> GeometryKDTree2d::QueryWithinDistance(const Point2d& point, double maxDistance) const
{
    std::vector<std::size_t> result;
    const double maxDistanceSquared = maxDistance * maxDistance;
    for (const auto& entry : entries_)
    {
        const double distanceSquared = DistanceSquared(entry.point, point);
        if (distanceSquared <= maxDistanceSquared)
        {
            result.push_back(entry.id);
        }
    }
    return result;
}

std::optional<KDTreeHit2d> GeometryKDTree2d::Nearest(const Point2d& point) const
{
    std::optional<KDTreeHit2d> best;
    for (const auto& entry : entries_)
    {
        const double distanceSquared = DistanceSquared(entry.point, point);
        if (!best || distanceSquared < best->distanceSquared)
        {
            best = KDTreeHit2d{entry.id, entry.point, distanceSquared};
        }
    }
    return best;
}

std::string GeometryKDTree2d::DebugString() const
{
    std::ostringstream stream;
    stream << "GeometryKDTree2d{size=" << Size()
           << ", valid=" << (IsValid() ? "true" : "false") << "}";
    return stream.str();
}

const std::vector<KDTreeEntry2d>& GeometryKDTree2d::Data() const
{
    return entries_;
}

std::vector<KDTreeEntry2d>& GeometryKDTree2d::Data()
{
    return entries_;
}
} // namespace geometry::sdk
