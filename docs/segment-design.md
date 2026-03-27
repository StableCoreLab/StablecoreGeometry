# Segment Design

## Purpose

本文定义当前仓库中 2D segment 体系的设计边界，作为后续维护和文档示例的参考基线。

这份文档只覆盖：
- `Segment2<T>` / `LineSegment2<T>` / `ArcSegment2<T>` 值类型层
- `sdk::Segment2d` / `sdk::LineSegment2d` / `sdk::ArcSegment2d` SDK 层
- 与 segment 直接相关的命名、参数化、包围盒和成员方法边界

不覆盖：
- 投影、相交、偏移等跨对象算法细节
- polyline / polygon 组合语义
- 3D segment 体系

## Current Structure

当前 segment 体系分两层：

### 1. `types` 层

文件：
- `include/types/Segment2.h`
- `include/types/LineSegment2.h`
- `include/types/ArcSegment2.h`

职责：
- 定义轻量、透明、可复制的基础值类型
- 提供 segment 的天然属性和直接参数化行为
- 作为更高层 polyline / polygon / 算法实现的基础几何单元

### 2. `sdk` 层

文件：
- `include/sdk/Segment2d.h`
- `include/sdk/LineSegment2d.h`
- `include/sdk/ArcSegment2d.h`

职责：
- 提供 SDK 暴露面
- 统一调试输出、克隆、多态承载等工程化能力
- 与其他 SDK 层对象和算法头协同

## Core Types

### `SegmentKind2`

当前公开枚举：
- `Line`
- `Arc`

它用于显式区分 segment 具体类型，不应再扩展成承载复杂策略的状态枚举。

### `ArcDirection`

当前公开枚举：
- `Clockwise`
- `CounterClockwise`

它仅表达圆弧方向，不承担一般方向系统的职责。

### `Segment2<T>`

`Segment2<T>` 是 `types` 层的统一抽象基类。

当前核心接口：
- `Kind()`
- `StartPoint()`
- `EndPoint()`
- `Length()`
- `Bounds()`
- `PointAt(parameter)`
- `PointAtLength(distanceFromStart, clampToSegment)`
- `IsValid()`

设计结论：
- 这是一个薄抽象，只承载所有 segment 共有的几何行为
- 不在这里塞复杂算法
- 不在这里扩展和自由函数完全重叠的额外接口

### `LineSegment2<T>`

`LineSegment2<T>` 表达有限直线段。

当前设计特点：
- 数据简单，主要由起点和终点定义
- 成员行为直接覆盖长度、包围盒、参数点、按长度取点等天然属性

### `ArcSegment2<T>`

`ArcSegment2<T>` 表达有限圆弧段。

当前设计特点：
- 通过圆心、半径、起止角与方向定义
- 提供 `Direction()`、`SignedSweep()` 这类弧特有行为
- 与 `LineSegment2<T>` 共享统一的 segment 抽象面

## Naming Rules

当前 segment 体系统一采用短名风格：
- `Kind()`
- `StartPoint()`
- `EndPoint()`
- `Length()`
- `Bounds()`
- `PointAt()`
- `PointAtLength()`

结论：
- 不再使用新的 `GetXxx` 风格
- 历史文档若仍写 `GetBoundingBox` / `GetPointAt` / `GetKind`，都应理解为旧措辞，后续应同步改为当前短名

## Member vs Free Function Boundary

这是当前 segment 体系最重要的收口规则之一。

### 成员方法负责什么

成员方法负责对象自身天然就应知道的事：
- `Kind()`
- `StartPoint()`
- `EndPoint()`
- `Length()`
- `Bounds()`
- `PointAt()`
- `PointAtLength()`
- `IsValid()`

### 自由函数负责什么

自由函数负责跨对象、跨类型或算法语义更强的行为：
- `ProjectPointToSegment(...)`
- `Intersect(...)`
- `Distance(...)`
- `Offset(...)`
- `Relate(...)`

### 当前收口结论

以下自由函数如果存在，只应视为兼容层，而不是主推荐入口：
- `Length(segment)`
- `Bounds(segment)`
- `PointAt(segment, ...)`
- `PointAtLength(segment, ...)`

后续文档和测试示例应优先写成：
- `segment.Length()`
- `segment.Bounds()`
- `segment.PointAt(t)`
- `segment.PointAtLength(length)`

参考：
- `docs/api-member-free-function-checklist.md`
- `docs/sdk-2d-api-convergence.md`

## Parameterization Rules

### `PointAt(parameter)`

统一语义：
- `parameter = 0` 表示起点
- `parameter = 1` 表示终点
- 对位于 `[0, 1]` 内的参数，返回 segment 上对应位置

对于超出 `[0, 1]` 的参数：
- 当前实现允许按各自 segment 的自然参数化继续外推
- 但文档、测试与上层算法不应默认依赖所有 segment 在越界参数上的完全一致工程语义

### `PointAtLength(distanceFromStart, clampToSegment)`

统一语义：
- 以起点为长度零点
- `distanceFromStart = Length()` 对应终点
- `clampToSegment = true` 时，将距离截到有效长度范围内
- `clampToSegment = false` 时，允许按自然方向外推

一致性要求：
- 对有效 segment，`PointAtLength(Length())` 应与 `EndPoint()` 一致
- 对 `[0, 1]` 内参数，`PointAt(t)` 应与 `PointAtLength(t * Length())` 给出同一几何点

## Bounds Rules

`Bounds()` 统一表示 axis-aligned bounding box。

要求：
- 返回的是最小轴对齐包围盒
- 语义应与 `Box2<T>` 的有效性和边界规则一致
- 不把“无效 segment 的包围盒”伪装成有效几何范围

设计结论：
- `Bounds()` 是 segment 自身天然属性，因此保留为成员方法
- `Bounds(segment)` 这类自由函数只应保留为兼容层

## Validity Rules

### `LineSegment2<T>`

有效的最小条件：
- 起点有效
- 终点有效
- 起点和终点不是零长度重合

### `ArcSegment2<T>`

有效的最小条件：
- 圆心有效
- 半径有效且大于零
- 起止角有效
- 扫角不是零

当前原则：
- `IsValid()` 只做对象自身几何合法性判断
- 更强的算法前置条件应由具体算法层额外检查

## SDK Layer Notes

SDK 层 `Segment2d` 保留多态接口：
- `Kind()`
- `IsValid()`
- `StartPoint()`
- `EndPoint()`
- `Length()`
- `Bounds()`
- `PointAt()`
- `PointAtLength()`
- `DebugString()`
- `Clone()`

当前设计判断：
- 这层多态仍然是可接受的
- 但它不应继续承担越来越多的算法职责
- 若未来需要进一步瘦身，优先动算法自由函数边界，而不是先动 segment 多态面

## What Still Matters

当前 segment 体系剩余值得持续关注的问题不是命名统一，而是：
- 成员方法与自由函数之间仍有重复入口
- `types` 层与 `sdk` 层之间存在双层 segment 体系，需要持续控制复杂度
- 文档示例若继续混用成员和自由函数，会重新制造风格分裂

## Bottom Line

当前 2D segment 体系已经有稳定主线：
- 类型名稳定
- 成员接口名稳定
- 参数化和包围盒语义稳定

后续的重点不应是再改名字，而应是：
- 坚持成员方法作为 segment 天然属性主入口
- 把完全同构的自由函数收敛为兼容层
- 让上层文档和示例都按这套边界书写
