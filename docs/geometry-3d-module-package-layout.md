# 3D Module and Package Layout

## 1. Purpose

本文把当前 3D 设计映射到未来 StableCore C++ 代码库的模块、头文件和源文件布局。

目标是：
- 提前确定 3D 代码不应如何散落
- 保持公开 API、内部 helper、重算法 preprocess 的边界清楚
- 为后续真正落代码提供稳定目录结构

## 2. Design Principles

模块布局应满足：
- 公开类型与内部实现分离
- 值类型、拓扑类型、服务型算法分层
- conversion / validation / healing / preprocess 不要混在一个超大文件中
- 2D 和 3D 的命名风格保持一致，但实现目录允许分层更细

## 3. Top-Level Layout Proposal

建议在现有仓库中增量增加这些区块：
- `include/types3d/`
- `include/sdk3d/`
- `src/types3d/`
- `src/sdk3d/`
- `src/internal3d/`
- `tests/3d-capabilities/`
- `tests/3d-gaps/`

如果团队后续更希望沿用现有 `include/types` / `include/sdk`，也可以做子目录形式：
- `include/types/3d/`
- `include/sdk/3d/`
- `src/types/3d/`
- `src/sdk/3d/`
- `src/internal/3d/`

从隔离性看，我更推荐显式 `types3d / sdk3d / internal3d`。

## 4. Public Type Modules

### 4.1 `include/types3d/`

建议放：
- `Point3.h`
- `Vector3.h`
- `Direction3d.h`
- `Box3.h`
- `Intervald.h`
- `Matrix3d.h`
- `Transform3d.h`
- `Plane.h`
- `Line3d.h`
- `Ray3d.h`
- `LineSegment3d.h`
- `Triangle3d.h`

特点：
- 纯值类型
- 头文件为主
- 少量内联实现可接受

### 4.2 `src/types3d/`

只放少量需要 out-of-line 的实现：
- heavy string/debug helpers
- non-trivial matrix / transform methods

## 5. Parametric Geometry Modules

### 5.1 `include/sdk3d/curve_surface/`

建议放：
- `Curve3d.h`
- `CurveEval3d.h`
- `CurveProjectionResult.h`
- `LineCurve3d.h`
- `ArcCurve3d.h`
- `NurbsCurve3d.h`
- `Surface.h`
- `SurfaceEval3d.h`
- `SurfaceProjectionResult.h`
- `PlaneSurface.h`
- `RuledSurface.h`
- `NurbsSurface.h`
- `CurveOnSurface.h`

### 5.2 `src/sdk3d/curve_surface/`

一类文件一个实现：
- `Curve3d.cpp`
- `Surface.cpp`
- `LineCurve3d.cpp`
- `PlaneSurface.cpp`

避免再次出现“所有曲面 helper 堆在一个超大 `GeometrySurface.cpp`”的情况。

## 6. Topology Modules

### 6.1 `include/sdk3d/topology/`

建议放：
- `PolyhedronBody.h`
- `PolyhedronFace.h`
- `PolyhedronLoop.h`
- `BrepVertex.h`
- `BrepEdge.h`
- `BrepCoedge.h`
- `BrepLoop.h`
- `BrepFace.h`
- `BrepShell.h`
- `BrepBody.h`
- `GeometryEntityRef.h`

### 6.2 `src/sdk3d/topology/`

建议分文件实现，不做巨型拓扑翻译单元。

## 7. Mesh Modules

### 7.1 `include/sdk3d/mesh/`

建议放：
- `TriangleMesh.h`
- `MeshVertex.h`
- `MeshTriangle.h`
- `MeshTopologyIndex.h`
- `TessellationOptions.h`

### 7.2 `src/sdk3d/mesh/`

建议放：
- `TriangleMesh.cpp`
- `MeshTopologyIndex.cpp`
- `GeometryMeshConversion.cpp`
- `GeometryMeshValidation.cpp`

## 8. Service Modules

### 8.1 `include/sdk3d/services/`

