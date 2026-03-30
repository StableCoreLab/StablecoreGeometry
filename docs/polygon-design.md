# Polygon 设计

## 1. 目的

本文定义当前仓库中 2D polygon 体系的设计边界，作为后续维护、文档示例和 API 收口的参考基线。

## 2. 当前对象范围

当前 polygon 体系包括：

- `Polygon2<T>`
- `sdk::Polygon2d`
- `MultiPolygon2d`

## 3. 核心语义

polygon 是区域对象，不是单纯路径对象。

其核心语义包括：

- outer ring
- hole rings
- 面积语义
- 方向规则
- 区域包含关系

## 4. 天然成员能力

当前适合保留为成员方法的包括：

- `OuterRing()`
- `HoleCount()`
- `HoleAt(...)`
- `Bounds()`
- `Area()`

## 5. 非成员能力

以下能力继续放自由函数 / 算法层：

- boolean
- offset
- relation
- topology build
- rebuild / repair
- cut / split

## 6. 当前固定结论

- polygon 继续明确作为区域对象
- hole 语义是第一等设计要素
- 复杂区域算法继续保留在自由函数层
