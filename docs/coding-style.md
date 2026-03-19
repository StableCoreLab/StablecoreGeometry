```text
docs/coding-style.md
```

---

# ✅ StableCore Geometry 代码规范 v1.0（可执行版）

---

# 1. 命名空间与目录

## 1.1 目录结构（对外语义清晰）

```text
include/stablecore/geometry/
```

## 1.2 命名空间（内部简洁）

```cpp
namespace geometry
{
}
```

👉 说明：

* 对外：`stablecore/geometry`（清晰）
* 对内：`geometry::`（简洁）

---

# 2. 类型命名

## 2.1 使用 PascalCase

```cpp
struct Point2d;
struct Vector2d;
struct Segment2d;
struct Box2d;
struct ProjectionResult;
```

---

## 2.2 维度后缀统一

```cpp
2d / 3d
```

示例：

```cpp
Point2d   // ✅
Point2D   // ❌
Point2    // ❌
```

---

# 3. 函数命名

## 3.1 使用 PascalCase

```cpp
Distance
DistanceSquared
Dot
Cross
ProjectPointToSegment
IntersectSegments
```

---

## 3.2 布尔函数必须表达语义

```cpp
IsZero
IsEqual
IsParallel
IsPointOnSegment
```

---

## 3.3 禁止无意义前缀

```cpp
CalcDistance   // ❌
DoIntersect    // ❌
HandleSegment  // ❌
```

---

# 4. 变量命名

## 4.1 普通变量：lowerCamelCase

```cpp
startPoint
endPoint
distanceSquared
intersectionPoint
```

---

## 4.2 成员变量：统一 `m_` 前缀（你已确定）

```cpp
class Segment2d
{
public:
    Point2d m_start;
    Point2d m_end;
};
```

---

## 4.3 布尔变量必须可读

```cpp
bool isValid;
bool isParallel;
bool hasIntersection;
```

---

# 5. 常量规范（你已确定）

## 5.1 使用 `k` 前缀

```cpp
constexpr double kDefaultEpsilon = 1e-9;
constexpr double kPi = 3.14159265358979323846;
```

---

## 5.2 禁止裸常量

```cpp
fabs(a - b) < 1e-9   // ❌
```

必须写成：

```cpp
IsZero(a - b)        // ✅
```

---

# 6. 核心工具函数（必须统一）

```cpp
namespace geometry
{
constexpr double kDefaultEpsilon = 1e-9;

bool IsZero(double value, double eps = kDefaultEpsilon);
bool IsEqual(double lhs, double rhs, double eps = kDefaultEpsilon);
bool IsLess(double lhs, double rhs, double eps = kDefaultEpsilon);
bool IsGreater(double lhs, double rhs, double eps = kDefaultEpsilon);
}
```

👉 这是**几何库稳定性的核心**

---

# 7. 结果类型规范（非常重要）

## 7.1 禁止复杂算法只返回 bool

❌：

```cpp
bool IntersectSegments(...);
```

---

## 7.2 必须返回结构体

```cpp
struct SegmentIntersectionResult
{
    bool m_intersect{false};
    bool m_parallel{false};
    bool m_collinear{false};
    Point2d m_point{};
};
```

---

# 8. 文件命名规范

## 8.1 PascalCase

```text
Point2d.h
Vector2d.h
Segment2d.h
Distance2d.h
Distance2d.cpp
Intersect2d.h
```

---

## 8.2 一文件一职责

```text
Point2d.h     → 只放 Point2d
Distance2d.h  → 距离相关
```

---

# 9. 目录结构（最终版）

```text
include/
  common/
	Epsilon.h
	MathUtils.h
  types/
	Point2d.h
	Vector2d.h
	Segment2d.h
	Box2d.h
  algorithm/
	Distance2d.h
	Intersect2d.h
	Project2d.h

src/
  Distance2d.cpp
  Intersect2d.cpp
  Project2d.cpp

tests/
  test_point2d.cpp
  test_distance2d.cpp
  test_intersect2d.cpp

docs/
  coding-style.md
```

---

# 10. 示例代码（标准风格）

```cpp
#pragma once

namespace geometry
{
constexpr double kDefaultEpsilon = 1e-9;

struct Point2d
{
    double x{0.0};
    double y{0.0};
};

struct Segment2d
{
    Point2d m_start;
    Point2d m_end;
};

struct ProjectionResult
{
    Point2d m_point{};
    double m_parameter{0.0};
    double m_distanceSquared{0.0};
    bool m_isOnSegment{false};
};

bool IsZero(double value, double eps = kDefaultEpsilon);

double Distance(const Point2d& lhs, const Point2d& rhs);

ProjectionResult ProjectPointToSegment(
    const Point2d& point,
    const Segment2d& segment);
}
```

---

# 🔥 最关键的三条（你必须坚持）

如果只记住 3 条，就记住这 3 条：

### 1️⃣ 所有几何判断必须走容差函数

👉 禁止裸 `1e-6 / 1e-9`

---

### 2️⃣ 复杂算法必须返回 Result 结构

👉 不要只返回 bool

---

### 3️⃣ 命名必须表达“工程语义”

👉 `ProjectPointToSegment` > `Proj`

---



