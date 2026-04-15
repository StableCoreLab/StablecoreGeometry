#include "sdk/KDTree2d.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <sstream>
#include <utility>

#include "algorithm/Predicate2.h"
#include "sdk/GeometryMetrics.h"

namespace geometry::sdk
{
namespace
{
struct KDNode
{
    std::size_t entryIndex{0};
    int axis{0};
    std::unique_ptr<KDNode> left{};
    std::unique_ptr<KDNode> right{};
};

[[nodiscard]] double Coordinate(const Point2d& point, int axis)
{
    return axis == 0 ? point.x : point.y;
}

[[nodiscard]] std::unique_ptr<KDNode> BuildKDTree(
    const std::vector<KDTreeEntry2d>& entries,
    std::vector<std::size_t>::iterator begin,
    std::vector<std::size_t>::iterator end,
    int depth)
{
    if (begin >= end)
    {
        return {};
    }

    const int axis = depth % 2;
    const auto mid = begin + (end - begin) / 2;
    std::nth_element(
        begin,
        mid,
        end,
        [&entries, axis](std::size_t lhs, std::size_t rhs) {
            return Coordinate(entries[lhs].point, axis) < Coordinate(entries[rhs].point, axis);
        });

    auto node = std::make_unique<KDNode>();
    node->entryIndex = *mid;
    node->axis = axis;
    node->left = BuildKDTree(entries, begin, mid, depth + 1);
    node->right = BuildKDTree(entries, mid + 1, end, depth + 1);
    return node;
}

void QueryExact(
    const KDNode* node,
    const std::vector<KDTreeEntry2d>& entries,
    const Point2d& point,
    double eps,
    std::vector<std::size_t>& result)
{
    if (node == nullptr)
    {
        return;
    }

    const KDTreeEntry2d& entry = entries[node->entryIndex];
    if (IsEqual(entry.point, point, eps))
    {
        result.push_back(entry.id);
    }

    const double delta = Coordinate(point, node->axis) - Coordinate(entry.point, node->axis);
    const KDNode* nearChild = delta <= 0.0 ? node->left.get() : node->right.get();
    const KDNode* farChild = delta <= 0.0 ? node->right.get() : node->left.get();
    QueryExact(nearChild, entries, point, eps, result);
    if (std::abs(delta) <= eps)
    {
        QueryExact(farChild, entries, point, eps, result);
    }
}

void QueryRadius(
    const KDNode* node,
    const std::vector<KDTreeEntry2d>& entries,
    const Point2d& point,
    double maxDistanceSquared,
    std::vector<std::size_t>& result)
{
    if (node == nullptr)
    {
        return;
    }

    const KDTreeEntry2d& entry = entries[node->entryIndex];
    const double distanceSquared = DistanceSquared(entry.point, point);
    if (distanceSquared <= maxDistanceSquared)
    {
        result.push_back(entry.id);
    }

    const double delta = Coordinate(point, node->axis) - Coordinate(entry.point, node->axis);
    const KDNode* nearChild = delta <= 0.0 ? node->left.get() : node->right.get();
    const KDNode* farChild = delta <= 0.0 ? node->right.get() : node->left.get();
    QueryRadius(nearChild, entries, point, maxDistanceSquared, result);
    if (delta * delta <= maxDistanceSquared)
    {
        QueryRadius(farChild, entries, point, maxDistanceSquared, result);
    }
}

void QueryNearest(
    const KDNode* node,
    const std::vector<KDTreeEntry2d>& entries,
    const Point2d& point,
    std::optional<KDTreeHit2d>& best)
{
    if (node == nullptr)
    {
        return;
    }

    const KDTreeEntry2d& entry = entries[node->entryIndex];
    const double distanceSquared = DistanceSquared(entry.point, point);
    if (!best || distanceSquared < best->distanceSquared)
    {
        best = KDTreeHit2d{entry.id, entry.point, distanceSquared};
    }

    const double delta = Coordinate(point, node->axis) - Coordinate(entry.point, node->axis);
    const KDNode* nearChild = delta <= 0.0 ? node->left.get() : node->right.get();
    const KDNode* farChild = delta <= 0.0 ? node->right.get() : node->left.get();
    QueryNearest(nearChild, entries, point, best);
    if (!best || delta * delta < best->distanceSquared)
    {
        QueryNearest(farChild, entries, point, best);
    }
}
} // namespace

KDTree2d::KDTree2d(std::vector<KDTreeEntry2d> entries)
    : entries_(std::move(entries))
{
}

void KDTree2d::Clear()
{
    entries_.clear();
}

void KDTree2d::Add(std::size_t id, const Point2d& point)
{
    entries_.push_back(KDTreeEntry2d{id, point});
}

bool KDTree2d::Remove(std::size_t id)
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

void KDTree2d::Update(std::size_t id, const Point2d& point)
{
    if (auto* entry = const_cast<KDTreeEntry2d*>(Find(id)); entry != nullptr)
    {
        entry->point = point;
        return;
    }

    Add(id, point);
}

std::size_t KDTree2d::Size() const
{
    return entries_.size();
}

bool KDTree2d::IsEmpty() const
{
    return entries_.empty();
}

bool KDTree2d::IsValid() const
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

bool KDTree2d::Contains(std::size_t id) const
{
    return Find(id) != nullptr;
}

const KDTreeEntry2d* KDTree2d::Find(std::size_t id) const
{
    const auto it = std::find_if(entries_.begin(), entries_.end(), [id](const KDTreeEntry2d& entry) {
        return entry.id == id;
    });
    return it == entries_.end() ? nullptr : &*it;
}

std::vector<std::size_t> KDTree2d::Query(const Point2d& point, double eps) const
{
    std::vector<std::size_t> indices(entries_.size());
    for (std::size_t i = 0; i < entries_.size(); ++i)
    {
        indices[i] = i;
    }
    const std::unique_ptr<KDNode> root = BuildKDTree(entries_, indices.begin(), indices.end(), 0);

    std::vector<std::size_t> result;
    QueryExact(root.get(), entries_, point, eps, result);
    return result;
}

std::vector<std::size_t> KDTree2d::QueryWithinDistance(const Point2d& point, double maxDistance) const
{
    std::vector<std::size_t> indices(entries_.size());
    for (std::size_t i = 0; i < entries_.size(); ++i)
    {
        indices[i] = i;
    }
    const std::unique_ptr<KDNode> root = BuildKDTree(entries_, indices.begin(), indices.end(), 0);

    std::vector<std::size_t> result;
    const double maxDistanceSquared = maxDistance * maxDistance;
    QueryRadius(root.get(), entries_, point, maxDistanceSquared, result);
    return result;
}

std::optional<KDTreeHit2d> KDTree2d::Nearest(const Point2d& point) const
{
    std::vector<std::size_t> indices(entries_.size());
    for (std::size_t i = 0; i < entries_.size(); ++i)
    {
        indices[i] = i;
    }
    const std::unique_ptr<KDNode> root = BuildKDTree(entries_, indices.begin(), indices.end(), 0);

    std::optional<KDTreeHit2d> best;
    QueryNearest(root.get(), entries_, point, best);
    return best;
}

std::string KDTree2d::DebugString() const
{
    std::ostringstream stream;
    stream << "KDTree2d{size=" << Size()
           << ", valid=" << (IsValid() ? "true" : "false") << "}";
    return stream.str();
}

const std::vector<KDTreeEntry2d>& KDTree2d::Entries() const
{
    return entries_;
}

std::vector<KDTreeEntry2d>& KDTree2d::Entries()
{
    return entries_;
}

const std::vector<KDTreeEntry2d>& KDTree2d::Data() const
{
    return Entries();
}

std::vector<KDTreeEntry2d>& KDTree2d::Data()
{
    return Entries();
}
} // namespace geometry::sdk
