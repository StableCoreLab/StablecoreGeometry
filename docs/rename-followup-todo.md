# 稳定 API 后续待办

这份文档用于记录 API 稳定化工作里那些“要持续收敛，但不要求单轮做完”的事项。
目标是让 `include/sdk` 始终保持为唯一的产品侧入口，同时继续把内部 helper 面收紧。

## 当前重点

- 保持 Delphi-facing 接口稳定。
- 优先做接口先行的后续工作，不要为了方便就扩散新的内部 helper。
- 只有当子集稳定时，才把 gap 文档里的条目移入 capability 测试。
- 保持待办列表短小、直接、可执行。

## P1：公共 API 面

- `GeometrySearchPoly`
  - 保持 `include/sdk/GeometrySearchPoly.h` 作为稳定入口。
  - 维持当前子集：invalid-input contract、candidate ranking、branch scoring、candidate-level fake-edge diagnostics、result/diagnostics consistency、auto-flag gating、smallest-containing lookup。
  - 下一轮 follow-up 提示：在不破坏 result consistency 的前提下，继续加深 richer fake-edge explanation 和 ambiguous recovery。

- `GeometryBodyBoolean`
  - 保持 `include/sdk/GeometryBodyBoolean.h` 稳定。
  - 维持当前子集：invalid-input contract、identical/disjoint closed-body capability、axis-aligned single-box overlap、face-touching union。
  - 下一轮 follow-up 提示：谨慎加深 overlap 语义，同时把 touching intersection/difference 和 shell-policy 继续保留在 gap 状态。

- `Geometry.h`
  - 保持 umbrella header 作为唯一的产品侧聚合入口。
  - 不要把临时 helper 重新暴露到 umbrella header 上。

## P2：内部算法拆层

- `GeometryBrepConversion`
  - 继续保持当前 non-planar repair 的拆分：
    - support-plane scoring
    - representative target aggregation
    - cross-face snapping
    - topology reconciliation
  - follow-up 提示：只有在当前 representative-average 覆盖仍稳定时，才增加新的 repair 子集。

- `GeometryHealing`
  - 保持 conservative trim-backfill、shell-cap fallback 和 aggressive closure 的当前拆分。
  - follow-up 提示：只有当新的 shell repair 子集能在 capability 矩阵里清楚描述时，才继续扩展。

- `GeometrySection`
  - 保持当前 contour 处理拆分稳定。
  - follow-up 提示：继续往更一般的 mixed coplanar / non-planar arbitration 方向推进，但不要把 gap 边界冲掉。

## P3：文档卫生

- 每批推进后都要同步这些文档：
  - `docs/session-handoff.md`
  - `docs/next-task-prompt.md`
  - `docs/test-capability-coverage.md`
  - `docs/design-doc-sync-tracker.md`
- 如果接口矩阵变化，还要同步：
  - `docs/delphi-interface-fasttrack.md`
  - `docs/delphi-test-fasttrack-matrix.md`
- 如果某个 capability 已经稳定，必须在同一批次把它从 gap inventory 里移出去。

## Follow-up 提示模板

- “继续把 `GeometrySearchPoly` 从当前 branch-scored + fake-edge diagnostic 子集推进到更丰富解释和 ambiguous recovery。”
- “继续把 `GeometryBodyBoolean` 从 identical/disjoint + axis-aligned single-box overlap / face-touching union 推进到更丰富 overlap，同时保持 gap contract 稳定。”
- “保持 `GeometrySection` / `GeometryHealing` 的拆层清晰，并在同一轮同步 capability/gap inventory。”

