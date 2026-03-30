# 3D 几何库设计

## 1. 目的

本文用于定义 StableCore 后续三维算法库的目标边界、类型层次、模块划分和演进顺序。

这份设计不是把当前 2D SDK 机械扩展到 3D，而是结合两类参考经验：

- Delphi 侧：`Geo3DLib` 已提供 3D 类型、曲面、多面体、BoxTree、Lofting 等工程模块，延续了强容差和工程恢复风格
- GGP 侧：已形成较完整的 3D 对象层次，包含 `Curve3d`、`Surface`、`Plane`、`NurbsCurve3d`、`NurbsSurface`、`BrepBody`、`PolyhedronBody`、`TriangleMesh`、`Topology` 等模块

设计目标：

- 先建立稳定的 3D 工程几何内核
- 再逐步补齐曲线、曲面、实体、网格和拓扑处理
- 保持 API 可维护，而不是一开始追求大而全

## 2. 参考基线

本设计主要参考：

- 当前仓库 2D SDK 的类型评审与模块边界结论
- Delphi `Geo3DLib` 的 3D 类型、曲面、多面体、BoxTree、Lofting 等工程实现
- GGP 的 3D 模块轮廓与对象分层

代表性参考文件：

- 当前仓库：
  - `docs/sdk-type-design-review.md`
  - `docs/delphi-geometry-parity.md`
- Delphi 3D：
  - `D:\code\GFY2.0\Controls\GAEAResource\GCL\Geo3DLib\Source\GGLGeo3DTypes.pas`
  - `D:\code\GFY2.0\Controls\GAEAResource\GCL\Geo3DLib\Source\GGLSurface3d.pas`
  - `D:\code\GFY2.0\Controls\GAEAResource\GCL\Geo3DLib\Source\GGLPolyhedron3d.pas`
  - `D:\code\GFY2.0\Controls\GAEAResource\GCL\Geo3DLib\Source\GGLBoxTree3d.pas`
  - `D:\code\GFY2.0\Controls\GAEAResource\GCL\Geo3DLib\Source\GGLLofting.pas`
- GGP：
  - `D:\code\GGP_ProdN\Source\Geometry\Curve3d.cpp`
  - `D:\code\GGP_ProdN\Source\Geometry\Surface.cpp`
  - `D:\code\GGP_ProdN\Source\Geometry\BrepBody.cpp`
  - 同目录下的 `Plane.cpp`、`Line3d.cpp`、`NurbsCurve3d.cpp`、`NurbsSurface.cpp`、`PolyhedronBody.cpp`、`TriangleMesh.cpp`、`Topology.cpp`

结论：

- Delphi 对 3D 的参考价值主要体现在工程对象、恢复、投影与切分路径
- GGP 对 3D 抽象层次、参数化接口、BRep ownership 关系更有直接参考意义
- StableCore 应综合两者：对象分层参考 GGP，工程恢复与 2D/3D 桥接策略参考 Delphi

## 3. 核心目标

三维库的核心目标应是：

- 提供稳定的 3D 值类型与参数化对象抽象
- 提供统一的 3D predicate / projection / intersection / measurement 基础设施
- 支持 curve-surface-body 的分层算法，而不是把所有能力塞进单一大对象
- 为后续 BRep、mesh、offset、section、projection、boolean、topology 预留清晰内部层
- 尽量复用 2D 库已验证过的工程设计经验，避免重复踩命名、职责边界与内部 helper 分叉的问题

非目标：

- 第一阶段不追求完整 CAD 内核
- 第一阶段不追求所有解析曲面、自由曲线、实体 boolean 一次到位
- 第一阶段不把 mesh 与 BRep 混成一个统一数据结构

## 4. 主要设计原则

### 4.1 对外风格与 2D SDK 保持一致

公开命名延续当前 SDK 的短名风格：

- `PointAt`
- `Bounds`
- `Count`
- `Data`
- `Length`
- `Area`
- `Volume`

不再扩散新的 `GetXxx` 风格公开 API。

### 4.2 先类型，后算法，再拓扑

三维库应继续保持：

- 类型负责表达几何语义与少量天然属性
- 算法由自由函数承担
- 拓扑与索引由专门服务类承担

不要一开始就把复杂算法塞进成员函数，否则会重复 2D 中已暴露出的职责重叠问题。

### 4.3 几何、拓扑、恢复分离

3D 中更要明确区分三层：

