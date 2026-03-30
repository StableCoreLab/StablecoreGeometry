# 3D 第一阶段实施路线图

## 1. 目的

本文将当前 3D 设计文档组收束为第一阶段可执行的实施顺序。

目标不是把所有模块同时开工，而是明确：

- 哪些能力必须先做
- 哪些模块可以并行推进
- 哪些能力应放到第二阶段以后

## 2. 当前已有设计层

当前已经成文的 3D 设计层包括：

- 整体库方向
- 基础值类型
- 参数曲线 / 曲面层
- polyhedron / BRep 拓扑层
- triangle mesh / conversion 层
- 共享 3D service 层
- validation / healing 层

## 3. 第一阶段产品目标

第一阶段目标定义为：

- 建立稳定的 3D 基础类型与服务底座
- 支持以平面为主导的 polyhedron 工作流
- 支持 curve / surface 的基础求值与投影
- 支持 triangle mesh 生成与基础校验
- 建立轻量 BRep topology skeleton，但不急于做完整精确 boolean

## 4. 推荐阶段划分

建议拆成四个阶段块，而不是一条长链。

### 4.1 Phase A：基础内核

必须最先完成：

- `Point3d` / `Vector3d` / `Direction3d`
- `Box3d` / `Intervald`
- `Plane` / `Line3d` / `Ray3d` / `LineSegment3d` / `Triangle3d`
- `Transform3d` / `Matrix3d`
- `GeometryTolerance3d` / `GeometryContext3d`

这是所有后续层的共同底座。

### 4.2 Phase B：参数对象与服务核心

紧接着完成：

- `CurveEval3d` / `SurfaceEval3d`
- `Curve3d` / `Surface` 抽象基类
- `LineCurve3d`
- `PlaneSurface`
- point-line-plane predicates
- projection / intersection / distance 第一批能力

完成这一步后，3D 计算内核才真正可用。

### 4.3 Phase C：离散层与拓扑骨架

然后并行推进两条线：

- `TriangleMesh` / mesh conversion
- `PolyhedronBody` / 基础平面 face 工作流
- `BrepVertex` / `BrepEdge` / `BrepCoedge` / `BrepLoop` / `BrepFace` / `BrepBody` skeleton

这一阶段先不追完整算法，只要求结构成立、基础查询成立。

### 4.4 Phase D：校验与受控恢复

最后补：

- mesh validation
- polyhedron validation
- BRep validation
- conservative healing
- algorithm preprocess internals

这样后续 section / boolean / tessellation 才不会建立在脆弱输入之上。

## 5. 并行推进建议

适合并行的块：

- value types 与 tolerance / context
- mesh core 与 parametric base class
- polyhedron body 与 validation report types

不建议过早并行的块：

- 重型 BRep 算法
- healing 与 exact boolean
- 完整 NURBS 实现

## 6. 第一批公开 API

建议第一批公开的 3D API：

- 基础值类型
- `Curve3d` / `Surface` 基础协议
- `LineCurve3d`
- `PlaneSurface`
- `TriangleMesh`
- 第一批 3D predicate / projection / intersection 服务
- validation report types

第一批不建议急着公开：

- 完整 `BrepBody` 编辑能力
- `NurbsSurface` 编辑能力
- 精确 body boolean
- 高级 healing pipeline

## 7. 具体里程碑顺序

建议里程碑如下：

1. `geometry-3d-types`
   - 完成值类型与变换
   - 完成 tolerance / context
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

## 8. 明确延后的内容

建议明确后置：

- 完整 NURBS authoring
- 精确 BRep boolean
- shell offset / body offset
- 非平面 polyhedron 导入修复
- 高级 sewing / gap patching
- 全功能 topology editor

## 9. 为什么路线图要保守

原因很直接：

- 3D 真正困难的是恢复链与层间耦合
- 如果过早做 exact boolean 或全功能 BRep，会把设计期优势全部吃掉
- 先把值类型、服务、mesh、polyhedron、BRep skeleton 建稳，后续迭代速度反而更快

## 10. 当前之后的直接设计任务

在当前文档阶段之后，最自然的两个后续任务是：

- 编写 `section / tessellation / body-validation integration` 设计
- 编写第一阶段 API package / module layout 文档

## 11. 当前固定结论

这份路线图目前固定的结论是：

- 先打底座，再做 topology，再补恢复
- 以平面为主导的 workflow 应先于通用 exact BRep
- mesh 不是附属物，而是第一阶段核心交付之一
- BRep 第一阶段只做 skeleton 与基础校验，不追完整高阶算法
