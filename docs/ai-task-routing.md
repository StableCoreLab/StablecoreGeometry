# AI 任务分派指南

## 目标

本文件用于把后续任务按复杂度、风险和执行边界分配给合适的模型。

默认前提：

- 所有任务先满足 `docs/ai-execution-spec.md`
- 每轮必须形成闭环能力单元
- 每轮必须同时包含 capability / edge-case / gap tests
- 不允许只写代码不写测试

## 分派原则

- 涉及算法策略、能力边界裁剪、复杂 contract 保持的任务，分配给 A 类。
- 范围清晰、边界明确、主要是沿既有策略扩展能力的任务，分配给 B 类。
- 只涉及文档同步、矩阵维护、路线整理的任务，分配给 C 类。
- 优先推进稳定发布 API 的对外入口，不鼓励依赖临时内部 helper。

## A 类

建议模型：

- `gpt-5.4`
- `gpt-5.2`
- `gpt-5.3-codex`

建议推理强度：

- `high`
- `xhigh`

适用任务：

- `BrepConversion` 的 non-planar repair 主算法推进
- `Section` 的高阶语义推进
- `Healing` 的多 shell arbitration
- `BodyBoolean` 的 touching / overlap contract
- `SearchPoly` 的 richer fake-edge explanation / ambiguous recovery

适合原因：

- 这类任务的核心在于策略和边界，不只是补实现
- 容易破坏 published API contract
- 往往需要同时平衡 API、测试和行为边界

## B 类

建议模型：

- `gpt-5.4-mini`
- `gpt-5.2-codex`
- `gpt-5.1-codex-max`

建议推理强度：

- `medium`
- `high`

适用任务：

- 已知安全子集的 capability 扩展
- `SearchPoly` 的 contract / diagnostics / consistency 补强
- 已确定方向的内部 pass / helper 拆层
- published API 风格统一、接口收口、umbrella header 整理

适合原因：

- 主要是沿既定方向推进，不是重写策略
- 需要理解代码，但通常不需要最高强度推理
- 边界清晰时适合并行推进

## C 类

建议模型：

- `gpt-5.4-mini`

建议推理强度：

- `low`
- `medium`

适用任务：

- `session-handoff` / `next-task-prompt` / `coverage` / `sync-tracker` 同步
- capability matrix、checklist、follow-up prompt 整理
- 批次收尾、提交说明、路线文档维护

适合原因：

- 主要是同步、整理、归档
- 算法风险低
- 很适合作为与主线实现并行的侧线工作

## 当前建议批次

### 批次 1：A 类主线

- `Section`
- `Healing`
- `BodyBoolean`

### 批次 2：B 类并行

- `SearchPoly` 的 contract / diagnostics / explanation 补强
- 已明确方向的内部 pass 整理
- published API 风格统一与接口收口

### 批次 3：C 类收尾

- 交接文档同步
- 覆盖矩阵与清单维护
- 提交前文档和说明整理

## 补充说明

- 只要任务开始改变算法策略，就应提升到 A 类。
- 只要任务主要是执行既定方案、写入范围明确，通常 B 类就足够。
- 只要任务只涉及文档、矩阵、交接和整理，保持在 C 类即可。
- 不要把大范围 API 重设计交给轻量模型。
