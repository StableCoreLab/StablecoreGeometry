# 下一次任务提示词

你正在继续 `stablecore-geometry` 的 Delphi 能力替代与稳定 SDK 收敛工作。不要重新做大范围盘点，直接基于下面的当前状态继续写代码、测试代码与文档。

## 工作约束

- 工作区：`D:\code\stablecore\stablecore-geometry`
- 执行前先遵守 `docs/ai-execution-spec.md` 的全部条款；如果与单轮实现取舍冲突，以该规范为准
- 只写代码、测试代码、文档
- 不要编译
- 不要跑构建
- 不要回退已有改动
- 每一轮必须形成两个 closed capability unit：
  - 至少 2 个新的 deterministic、testable capability
  - 至少 1 个明确标注的 gap 或未支持边界
  - 代码 + 测试 + 文档必须同时落地
- 每一轮必须完整完成 P1，并且至少触达2个 P2 / P3
- 不允许只做单模块而不覆盖本轮要求的 breadth
- 只要存在歧义或未稳定支持，必须保留或新增 gap test
- 如果模块带 diagnostics / explanation，必须保持 result / diagnostics contract 一致，不能 silent fallback
- 每完成一轮后至少同步：
  - `docs/session-handoff.md`
  - `docs/next-task-prompt.md`
  - `docs/test-capability-coverage.md`
  - `docs/design-doc-sync-tracker.md`
- 完成后直接提交

## 当前状态（2026-04-05）

> 本轮已继续推进 P1/P2/P3：新增 strip-adjacent merged-area + vertex-attached + edge-attached dual-open stable mixed-content 子集、`GeometryHealing` 的 independent-plus-competing-pair-plus-vertex-touch four-shell arbitration 子集，以及 `GeometryBodyBoolean` 的 deterministic disjoint empty intersection 子集；下面状态已与当前代码库对齐。

### GeometrySection