建议放：
- `GeometryTolerance3d.h`
- `GeometryContext3d.h`
- `GeometryPredicate3d.h`
- `GeometryRelation3d.h`
- `GeometryProjection3d.h`
- `GeometryMeasure3d.h`
- `GeometryIntersection3d.h`
- `GeometrySearch3d.h`
- `GeometryValidation3d.h`
- `GeometryHealing3d.h`

### 8.2 `src/sdk3d/services/`

建议放：
- `GeometryPredicate3d.cpp`
- `GeometryProjection3d.cpp`
- `GeometryMeasure3d.cpp`
- `GeometryIntersection3d.cpp`
- `GeometrySearch3d.cpp`
- `GeometryValidation3d.cpp`
- `GeometryHealing3d.cpp`

## 9. Internal Modules

### 9.1 `src/internal3d/`

这里存放不应直接暴露的共享内部层：
- `GeometryPredicate3dInternal.*`
- `GeometryProjection3dInternal.*`
- `GeometryIntersection3dInternal.*`
- `GeometrySearch3dInternal.*`
- `GeometryBrepPreprocessInternal.*`
- `GeometryMeshConversionInternal.*`
- `GeometrySectionInternal.*`
- `GeometryTessellationInternal.*`
- `GeometryValidationInternal3d.*`

### 9.2 Boundary Rule

规则应是：
- `include/sdk3d` 绝不包含 internal-only types
- `src/sdk3d` 可以依赖 `src/internal3d` 的私有头
- tests 可以通过公开接口覆盖 internal 行为，不直接 include internal 头

## 10. Builder and Result Modules

建议额外有两个小分组：
- `include/sdk3d/builders/`
- `include/sdk3d/results/`

其中：
- builders 放 `PolyhedronBuilder` / `BrepBuilder`
- results 放 projection/intersection/validation/healing/section 结果对象

这样返回结构不会分散在各个算法头里。

## 11. Test Layout

建议测试同步按 2D 的 `capabilities / gaps` 方式铺开：
- `tests/3d-capabilities/test_types3d.cpp`
- `tests/3d-capabilities/test_curve_surface.cpp`
- `tests/3d-capabilities/test_polyhedron.cpp`
- `tests/3d-capabilities/test_trianglemesh.cpp`
- `tests/3d-capabilities/test_projection_intersection.cpp`
- `tests/3d-gaps/test_section_gaps.cpp`
- `tests/3d-gaps/test_tessellation_gaps.cpp`
- `tests/3d-gaps/test_brep_healing_gaps.cpp`

## 12. Phase-1 Minimal File Set

如果现在就开始落代码，第一批最小文件集建议是：
- `include/types3d/Point3.h`
- `include/types3d/Vector3.h`
- `include/types3d/Box3.h`
- `include/sdk3d/services/GeometryTolerance3d.h`
- `include/sdk3d/services/GeometryPredicate3d.h`
- `include/sdk3d/curve_surface/Curve3d.h`
- `include/sdk3d/curve_surface/Surface.h`
- `include/sdk3d/curve_surface/LineCurve3d.h`
- `include/sdk3d/curve_surface/PlaneSurface.h`
- `src/sdk3d/services/GeometryPredicate3d.cpp`
- `src/sdk3d/curve_surface/Curve3d.cpp`
- `src/sdk3d/curve_surface/Surface.cpp`

## 13. What To Avoid

明确避免：
- 一个 `Geometry3d.cpp` 塞所有实现
- 公开头直接暴露 preprocess/internal helper
- topology、services、conversion 互相循环包含
- mesh 和 BRep 共用同一个核心存储类型

## 14. Key Layout Decisions

这份布局文档最终固定的结论是：
- 3D 代码应从一开始就按 `types / topology / mesh / services / internal` 分开
- result types 和 builders 值得单独分组
- internal shared layers 必须有专门目录，不能继续散落
- 3D tests 应直接沿用 `capabilities / gaps` 的组织经验
