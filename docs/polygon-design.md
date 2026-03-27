# Polygon Design

## Purpose

本文定义当前仓库中 2D polygon 体系的设计边界，作为后续维护、文档示例和 API 收口的参考基线。

覆盖范围：
- `Polygon2<T>` 值类型层
- `sdk::Polygon2d`
- `sdk::MultiPolygon2d`
- 与 polygon 直接相关的 outer ring / hole、方向、面积和包围盒语义

不覆盖：
- boolean / relation / topology / offset 的算法细节
- line-network build 和 SearchPoly 风格恢复逻辑

## Current Structure

### 1. `types` 层

文件：
- `include/types/Polygon2.h`

职责：
- 表达由一个 outer ring 和零个或多个 hole 组成的 2D 区域对象
- 提供面积、周长、质心、包围盒和基本有效性规则

### 2. `sdk` 层

文件：
- `include/sdk/Polygon2d.h`
- `include/sdk/MultiPolygon2d.h`

职责：
- 提供 SDK 暴露面
- 与布尔、偏移、拓扑、关系、构面等算法模块协同
- 为上层 workflow 提供稳定公共对象

## Core Types

### `Polygon2<T>`

`types` 层当前核心接口：
- `OuterRing()`
- `HoleCount()`
- `HoleAt(index)`
- `Area()`
- `Perimeter()`
- `Centroid()`
- `Bounds()`
- `IsValid()`

设计结论：
- polygon 是区域对象，不是简单的闭合 polyline
- outer ring 和 hole 都是结构上显式存在的组成部分

### `sdk::Polygon2d`

SDK 层当前公开接口：
- `IsValid()`
- `PointCount()`
- `SegmentCount()`
- `HoleCount()`
- `OuterRing()`
- `HoleAt(index)`
- `Bounds()`
- `DebugString()`

说明：
- SDK 层更偏工程对象使用面
- 面积、质心、布尔、关系等更高层行为通过 shape ops / boolean / topology 等自由函数模块提供

### `sdk::MultiPolygon2d`

当前公开接口：
- `Count()`
- `IsEmpty()`
- `IsValid()`
- `Add(...)`
- `PolygonAt(index)`
- `PointCount()`
- `SegmentCount()`
- `HoleCount()`
- `Bounds()`
- `Data()`

设计结论：
- 聚合容器统一使用 `Count()`
- 不再恢复 `PolygonCount()` 这类重复命名

## Naming Rules

当前 polygon 体系统一采用短名风格：
- `OuterRing()`
- `HoleCount()`
- `HoleAt()`
- `Bounds()`
- `Count()`

结论：
- 不再使用新的 `GetXxx` 风格
- 历史文档若仍写 `GetOuterRing` / `GetHoleCount` / `GetHole` / `GetBoundingBox`，应理解为旧措辞

## Structural Rules

### Outer Ring

outer ring 是 polygon 的主边界。

要求：
- outer ring 必须有效
- outer ring 必须闭合
- outer ring 的有向面积必须为正

当前设计结论：
- outer ring 统一采用 counterclockwise 方向

### Holes

hole 是 polygon 内部扣除区域。

要求：
- 每个 hole 必须有效
- 每个 hole 必须闭合
- hole 的有向面积必须为负

当前设计结论：
- hole 统一采用 clockwise 方向

### Region Semantics

polygon 的几何语义是：
- outer ring 内部算区域
- holes 内部从区域中扣除

因此 polygon 的面积、周长、质心和关系判断，都不应退化成“只看外环”的简化对象模型。

## Area / Centroid / Bounds Rules

### `Area()`

当前 `types` 层 `Area()` 规则：
- outer ring 贡献正面积
- holes 贡献负面积
- 最终返回绝对值面积

### `Centroid()`

当前 `types` 层 `Centroid()` 规则：
- 基于 outer ring 和 holes 的综合面积矩
- 不是只对 outer ring 做简单平均

### `Bounds()`

`Bounds()` 统一表示 axis-aligned bounding box。

要求：
- 返回整个 polygon 的最小轴对齐包围盒
- holes 仍属于 polygon 几何范围的一部分，因此 `Bounds()` 需要覆盖 outer 和 holes 的坐标范围
- 语义与 `Box2<T>` 保持一致

## Validity Rules

当前 `types` 层最小有效性条件：
- outer ring 有效且闭合
- outer ring 有正有向面积
- 每个 hole 有效且闭合
- 每个 hole 有负有向面积

说明：
- 这里的 `IsValid()` 主要收口方向和结构层约束
- 更强的几何约束，例如 hole 与 outer 的实际包含关系、自交、复杂拓扑一致性等，可以由更高层验证模块继续承担

## Member vs Free Function Boundary

polygon 相关边界沿用当前 2D 收口规则。

### 对象自身天然结构行为

保留为成员或对象方法：
- `OuterRing()`
- `HoleCount()`
- `HoleAt()`
- `Bounds()`
- `IsValid()`

### 统一 shape / algorithm 入口

保留为自由函数：
- `Area(const Polygon2d&)`
- `Centroid(const Polygon2d&)`
- `Contains(...)`
- `Relate(...)`
- `Intersect(...)`
- `Union(...)`
- `Difference(...)`
- `Offset(...)`

当前结论：
- polygon 的区域语义强，很多高层能力继续保留为自由函数更合理
- 但结构性读取接口应稳定留在对象自身，不再扩散 `GetXxx` 风格重复入口

## SDK Layer Notes

SDK 层 `Polygon2d` 当前使用 PImpl。

这意味着：
- 它与 `LineSegment2d` / `ArcSegment2d` 这类透明值类型不完全同路
- 这仍是当前 SDK 设计里的一个特例

当前判断：
- 短期可以接受
- 长期仍应和 `Polyline2d` 一起评估是否继续保留为 PImpl 特例

## Bottom Line

当前 2D polygon 体系的主线已经稳定：
- outer / hole 结构明确
- 方向约束明确
- 短名风格明确
- 区域对象和路径对象边界明确

后续重点不应再放在命名改动，而应放在：
- 保持结构访问接口稳定
- 让高层区域算法继续收敛在自由函数模块中
- 避免文档和示例继续混用旧接口名或模糊 outer / hole 语义
