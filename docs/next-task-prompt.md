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
  - 至少 3 个新的 deterministic、testable capability
  - 其中至少 2 个必须属于 P1
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

> 本轮已继续推进 P1/P2/P3：新增 `GeometrySection` 的 merged polygon-with-hole + edge-attached / vertex-attached outer-boundary open-contour 子集，新增 `GeometryHealing` 的 closed-shell + partial-overlap pair / independent+vertex-touch+partial-overlap pair arbitration 子集，并继续细化 `GeometrySearchPoly` ambiguous recovery gap。下面状态已与当前代码库对齐。

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
  - detached + dual edge-attached open contours 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon + 3 open contours）
  - mixed vertex-attached + edge-attached dual-open contours 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon + 2 open contours）
  - vertex-touch + edge-touch 双 open contours 在 Polyhedron / Brep 路径都可稳定保留为两条 open contours，不会误塌缩为单 polyline
  - detached + vertex-attached + edge-attached triple-open contours 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon + 3 open contours）
  - dual disjoint non-planar closed loops 在 Polyhedron 路径可稳定保留为 `2 polygons / 2 contours / 2 topology roots`
  - non-planar dominant closed loop + interior open spur 在 Polyhedron / Brep 路径都可稳定保留为 `1 polygon + 1 open contour`
  - edge-adjacent mixed coplanar + non-planar area 在 Polyhedron / Brep 路径都可稳定 merge 为单 polygon（area=2）
  - strip-adjacent mixed coplanar + non-planar area 在 Polyhedron / Brep 路径都可稳定 merge 为单 polygon（area=3）
  - L-corner mixed coplanar + non-planar area 在 Polyhedron / Brep 路径都可稳定 merge 为单 polygon（area=3）
  - frame-with-hole + adjacent non-planar area 在 Polyhedron / Brep 路径都可稳定 merge 为单 polygon-with-hole（area=9）
  - merged polygon-with-hole + detached open contour 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon-with-hole + 1 open contour / area=9）
  - merged polygon-with-hole + edge-attached outer-boundary open contour 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon-with-hole + 1 open contour / area=9）
  - merged polygon-with-hole + vertex-attached outer-boundary open contour 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon-with-hole + 1 open contour / area=9）
  - strip-adjacent merged area + detached open contour 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon + 1 open contour / area=3）
  - strip-adjacent merged area + edge-attached open contour 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon + 1 open contour / area=3）
  - strip-adjacent merged area + vertex-attached open contour 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon + 1 open contour / area=3）
  - strip-adjacent merged area + vertex-attached + edge-attached dual-open contours 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon + 2 open contours / area=3）
  - strip-adjacent merged area + detached + vertex-attached + edge-attached triple-open contours 在 Polyhedron / Brep 路径都可稳定保留为 `Mixed`（1 polygon + 3 open contours / area=3）
  - detached-left + edge-attached mixed open contours 在 Polyhedron / Brep 路径都可稳定按 `boundary-attached -> detached` 排序输出
- 当前仍保留的 gap：
  - ambiguous non-manifold contour stitching
  - mixed open-curve / area edge-adjacency arbitration（超出已收敛的 vertex-touch + edge-touch 双 open representative 子集）
  - 非邻接 coplanar fragments 跨 convex-hull gap 的 merge
  - mixed coplanar/non-planar merged polygon-with-hole 的 higher-order boundary-attached open-curve arbitration（超出 single edge-attached / single vertex-attached outer-boundary spur 子集）

### GeometryHealing

