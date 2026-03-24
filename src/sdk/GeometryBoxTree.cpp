#include "sdk/GeometryBoxTree.h"

#include <algorithm>
#include <sstream>
#include <utility>

#include "sdk/GeometryMetrics.h"

namespace geometry::sdk
{
namespace
{
[[nodiscard]] bool Matches(const BoxTreeEntry2d& entry, std::size_t id)
{
    return entry.id == id;
}
} // namespace

GeometryBoxTree2d::GeometryBoxTree2d(std::vector<BoxTreeEntry2d> entries)
    : entries_(std::move(entries))
{
}

void GeometryBoxTree2d::Clear()
{
    entries_.clear();
}

void GeometryBoxTree2d::Add(std::size_t id, const Box2d& box)
{
    entries_.push_back(BoxTreeEntry2d{id, box});
}

bool GeometryBoxTree2d::Remove(std::size_t id)
{
    const auto it = std::remove_if(entries_.begin(), entries_.end(), [id](const BoxTreeEntry2d& entry) {
        return entry.id == id;
    });
    if (it == entries_.end())
    {
        return false;
    }

    entries_.erase(it, entries_.end());
    return true;
}

void GeometryBoxTree2d::Update(std::size_t id, const Box2d& box)
{
    if (auto* entry = const_cast<BoxTreeEntry2d*>(Find(id)); entry != nullptr)
    {
        entry->box = box;
        return;
    }

    Add(id, box);
}

std::size_t GeometryBoxTree2d::Size() const
{
    return entries_.size();
}

bool GeometryBoxTree2d::IsEmpty() const
{
    return entries_.empty();
}

bool GeometryBoxTree2d::IsValid() const
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

bool GeometryBoxTree2d::Contains(std::size_t id) const
{
    return Find(id) != nullptr;
}

const BoxTreeEntry2d* GeometryBoxTree2d::Find(std::size_t id) const
{
    const auto it = std::find_if(entries_.begin(), entries_.end(), [id](const BoxTreeEntry2d& entry) {
        return entry.id == id;
    });
    return it == entries_.end() ? nullptr : &*it;
}

std::vector<std::size_t> GeometryBoxTree2d::Query(const Box2d& box, double eps) const
{
    std::vector<std::size_t> result;
    for (const auto& entry : entries_)
    {
        if (Intersects(entry.box, box, eps))
        {
            result.push_back(entry.id);
        }
    }
    return result;
}

std::vector<std::size_t> GeometryBoxTree2d::QueryContaining(const Point2d& point, double eps) const
{
    std::vector<std::size_t> result;
    for (const auto& entry : entries_)
    {
        if (geometry::sdk::Contains(entry.box, point, eps))
        {
            result.push_back(entry.id);
        }
    }
    return result;
}

std::string GeometryBoxTree2d::DebugString() const
{
    std::ostringstream stream;
    stream << "GeometryBoxTree2d{size=" << Size()
           << ", valid=" << (IsValid() ? "true" : "false") << "}";
    return stream.str();
}

const std::vector<BoxTreeEntry2d>& GeometryBoxTree2d::Data() const
{
    return entries_;
}

std::vector<BoxTreeEntry2d>& GeometryBoxTree2d::Data()
{
    return entries_;
}
} // namespace geometry::sdk
