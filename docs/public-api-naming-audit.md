# 公共 API 命名审计

本文记录最终一轮重构之前 `Include` 的当前命名状态。
目标是在保持外部表面稳定的同时，识别已经对齐的符号，以及如果未来真的要改名时应该只保留别名的符号。

## 审计摘要

- `Include` 中已经没有紧急需要处理的 `Get*` / `Set*` / `Calc*` 风格公共 API 名称。
- 当前 SDK 整体已经遵循较稳定的短命名风格。
- 剩余的命名问题，主要是要不要保留 `Geometry*`、`Brep*`、`Polyhedron*` 这类历史模块前缀，而不是修复明显错误的命名规范。
- 这一轮的默认策略是保留并文档化，而不是直接改名。

## 已收敛符号

| Area | Current names | Status | Notes |
| --- | --- | --- | --- |
| umbrella 入口 | `Geometry.h` | 稳定 | 保持为唯一面向产品的 umbrella 头文件。 |
| SDK 聚合 | `GeometryApi.h` | 稳定 | 供不想使用 umbrella 的使用者直接聚合。 |
| 共享类型 | `GeometryTypes.h`, `Results.h`, `Validation.h` | 稳定 | 结果类型和辅助类型已经符合公开表面的风格。 |
| 2D 值类型 | `Point2d`, `Vector2d`, `Box2d`, `Segment2d`, `LineSegment2d`, `ArcSegment2d`, `Polyline2d`, `Polygon2d`, `Rectangle2d` | 稳定 | 简短、清晰、并且已经一致。 |
| 3D 值类型 | `Point3d`, `Vector3d`, `Direction3d`, `Box3d`, `Intervald`, `Line3d`, `Ray3d`, `Triangle3d`, `Transform3d`, `Matrix3d`, `Plane` | 稳定 | 3D 类型层的命名已经统一。 |
| 核心查询 | `IsValid`, `IsEmpty`, `IsClosed`, `IsPeriodic`, `HasIntersection`, `HasNeighbor`, `HasSelfIntersection`, `Contains`, `Intersects` | 稳定 | 这是布尔行为的首选动词形式。 |
| 几何内在查询 | `PointAt`, `Bounds`, `Length`, `Area`, `VertexCount`, `VertexAt`, `StartPoint`, `EndPoint`, `OuterRing`, `HoleCount`, `HoleAt` | 稳定 | 这些已经是规范查询名。 |
| 算法入口 | `Project`, `Convert`, `Heal`, `Section`, `Validate`, `Measure`, `Search` 系列 | 稳定 | 模块名本身已经像操作族，而不是 helper。 |
| 布尔 / 截面 / 修复 / 搜索模块 | `BodyBoolean`, `Section`, `Healing`, `SearchPoly` | 稳定 | 这些已经是既定的外部模块名，这一轮不应改名。 |
| 空间索引模块 | `BoxTree2d`, `KDTree2d`, `SegmentSearch2d` | 稳定 | 旧的 `Geometry*` 名字保留为兼容别名。 |
| Brep / polyhedron 系列 | `BrepBody`, `BrepFace`, `BrepShell`, `BrepLoop`, `BrepEdge`, `BrepVertex`, `PolyhedronBody`, `PolyhedronFace3d`, `PolyhedronLoop3d` | 稳定 | 这类领域前缀属于外部契约的一部分，应当保留。 |

## 仅兼容符号

这些名字并不是问题符号，只是在未来如果真的改名时，应该只作为别名保留。

| Symbol group | Recommendation | Reason |
| --- | --- | --- |
| `Brep*` 和 `Polyhedron*` 系列 | 保留 | 领域前缀有实际价值，且下游已经习惯。 |
| `*3d` 结果和辅助结构体 | 保留 | 后缀能够明确表达维度信息。 |
| `Options` / `Result` / `Issue` 后缀类型 | 保留 | 这种风格与当前 SDK 一致，也便于扫描。 |

## 延后处理的改名候选

这一轮不建议做公开改名。如果以后真的要做大版本清理，较合理的候选只有：

- 把少数历史性的 `Geometry*` 索引类型名字收拢成更短的别名
- 未来如果有 helper-only 命名意外泄漏进 `Include`，再统一收敛

这两类都应当按兼容迁移处理，而不是当作可以原地修完的清理工作。

## 操作规则

新增任何公共符号之前：

1. 先确认是否已有稳定名字覆盖这个用例。
2. 如果行为属于单个对象，优先成员方法。
3. 只有在操作需要组合多个对象，或需要容差 / 上下文输入时，才优先自由函数。
4. 新名字要短，并且能直接表达意图。
5. 如果无法避免改名，必须先把旧符号作为兼容别名一起发布。