- public SDK 入口保持不变
- 已覆盖 conservative trim-backfill 与 representative aggressive shell boundary-cap 子集
- aggressive boundary-cap fallback 已从单-shell body 推进到 mixed body 内的 eligible shared-edge shell 子集
- aggressive boundary-cap fallback 已进一步覆盖“同一 body 内多个彼此独立 eligible shared-edge shells 并存”的 deterministic 子集
- aggressive boundary-cap 当前已新增 shared-boundary-edge 级别的保守 competing-shell arbitration 子集：独立 eligible shell 仍可闭合，而与其他 open shell 共享 boundary edge 的 eligible shells 保持 open
- aggressive boundary-cap 当前已进一步覆盖 vertex-touch non-competing 子集：仅共享单个顶点、但不共享 boundary edge 的 eligible shared-edge shells 仍可分别闭壳
- aggressive boundary-cap 当前已进一步覆盖 competing-pair-plus-vertex-touch 组合 arbitration 子集：共享 boundary edge 的 competing pair 继续保持 open，而仅 vertex-touch 的第三个 eligible shell 仍可独立闭壳
- aggressive boundary-cap 当前已进一步覆盖 independent-plus-competing-pair-plus-vertex-touch 四壳组合 arbitration 子集：独立 eligible shell 与仅 vertex-touch 的 eligible shell 都可闭壳，而 competing pair 继续保持 open
- aggressive boundary-cap 当前已进一步覆盖 mixed closed-shell + competing-pair + vertex-touch shell arbitration 子集：closed shell 保持稳定，competing pair 继续保持 open，而仅 vertex-touch 的 eligible shell 可独立闭壳
- aggressive boundary-cap 当前已进一步覆盖 duplicated-topology geometrically coincident shared-boundary-loop arbitration 子集：几何上共享同一 boundary loop、但 topology 独立的 eligible shells 会保守保持 open
- aggressive boundary-cap 当前已进一步覆盖 partial-overlap shared-boundary-loop arbitration 子集：共线且区间重叠的 boundary spans 也会保守保持 open
- aggressive boundary-cap 当前已进一步覆盖 independent + partial-overlap pair local arbitration 子集：独立 eligible shell 可继续闭壳，而 partial-overlap competing pair 保持 open
- aggressive boundary-cap 当前已进一步覆盖 mixed closed-shell + partial-overlap pair arbitration 子集：closed shell 保持稳定，而 partial-overlap competing pair 继续保持 open
- aggressive boundary-cap 当前已进一步覆盖 independent + vertex-touch + partial-overlap pair local arbitration 子集：独立 eligible shell 与仅 vertex-touch 的 eligible shell 可继续闭壳，而 partial-overlap competing pair 保持 open
- aggressive boundary-cap 当前已进一步覆盖 non-planar shared-edge shell reject 子集：共享 interior edge 但整体不共面的 shell 不会误走 planar boundary-cap，而会继续保持 open
- src/sdk/GeometryHealing.cpp 已按 trim-backfill / shell-cap / aggressive 三个内部 pass helper 拆层，便于继续推进而不改外部 contract
- 更一般 multi-shell shared-boundary-loop / shared-edge arbitration、non-planar shell repair、mesh/body joint healing 仍为 gap

### GeometrySearchPoly

- 稳定 SDK 入口位于 `include/sdk/GeometrySearchPoly.h`
- 已覆盖 diagnostics、candidate ranking、smallest-containing candidate、branch scoring、candidate-level fake-edge diagnostics、result / diagnostics consistency、auto-flag gating
- `SearchPolyResult2d` 已补充 deterministic top-candidate explanation：best-candidate synthetic metrics、top-score margin、synthetic/branch aggregate counts、ambiguous-top count，以及 runner-up synthetic / branch penalty explanation；并已补充 `bestCandidateSyntheticEdgeKind` / `runnerUpSyntheticEdgeKind`
- `SearchPolyResult2d` 已进一步补充 synthetic-source summary explanation：`bestCandidateSyntheticEdgeSource` / `runnerUpSyntheticEdgeSource` / `ambiguousTopSyntheticEdgeSource`
- `SearchPolyResult2d` 已进一步补充 ambiguous-top summary explanation：`ambiguousTopPenaltyKind` / `ambiguousTopSyntheticEdgeKind`，以及 `ambiguousTopCandidateCountWithSyntheticEdges` / `ambiguousTopCandidateCountWithBranchPenalty`
- `SearchPolyCandidate2d` 已补充 candidate-level causal explanation：`dominantPenaltyKind`、`dominantSyntheticEdgeKind`、`dominantSyntheticEdgeSource`、`inferredSyntheticEdgeLengths`、`inferredSyntheticEdges`、`inferredSyntheticEdgeKinds`、`inferredSyntheticEdgeSources`，以及逐边 line-network mapping（start/end vertex index、start/end degree、dangling-touch-count、branch-touch-count）；`SearchPolygonContainingPoint(...)` 路径同样保留这些 explanation
- clean top candidate 对 synthetic runner-up 的 causal explanation 已形成稳定子集：产品侧可直接读取 `bestCandidate*` / `runnerUp*` / candidate-level dominant penalty/source 来解释“为什么 fake-edge 候选输了”
- richer fake-edge explanation、Delphi 级 ambiguous recovery、完整 smart-search parity 仍为 gap

### GeometryBodyBoolean

