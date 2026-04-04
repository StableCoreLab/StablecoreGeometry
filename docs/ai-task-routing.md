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

可直接派发的提示词：

#### 提示词 A1：`GeometryBrepConversion`

```text
请在 `D:\code\stablecore\stablecore-geometry` 中继续推进 `GeometryBrepConversion` 的 non-planar repair 主算法。

目标：
1. 不改对外 SDK 入口；
2. 将当前 non-planar repair 内部流程进一步拆为更清晰的 pass：
   - support-plane scoring
   - representative target aggregation
   - cross-face snapping
   - topology reconciliation
3. 在现有 capability/gap 体系下，新增或转正最小代表性 capability；
4. 同步更新文档并提交。

要求：
- 只写代码、测试代码、文档
- 不要编译
- 不要跑构建
- 不要回退已有改动
- 每完成一个阶段，更新：
  - `docs/session-handoff.md`
  - `docs/next-task-prompt.md`
  - `docs/test-capability-coverage.md`
  - `docs/design-doc-sync-tracker.md`
  - 必要时更新 gap 文档
- 完成后直接提交

重点文件：
- `src/sdk/GeometryBrepConversion.cpp`
- `tests/capabilities/test_3d_conversion.cpp`
- `tests/gaps/test_3d_conversion_gaps.cpp`
```

#### 提示词 A2：`GeometryBodyBoolean`

```text
请在 `D:\code\stablecore\stablecore-geometry` 中继续推进 `GeometryBodyBoolean`。

当前状态：
- 已支持 deterministic identical/disjoint closed-body 子集
- richer overlap / shell-policy / topology-preserving healing integration 仍是 gap

目标：
1. 在不破坏现有 public contract 的前提下，补第一批 overlap 子集；
2. 保持 `InvalidInput` / `UnsupportedOperation` contract 稳定；
3. 新增 capability tests；
4. 未覆盖部分继续保留为 gap；
5. 同步文档并提交。

要求：
- 只写代码、测试代码、文档
- 不要编译
- 不要跑构建
- 保持 `include/sdk/GeometryBodyBoolean.h` 的 public contract 稳定
- 完成后更新：
  - `docs/session-handoff.md`
  - `docs/next-task-prompt.md`
  - `docs/test-capability-coverage.md`
  - `docs/delphi-interface-fasttrack.md`
  - `docs/delphi-test-fasttrack-matrix.md`
  - `docs/design-doc-sync-tracker.md`
- 完成后直接提交

重点文件：
- `src/sdk/GeometryBodyBoolean.cpp`
- `tests/capabilities/test_3d_body_boolean_sdk.cpp`
- `tests/gaps/test_3d_body_boolean_gaps.cpp`
```

#### 提示词 A3：`GeometrySearchPoly`

```text
请在 `D:\code\stablecore\stablecore-geometry` 中继续深化 `GeometrySearchPoly`。

当前状态：
- 已有稳定 SDK 入口
- 已补 diagnostics、candidate ranking、smallest-containing candidate
- Delphi 级 branch scoring、fake-edge explanation、ambiguous recovery 仍为 gap

目标：
1. 在 `GeometrySearchPoly.h` 的稳定 SDK 面上继续加深实现；
2. 增加 branch scoring；
3. 增加 fake-edge explanation 或等价 diagnostics；
4. 保持产品侧不再直接依赖 `BuildMultiPolygonByLines(...)`；
5. 更新 capability/gap tests 和文档并提交。

要求：
- 只写代码、测试代码、文档
- 不要编译
- 不要跑构建
- 对外稳定入口保持在 `include/sdk/GeometrySearchPoly.h`
- 完成后更新：
  - `docs/session-handoff.md`
  - `docs/next-task-prompt.md`
  - `docs/test-capability-coverage.md`
  - `docs/delphi-interface-fasttrack.md`
  - `docs/delphi-test-fasttrack-matrix.md`
  - `docs/design-doc-sync-tracker.md`
- 完成后直接提交

重点文件：
- `include/sdk/GeometrySearchPoly.h`
- `src/sdk/GeometrySearchPoly.cpp`
- `tests/capabilities/test_searchpoly_sdk.cpp`
- `tests/gaps/test_searchpoly_gaps.cpp`
```

#### 提示词 A4：`GeometryHealing`

