# Delphi 测试快通道矩阵

## 目的

这份矩阵把 Delphi 替代工作收敛成“接口 + 测试”的推进程序。

## contract 与 capability 矩阵

| SDK 面 | 最低测试要求 | 当前状态 |
| --- | --- | --- |
| `SearchPoly.h` | invalid-input contract、closed-loop representative capability、candidate ranking、branch scoring、fake-edge diagnostics、result/diagnostics consistency、auto-flag gating、point-containing candidate lookup | 第三批已落地 |
| `Offset.h` | line/arc/polyline/polygon offset contract、重建输出 capability、split-output capability | 已覆盖 |
| `Relation.h` | point location、containment 排序、代表性 nested/hole 拓扑 capability | 已覆盖子集 |
| `Boolean.h` | invalid operands、crossing / containment / equal / touching 代表性 capability | 已覆盖子集 |
| `Section.h` | section contract、deterministic contour/segment 计数、topology/components、Brep 路径对齐 | 已覆盖子集 |
| `BrepConversion.h` | invalid-input contract、代表性 conversion capability、明确的 open-gap 覆盖 | 已覆盖子集 |
| `Healing.h` | conservative trim-backfill contract、代表性 aggressive-policy capability、明确的 open-gap 覆盖 | 已覆盖子集 |
| `BodyBoolean.h` | invalid-input contract、identical/disjoint closed-body 代表性 capability、axis-aligned single-box overlap 和 face-touching union capability、以及超出该子集的 gap 覆盖 | 已落第一批 overlap/touching 子集 |
| `MeshConversion.h` | 代表性 Brep / Polyhedron mesh conversion capability、fidelity gap | 已覆盖子集 |

## 快通道完成定义

算法库可视为适合产品开发，需满足：

1. 约定好的 Delphi-facing SDK 接口已存在；
2. 每个接口都有 contract 测试；
3. 每个已实现子集都有 capability 测试；
4. 每个未实现区域都有 gap 测试；
5. 已承诺的测试矩阵全部为绿。

## 当前立即待办

1. 继续推进 `BodyBoolean`，超越 identical/disjoint + axis-aligned single-box overlap / face-touching union 子集。
2. 继续推进 `SearchPoly`，超越当前 branch-scored + candidate fake-edge diagnostic 子集，向更丰富解释和 ambiguous recovery 前进，同时保持 result-consistency 覆盖稳定。
3. 及时同步 `docs/test-capability-coverage.md`、`docs/session-handoff.md` 和 `docs/design-doc-sync-tracker.md`，只要某个子集状态发生变化就更新。