- `include/sdk/GeometryBodyBoolean.h` public contract 保持稳定
- 已覆盖 identical / disjoint closed-body 子集、deterministic disjoint empty intersection / ordered-multi-body union 子集、axis-aligned single-box overlap 子集、axis-aligned contained intersection / union 子集、face-touching union 子集、face-touching external difference 子集、identical difference-empty 子集、axis-aligned contained difference-empty 子集、axis-aligned edge/vertex-touching ordered multi-body union / external difference 子集，以及 axis-aligned face/edge/vertex touching empty intersection 子集
- 已进一步覆盖 explicit unsupported contract 子集：face-touching L-shape non-box union 与 rotated-box positive-volume intersection 在 Polyhedron / Brep 路径都稳定返回 `UnsupportedOperation`
- 更一般 non-axis-aligned / richer touching intersection、shell-policy、healing integration 仍为 gap

### Geometry.h / include-sdk 收口

- `include/sdk/Geometry.h` 继续作为稳定 umbrella header，只聚合 `GeometryApi` / `GeometrySearchPoly` / `GeometryBodyBoolean`
- 已新增仅包含 `sdk/Geometry.h` 的 umbrella contract test，确认产品侧可以只依赖稳定 SDK 入口
- `Options / Result / Issue` 风格统一继续作为命名与暴露面收口的一部分，但本轮不做大范围 API 重设计

## 下一轮优先级

### P1：继续深化 GeometrySection

优先方向：

- 在不改 public SDK 的前提下继续推进更高阶 section 语义
- 重点补，但要尽量落成“具体测试场景”，不要只写抽象语义：
  - 已转正：
    - `VertexTouchThenEdgeTouchOpenContoursDoNotCollapseIntoSinglePolyline`
    - `NonPlanarLoopWithInteriorOpenSpurKeepsClosedContourAndOpenContourSeparate`
    - `LCornerCoplanarPatchAndNonPlanarAreaMergeIntoSinglePolygon`
    - `MixedMergedAreaWithInteriorHoleStaysSinglePolygonWithHole`
  - 当前剩余重点：
    - `MixedMergedAreaWithInteriorHoleAndDualBoundaryAttachedOpenContoursStillNeedArbitration`
      - 场景：coplanar frame-with-hole 与相邻 non-planar section area 已 merge 成单 polygon-with-hole，再额外挂两条接在 merged outer boundary 上的 open contours（例如一条 edge-attached、一条 vertex-attached）
      - 要解决的问题：确认 dual boundary-attached open contours 不会破坏 hole 语义，也不会彼此错误拼接或改写稳定排序
      - 期望：未来稳定支持 `1 polygon-with-hole + 2 open contours`
    - 更一般 ambiguous non-manifold contour stitching
      - 建议继续落成具体测试名，而不是只保留抽象 gap 名词
  - 当前 detached-left + edge-attached ordering 只是一个 representative ordering 子集，不等于更一般 adjacency 语义已闭合
- 保持 capability / gap 边界清晰，不要把仍不稳定语义伪装成已完成
- 实现方式要求：
  - 优先把 `tests/gaps` 里已经具体化的用例转成 `tests/capabilities`
  - 每轮至少把 3 个 gap 场景推进为 capability test 通过
  - 其中至少 2 个 gap 场景必须来自 P1 方向，也就是 `GeometrySection`
  - 如果某个 gap 仍然不能稳定通过，就保留 gap test，并在本轮给出更精确的失败场景描述

建议触达文件：

- `src/sdk/GeometrySection.cpp`
- `tests/capabilities/test_3d_section.cpp`
- `tests/gaps/test_3d_section_gaps.cpp`

### P2：继续深化 GeometryHealing

- 继续扩展 aggressive shell policy，但保持 conservative trim-backfill 与 topology-changing aggressive closure 的边界清晰
- 当前 mixed body 中的 per-shell shared-edge boundary-cap 已有代表性 capability；下一步优先考虑更一般 multi-shell arbitration，而不是回退到单-shell-only
- 当前已补独立 shell 可闭合、competing shared-boundary-edge shells 保守跳过、duplicated-topology geometrically coincident shared-boundary-loop 保守跳过、partial-overlap shared-boundary-loop 保守跳过、vertex-touch non-competing 可闭合、competing-pair-plus-vertex-touch 组合 arbitration、independent-plus-competing-pair-plus-vertex-touch 四壳组合子集、independent-plus-partial-overlap-pair local arbitration、mixed closed-shell + competing-pair + vertex-touch shell 子集、mixed closed-shell + partial-overlap pair 子集，以及 independent + vertex-touch + partial-overlap pair 子集；下一步优先推进更一般 three-shell / mixed closed-shell + partial-overlap shared-boundary-loop arbitration，而不是继续扩散仅共享单点的子例
- 实现方式要求：
  - 优先把 `tests/gaps/test_3d_healing_gaps.cpp` 里具体化的用例转成 capability tests
  - 每轮至少有 1 个 healing gap 场景推进成 capability
  - 如果当前轮不能稳定实现，就把 gap 描述改成更具体的输入拓扑和期望结果