- public SDK 入口保持不变
- 已收敛的代表性 capability：
  - 相邻 coplanar faces merge 为单 polygon
  - 三面 coplanar strip merge（Polyhedron / Brep）
  - 四片 coplanar frame merge 为单 polygon-with-hole
  - unit cube / rectangular prism / triangular prism 的多组 deterministic non-planar section perimeter 子集
  - mixed coplanar frame + non-planar cube section 在 Polyhedron / Brep 路径共存（2 polygons / 3 closed contours / total area=9）
  - detached / vertex-attached / edge-attached mixed area + open contour 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`
  - dual edge-attached open contours 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon + 2 open contours）
  - mixed vertex-attached + edge-attached dual-open contours 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon + 2 open contours）
  - detached + vertex-attached + edge-attached triple-open contours 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon + 3 open contours）
  - edge-adjacent mixed coplanar + non-planar area 在 Polyhedron / Brep 路径都可稳定 merge 为单 polygon（area=2）
  - strip-adjacent mixed coplanar + non-planar area 在 Polyhedron / Brep 路径都可稳定 merge 为单 polygon（area=3）
  - strip-adjacent merged area + detached open contour 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon + 1 open contour / area=3）
  - strip-adjacent merged area + edge-attached open contour 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon + 1 open contour / area=3）
  - strip-adjacent merged area + vertex-attached open contour 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon + 1 open contour / area=3）
  - strip-adjacent merged area + vertex-attached + edge-attached dual-open contours 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon + 2 open contours / area=3）
- 当前仍保留的 gap：
  - ambiguous non-manifold contour stitching
  - mixed open-curve / area edge-adjacency arbitration
  - 非邻接 coplanar fragments 跨 convex-hull gap 的 merge
  - 更一般 mixed coplanar/non-planar adjacency arbitration

### GeometryHealing

- public SDK 入口保持不变
- 已覆盖 conservative trim-backfill 与 representative aggressive shell boundary-cap 子集
- aggressive boundary-cap fallback 已从单-shell body 推进到 mixed body 内的 eligible shared-edge shell 子集
- aggressive boundary-cap fallback 已进一步覆盖“同一 body 内多个彼此独立 eligible shared-edge shells 并存”的 deterministic 子集
- aggressive boundary-cap 当前已新增 shared-boundary-edge 级别的保守 competing-shell arbitration 子集：独立 eligible shell 仍可闭合，而与其他 open shell 共享 boundary edge 的 eligible shells 保持 open
- aggressive boundary-cap 当前已进一步覆盖 vertex-touch non-competing 子集：仅共享单个顶点、但不共享 boundary edge 的 eligible shared-edge shells 仍可分别闭壳
- aggressive boundary-cap 当前已进一步覆盖 competing-pair-plus-vertex-touch 组合 arbitration 子集：共享 boundary edge 的 competing pair 继续保持 open，而仅 vertex-touch 的第三个 eligible shell 仍可独立闭壳
- aggressive boundary-cap 当前已进一步覆盖 independent-plus-competing-pair-plus-vertex-touch 四壳组合 arbitration 子集：独立 eligible shell 与仅 vertex-touch 的 eligible shell 都可闭壳，而 competing pair 继续保持 open
- src/sdk/GeometryHealing.cpp 已按 trim-backfill / shell-cap / aggressive 三个内部 pass helper 拆层，便于继续推进而不改外部 contract
- 更一般 multi-shell shared-boundary-loop / shared-edge arbitration、non-planar shell repair、mesh/body joint healing 仍为 gap

### GeometrySearchPoly

- 稳定 SDK 入口位于 `include/sdk/GeometrySearchPoly.h`
- 已覆盖 diagnostics、candidate ranking、smallest-containing candidate、branch scoring、candidate-level fake-edge diagnostics、result / diagnostics consistency、auto-flag gating
- `SearchPolyResult2d` 已补充 deterministic top-candidate explanation：best-candidate synthetic metrics、top-score margin、synthetic/branch aggregate counts、ambiguous-top count，以及 runner-up synthetic / branch penalty explanation；并已补充 `bestCandidateSyntheticEdgeKind` / `runnerUpSyntheticEdgeKind`
- `SearchPolyResult2d` 已进一步补充 synthetic-source summary explanation：`bestCandidateSyntheticEdgeSource` / `runnerUpSyntheticEdgeSource` / `ambiguousTopSyntheticEdgeSource`
- `SearchPolyResult2d` 已进一步补充 ambiguous-top summary explanation：`ambiguousTopPenaltyKind` / `ambiguousTopSyntheticEdgeKind`，以及 `ambiguousTopCandidateCountWithSyntheticEdges` / `ambiguousTopCandidateCountWithBranchPenalty`
- `SearchPolyCandidate2d` 已补充 candidate-level causal explanation：`dominantPenaltyKind`、`dominantSyntheticEdgeKind`、`dominantSyntheticEdgeSource`、`inferredSyntheticEdgeLengths`、`inferredSyntheticEdges`、`inferredSyntheticEdgeKinds`、`inferredSyntheticEdgeSources`，以及逐边 line-network mapping（start/end vertex index、start/end degree、dangling-touch-count、branch-touch-count）；`SearchPolygonContainingPoint(...)` 路径同样保留这些 explanation
- richer fake-edge explanation、Delphi 级 ambiguous recovery、完整 smart-search parity 仍为 gap

### GeometryBodyBoolean

- `include/sdk/GeometryBodyBoolean.h` public contract 保持稳定
- 已覆盖 identical / disjoint closed-body 子集、deterministic disjoint empty intersection 子集、axis-aligned single-box overlap 子集、axis-aligned contained intersection / union 子集、face-touching union 子集、face-touching external difference 子集、identical difference-empty 子集、axis-aligned contained difference-empty 子集，以及 axis-aligned face/edge/vertex touching empty intersection 子集
- 非 axis-aligned / richer touching intersection、非 box touching、shell-policy、healing integration 仍为 gap

### Geometry.h / include-sdk 收口

- `include/sdk/Geometry.h` 继续作为稳定 umbrella header，只聚合 `GeometryApi` / `GeometrySearchPoly` / `GeometryBodyBoolean`
- 已新增仅包含 `sdk/Geometry.h` 的 umbrella contract test，确认产品侧可以只依赖稳定 SDK 入口
- `Options / Result / Issue` 风格统一继续作为命名与暴露面收口的一部分，但本轮不做大范围 API 重设计

## 下一轮优先级

### P1：继续深化 GeometrySection

优先方向：

- 在不改 public SDK 的前提下继续推进更高阶 section 语义
- 重点补：
  - mixed open-curve / area edge-adjacency arbitration
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
- 当前已补独立 shell 可闭合、competing shared-boundary-edge shells 保守跳过、vertex-touch non-competing 可闭合、competing-pair-plus-vertex-touch 组合 arbitration、以及 independent-plus-competing-pair-plus-vertex-touch 四壳组合子集；下一步优先推进真正共享边界回路的 multi-shell arbitration，而不是继续扩散仅共享单点的子例

### P3：继续深化 GeometrySearchPoly / GeometryBodyBoolean

- GeometrySearchPoly：推进 richer fake-edge explanation 与 ambiguous recovery，但保持 result-consistency / auto-flag contract 只做确定性补强
- 当前 SearchPoly 已有 top-candidate + runner-up explanation 摘要，以及 candidate-level penalty kind / dominant-synthetic-kind / dominant-synthetic-source / synthetic-edge-lengths / synthetic-edge-list / synthetic-edge-kind / synthetic-edge-source / line-network touch mapping / vertex identity mapping；下一步优先考虑更强的 fake-edge causal explanation 与 ambiguous recovery，而不是继续扩散临时摘要字段
- GeometryBodyBoolean：推进更一般 overlap / touching 子集，但保持 InvalidInput / UnsupportedOperation contract 稳定；当前 disjoint / axis-aligned contained / touching union / external difference / empty intersection 子集已收敛，下一步优先考虑 non-axis-aligned / richer touching intersection 与非单-box touching 边界

### P4：继续推进 SDK 风格统一与接口收口

- 继续收紧 `include/sdk` 暴露面，优先通过 umbrella / public contract 文档收口，而不是扩散临时 helper
- 继续统一 Delphi-facing SDK 的 `Options / Result / Issue` 风格，但避免引入大范围 API 重设计

## 交付口径

完成后请明确说明：

1. 本轮新增了哪些稳定 capability
2. 哪些 gap 继续保留
3. 更新了哪些代码、测试与文档
4. 提交号是什么

并且默认满足以下检查：

- 至少包含 capability test（happy path）
- 至少包含 edge-case test（boundary condition）
- 如存在歧义或未支持边界，至少包含 gap test
- 不把不稳定逻辑伪装成 capability
- 不引入 breaking public SDK contract
