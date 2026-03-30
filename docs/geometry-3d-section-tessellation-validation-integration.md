# 3D Section、Tessellation 与 Validation 集成设计

## 1. 目的

本文定义 StableCore 3D 设计中 `section`、`tessellation`、`body validation` 三条线如何接入当前整体 3D 设计体系。

## 2. 为什么这三条线要一起规划

这三条线天然互相关联：

- `section` 依赖稳定的几何求交与拓扑表达
- `tessellation` 依赖参数对象求值与边界结构
- `validation` 需要覆盖输入体、section 结果和 tessellation 结果

如果三者分别发展，很容易出现：

- 数据结构重复
- 容差规则分叉
- 错误状态表达不统一

## 3. `section` 的定位

`section` 是几何对象之间的切分 / 截面生成能力。

第一阶段建议优先支持：

- plane 与 polyhedron 的 section
- plane 与基础参数对象的相交
- 以平面为主导的轮廓提取路径

它应建立在：

- 3D 服务层
- 2D polygon workflow 投影回收能力
- polyhedron / BRep 基础拓扑之上

## 4. `tessellation` 的定位

`tessellation` 负责把参数对象或实体对象转为离散 mesh 表达。

第一阶段建议优先支持：

- plane-dominant polyhedron -> triangle mesh
- surface -> triangle mesh 的基础路径
- validation 可消费的 mesh 输出

## 5. `validation` 的定位

`validation` 负责：

- 检查输入对象是否合法
- 识别退化边、破损 loop、错误法向、开壳等问题
- 为 healing 与后续算法提供一致的问题报告

它应覆盖：

- `TriangleMesh`
- `PolyhedronBody`
- `BrepBody`
- section 输出
- tessellation 输出

## 6. 集成原则

### 6.1 统一容差

三者都必须共享同一套 `GeometryTolerance3d` / `GeometryContext3d`。

### 6.2 统一问题报告

建议通过统一的 issue / report 类型表达：

- 错误类型
- 位置或关联对象
- 严重级别
- 是否可自动修复

### 6.3 `section` 与 2D workflow 桥接

对于 plane-dominant section，建议流程为：

1. 在 3D 中求 section primitive
2. 投影到局部 2D 平面
3. 复用现有 2D polygon / topology / rebuild workflow
4. 再映回 3D 或转为 polyhedron / mesh 消费结果

### 6.4 `tessellation` 不直接承担 healing

`tessellation` 可以暴露问题，但不应直接内嵌复杂修复逻辑。复杂修复应由 validation / healing 层负责。

## 7. 当前固定结论

当前应固定：

- `section`、`tessellation`、`validation` 必须共享容差与问题报告体系
- plane-dominant section 应优先复用现有 2D 工作流
- tessellation 与 healing 分层，不混做
