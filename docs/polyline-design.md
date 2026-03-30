# Polyline 设计

## 1. 目的

本文定义当前仓库中 2D polyline 体系的设计边界，作为后续维护、文档示例和 API 收口的参考基线。

## 2. 当前对象范围

当前 polyline 体系包括：

- `Polyline2<T>`
- `sdk::Polyline2d`
- `MultiPolyline2d`

## 3. 设计重点

### 3.1 polyline 是有序路径对象

polyline 的核心语义是按顺序组织的顶点 / 边段路径。

### 3.2 闭合语义明确

应明确区分：

- 开放 polyline
- 闭合 polyline

不要把是否闭合混成隐含行为。

### 3.3 天然成员能力

当前适合保留为成员方法的包括：

- `SegmentCount()`
- `VertexCount()`
- `VertexAt(...)`
- `StartPoint()`
- `EndPoint()`
- `Bounds()`
- `PointAt(...)`
- `PointAtLength(...)`

## 4. 当前固定结论

- polyline 继续作为路径对象，而不是区域对象
- API 命名保持短名风格
- 大型算法与重建逻辑继续留在自由函数层
