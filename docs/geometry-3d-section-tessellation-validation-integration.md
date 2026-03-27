# 3D Section, Tessellation, and Validation Integration

## 1. Purpose

本文定义 StableCore 3D 设计中 `section`、`tessellation`、`body validation` 三条线如何接到当前八层设计体系上。

目标是：
- 明确这三条线分别依赖哪些底层模块
- 明确哪些能力可以共用，哪些不能混在一起
- 为第一阶段实现提供跨层串联图

## 2. Why These Three Need an Integration Doc

这三条线看起来分散，但实际高度耦合：
- `section` 依赖 intersection、projection、topology、preprocess、validation
- `tessellation` 依赖 curve/surface evaluation、mesh conversion、trim boundaries、search
- `body validation` 依赖 topology、geometry predicates、search、diagnostics

如果不提前定义串联关系，后续很容易重复造内部 helper。

## 3. Layer Mapping

当前八层设计与这三条线的关系如下：
- foundational value types
  - 提供点、向量、box、plane、transform、interval
- parametric curve/surface layer
  - 提供 `PointAt`、导数、normal、project、trim、split
- polyhedron/BRep topology layer
  - 提供 face/loop/edge/coedge/shell/body 结构
- triangle mesh / conversion layer
  - 提供离散输出和 mesh-level validation carrier
- shared 3D services layer
  - 提供 predicates / projection / intersection / search / measure
- validation / healing layer
  - 提供 issue/report/result 和 conservative repair
- roadmap layer
  - 规定优先级和后置边界

## 4. Section Pipeline

### 4.1 Supported First-Phase Section Targets

第一阶段最合理的 section 范围：
- `PolyhedronBody x Plane`
- `BrepFace x Plane`
- `BrepBody x Plane` 的受限版本
- `TriangleMesh x Plane`

不建议一开始就做：
- general surface-surface section
- exact body-body section graph reconstruction

### 4.2 Shared Dependencies

`section` 至少依赖：
- `GeometryIntersection3d`
- `GeometryProjection3d`
- `GeometrySearch3d`
- `CurveOnSurface`
- `GeometryValidation3d`

### 4.3 Polyhedron-Dominant Section Path

建议先走平面体路径：
1. 用 `GeometrySearch3d` 找 plane-face 候选
2. 用 face plane / edge clipping 计算交段
3. 拼接 section polylines
4. 输出 `Polyline3d` 或受限 `Curve3d` 集合
5. 用 validation 过滤零长段、重复段、开放链异常

这条线最适合成为第一条真正落代码的 3D 算法链。

### 4.4 BRep Section Path

BRep section 第一阶段建议只做到：
- plane 与 face support surface 的 candidate intersection
- edge / trim boundary 裁切
- curve fragment 输出

完整 body-level loop rebuild 后置。

## 5. Tessellation Pipeline

### 5.1 First-Phase Tessellation Targets

第一阶段建议支持：
- `PlaneSurface -> TriangleMesh`
- `PolyhedronBody -> TriangleMesh`
- `BrepBody -> TriangleMesh` 的受限版

### 5.2 Shared Dependencies

`tessellation` 依赖：
- `Curve3d` / `Surface` evaluation
- `GeometryMeasure3d`
- `GeometryProjection3d`
- `GeometryMeshConversion`
- `GeometryValidation3d`
- `GeometryMeshSearch`

### 5.3 Trim-Aware Surface Tessellation

对于 `PlaneSurface` 或后续 `NurbsSurface`，tessellation 应走两段：
1. 生成参数域采样候选
2. 用 trim loops / preimage curves 裁掉无效三角

这意味着：
- trim boundary 不是后处理细节
- `CurveOnSurface` 是 tessellation 可行性的前提之一

### 5.4 Body Tessellation

`BrepBody -> TriangleMesh` 应拆成：
- face-local tessellation
- boundary welding / shared-vertex reconciliation
- mesh validation

不要直接从 body 一次性吐网格而跳过 face-level diagnostics。

## 6. Body Validation Pipeline

### 6.1 Validation Inputs

`body validation` 需要覆盖：
- `PolyhedronBody`
- `BrepBody`
- `TriangleMesh`

### 6.2 Shared Dependencies

`body validation` 依赖：
- `GeometryPredicate3d`
- `GeometryRelation3d`
- `GeometrySearch3d`
- `GeometryDiagnostics3d`
- `GeometryValidation3d`

### 6.3 Validation Stages

建议统一成三阶段：
1. primitive checks
2. topology consistency checks
3. spatial inconsistency checks

例如：
- primitive: zero edge, invalid interval, degenerate triangle
- topology: broken loop, bad coedge use count, open shell
- spatial: self-intersection candidate, face normal mismatch, tiny gap cluster

## 7. Shared Internal Infrastructure

这三条线共用的内部基础设施应提前固定：
- candidate pruning with `BoxTree/AABBTree`
- structured diagnostics
- consistent tolerance/context passing
- trim-boundary and preimage utilities
- body preprocess caches

这是避免 3D 重复出现 2D boolean helper 分叉的关键。

## 8. Recommended Implementation Sequence

建议先后顺序：
1. `PolyhedronBody x Plane` section
2. `PlaneSurface -> TriangleMesh`
3. `PolyhedronBody -> TriangleMesh`
4. mesh / polyhedron validation reports
5. `BrepFace x Plane` section
6. restricted `BrepBody -> TriangleMesh`
7. restricted `BrepBody` validation

## 9. First Public API Cut

第一阶段对外建议公开：
- section result types
- `GeometryMeshConversion`
- `GeometryValidation3d`
- tessellation options

第一阶段建议内部化：
- section graph rebuild internals
- body preprocess caches
- advanced trim-aware adaptive tessellation internals

## 10. Key Integration Decisions

这份 integration 文档最终固定的结论是：
- `section`、`tessellation`、`validation` 不是三条互不相关的线，而是共享同一服务底层
- `CurveOnSurface` 和 trim-boundary utilities 是 section 与 tessellation 的共同关键点
- first-phase 最应先落代码的是 plane-dominant polyhedron section + tessellation + validation 组合
