# TriangleMesh 与 Mesh Conversion 设计

## 1. 目的

本文定义 StableCore 三维库中的 `TriangleMesh` 层，以及 mesh 与 polyhedron / BRep 之间的转换服务。

## 2. 设计目标

这一层的目标是：

- 提供稳定的三角网格表达
- 为显示、分析、导出、验证提供统一离散表示
- 与 polyhedron / BRep 明确分层
- 为后续 tessellation 与 mesh validation 提供底座

## 3. `TriangleMesh` 的定位

`TriangleMesh` 是离散几何表达，不是精确几何对象。

它适合承担：

- 顶点与三角面存储
- 基础 bounds / transform
- 网格级校验
- 导出与下游显示消费

它不适合承担：

- 精确拓扑编辑
- 参数曲面表达
- BRep ownership 语义

## 4. 与 `PolyhedronBody` / `BrepBody` 的关系

三者必须分开：

- `TriangleMesh`：离散三角片表达
- `PolyhedronBody`：平面主导的工程体表达
- `BrepBody`：精确边界表示

转换关系应是显式服务，不应在类型内隐式混合。

## 5. 转换服务建议

建议提供独立 mesh conversion 服务，负责：

- `PolyhedronBody -> TriangleMesh`
- `Surface -> TriangleMesh`
- 受限 `BrepBody -> TriangleMesh`

后续若需要反向路径，也应显式设计，不要提前承诺。

## 6. `TriangleMesh` 最小能力

第一阶段建议支持：

- 顶点数组访问
- 三角面数组访问
- `Bounds()`
- `Transform(...)`
- 基础有效性检查
- 法向 / 面积等基本派生信息

## 7. 当前固定结论

当前 mesh 层应固定：

- 作为离散层独立存在
- 不与 BRep 混同
- 转换通过独立服务完成
- validation 与 tessellation 都可复用该层