- geometry：点、向量、曲线、曲面、体的数学 / 参数化表示
- topology：vertex / edge / coedge / loop / face / shell / body 的连接关系
- recovery / preprocess：缝合、容差合并、退化边清理、自交修复、候选重建

不要把恢复逻辑直接塞进 BRep 主体类型中。

### 4.4 容差必须是基础设施

3D 比 2D 更依赖统一容差规则。

必须避免：

- 在不同模块散落不同的 `1e-6` / `1e-9`
- curve / surface / body 各自定义一套不兼容的容差语义

建议统一三类容差：

- distance epsilon
- angle epsilon
- parameter epsilon

并通过上下文对象向算法传播，而不是让每个算法自己猜。

### 4.5 共享内部层要尽早规划

2D 已暴露出一个问题：boolean / path / offset 的内部图处理 helper 会逐步分叉。

3D 应提前规划共享内部层，至少包括：

- `Geometry3dPredicateInternal.*`
- `Geometry3dIntersectionInternal.*`
- `GeometryBrepPreprocessInternal.*`
- `GeometryBrepTopologyInternal.*`
- `GeometryMeshConversionInternal.*`

这样后续 section / boolean / sewing / shell-build 不会各写一套局部版本。

## 5. 推荐分层

建议三维库采用五层结构。

### 5.1 值类型层

负责轻量、透明、可复制的值类型：

- `Point3d`
- `Vector3d`
- `Direction3d`
- `Box3d`
- `Intervald`
- `Transform3d`
- `Plane`
- `Line3d`
- `Ray3d`
- `LineSegment3d`
- `Triangle3d`

这一层优先做成透明值类型，不建议使用 PImpl。

### 5.2 参数几何层

负责参数化对象抽象：

- `Curve3d`
- `LineCurve3d`
- `Arc3d`
- `Ellipse3d`
- `NurbsCurve3d`
- `PlaneSurface`
- `RuledSurface`
- `OffsetSurface`
- `NurbsSurface`
- `Surface`

需要明确：

- 曲线是 1D 参数对象
- 曲面是 2D 参数对象
- 必须支持 `PointAt`、导数、法向、参数区间、bounds / cone / tilted bounds 等基础能力

### 5.3 拓扑层

负责实体与边界表示：

- `BrepVertex`
- `BrepEdge`
- `BrepCoedge`
- `BrepLoop`
- `BrepFace`
- `BrepShell`
- `BrepBody`

这一层主要负责：

- 拓扑连接关系
- 参数曲线、三维曲线、支撑曲面的关联
- 方向一致性
- 有效性约束

### 5.4 网格 / 多面体层

负责离散表达：

- `TriangleMesh`
- `PolyhedronBody`
- `MeshTopology`
- `MeshAABBTree`

必须与 BRep 分开。原因：

- 精确几何与离散几何的修改策略不同
- boolean、缝合、投影、测量对两者依赖的数据不同
- 强行统一只会让接口变脏

### 5.5 算法 / 服务层

负责三维算法与服务对象：

- predicate / relation
- measure / projection / distance
- intersection
- section / slice
- body validation
- sewing / healing
- body boolean
- body offset / shell offset
- tessellation / mesh conversion
- topology indexing / search

## 6. 对外类型系统建议

### 6.1 第一版稳定公开类型

建议第一阶段优先公开：

- `Point3d`
- `Vector3d`
- `Box3d`
- `Transform3d`
- `Plane`
- `Line3d`
- `Ray3d`
- `LineSegment3d`
- `Triangle3d`
- `Curve3d`
- `Surface`
- `TriangleMesh`

原因：

- 这些类型最容易形成稳定语义
- 足以支撑大量基础算法
- 还不必立即承诺完整 BRep ABI

### 6.2 延后稳定的类型

这些类型建议先内部稳定，再考虑公开：

- `BrepBody`
- `BrepFace`
- `BrepEdge`
- `PolyhedronBody`
- `NurbsSurface`
- `NurbsCurve3d`
- `OffsetSurface`

原因：

- 内部状态复杂
- 与拓扑和容差强耦合
- 后续 ABI / 生命周期策略仍可能调整

## 7. 当前固定结论

当前 3D 设计应固定以下结论：

- 公开 API 命名继续受 2D 规则约束
- `Plane` 与 `PlaneSurface` 必须分离
- `Curve3d` / `Surface` 是核心参数协议
- `CurveOnSurface` 必须在 BRep 算法前规划
- `PolyhedronBody` 与 `BrepBody` 不应过早合并
- `BrepCoedge` 是必需项，不是可选项
