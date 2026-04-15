# StableCore Geometry 总体设计

## 1. 目标定位

StableCore Geometry 是面向工程计算的 C++ 几何库，不是通用数学教材式库，也不是只做单点算法实验的仓库。当前代码已经形成了清晰的 2D 核心、3D 服务、BRep / polyhedron、mesh、section、healing、boolean、validation 和 search 子系统。

这个仓库的目标是：

- 面向工程场景，而不是面向竞赛题
- 先保证稳定、可测、可维护，再追求算法覆盖度
- 让类型层、算法层、服务层职责清楚
- 用统一容差和统一 diagnostics 约束跨模块行为

## 2. 当前代码结构

当前仓库主结构如下：

```text
include/
  common/
  types/
  sdk/
  serialize/
  export/

src/
  common/
  sdk/
  serialize/

tests/
  capabilities/
  gaps/
```

## 3. 公开入口

### 3.1 统一 umbrella

对外优先使用：

```cpp
#include "sdk/Geometry.h"
```

`Geometry.h` 是当前稳定 umbrella 入口，它聚合了 `GeometryApi`、`GeometryEpsilon`、`SearchPoly`、`BodyBoolean` 以及当前主要的 2D / 3D SDK 头文件。

### 3.2 值类型与 SDK 类型层

当前公开值类型和类型别名主要分布在：

- `include/types/*`
- `include/sdk/GeometryTypes.h`

典型类型包括：

- 2D: `Point2d`, `Vector2d`, `Box2d`, `Segment2d`, `LineSegment2d`, `ArcSegment2d`, `Polyline2d`, `Polygon2d`
- 3D: `Point3d`, `Vector3d`, `Direction3d`, `Box3d`, `Intervald`, `Line3d`, `Ray3d`, `LineSegment3d`, `Triangle3d`, `Transform3d`, `Matrix3d`, `Plane`
- 2D / 3D 结果与辅助类型：`SegmentProjection2d`, `GeometryTolerance3d`, `GeometryContext3d`, 各类 projection / intersection / validation / conversion / section / boolean 结果结构体

## 4. 设计原则

### 4.1 类型层与服务层分离

类型层负责表达几何数据与少量天然属性，服务层负责复杂算法、拓扑推导、恢复和诊断。

典型边界是：

- `Point2<T>`、`Vector2<T>`、`Box2<T>`、`Polyline2<T>`、`Polygon2<T>`、`Segment2<T>` 负责数据与基础不变量
- `Measure`、`Projection`、`Intersection`、`Relation`、`Section`、`Healing`、`BodyBoolean`、`SearchPoly` 负责算法与流程

### 4.2 容差是基础设施

工程几何必须统一处理浮点误差、共线、闭合、投影、相交和退化情况。

当前代码已经将容差集中到：

- `include/common/GeometryEpsilon.h`
- `include/sdk/GeometryTypes.h` 中的 `GeometryTolerance3d` / `GeometryContext3d`

原则上不应在算法里散落固定魔法数。

### 4.3 复杂结果使用结构体

对于投影、求交、section、boolean、search 这类算法，调用方通常需要的不只是 `bool`，还包括：

- 结果类型
- 诊断信息
- 参数位置
- 退化状态
- 选择依据

所以当前设计倾向于返回结构化结果，而不是只返回单一标志位。

### 4.4 命名保持短名风格

当前代码沿用短名 API 风格，例如：

- `PointAt`
- `Bounds`
- `Length`
- `Area`
- `Validate`
- `ConvertTo...`
- `Rebuild...`

公开 API 不再回到 `GetXxx` 风格。

## 5. 2D 核心模块

当前 2D 核心围绕以下能力组织：

- 基础类型：`Point2`, `Vector2`, `Segment2`, `LineSegment2`, `ArcSegment2`, `Box2`, `Polyline2`, `Polygon2`
- SDK 包装：`Segment2d`, `LineSegment2d`, `ArcSegment2d`, `Polyline2d`, `Polygon2d`, `MultiPolyline2d`, `MultiPolygon2d`, `Rectangle2d`, `Circle2d`, `Ellipse2d`
- 算法服务：`Algorithms`, `AxisOps`, `Boolean`, `Measure`, `Offset`, `PathOps`, `Projection`, `Relation`, `Sampling`, `Topology`, `Validation`, `SearchPoly`, `GeometrySegmentSearch`, `GeometryBoxTree`, `GeometryKDTree`
- 结果与辅助：`Results`, `GeometryTypes`, `GeometryEpsilon`

这部分已经不只是“点线面基础类型”，而是带有完整工程流程的 2D 服务层。

## 6. 3D 核心模块

当前 3D 侧已经形成了稳定的值类型、参数对象、拓扑对象和服务层。

### 6.1 值类型和参数对象

主要包括：

- 值类型：`Point3d`, `Vector3d`, `Direction3d`, `Box3d`, `Intervald`, `Line3d`, `Ray3d`, `LineSegment3d`, `Triangle3d`, `Matrix3d`, `Transform3d`, `Plane`
- 参数对象：`Curve3d`, `LineCurve3d`, `NurbsCurve3d`, `Surface`, `PlaneSurface`, `RuledSurface`, `OffsetSurface`, `NurbsSurface`, `CurveOnSurface`

### 6.2 拓扑和实体

主要包括：

- `BrepVertex`
- `BrepEdge`
- `BrepCoedge`
- `BrepLoop`
- `BrepFace`
- `BrepShell`
- `BrepBody`
- `PolyhedronLoop3d`
- `PolyhedronFace3d`
- `PolyhedronBody`
- `TriangleMesh`

### 6.3 3D 服务层

当前 3D 服务层已覆盖：

- `Projection` / `Projection3d`
- `Intersection` / `Intersection3d`
- `Measure`
- `Relation` / `Relation3d`
- `Section`
- `Healing`
- `BodyBoolean`
- `BrepConversion`
- `BrepEditing`
- `MeshConversion`
- `MeshOps`
- `MeshRepair`
- `Tessellation`
- `Transform`
- `Validation`

## 7. 目录与职责边界

### 7.1 `common`

放容差和基础数值常量。

### 7.2 `types`

放透明值类型和基础几何语义类型。

### 7.3 `sdk`

放面向产品侧的公开头文件和服务接口。

### 7.4 `serialize`

放文本序列化和反序列化支持。

### 7.5 `src`

放对应实现。

### 7.6 `tests`

`tests/capabilities` 放已成立能力，`tests/gaps` 放仍未收敛的已知差距。

## 8. 当前边界结论

- 公共 API 继续使用稳定短名
- 类型层和服务层保持分离
- 容差和 diagnostics 统一管理
- 复杂算法优先返回结构化结果
- 2D 和 3D 可以共享设计原则，但不强行共享同一套数据层实现
- 当前更细的历史设计已合并到本总览页，避免多个设计稿继续分叉
