# 3D Body Validation and Healing Design

## 1. Purpose

本文定义 StableCore 三维库中面向 `PolyhedronBody` / `BrepBody` / `TriangleMesh` 的 validation 与 healing 层。

目标是：
- 把“检查问题”和“修复问题”从核心几何/拓扑类型中分离出来
- 明确 validation、healing、boolean、conversion 的边界
- 为后续 body boolean、sewing、offset、import cleanup 提供统一恢复基础设施

## 2. Why This Layer Must Be Separate

如果没有独立 validation / healing 层，后续会出现两个问题：
- 核心类型里堆满杂项风险判定、修补 helper、局部 epsilon 特判
- boolean / section / import / conversion 各自复制一套修复流程

2D boolean 现在已经说明：真正耗时的不是基础功能，而是退化恢复链。3D 只会更明显。

## 3. Scope

本层覆盖：
- `PolyhedronBody` validity checks
- `BrepBody` topology / geometry consistency checks
- `TriangleMesh` quality / manifold checks
- healing / preprocess / repair pipeline
- diagnostics and structured issue reports

本层不直接覆盖：
- 最终 boolean 结果分类
- tessellation 实现本身
- import/export 格式解析

## 4. Layer Split

建议拆成四层：
- `GeometryValidation3d`
- `GeometryHealing3d`
- `GeometryBrepPreprocessInternal`
- `GeometryDiagnostics3d`

### 4.1 `GeometryValidation3d`

职责：
- 做只读检查
- 返回结构化 issue 列表
- 不修改对象

### 4.2 `GeometryHealing3d`

职责：
- 在显式策略和容差下执行修复
- 返回修复结果和诊断日志
- 尽量保持可追踪的改动摘要

### 4.3 `GeometryBrepPreprocessInternal`

职责：
- 为 boolean / section / offset 等重算法准备内部预处理
- 这层可比公开 healing 更激进，但不直接对外暴露

### 4.4 `GeometryDiagnostics3d`

职责：
- 统一 issue code、severity、location、evidence
- 支持验证报告、修复报告、调试追踪

## 5. Common Report Types

建议先统一结果结构。

### 5.1 `GeometryIssue3d`

建议字段：
- `GeometryIssueCode code`
- `GeometryIssueSeverity severity`
- `std::string message`
- `GeometryEntityRef entity`
- 可选 `Point3d location`
- 可选 `double measuredValue`

### 5.2 `GeometryValidationReport3d`

建议字段：
- `bool isValid`
- `std::vector<GeometryIssue3d> issues`
- 统计摘要

### 5.3 `GeometryHealingAction3d`

建议字段：
- `GeometryHealingActionCode code`
- `std::string message`
- `std::vector<GeometryEntityRef> affectedEntities`

### 5.4 `GeometryHealingResult3d`

建议字段：
- `bool changed`
- `bool success`
- `GeometryValidationReport3d before`
- `GeometryValidationReport3d after`
- `std::vector<GeometryHealingAction3d> actions`

## 6. Validation Targets

### 6.1 Value / Primitive-Level Checks

优先支持：
- invalid point / vector values
- zero-length segment
- zero-area triangle
- invalid plane / frame definition
- invalid parameter interval

### 6.2 Mesh-Level Checks

优先支持：
- invalid triangle indices
- degenerate triangles
- duplicate triangles
- inconsistent orientation clusters
- open boundaries
- non-manifold edges
- isolated vertices

### 6.3 Polyhedron-Level Checks

优先支持：
- face plane mismatch
- loop not closed
- self-overlapping projected loops
- inconsistent face direction
- dangling edges
- shell not closed

### 6.4 BRep-Level Checks

优先支持：
- edge curve and vertex mismatch
- coedge loop not closed in parameter space
- missing or invalid preimage curve
- face surface / loop inconsistency
- non-manifold edge use count
- shell orientation mismatch
- body self-intersection candidates

## 7. Healing Targets

### 7.1 Safe First-Phase Healing

建议第一阶段只做相对保守的修复：
- merge near-duplicate vertices
- remove zero-length edges
- remove degenerate triangles
- normalize loop orientation
- rebuild cached bounds / adjacency
- weld mesh vertices under explicit tolerance

### 7.2 Second-Phase Healing

后续再考虑：
- repair tiny gap chains
- bridge small cracks
- rebuild missing coedges from face trims
- split self-intersecting loops
- surface-aware face trimming repair

### 7.3 Avoid Magical Auto-Repair

不建议第一阶段做隐式“大修复”。

规则应是：
- validation 默认只报问题
- healing 必须显式调用
- healing 需要 strategy/options

## 8. Healing Strategy Objects

建议增加：
- `GeometryHealingOptions3d`
- `GeometryMeshHealingOptions`
- `GeometryBrepHealingOptions`

例如：
- `mergeVertexTolerance`
- `removeTinyEdgeThreshold`
- `removeTinyFaceThreshold`
- `weldMeshVertices`
- `repairLoopOrientation`
- `allowTopologyRewrite`

## 9. BRep Preprocess for Heavy Algorithms

对于 boolean / section / offset，不应只依赖公开 healing。

应单独规划内部 preprocess：
- normalize tolerances
- snap near-coincident vertices
- collapse tiny edges
- rebuild coedge adjacency cache
- detect face-face candidate pairs
- generate robust split-ready representations

这是 3D 对应于当前 2D boolean preprocess 的核心层。

## 10. Validation vs Healing vs Boolean

边界必须清楚：
- validation: 发现问题
- healing: 尝试修复问题
- boolean: 在可接受输入上构建结果

如果 boolean 内部必须做局部预处理，也应归入 preprocess/internal，而不是把 boolean 本体变成修复总控。

## 11. Entity References

为了让报告可用，建议统一实体引用：
- `GeometryEntityKind`
- `GeometryEntityId`
- `GeometryEntityRef`

可指向：
- vertex
- edge
- coedge
- loop
- face
- shell
- body
- triangle

## 12. Diagnostics and Logging

建议 diagnostics 支持三个粒度：
- summary
- issue list
- verbose trace

这样：
- 用户层可以只看摘要
- 内部算法可以保留详细修复轨迹

## 13. Recommended First Public Surface Area

第一阶段建议公开：
- `GeometryValidationReport3d`
- `GeometryIssue3d`
- `GeometryValidation3d`
- `GeometryHealingOptions3d`
- `GeometryHealingResult3d`
- `GeometryHealing3d`

第一阶段建议内部化：
- aggressive preprocess
- boolean-specific recovery helpers
- auto-generated repair pipelines

## 14. Recommended Implementation Order

建议顺序：
1. issue/report/result 类型
2. mesh validation
3. polyhedron validation
4. BRep topological validation
5. conservative healing actions
6. boolean/section preprocess internals
7. richer diagnostics and traces

## 15. Key Design Decisions

这层最终要固定的结论是：
- validation 和 healing 必须分离
- diagnostics 必须结构化，而不是只返回 `bool`
- first-phase healing 以保守、可解释、显式调用为主
- heavy algorithm preprocess 是独立内部层，不应与公开 healing 混在一起