- 建议直接转成以下测试目标：
  - `AggressiveHealingKeepsClosedAndIndependentShellsWhileSkippingPartialOverlapPair`
    - 场景：body 内同时存在一个已闭壳 shell、一个独立 eligible shell，以及一对 partial-overlap competing shells
    - 要解决的问题：确认 arbitration 仍保持局部，closed / independent shells 不会被 partial-overlap pair 拖成保守整体跳过
    - 期望：`closed + closed + open + open`

### P3：继续深化 GeometrySearchPoly / GeometryBodyBoolean

- GeometrySearchPoly：推进 richer fake-edge explanation 与 ambiguous recovery，但保持 result-consistency / auto-flag contract 只做确定性补强
- 当前 SearchPoly 已有 top-candidate + runner-up explanation 摘要，以及 candidate-level penalty kind / dominant-synthetic-kind / dominant-synthetic-source / synthetic-edge-lengths / synthetic-edge-list / synthetic-edge-kind / synthetic-edge-source / line-network touch mapping / vertex identity mapping；下一步优先考虑更强的 fake-edge causal explanation 与 ambiguous recovery，而不是继续扩散临时摘要字段
- GeometrySearchPoly 可直接转成以下测试目标：
  - `SearchPolygonsReportsAmbiguousRecoveryWhenTwoCandidatesTieAfterSyntheticPenaltyNormalization`
    - 场景：两个 tied-top candidates 都需要 synthetic edges，且 dominant synthetic source 不同（例如 `SingleGapClose` 对 `MixedBridge`）
    - 要解决的问题：明确 ambiguous recovery 需要返回什么解释，而不是只有 `ambiguousTopCandidateCount`
- GeometryBodyBoolean：推进更一般 overlap / touching 子集，但保持 InvalidInput / UnsupportedOperation contract 稳定；当前 disjoint ordered-union / axis-aligned contained / face-touching single-box union / edge-vertex-touching ordered multi-body union / external difference / empty intersection 子集已收敛，face-touching L-shape union 与 rotated-box positive-volume intersection 也已明确收口到 explicit unsupported contract；下一步优先考虑 shell-policy 与更一般 non-axis-aligned / richer touching intersection
- 实现方式要求：
  - 优先把 `tests/gaps/test_searchpoly_gaps.cpp` 和 `tests/gaps/test_3d_body_boolean_gaps.cpp` 里能明确落地的场景转成 capability
  - 每轮至少有 1 个 P2 / P3 场景从 gap 转成 capability
  - 其余未能实现的项，保留 gap test，不要停留在文案层
- GeometryBodyBoolean 建议直接转成以下测试目标：
  - `ContainedShellPolicyOptionStillHasNoEffectAndStaysGap`
    - 场景：`operateOnShells=true` 的 contained / touching 输入
    - 要解决的问题：明确 shell-policy 尚未接管语义，避免产品侧误以为 option 已生效
    - 期望：当前结果与默认路径一致，或 gap test 明确未支持

### P4：继续推进 SDK 风格统一与接口收口

- 继续收紧 `include/sdk` 暴露面，优先通过 umbrella / public contract 文档收口，而不是扩散临时 helper
- 继续统一 Delphi-facing SDK 的 `Options / Result / Issue` 风格，但避免引入大范围 API 重设计

## 剩余任务具体化清单

后续轮次优先从下面这些“可直接命名为测试”的条目里挑，而不是继续写抽象 gap 名词：

1. `GeometrySection`
   - `MixedMergedAreaWithInteriorHoleAndDualBoundaryAttachedOpenContoursStillNeedArbitration`
2. `GeometryHealing`
   - `AggressiveHealingKeepsClosedAndIndependentShellsWhileSkippingPartialOverlapPair`
3. `GeometrySearchPoly`
   - `SearchPolygonsReportsAmbiguousRecoveryWhenTwoCandidatesTieAfterSyntheticPenaltyNormalization`
4. `GeometryBodyBoolean`
   - `ContainedShellPolicyOptionStillHasNoEffectAndStaysGap`

## 本轮执行口径

- 目标不是继续扩充抽象清单，而是把 gap tests 一个个转成 capability tests
- 每轮至少实现 3 项能力提升
- 其中至少 2 项必须来自 P1，也就是 `GeometrySection`
- 其余 1 项可以来自 P2 / P3
- 如果某条 gap 仍然不能稳定通过，就继续保留 gap test，并把失败场景描述改得更具体、更可复现

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
