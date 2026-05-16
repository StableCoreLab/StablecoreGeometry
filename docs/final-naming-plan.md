# 最终命名规划

本文定义当前发布版 Geometry 公共面的命名规则。

## 总则

- `SC` 只保留给工程身份使用，例如仓库名、解决方案名、CMake target、包名和内部构建标识。
- 公共类型名、公共函数名、公共数据结构不使用 `SC` 前缀。
- 默认发布面不使用 `ISC`。
- 需要跨 DLL 的公共 ABI 稳定数据结构继续通过 `GEOMETRY_API` 导出。
- 公共面按目录和语义命名，不按产品前缀组织。

## 命名空间

- 当前公共命名空间为 `Geometry`。
- `Geometry::Sdk` 只允许作为迁移期间的临时别名。
- 本文档适用于源码树中的文件、类型、结果和公共 API，不适用于安装后的目录前缀。

### 兼容策略

- 只保留一个从 `Geometry::Sdk` 指向 `Geometry` 的过渡别名。
- 不恢复 `SC*` 或 `ISC*` 兼容类。
- 不把兼容头放进默认安装面。
- 迁移期结束后删除别名。

## 源码 Include/Core

保留：

- `GeometryApi.h`
- `GeometryTypes.h`
- `Algorithms.h`
- `AxisOps.h`
- `Boolean.h`
- `Editing.h`
- `Intersection.h`
- `Measure.h`
- `Metrics.h`
- `Offset.h`
- `Projection.h`
- `Relation.h`
- `Results.h`
- `Sampling.h`
- `SearchPoly.h`
- `Section.h`
- `ShapeOps.h`
- `Transform.h`
- `Validation.h`

删除：

- `SC*` 兼容名字
- `ISC*` 兼容名字

## 源码 Include/Geometry2d

保留：

- `ArcSegment2d.h`
- `BoxTree2d.h`
- `Circle2d.h`
- `Ellipse2d.h`
- `KDTree2d.h`
- `LineSegment2d.h`
- `MultiPolygon2d.h`
- `MultiPolyline2d.h`
- `PathOps.h`
- `Polygon2d.h`
- `Polyline2d.h`
- `Rectangle2d.h`
- `Segment2d.h`
- `SegmentSearch2d.h`

删除：

- `SCArcSegment2d.h`
- `SCCircle2d.h`
- `SCLineSegment2d.h`
- `SCMultiPolygon2d.h`
- `SCMultiPolyline2d.h`
- `SCSegment2d.h`
- `ISCPolygon2d.h`
- `ISCPolyline2d.h`

## 源码 Include/Geometry3d

保留：

- `Curve3d.h`
- `CurveOnSurface.h`
- `LineCurve3d.h`
- `NurbsCurve3d.h`
- `NurbsSurface.h`
- `OffsetSurface.h`
- `PlaneSurface.h`
- `RuledSurface.h`
- `Surface.h`
- `TriangleMesh.h`

删除：

- `SCCurve3d.h`

## 源码 Include/Brep

保留：

- `BodyBoolean.h`
- `BrepBody.h`
- `BrepCoedge.h`
- `BrepConversion.h`
- `BrepEdge.h`
- `BrepEditing.h`
- `BrepFace.h`
- `BrepLoop.h`
- `BrepShell.h`
- `BrepVertex.h`
- `Healing.h`
- `MeshConversion.h`
- `MeshOps.h`
- `MeshRepair.h`
- `PolyhedronBody.h`
- `PolyhedronFace3d.h`
- `PolyhedronLoop3d.h`
- `Tessellation.h`
- `Topology.h`

删除：

- 不再保留额外的公共包装头

## 源码 Include/Support 与 Include/Types

保留：

- `Support/Epsilon.h`
- `Support/Geometry2d/Normalize2.h`
- `Support/Geometry2d/Predicate2.h`
- `Types/Geometry2d/*`
- `Types/Geometry3d/*`
- `Types/Detail/Segment2Detail.h`

删除：

- 无用整数别名和兼容辅助
- 所有 `SC*` / `ISC*` 兼容头

## ABI 说明

- 需要跨 DLL 的公共值类型，应继续放在导出头里并保持 `GEOMETRY_API`。
- 不要为了 DLL 可见性给值类型额外加 `SC` 前缀。
- 只供内部使用的类型，应放入 detail 头或源文件，而不是继续放进源码 `Include/`。
