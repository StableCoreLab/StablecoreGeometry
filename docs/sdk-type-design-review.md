# SDK Type Design Review

## Scope

本评审只回顾当前算法库公开类型设计、派生体系、类/结构体命名、方法位置与模块边界，不涉及实现正确性和性能细节。

参考范围：
- `include/sdk`
- `src/sdk`

## Overall Impression

当前库整体上是“值类型 + 自由函数算法 + 少量索引类 + 一条 segment 多态链”的设计：
- 值类型：`LineSegment2d`、`ArcSegment2d`、`Circle2d`、`Ellipse2d`、`Rectangle2d`、`Polyline2d`、`Polygon2d`
- 聚合类型：`MultiPolyline2d`、`MultiPolygon2d`
- 索引/服务类：`GeometryBoxTree2d`、`GeometryKDTree2d`、`GeometrySegmentSearch2d`、`PolygonTopology2d`
- 结果类型：`SegmentProjection2d`、`SegmentIntersection2d`、`AxisProjection2d` 等 POD/轻量 struct
- 多态链：`Segment2d` -> `LineSegment2d` / `ArcSegment2d`

大方向是合理的，API 也基本易懂。但从长期维护看，有几处设计债已经比较明显，尤其是命名风格和职责边界。

## Main Findings

### 1. Public API naming style is not fully unified

现状：
- SDK 层大量接口采用短名风格：`PointAt`、`Bounds`、`Count`、`HoleAt`、`Contains`
- 但底层桥接和部分公开语义明显仍受旧内核影响：`GetVertexCount`、`GetBoundingBox`、`GetMinPoint`、`GetMaxPoint`
- `MultiPolyline2d` / `MultiPolygon2d` 同时提供 `Count()` 和 `PolylineCount()` / `PolygonCount()`，存在重复语义

影响：
- 外部用户难以判断推荐命名风格到底是 `Xxx()` 还是 `GetXxx()`。
- 随着 API 增长，重复命名会继续扩散，降低一致性。

建议：
- 对公开 SDK 统一采用现在已经占主导的短名风格：`PointAt` / `Bounds` / `Count` / `Data`。
- `PolylineCount()` / `PolygonCount()` 建议逐步标记为兼容别名，保留一段时间后只保留 `Count()`。
- 不再新增新的 `GetXxx` 风格公开 API。