```text
请在 `D:\code\stablecore\stablecore-geometry` 中继续推进 `GeometryHealing` 的更一般 aggressive shell policy。

目标：
1. 不改 public SDK 入口；
2. 在当前 deterministic 子集之外，继续补更一般的 aggressive shell repair policy；
3. 保持 conservative trim-backfill 与 aggressive policy 的边界清晰；
4. 新增代表性 capability 或收敛 gap 子集；
5. 同步文档并提交。

要求：
- 只写代码、测试代码、文档
- 不要编译
- 不要跑构建
- 不要回退已有改动
- 完成后更新：
  - `docs/session-handoff.md`
  - `docs/next-task-prompt.md`
  - `docs/test-capability-coverage.md`
  - `docs/design-doc-sync-tracker.md`
  - 必要时更新 `tests/gaps/test_3d_healing_gaps.cpp`
- 完成后直接提交

重点文件：
- `src/sdk/GeometryHealing.cpp`
- `tests/capabilities/test_3d_healing.cpp`
- `tests/gaps/test_3d_healing_gaps.cpp`
```

#### 提示词 A5：`GeometrySection`

```text
请在 `D:\code\stablecore\stablecore-geometry` 中继续推进 `GeometrySection` 的高阶 section 语义。

目标：
1. 不改 public SDK 入口；
2. 继续推进：
   - non-planar dominant contour stitching
   - 更高阶 coplanar fragment merge 语义
3. 在现有 capability/gap 体系下补最小代表性 capability；
4. 同步文档并提交。

要求：
- 只写代码、测试代码、文档
- 不要编译
- 不要跑构建
- 不要回退已有改动
- 完成后更新：
  - `docs/session-handoff.md`
  - `docs/next-task-prompt.md`
  - `docs/test-capability-coverage.md`
  - `docs/design-doc-sync-tracker.md`
  - 必要时更新 `tests/gaps/test_3d_section_gaps.cpp`
- 完成后直接提交

重点文件：
- `src/sdk/GeometrySection.cpp`
- `tests/capabilities/test_3d_section.cpp`
- `tests/gaps/test_3d_section_gaps.cpp`
```

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

可直接派发的提示词：

#### 提示词 B1：`GeometryBodyBoolean` 已知安全子集扩张

```text
请在 `D:\code\stablecore\stablecore-geometry` 中继续扩张 `GeometryBodyBoolean` 的“已知安全” capability 子集。

当前边界：
- 已有 deterministic identical/disjoint closed-body 子集
- 更复杂 overlap 与 shell-policy 仍由更强模型负责

目标：
1. 仅在策略边界清晰、不会破坏 public contract 的前提下，补代表性 capability；
2. 继续保持 `InvalidInput` / `UnsupportedOperation` 语义稳定；
3. 新增 deterministic tests；
4. 无法稳定支持的情形继续保留为 gap；
5. 同步文档并提交。

要求：
- 只写代码、测试代码、文档
- 不要编译
- 不要跑构建
- 不要擅自改变算法总策略
- 完成后更新：
  - `docs/session-handoff.md`
  - `docs/next-task-prompt.md`
  - `docs/test-capability-coverage.md`
  - `docs/delphi-test-fasttrack-matrix.md`
  - `docs/design-doc-sync-tracker.md`
- 完成后直接提交

重点文件：
- `src/sdk/GeometryBodyBoolean.cpp`
- `tests/capabilities/test_3d_body_boolean_sdk.cpp`
- `tests/gaps/test_3d_body_boolean_gaps.cpp`
```

#### 提示词 B2：`GeometrySearchPoly` 合同与诊断测试补强

```text
请在 `D:\code\stablecore\stablecore-geometry` 中继续补强 `GeometrySearchPoly` 的 contract tests、diagnostics tests 和 result 一致性测试。

目标：
1. 不改稳定 SDK 入口；
2. 围绕现有 `GeometrySearchPoly` 行为补充 deterministic tests；
3. 强化 `issue / diagnostics / candidates / used* flags` 的一致性断言；
4. 将已看清的小 gap 子集转正为 capability；
5. 同步文档并提交。

要求：
- 只写代码、测试代码、文档
- 不要编译
- 不要跑构建
- 不要擅自引入新的 smart-search 总策略
- 完成后更新：
  - `docs/session-handoff.md`
  - `docs/next-task-prompt.md`
  - `docs/test-capability-coverage.md`
  - `docs/delphi-test-fasttrack-matrix.md`
  - `docs/design-doc-sync-tracker.md`
- 完成后直接提交

重点文件：
- `tests/capabilities/test_searchpoly_sdk.cpp`
- `tests/gaps/test_searchpoly_gaps.cpp`
- 必要时少量调整 `src/sdk/GeometrySearchPoly.cpp`
```

#### 提示词 B3：内部拆层与 pass 整理

