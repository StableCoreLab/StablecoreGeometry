# Box Design

## Purpose

本文定义当前仓库中 2D `Box2<T>` 体系的设计边界，作为后续维护、文档示例和 API 收口的参考基线。

覆盖范围：
- `Box2<T>` 值类型层
- 与 `MinPoint / MaxPoint / Width / Height / Area / Center / ExpandToInclude` 相关的语义
- box 与其他 2D 类型之间的基础协作边界

不覆盖：
- box tree / spatial index 实现细节
- polygon / polyline / boolean / offset 的高层算法逻辑

## Current Structure

文件：
- `include/types/Box2.h`

当前 `Box2<T>` 是透明值类型：
- 无 PImpl
- 可复制
- 直接建立在 `Point2<T>` 之上

它主要服务于：
- 基础几何对象的 `Bounds()`
- 粗判和空间过滤
- 更高层索引结构

## Core API

当前核心接口：
- `FromMinMax(minPoint, maxPoint)`
- `IsValid()`
- `MinPoint()`
- `MaxPoint()`
- `Width()`
- `Height()`
- `Area()`
- `Center()`
- `ExpandToInclude(point)`
- `ExpandToInclude(box)`
- `AlmostEquals(other, eps)`
- `DebugString()`

设计结论：
- `Box2<T>` 负责最基础、最稳定的 axis-aligned bounding box 语义
- 不应扩展成承载旋转矩形、业务矩形或复杂区域语义的泛化对象

## Naming Rules

当前 box 体系统一采用短名风格：
- `MinPoint()`
- `MaxPoint()`
- `Width()`
- `Height()`
- `Area()`
- `Center()`

结论：
- 不再使用新的 `GetMinPoint()` / `GetMaxPoint()` 风格
- 历史文档若仍写 `GetMinPoint` / `GetMaxPoint`，应理解为旧措辞

## Semantic Rules

### Axis-Aligned Only

`Box2<T>` 只表达 axis-aligned bounding box。

它不是：
- 任意旋转矩形
- 业务语义矩形对象
- 一般四边形

如果未来需要这类对象，应另建类型，而不是继续膨胀 `Box2<T>`。

### Closed Range Semantics

当前 box 语义按闭区间处理：
- `min.x <= x <= max.x`
- `min.y <= y <= max.y`

这意味着：
- 边界点属于 box
- 退化到线段或点的 box 仍可表达

### Validity

当前 `IsValid()` 规则：
- `valid_` 为真
- `min_` / `max_` 自身有效
- `min_.x <= max_.x`
- `min_.y <= max_.y`

默认构造得到无效 box。

设计结论：
- “可构造”不代表“有效”
- 算法或调用方若需要使用 box，必须区分无效状态和有效状态

## Geometric Behaviors

### `MinPoint()` / `MaxPoint()`

这两个接口提供 box 的主数据访问面。

它们是 box 自身的天然属性，应继续保留为成员方法主入口。

### `Width()` / `Height()` / `Area()` / `Center()`

这几个接口都建立在有效 box 之上。

当前规则：
- 调用前默认 box 已有效
- 无效 box 不应在上层流程中被当成正常几何范围继续使用

### `ExpandToInclude(...)`

当前 `ExpandToInclude(point)` / `ExpandToInclude(box)` 规则：
- 若当前 box 无效，则以输入建立初始有效 box
- 若当前 box 有效，则扩大到包含目标

设计结论：
- 这是 box 最核心的增量构造行为
- 后续 bounds 聚合逻辑应尽量复用这套语义，而不是各处单独写范围合并规则

## Relation To Other Types

### With Segment / Polyline / Polygon

`Bounds()` 统一表示 axis-aligned bounding box。

因此：
- segment 的 `Bounds()`
- polyline 的 `Bounds()`
- polygon 的 `Bounds()`

都应最终回到 `Box2<T>` 这套语义，而不是各自重新发明 box 规则。

### With Metrics / Filtering

跨对象过滤行为继续保留为自由函数，例如：
- `Contains(const Box2d&, const Point2d&, eps)`
- `Intersects(const Box2d&, const Box2d&, eps)`

设计结论：
- `Box2<T>` 负责表达数据和天然属性
- 具体关系判断仍适合放在自由函数或索引模块中

## Member vs Free Function Boundary

box 相关边界按当前 2D 收口规则处理：

### 对象自身天然行为

保留为成员方法：
- `MinPoint()`
- `MaxPoint()`
- `Width()`
- `Height()`
- `Area()`
- `Center()`
- `ExpandToInclude(...)`
- `IsValid()`

### 跨对象关系与过滤

保留为自由函数：
- `Contains(box, point, eps)`
- `Intersects(boxA, boxB, eps)`

## Bottom Line

当前 `Box2<T>` 的主线已经稳定：
- 命名稳定
- 有效性规则稳定
- 闭区间语义稳定
- 与其他类型的 `Bounds()` 关系稳定

后续重点不应再放在 box 命名改动，而应放在：
- 保持所有 `Bounds()` 语义继续统一
- 避免把 `Box2<T>` 膨胀成非 AABB 泛化对象
- 避免文档继续混用旧 `GetMinPoint` / `GetMaxPoint` 风格