涉及位置：
- [MultiPolyline2d.h](/D:/code/stablecore-geometry/include/sdk/MultiPolyline2d.h#L20)
- [MultiPolygon2d.h](/D:/code/stablecore-geometry/include/sdk/MultiPolygon2d.h#L20)
- [Polyline2d.cpp](/D:/code/stablecore-geometry/src/sdk/Polyline2d.cpp#L104)
- [Polygon2d.cpp](/D:/code/stablecore-geometry/src/sdk/Polygon2d.cpp#L109)

### 2. Member methods and free functions overlap too much around segment geometry

现状：
- `Segment2d` 及其派生类已经有成员：`Length()`、`Bounds()`、`PointAt()`、`PointAtLength()`
- 同时 `GeometryMetrics.h`、`GeometryProjection.h` 又提供同名或近平行自由函数：`Length(segment)`、`Bounds(segment)`、`PointAt(segment)`、`PointAtLength(segment)`

影响：
- 使用者需要反复判断“这是成员调用还是自由函数调用更标准”。
- 文档和示例容易分裂成两种风格。
- 未来若行为差异扩大，会造成维护风险。

建议：
- 明确规则：
  - 几何对象天然属性/直接参数化行为：优先成员方法，例如 `segment.Length()`、`segment.PointAt()`。
  - 跨类型算法、需要额外策略或多对象参与：放自由函数，例如 `ProjectPointToSegment`、`Intersect`、`Relate`。
- `GeometryMetrics.h` / `GeometryProjection.h` 中与成员完全同构的转发函数可逐步弱化为兼容层，不再继续扩张。
- 后续文档和测试示例优先展示成员用法。

涉及位置：
- [Segment2d.h](/D:/code/stablecore-geometry/include/sdk/Segment2d.h#L19)
- [GeometryMetrics.h](/D:/code/stablecore-geometry/include/sdk/GeometryMetrics.h#L22)
- [GeometryProjection.h](/D:/code/stablecore-geometry/include/sdk/GeometryProjection.h#L32)

### 3. `Polyline2d` / `Polygon2d` use PImpl, but sibling value types do not

现状：
- `Polyline2d` 和 `Polygon2d` 使用 `Impl` + `unique_ptr` 包装内部旧几何内核对象。
- `LineSegment2d`、`ArcSegment2d`、`Circle2d`、`Ellipse2d`、`Rectangle2d` 都是直接公开字段的轻值类型。
- `MultiPolyline2d` / `MultiPolygon2d` 也是直接持有 `std::vector`。

影响：
- 整个 SDK 值语义模型不统一：有的类型是透明聚合，有的类型是隐藏实现。
- `Polyline2d` / `Polygon2d` 每次复制都有额外分配和桥接成本，API 读者也很难直观看出这一点。
- 如果未来想做更彻底的 value-type 优化，这两个类型会成为特殊点。

建议：
- 中短期可以保留 PImpl，因为它确实隔离了内部 `geometry::Polyline2d` / `geometry::Polygon2d`。
- 但应在设计层明确：这是“兼容旧内核的过渡层”，不是 SDK 全面推荐模式。
- 长期建议二选一：
  - 要么继续彻底封装，把更多类型都收敛成隐藏实现；
  - 要么逐步去 PImpl 化，让 `Polyline2d` / `Polygon2d` 也成为透明值类型。
- 以当前库规模，我更倾向后者：SDK 对象普遍偏轻量，统一成透明值类型更自然。

涉及位置：
- [Polyline2d.h](/D:/code/stablecore-geometry/include/sdk/Polyline2d.h#L19)
- [Polygon2d.h](/D:/code/stablecore-geometry/include/sdk/Polygon2d.h#L14)
- [Polyline2d.cpp](/D:/code/stablecore-geometry/src/sdk/Polyline2d.cpp#L46)
- [Polygon2d.cpp](/D:/code/stablecore-geometry/src/sdk/Polygon2d.cpp#L52)

### 4. `Data()` exposes mutable internals too directly on container/index classes

现状：
- `MultiPolyline2d`、`MultiPolygon2d`、`GeometryBoxTree2d`、`GeometryKDTree2d`、`GeometrySegmentSearch2d` 都暴露 `Data()` 的可变引用。

影响：
- 外部可以绕过 `Add` / `Remove` / `Update` 直接改内部容器。
- 对普通聚合容器问题还不算大，但对索引类会破坏类不变量和重建时机，弱化封装边界。

建议：
- 对 `MultiPolyline2d` / `MultiPolygon2d` 可保留 `Data()` 可变引用，因其本质就是轻封装容器。
- 对索引类 `GeometryBoxTree2d` / `GeometryKDTree2d` / `GeometrySegmentSearch2d`，建议后续只保留 `const Data()` 或彻底去掉可变 `Data()`。
- 索引类的修改路径应统一走 `Add` / `Remove` / `Update`，否则很容易出现“数据变了但内部状态约束不再可信”的问题。

涉及位置：
- [GeometryBoxTree.h](/D:/code/stablecore-geometry/include/sdk/GeometryBoxTree.h#L50)
- [GeometryKDTree.h](/D:/code/stablecore-geometry/include/sdk/GeometryKDTree.h#L57)
- [GeometrySegmentSearch.h](/D:/code/stablecore-geometry/include/sdk/GeometrySegmentSearch.h#L68)
- [MultiPolyline2d.h](/D:/code/stablecore-geometry/include/sdk/MultiPolyline2d.h#L39)
- [MultiPolygon2d.h](/D:/code/stablecore-geometry/include/sdk/MultiPolygon2d.h#L40)

### 5. `Geometry*` module boundaries are usable but not yet cleanly layered

现状：
- `GeometryShapeOps.h`、`GeometryMetrics.h`、`GeometryProjection.h`、`GeometryRelation.h` 都各自合理，但边界有重叠。
- 例如 `Bounds` / `Length` 既在对象成员又在 `GeometryMetrics` / `GeometryShapeOps` 中出现；`PointAt` 既在成员又在 `GeometryProjection` 中出现。
- `Geometry.h` 作为总入口继续把几乎所有头全部 re-export。

影响：
- 模块命名看起来很多，但职责并没有完全分开，长期容易继续“哪里方便就往哪里加函数”。
- 读代码时需要记忆“这个函数为什么在这个头里”。

建议：
- 后续可按职责收敛为更稳定的四层：
  - types: `GeometryTypes.h`, `GeometryResults.h`, shape/value objects
  - intrinsic ops: 成员方法 + 少量 `GeometryShapeOps`
  - algorithms: `Intersection`, `Projection`, `Offset`, `Boolean`, `Topology`
  - indexing/services: `BoxTree`, `KDTree`, `SegmentSearch`
- 对完全重复转发的自由函数不再增加，逐步让模块边界更清楚。
- `Geometry.h` 可以保留，但建议文档中明确它是 convenience umbrella header，不是推荐精细依赖方式。

涉及位置：
- [Geometry.h](/D:/code/stablecore-geometry/include/sdk/Geometry.h#L3)
- [GeometryShapeOps.h](/D:/code/stablecore-geometry/include/sdk/GeometryShapeOps.h#L15)
- [GeometryMetrics.h](/D:/code/stablecore-geometry/include/sdk/GeometryMetrics.h#L11)
- [GeometryProjection.h](/D:/code/stablecore-geometry/include/sdk/GeometryProjection.h#L11)

### 6. Search / Boolean / Offset helper structs are heavily local and partly duplicated

现状：
- `GeometryBoolean.cpp` 和 `GeometryPathOps.cpp` 中都各自定义了 `DirectedEdge`、`RawSegment` 一类局部 struct。
- 这些 helper 当前放在 `.cpp` 内部命名空间中，封装性是好的。
- 但它们已经承载相近的“图边/切分段/候选 ring”语义。

影响：
- 算法继续增强时，search / boolean / offset 的预处理很可能继续分叉。
- 共享策略难以沉淀成一个真正的 topology/preprocess 子模块。

建议：
- 现在先不要急着公开这些类型。
- 但可以考虑在 `src/sdk` 内部逐步抽出 `GeometryPolygonGraphInternal.*` 或 `GeometryPolygonPreprocessInternal.*` 一类内部模块，统一维护：
  - split segment
  - duplicate undirected edge cleanup
  - face ring extraction
  - candidate scoring metadata
- 这样比继续在多个 `.cpp` 中复制“局部相似 struct + 相似流程”更利于后续补 Delphi/GGP parity。

涉及位置：
- [GeometryBoolean.cpp](/D:/code/stablecore-geometry/src/sdk/GeometryBoolean.cpp#L31)
- [GeometryPathOps.cpp](/D:/code/stablecore-geometry/src/sdk/GeometryPathOps.cpp#L24)

## Areas That Look Good

以下设计目前是合理的，不建议为了“纯洁性”而主动折腾：

- `Segment2d` 作为唯一多态抽象面，层级很浅，负担可控。
- `LineSegment2d` / `ArcSegment2d` 直接公开字段，适合作为轻量值对象。
- `Circle2d` / `Ellipse2d` / `Rectangle2d` 不强行塞进 `Segment2d` 继承体系，这个边界是对的。
- `GeometryResults.h` / `GeometryTypes.h` 中多数结果 struct 命名清晰，且 `IsValid()` / `AlmostEquals()` 很实用。
- `PolygonTopology2d` 独立成类而不是一堆散函数，方向是对的。

## Suggested Priority

如果后续要做 API/结构层面的整理，我建议优先顺序如下：

1. 统一公开命名风格，停止继续扩散重复别名。
2. 明确成员方法 vs 自由函数的边界，减少重复入口。
3. 收紧索引类 `Data()` 可变暴露。
4. 逐步抽内部 polygon preprocess / graph 模块，服务 search / boolean / offset 共用。
5. 视中长期计划决定 `Polyline2d` / `Polygon2d` 是否继续保留 PImpl。

## Bottom Line

当前类设计没有“必须马上推翻重来”的问题，主线仍然可维护。真正不合适的地方主要不是继承层级本身，而是：
- 命名风格尚未完全统一
- 成员接口与自由函数有重复
- 容器/索引类封装边界偏松
- polygon 内部图处理逻辑还未形成共享内部层

这些问题短期不会阻塞算法继续补齐，但如果准备把这个库长期做成稳定 SDK，建议尽早开始收口。
