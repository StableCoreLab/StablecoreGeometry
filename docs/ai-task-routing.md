# AI 任务分派指南

## 目标

这份文档用于把 `stablecore-geometry` 的后续工作按任务复杂度、算法风险和执行边界分派给合适档位的 AI 模型。

默认前置规范：

- 所有任务先满足 [ai-execution-spec.md](ai-execution-spec.md)
- 每一轮必须形成 closed capability unit
- 每一轮必须完整完成 `P1`，并至少触达一个 `P2 / P3`
- 每一轮必须同时包含 capability / edge-case / gap tests
- 不允许只写代码不写测试，不允许把不稳定逻辑伪装成 capability

## 分派原则

- 涉及算法策略设计、能力边界裁剪、复杂 contract 保持的任务，派给 A 类
- 范围清晰、边界明确、主要是沿既有策略扩展 capability 或补测试的任务，派给 B 类
- 只涉及文档同步、矩阵维护、路线整理、提交收尾的任务，派给 C 类
- 优先推进稳定 SDK 对外入口，不鼓励产品侧依赖临时内部 helper

## A 类

推荐模型：

- `gpt-5.4`
- `gpt-5.2`
- `gpt-5.3-codex`

推荐推理强度：

- `high`
- `xhigh`

适用任务：

- `GeometryBrepConversion` 的 non-planar repair 主算法推进
- `GeometrySection` 的高阶 section 语义推进
- `GeometryHealing` 的竞争性 multi-shell arbitration
- `GeometryBodyBoolean` 的更一般 touching / overlap contract
- `GeometrySearchPoly` 的 richer fake-edge explanation / ambiguous recovery

适合原因：

- 这类任务的核心在于算法策略，而不只是补实现
- 容易破坏 public SDK contract
- 往往需要同时平衡 API、测试与行为边界

## B 类

推荐模型：

- `gpt-5.4-mini`
- `gpt-5.2-codex`
- `gpt-5.1-codex-max`

推荐推理强度：

- `medium`
- `high`

适用任务：

- 已知安全子集的 capability 扩展
- `GeometrySearchPoly` 的 contract / diagnostics / consistency 补强
- 已确定方向模块的内部 pass / helper 拆层
- SDK 风格统一、接口收口、umbrella header 整理

适合原因：

- 主要是沿既定方向推进，而不是重新发明策略
- 需要理解代码，但通常不需要最高强度的策略推演
- 在边界清晰时，适合并行推进

## C 类

推荐模型：

- `gpt-5.4-mini`

推荐推理强度：

- `low`
- `medium`

适用任务：

- `session-handoff` / `next-task-prompt` / `coverage` / `sync-tracker` 同步
- capability matrix、checklist、follow-up prompt 整理
- 批次收尾、提交说明、路线文档维护

适合原因：

- 主要是同步、整理、归纳
- 算法风险低
- 很适合作为与主线实现并行的侧线工作

## 当前建议批次

### 批次 1：A 类主线

- `GeometrySection`
- `GeometryHealing`
- `GeometryBodyBoolean`

### 批次 2：B 类并行

- `GeometrySearchPoly` 的 contract / diagnostics / explanation 补强
- 已明确方向模块的内部 pass 整理
- SDK 风格统一与接口收口

### 批次 3：C 类收尾

- 交接文档同步
- 覆盖矩阵与清单维护
- 提交前的文档与说明整理

## 补充说明

- 只要任务开始改变算法策略，就应上调到 A 类
- 如果任务主要是执行既定方案、写入范围明确，通常 B 类就足够
- 如果任务只涉及文档、矩阵、交接和整理，保持在 C 类即可
- 不要把大范围 API 重设计交给轻量模型