```text
请在 `D:\code\stablecore\stablecore-geometry` 中对已确定方向的大实现文件做内部拆层整理。

目标：
1. 不改 public SDK 入口；
2. 将大实现文件拆成更清晰的内部 helper / pass；
3. 提高文件结构可读性，减少后续强模型继续推进时的上下文负担；
4. 保持现有行为与 contract 不漂移；
5. 同步文档并提交。

要求：
- 只写代码、测试代码、文档
- 不要编译
- 不要跑构建
- 仅在高层方向已明确的模块上做拆层
- 不要顺手重写策略
- 完成后更新：
  - `docs/session-handoff.md`
  - `docs/next-task-prompt.md`
  - `docs/design-doc-sync-tracker.md`
  - 必要时更新对应 capability/gap 文档
- 完成后直接提交

适用文件举例：
- `src/sdk/GeometryHealing.cpp`
- `src/sdk/GeometrySection.cpp`
- 其他已确定方向但文件过大的 SDK 实现文件
```

#### 提示词 B4：SDK 风格统一与接口收口

```text
请在 `D:\code\stablecore\stablecore-geometry` 中推进 SDK 风格统一与接口收口。

目标：
1. 统一 `Options / Result / Issue` 风格；
2. 收口 `include/sdk` 暴露面；
3. 整理 `Geometry.h` umbrella header；
4. 保持产品侧只依赖稳定 SDK 入口；
5. 同步文档并提交。

要求：
- 只写代码、测试代码、文档
- 不要编译
- 不要跑构建
- 不要把大范围 API 重设计混进这一轮
- 优先做命名、收口、暴露面一致性
- 完成后更新：
  - `docs/session-handoff.md`
  - `docs/next-task-prompt.md`
  - `docs/rename-followup-todo.md`
  - `docs/design-doc-sync-tracker.md`
  - 必要时更新 `docs/api-member-free-function-checklist.md`
- 完成后直接提交
```

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

可直接派发的提示词：

#### 提示词 C1：交接与路线文档同步

```text
请在 `D:\code\stablecore\stablecore-geometry` 中只做交接与路线文档同步。

任务：
- 基于当前代码状态，更新：
  - `docs/session-handoff.md`
  - `docs/next-task-prompt.md`
  - `docs/test-capability-coverage.md`
  - `docs/design-doc-sync-tracker.md`
- 将最新已完成事项、下一轮安排、open gap 反映到文档中；
- 不修改算法实现；
- 完成后直接提交。

要求：
- 只写文档
- 不要编译
- 不要跑构建
- 不要修改 SDK/算法代码
```

#### 提示词 C2：快补矩阵与清单维护

```text
请在 `D:\code\stablecore\stablecore-geometry` 中只维护 fast-track 矩阵与清单文档。

任务：
- 基于当前代码状态，更新：
  - `docs/delphi-interface-fasttrack.md`
  - `docs/delphi-test-fasttrack-matrix.md`
  - `docs/rename-followup-todo.md`
  - `docs/api-member-free-function-checklist.md`
- 清理 capability/gap inventory；
- 补充缺失的 follow-up prompt 或 checklist；
- 不修改算法实现；
- 完成后直接提交。

要求：
- 只写文档
- 不要编译
- 不要跑构建
- 不要修改 SDK/算法代码
```

#### 提示词 C3：批次收尾与提交整理

```text
请在 `D:\code\stablecore\stablecore-geometry` 中只做一轮代码批次后的收尾整理。

任务：
- 核对本轮改动对应的文档是否同步；
- 补齐 capability/gap 说明中的遗漏；
- 整理简洁、准确的提交说明；
- 如有必要，更新 AI 分派文档中的后续派发提示词；
- 不修改算法实现；
- 完成后直接提交。

要求：
- 只写文档
- 不要编译
- 不要跑构建
- 不要修改 SDK/算法代码
```

适合原因：

- 这类工作主要是同步、整理、归纳
- 算法风险低
- 适合作为与核心算法并行推进的侧线工作

## 当前建议派发批次

### 批次 1：强模型主线

- 优先派发 `提示词 A1：GeometryBrepConversion`
- 优先派发 `提示词 A2：GeometryBodyBoolean`

### 批次 2：中档模型副线

- 可并行派发 `提示词 B2：GeometrySearchPoly 合同与诊断测试补强`
- 可并行派发 `提示词 B3：内部拆层与 pass 整理`
- 可并行派发 `提示词 B4：SDK 风格统一与接口收口`

### 批次 3：轻量模型收尾线

- 每轮代码批次后派发 `提示词 C1：交接与路线文档同步`
- 每轮阶段性收口后派发 `提示词 C2：快补矩阵与清单维护`
- 每轮提交前可派发 `提示词 C3：批次收尾与提交整理`

## 分派补充说明

- 只要任务开始改变算法策略，就应上调到 A 类。
- 如果任务主要是执行既定方案，并且写入范围明确，通常 B 类就足够。
- 如果任务只涉及文档、矩阵、交接和整理，保持在 C 类即可。
- 不要把大范围 API 重设计交给轻量模型。
