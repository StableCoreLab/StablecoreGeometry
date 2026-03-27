# 3D First-Phase Implementation Roadmap

## 1. Purpose

本文把当前 3D 设计文档组收束为第一阶段可执行的实施顺序。

目标不是把所有模块同时开工，而是明确：
- 哪些能力必须先做
- 哪些模块可以并行推进
- 哪些能力应作为第二阶段后置

## 2. Current Design Layers

当前已经成文的 3D 设计层包括：
- overall library direction
- foundational value types
- parametric curve/surface layer
- polyhedron/BRep topology layer
- triangle mesh / conversion layer
- shared 3D services layer
- validation / healing layer

## 3. Phase-1 Product Goal

第一阶段目标应定义为：
- 建立稳定的 3D 基础类型与服务底座
- 支持 plane-dominant polyhedron workflows
- 支持 curve / surface 基础求值与投影
- 支持 triangle mesh 生成与基础校验
- 建立轻量 BRep topology skeleton，但不急着做完整精确 boolean

## 4. Recommended Phase Split

建议分成四个阶段块，而不是一条长链。

### 4.1 Phase A: Foundational Kernel

必须最先完成：
- `Point3d` / `Vector3d` / `Direction3d`
- `Box3d` / `Intervald`
- `Plane` / `Line3d` / `Ray3d` / `LineSegment3d` / `Triangle3d`
- `Transform3d` / `Matrix3d`
- `GeometryTolerance3d` / `GeometryContext3d`

这是所有后续层的共同底座。

### 4.2 Phase B: Parametric and Service Core

紧接着完成：
- `CurveEval3d` / `SurfaceEval3d`
- `Curve3d` / `Surface` 抽象基类
- `LineCurve3d`
- `PlaneSurface`
- point-line-plane predicates
- projection / intersection / distance first batch

这一步完成后，3D 计算内核才真正可用。

### 4.3 Phase C: Discrete and Topology Skeleton

然后并行推进两条线：
- `TriangleMesh` / mesh conversion
- `PolyhedronBody` / basic planar face workflow
- `BrepVertex` / `BrepEdge` / `BrepCoedge` / `BrepLoop` / `BrepFace` / `BrepBody` skeleton

这一步先不追完整算法，只追结构成立和基础查询成立。

### 4.4 Phase D: Validation and Controlled Recovery

最后补：
- mesh validation
- polyhedron validation
- BRep validation
- conservative healing
- algorithm preprocess internals

这样后面 section / boolean / tessellation 才不会建立在脆弱输入上。

## 5. Parallelism Guidance

建议可并行的块：
- value types 与 tolerance/context
- mesh core 与 parametric base class
- polyhedron body 与 validation report types

建议不要过早并行的块：
- BRep heavy algorithms
- healing 与 exact boolean
- NURBS 完整实现

## 6. First Public API Cut

第一批建议公开的 3D API：
- foundational value types
- `Curve3d` / `Surface` base protocols
- `LineCurve3d`
- `PlaneSurface`
- `TriangleMesh`
- first-batch 3D predicate/projection/intersection services
- validation report types

第一批不建议急着公开：
- full `BrepBody` editing
- `NurbsSurface` editing
- exact body boolean
- advanced healing pipelines

## 7. Concrete Milestone Order

建议里程碑：

1. `geometry-3d-types`
- 完成值类型和变换
- 完成 tolerance/context

2. `geometry-3d-services-core`
- 完成基础 predicates / projections / intersections
- 完成统一结果结构

3. `curve-surface-core`
- 完成 `Curve3d` / `Surface`
- 完成 `LineCurve3d` / `PlaneSurface`

4. `trianglemesh-core`
- 完成 `TriangleMesh`
- 完成 bounds / transform / validate

5. `polyhedron-core`
- 完成平面 face / loop / body
- 接上 projected 2D polygon workflow

6. `brep-skeleton`
- 完成 vertex / edge / coedge / loop / face / body 框架
- 接上 `CurveOnSurface` 基础表达

7. `validation-healing-core`
- 完成 issues / reports / conservative repairs

8. `mesh-and-body-conversion`
- 完成 polyhedron -> mesh
- 完成 surface -> mesh
- 完成受限 planar polyhedron -> brep

## 8. What To Delay Deliberately

建议明确后置：
- full NURBS authoring
- exact BRep boolean
- shell offset / body offset
- non-planar polyhedron import repair
- advanced sewing / gap patching
- full feature-rich topology editor

## 9. Why This Roadmap Is Conservative

原因很直接：
- 3D 真正难的是恢复链和层间耦合
- 如果过早做 exact boolean 或全功能 BRep，会把设计期优势全部吃掉
- 先把值类型、服务、mesh、polyhedron、BRep skeleton 建稳，后面迭代速度反而会更快

## 10. Immediate Next Design Tasks

在当前文档阶段之后，最自然的两个后续任务是：
- 写 `section / tessellation / body-validation integration` 设计
- 写第一阶段 API package / module layout 文档

## 11. Key Roadmap Decisions

这份路线图最终固定的结论是：
- 先打底座，再做 topology，再补恢复
- plane-dominant workflow 应先于 exact general BRep
- mesh 不是附属物，而是第一阶段核心交付之一
- BRep 第一阶段只做 skeleton 和基础校验，不追完整高阶算法
