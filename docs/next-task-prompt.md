# 下一次任务提示词

你正在继续 `stablecore-geometry` 的 Delphi 能力替代与稳定 SDK 收敛工作。不要重新做大范围盘点，直接基于下面的当前状态继续写代码、测试代码与文档。

## 工作约束

- 工作区：`D:\code\stablecore\stablecore-geometry`
- 只写代码、测试代码、文档
- 不要编译
- 不要跑构建
- 不要回退已有改动
- 每完成一轮后至少同步：
  - `docs/session-handoff.md`
  - `docs/next-task-prompt.md`
  - `docs/test-capability-coverage.md`
  - `docs/design-doc-sync-tracker.md`
- 完成后直接提交

## 当前状态（2026-04-05）

> 本轮已继续推进 P1/P2/P3：新增 mixed area + open contour 的 representative section capability、mixed body 内 eligible shared-edge shell 的 aggressive boundary-cap capability、face-touching external difference 子集，以及 `GeometrySearchPoly` 的 top-candidate explanation 字段；下面状态已与当前代码库对齐。

### GeometrySection

- public SDK 入口保持不变
- 已收敛的代表性 capability：
  - 相邻 coplanar faces merge 为单 polygon
  - 三面 coplanar strip merge（Polyhedron / Brep）
  - 四片 coplanar frame merge 为单 polygon-with-hole
  - unit cube / rectangular prism / triangular prism 的多组 deterministic non-planar section perimeter 子集
  - mixed coplanar frame + non-planar cube section 在 Polyhedron / Brep 路径共存（2 polygons / 3 closed contours / total area=9）
- 当前仍保留的 gap：
  - ambiguous non-manifold contour stitching
  - mixed open-curve / area adjacency arbitration
  - 非邻接 coplanar fragments 跨 convex-hull gap 的 merge
  - 更一般 mixed coplanar/non-planar adjacency arbitration

### GeometryHealing

- public SDK 入口保持不变
- 已覆盖 conservative trim-backfill 与 representative aggressive shell boundary-cap 子集
- aggressive boundary-cap fallback 已从单-shell body 推进到 mixed body 内的 eligible shared-edge shell 子集
- src/sdk/GeometryHealing.cpp 已按 trim-backfill / shell-cap / aggressive 三个内部 pass helper 拆层，便于继续推进而不改外部 contract
- 更一般 multi-shell shared-edge arbitration、non-planar shell repair、mesh/body joint healing 仍为 gap

### GeometrySearchPoly

- 稳定 SDK 入口位于 `include/sdk/GeometrySearchPoly.h`
- 已覆盖 diagnostics、candidate ranking、smallest-containing candidate、branch scoring、candidate-level fake-edge diagnostics、result / diagnostics consistency、auto-flag gating
- `SearchPolyResult2d` 已补充 deterministic top-candidate explanation：best-candidate synthetic metrics、top-score margin、synthetic/branch aggregate counts、ambiguous-top count
- richer fake-edge explanation、Delphi 级 ambiguous recovery、完整 smart-search parity 仍为 gap

### GeometryBodyBoolean

- `include/sdk/GeometryBodyBoolean.h` public contract 保持稳定
- 已覆盖 identical / disjoint closed-body 子集、axis-aligned single-box overlap 子集、face-touching union 子集，以及 face-touching external difference 子集
- touching intersection、非 box touching、shell-policy、healing integration 仍为 gap

### Geometry.h / include-sdk 收口

- `include/sdk/Geometry.h` 继续作为稳定 umbrella header，只聚合 `GeometryApi` / `GeometrySearchPoly` / `GeometryBodyBoolean`
- 已新增仅包含 `sdk/Geometry.h` 的 umbrella contract test，确认产品侧可以只依赖稳定 SDK 入口
- `Options / Result / Issue` 风格统一继续作为命名与暴露面收口的一部分，但本轮不做大范围 API 重设计

## 下一轮优先级

### P1：继续深化 GeometrySection

优先方向：

- 在不改 public SDK 的前提下继续推进更高阶 section 语义
- 重点补：
  - mixed open-curve / area adjacency arbitration
  - 更一般 non-planar dominant contour stitching
  - 更一般 mixed coplanar/non-planar adjacency merge
- 保持 capability / gap 边界清晰，不要把仍不稳定语义伪装成已完成

建议触达文件：

- `src/sdk/GeometrySection.cpp`
- `tests/capabilities/test_3d_section.cpp`
- `tests/gaps/test_3d_section_gaps.cpp`

### P2：继续深化 GeometryHealing

- 继续扩展 aggressive shell policy，但保持 conservative trim-backfill 与 topology-changing aggressive closure 的边界清晰
- 当前 mixed body 中的 per-shell shared-edge boundary-cap 已有代表性 capability；下一步优先考虑更一般 multi-shell arbitration，而不是回退到单-shell-only

### P3：继续深化 GeometrySearchPoly / GeometryBodyBoolean

- GeometrySearchPoly：推进 richer fake-edge explanation 与 ambiguous recovery，但保持 result-consistency / auto-flag contract 只做确定性补强
- 当前 SearchPoly 已有 top-candidate explanation 摘要；下一步优先考虑更强的 fake-edge causal explanation 与 ambiguous recovery，而不是继续堆简单计数字段
- GeometryBodyBoolean：推进更一般 overlap / touching 子集，但保持 InvalidInput / UnsupportedOperation contract 稳定；当前 touching difference 的 external face-touching 子集已收敛，下一步优先考虑 touching intersection 表达与非单-box touching 边界

### P4：继续推进 SDK 风格统一与接口收口

- 继续收紧 `include/sdk` 暴露面，优先通过 umbrella / public contract 文档收口，而不是扩散临时 helper
- 继续统一 Delphi-facing SDK 的 `Options / Result / Issue` 风格，但避免引入大范围 API 重设计

## 交付口径

完成后请明确说明：

1. 本轮新增了哪些稳定 capability
2. 哪些 gap 继续保留
3. 更新了哪些代码、测试与文档
4. 提交号是什么
