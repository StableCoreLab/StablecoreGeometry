# 3D 模块与包布局

## 1. 目的

本文把当前 3D 设计映射到未来 StableCore C++ 代码库的模块、头文件和源文件布局。

目标是提前约束：

- 模块边界
- 头文件组织方式
- 内外部 API 的拆分
- 后续扩展时避免层间污染

## 2. 顶层原则

### 2.1 对外按能力分包，对内按分层组织

对外应尽量让调用方看到稳定能力入口。

对内应继续按分层拆开：

- 值类型
- 参数对象
- 服务层
- 拓扑层
- 网格层
- 校验 / 恢复层

### 2.2 公开头文件保持扁平可读

公开头文件不应暴露过深目录层次。

建议通过 SDK 聚合头文件提供稳定入口，同时保留分模块头文件供精细引用。

### 2.3 内部实现与公开 ABI 解耦

以下内容应尽量留在内部实现层：

- preprocess helpers
- topology internals
- section graph internals
- tessellation internals
- healing internals

## 3. 建议目录布局

```text
include/
  sdk/
  types/
  internal/

src/
  sdk/
  internal/

tests/
docs/
```

其中：

- `include/sdk/`：公开服务与聚合 API
- `include/types/`：公开值类型与基础对象类型
- `include/internal/`：仅内部实现依赖的辅助声明
- `src/sdk/`：公开服务对应实现
- `src/internal/`：内部共享 helper 与复杂算法实现

## 4. 建议模块拆分

### 4.1 值类型模块

建议放入：

- `Point3.h`
- `Vector3.h`
- `Direction3.h`
- `Box3.h`
- `Interval.h`
- `Matrix3.h`
- `Transform3.h`
- `Plane.h`
- `Line3.h`
- `Ray3.h`
- `LineSegment3.h`
- `Triangle3.h`

### 4.2 参数对象模块

建议放入：

- `Curve3d.h`
- `LineCurve3d.h`
- `Surface.h`
- `PlaneSurface.h`
- 后续再扩到 `NurbsCurve3d.h`、`NurbsSurface.h`

### 4.3 服务模块

建议拆成：

- `GeometryProjection.h`
- `GeometryIntersection.h`
- `GeometryRelation.h`
- `GeometryMeasure.h`
- `GeometrySection.h`
- `GeometryValidation.h`
- `GeometryHealing.h`

### 4.4 拓扑与网格模块

建议分开：

- `PolyhedronBody.h`
- `TriangleMesh.h`
- `BrepBody.h`
- 以及各自独立的 support 类型头文件

不要把 polyhedron、mesh、BRep 混在同一个大头文件中。

## 5. 聚合 API 组织

建议通过以下方式对外聚合：

- `GeometryTypes.h` 聚合基础类型
- `GeometryApi.h` 聚合常用公开服务
- 复杂对象如 `TriangleMesh`、`PolyhedronBody`、`BrepBody` 保持独立显式包含

## 6. 内部模块建议

建议尽早规划以下内部模块：

- `Geometry3dPredicateInternal.*`
- `Geometry3dIntersectionInternal.*`
- `GeometrySectionInternal.*`
- `GeometryTessellationInternal.*`
- `GeometryValidationInternal.*`
- `GeometryHealingInternal.*`
- `GeometryBrepTopologyInternal.*`
- `GeometryMeshConversionInternal.*`

这样后续多个算法模块可以共享同一套内部基础设施。

## 7. 当前固定结论

当前 3D 模块布局应继续坚持：

- `types` 与 `sdk` 分开
- mesh、polyhedron、BRep 分开
- 内部 helper 不直接暴露进 SDK 头文件
- 聚合入口存在，但不替代模块化包含
