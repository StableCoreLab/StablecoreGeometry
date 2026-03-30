# 3D Body Validation 与 Healing 设计

## 1. 目的

本文定义 StableCore 三维库中面向 `PolyhedronBody`、`BrepBody`、`TriangleMesh` 的 validation 与 healing 层。

## 2. validation 的职责

validation 负责：

- 检查对象结构是否合法
- 识别退化输入
- 输出统一问题报告
- 为 healing 与上层算法提供可消费状态

## 3. healing 的职责

healing 负责：

- 受控、保守地修复常见结构问题
- 不改变核心语义前提下提高输入可处理性
- 为后续 section、tessellation、boolean 打底

## 4. 当前覆盖对象

validation / healing 应逐步覆盖：

- `TriangleMesh`
- `PolyhedronBody`
- `BrepBody`

## 5. 统一问题报告

建议统一 issue / report 结构，至少表达：

- 问题类别
- 严重级别
- 关联对象
- 是否可自动修复
- 修复后状态

## 6. 当前固定结论

- validation 与 healing 是独立层，不混入对象本体
- healing 应保守，不应提前做激进几何重构
- 后续 section / tessellation / boolean 都应消费统一 validation 结果
