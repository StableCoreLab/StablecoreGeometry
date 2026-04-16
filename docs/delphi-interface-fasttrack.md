# Delphi 接口快通道

## 目标

这份文档用于固定 Delphi 几何栈的快通道替代目标：

- 尽早暴露完整接口面
- 每个公开入口都配上 contract 或 capability 测试
- 让产品开发可以直接对接稳定的 C++ SDK 名称
- 把算法深度工作留在这些接口之后

## Delphi 到 C++ 接口清单

| Delphi 能力族 | Delphi 依据 | C++ 目标面 | 当前状态 |
| --- | --- | --- | --- |
| `SearchPoly` / fake-edge / auto-close / branch cleanup | `GGLSearchPolyFunc2d.pas` | `SearchPoly.h` | 已落 branch-scored 子集 |
| polygon relation tree / contains hierarchy | `GGLPolyRelation.pas` | `Relation.h` + `Topology.h` | 可用子集 |
| 带重建 polygon 回收的 offset | `GGLOffsetFunc2d.pas` | `Offset.h` | 可用子集 |
| 2D boolean | GGJ + Geo2DLib 产品使用 | `Boolean.h` | 可用子集 |
| segment search / box-tree 加速 | Geo2DLib | `SegmentSearch2d.h` + `BoxTree2d.h` | 可用子集 |
| section / projected contour rebuild | `GGJSumpCommon.pas`, `GGL3DCommon.pas` | `Section.h` | 可用子集 |
| polyhedron / brep conversion | GGJ conversion 和 rebuild 路径 | `BrepConversion.h` | 可用子集，但仍有 open repair gap |
| brep healing / trim 回填 | GGJ healing 路径 | `Healing.h` | 可用子集，但仍有 open aggressive-policy gap |
| body / shell boolean | `GGL.pas` | `BodyBoolean.h` | 已落第一批 overlap + touching-union 子集 |
| brep 到 mesh / mesh 到 body 支撑链 | GGJ export + conversion 使用 | `MeshConversion.h`, `MeshRepair.h`, `MeshOps.h` | 可用子集，但仍有 fidelity gap |

## 快通道规则

1. 产品代码只应依赖 `Include` 下的 SDK 头文件。
2. 新的 Delphi parity 工作，应优先补稳定入口，再逐步加深内部实现。
3. 每个新接口都必须配套以下之一：
   - capability 测试，证明一个可支持的子集，或
   - gap 测试，明确记录缺失的算法深度。
4. “算法库完成”只表示约定好的接口清单已经把已承诺测试跑绿，不代表所有几何边界都已解决。

## 第一批快通道

- `SearchPoly.h`
  - 把 Delphi 风格的 polygon search 正式化为一级 SDK 入口，而不是继续让产品侧临时调用 `BuildMultiPolygonByLines(...)`
  - 当前稳定子集覆盖 invalid-input contract、candidate ranking、branch scoring、candidate-level fake-edge diagnostics、result/diagnostics consistency、auto-flag gating，以及 smallest-containing candidate lookup
- `BodyBoolean.h`
  - 预留 Delphi 风格的 body/shell boolean SDK 名称，方便产品现在就对接稳定 API
  - 当前稳定子集覆盖 invalid-input contract、identical/disjoint closed-body 子集、axis-aligned single-box overlap 子集（结果仍保持一个 closed box），以及 face-touching union 子集
- 配套测试
  - `UnitTests/Capabilities/TestSearchPoly.cpp`
  - `UnitTests/Capabilities/Test3dBodyBoolean.cpp`
  - `UnitTests/Gaps/Test3dBodyBooleanGaps.cpp`

## 下一批

- 继续把 `SearchPoly` 从当前 branch-scored + candidate fake-edge diagnostic 子集，推进到更丰富的 fake-edge explanation 和 Delphi 级 ambiguous recovery
- 继续把 `BodyBoolean` 从 identical/disjoint + axis-aligned single-box overlap / face-touching union 子集，推进到 non-box overlap、touching intersection/difference、shell-policy 和 healing-integrated cases
- 只有当对应 capability 测试转绿时，才继续缩小 gap 测试
