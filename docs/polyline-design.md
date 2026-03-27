# Polyline Design

## Purpose

本文定义当前仓库中 2D polyline 体系的设计边界，作为后续维护、文档示例和 API 收口的参考基线。

覆盖范围：
- `Polyline2<T>` 值类型层
- `sdk::Polyline2d`
- `sdk::MultiPolyline2d`
- 与 polyline 直接相关的闭合语义、计数接口、参数化和包围盒语义

不覆盖：
- path editing 算法细节
- polygon 语义
- line-network build / SearchPoly / boolean / offset 的实现细节

## Current Structure

### 1. `types` 层

文件：
- `include/types/Polyline2.h`

职责：
- 表达由有序 segment 序列组成的路径对象
- 承担点访问、长度、整体参数化和有效性校验等天然行为

### 2. `sdk` 层

文件：
- `include/sdk/Polyline2d.h`
- `include/sdk/MultiPolyline2d.h`

职责：
- 提供 SDK 暴露面
- 承担工程化接口、拷贝语义和调试输出
- 为上层 shape/path/offset/boolean 工作流提供公共对象

## Core Types

### `PolylineClosure`

当前闭合枚举：
- `Open`
- `Closed`

设计结论：
- 闭合状态是 polyline 语义的一部分，不应由“首尾点是否碰巧相等”隐式推断替代

### `Polyline2<T>`

`types` 层 polyline 的当前核心接口：
- `SegmentCount()`
- `SegmentAt(index)`
- `Closure()`
- `IsClosed()`
- `VertexCount()`
- `VertexAt(index)`
- `StartPoint()`
- `EndPoint()`
- `Length()`
- `Bounds()`
- `PointAt(parameter)`
- `PointAtLength(distanceFromStart, clampToPath)`
- `LengthAt(parameter)`
- `ParameterAtLength(distanceFromStart, clampToPath)`
- `IsValid()`

设计结论：
- 这是一个“有序 segment 序列 + 闭合状态”的路径对象
- 它不是简单的点数组别名
- 顶点访问和整体参数化都建立在 segment 语义之上

### `sdk::Polyline2d`

SDK 层当前公开接口：
- `IsValid()`
- `IsClosed()`
- `PointCount()`
- `SegmentCount()`
- `PointAt(index)`
- `Bounds()`
- `DebugString()`

说明：
- SDK 层当前公开的是“点序列风格”的 polyline 使用面
- `types` 层是更底层的 segment 组合表达
- 两层不是完全镜像，后续文档和代码要避免混淆这两个层次

### `sdk::MultiPolyline2d`

当前公开接口：
- `Count()`
- `IsEmpty()`
- `IsValid()`
- `Add(...)`
- `PolylineAt(index)`
- `PointCount()`
- `SegmentCount()`
- `Bounds()`
- `Data()`

设计结论：
- 聚合容器统一使用 `Count()`
- 不再恢复 `PolylineCount()` 这类重复命名

## Naming Rules

当前 polyline 体系统一采用短名风格：
- `SegmentCount()`
- `VertexCount()`
- `VertexAt()`
- `StartPoint()`
- `EndPoint()`
- `Bounds()`
- `PointAt()`
- `PointAtLength()`
- `Count()`

结论：
- 不再使用新的 `GetXxx` 风格
- 历史文档若仍写 `GetBoundingBox` / `GetVertexCount` / `GetPointAt`，应理解为旧措辞

## Semantic Rules

### Connectivity

`Polyline2<T>` 的有效性依赖 segment 连通性：
- 前一段 `EndPoint()` 必须与后一段 `StartPoint()` 连通
- 空 polyline 无效
- 总长度不能为零

### Closure

对闭合 polyline：
- `Closure() == Closed`
- 最后一段终点必须与第一段起点连通

对开放 polyline：
- 只要求段序连续，不要求首尾重合

### Vertex Model

当前 `types` 层顶点规则：
- open polyline 的顶点数 = 段数 + 1
- closed polyline 的顶点数 = 段数

设计结论：
- 顶点是由 segment 序列导出的统一视图
- 不是独立于 segment 语义的第二套主数据模型

## Parameterization Rules

### `PointAt(parameter)`

统一语义：
- `parameter = 0` 表示路径起点
- `parameter = 1` 表示路径终点
- 通过整体长度参数化映射到具体 segment

### `PointAtLength(distanceFromStart, clampToPath)`

统一语义：
- 以路径起点为零点
- `distanceFromStart = Length()` 对应路径终点
- `clampToPath = true` 时，将长度截到有效范围内
- `clampToPath = false` 时，允许向前或向后延伸到首段/末段的自然外推

一致性要求：
- `PointAt(t)` 与 `PointAtLength(LengthAt(t))` 在有效参数范围内应给出同一几何点

## Bounds Rules

`Bounds()` 统一表示 axis-aligned bounding box。

要求：
- 返回的是整条路径的最小轴对齐包围盒
- 语义与 `Box2<T>` 保持一致
- 不把无效路径的包围盒伪装成有效几何范围

设计结论：
- `Bounds()` 是 polyline 自身天然属性，保留为成员方法或对象方法
- `Bounds(const Polyline2d&)` 这类自由函数保留为 shape/path 统一算法入口时，应避免和文档主示例竞争

## Member vs Free Function Boundary

polyline 相关边界沿用当前 2D 收口规则：

### 对象自身天然行为

优先作为成员或对象方法：
- `SegmentCount()`
- `VertexCount()`
- `VertexAt()`
- `StartPoint()`
- `EndPoint()`
- `Length()`
- `Bounds()`
- `PointAt()`
- `PointAtLength()`
- `IsValid()`

### 跨对象或统一 shape 算法

保留为自由函数：
- `Length(const Polyline2d&)`
- `Bounds(const Polyline2d&)`
- 以及更高层 path/search/build 相关算法

当前结论：
- 对 polyline 来说，`Length(...)` / `Bounds(...)` 这种 shape ops 自由函数仍可保留
- 但文档示例若能直接写对象方法，应优先用对象方法

## SDK Layer Notes

SDK 层 `Polyline2d` 当前使用 PImpl。

这意味着：
- 它不是透明值类型
- 但对外仍尽量呈现轻量路径对象的使用方式

当前设计判断：
- 短期可以接受
- 长期仍应和 `Polygon2d` 一起评估是否继续作为 SDK 值类型体系中的 PImpl 特例

## Bottom Line

当前 2D polyline 体系的主线已经稳定：
- 闭合语义明确
- 计数语义明确
- 整体参数化明确
- 短名风格明确

后续重点不应再放在命名改动，而应放在：
- 保持 `types` 与 `sdk` 两层语义边界清楚
- 避免文档继续混用旧 `GetXxx` 风格
- 避免路径天然行为与自由函数入口继续重复扩张
