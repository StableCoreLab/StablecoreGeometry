# AI 任务分派指南

## 目标

这份文档用于按任务所需的 AI 模型能力与推理强度，对剩余工作进行分类，方便把任务分配给不同档位的模型，而不必每次重新评估范围。

## 分派原则

- 涉及算法策略设计的任务，使用前沿模型和高推理强度。
- 范围清晰、边界明确的实现与测试扩张任务，使用中档编码模型。
- 文档、矩阵、交接同步类任务，使用轻量模型。
- 优先推进稳定 SDK 对外接口，不鼓励产品侧依赖临时内部入口。

## 任务分类

### A 类：前沿模型 + 高推理

推荐模型：

- `gpt-5.4`
- `gpt-5.2`
- `gpt-5.3-codex`

推荐推理强度：

- `high`
- `xhigh`

适合任务：

- `GeometryBrepConversion`
  - 继续推进 non-planar repair 主算法
  - 继续拆分并深化：
    - support-plane scoring
    - representative target aggregation
    - cross-face snapping
    - topology reconciliation
- `GeometryBodyBoolean`
  - 从当前 identical/disjoint deterministic 子集继续推进
  - 补 richer overlap 的 `intersect / union / difference`
- `GeometrySearchPoly`
  - 向 Delphi 级 smart-search 深化
  - branch scoring
  - fake-edge explanation
  - ambiguous-result recovery
- `GeometryHealing`
  - 更一般的 aggressive shell policy
- `GeometrySection`
  - non-planar dominant contour stitching
  - 更高阶 coplanar fragment merge 语义

适合原因：

- 这类任务的关键在于算法策略，而不只是改代码量
- 错误实现容易破坏 SDK contract
- 往往需要同时平衡 API、测试和算法行为

### B 类：中档模型 + 中高推理

推荐模型：

- `gpt-5.4-mini`
- `gpt-5.2-codex`
- `gpt-5.1-codex-max`

推荐推理强度：

- `medium`
- `high`

适合任务：

- 在现有 `GeometryBodyBoolean` 上继续补“已知安全”的代表性 capability 子集
- 在现有 `GeometrySearchPoly` 上补 contract tests、diagnostics tests、result 一致性测试
- 在高层方向已明确后，对内部 helper 和大文件做拆层整理
- 将大实现文件拆成更清晰的内部 pass
- 统一 SDK 的 `Options / Result / Issue` 风格
- 收口 `include/sdk` 暴露面与 `Geometry.h` umbrella header
- 补 representative capability tests 与 deterministic 断言
- 将小范围、已看清的 gap 子集转正为 capability

适合原因：

- 这类任务更偏实现，不要求重新发明算法策略
- 需要理解代码，但不一定需要最强的策略推理
- 在明确文件边界后，很适合并行推进

### C 类：轻量模型 + 低中推理

推荐模型：

- `gpt-5.4-mini`

推荐推理强度：

- `low`
- `medium`

适合任务：

- `docs/session-handoff.md`
- `docs/next-task-prompt.md`
- `docs/test-capability-coverage.md`
- `docs/design-doc-sync-tracker.md`
- `docs/delphi-interface-fasttrack.md`
- `docs/delphi-test-fasttrack-matrix.md`
- `docs/rename-followup-todo.md`
- commit message 建议
- capability/gap inventory 清理
- 矩阵和 checklist 维护
- 为其他 AI 准备 follow-up prompt

适合原因：

- 这类工作主要是同步、整理、归纳
- 算法风险低
- 适合作为与核心算法并行推进的侧线工作

## 当前建议分派

### 高优先级 A 类

- `GeometryBodyBoolean`
  - 继续推进 richer overlap 子集
- `GeometryBrepConversion`
  - 继续推进 non-planar repair pass 化

### 高优先级 B 类

- `GeometrySearchPoly`
  - 在稳定 SDK 面上继续做 branch scoring 和 fake-edge explanation 原型
- `GeometryHealing`
  - 拆 conservative 与 aggressive 内部路径
- `GeometrySection`
  - 将 contour extraction、deterministic segment normalization、coplanar merge 拆成更清晰的阶段

### 持续进行的 C 类

- 每完成一个代码批次后，持续同步 handoff、next-task、coverage、fast-track matrix 等文档

## 可直接分派的工作项

### 工作项 1

- 领域：`GeometryBrepConversion`
- 分类：`A`
- 最适合：前沿编码模型
- 推理强度：`xhigh`
- 交付物：
  - 推进 non-planar repair
  - 保持 SDK contract 稳定
  - 补 representative capability / gap 更新

### 工作项 2

- 领域：`GeometryBodyBoolean`
- 分类：`A`
- 最适合：前沿编码模型
- 推理强度：`high`
- 交付物：
  - 增加第一批 overlap 子集
  - 保持 invalid/unsupported contract 稳定
  - 更新 capability/gap tests

### 工作项 3

- 领域：`GeometrySearchPoly`
- 分类：`A` 或 `B`
- 最适合：
  - 如果涉及 branch scoring 策略变化，优先用强模型
  - 如果只是按既定方向补实现，可用中档模型
- 推理强度：`high`
- 交付物：
  - 在稳定 SDK 面上继续深化 smart-search 行为

### 工作项 4

- 领域：`GeometryHealing`
- 分类：`B`
- 最适合：中档编码模型
- 推理强度：`medium/high`
- 交付物：
  - conservative / aggressive 内部路径拆层
  - 代表性测试补强

### 工作项 5

- 领域：`GeometrySection`
- 分类：`B`
- 最适合：中档编码模型
- 推理强度：`medium/high`
- 交付物：
  - 内部阶段拆分
  - 代表性测试补强

### 工作项 6

- 领域：文档与矩阵同步
- 分类：`C`
- 最适合：轻量模型
- 推理强度：`low/medium`
- 交付物：
  - 每轮代码批次后的文档同步

## 分派补充说明

- 只要任务开始改变算法策略，就应上调到 A 类。
- 如果任务主要是执行既定方案，并且写入范围明确，通常 B 类就足够。
- 如果任务只涉及文档、矩阵、交接和整理，保持在 C 类即可。
- 不要把大范围 API 重设计交给轻量模型。
