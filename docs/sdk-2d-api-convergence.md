# 2D API Convergence Notes

## Purpose

这份文档只做一件事：为当前 2D SDK 设定一组收口规则，避免后续继续扩散命名、职责边界和内部层分叉问题。

它不是全面重构方案，也不是逐文件改造清单。

## 1. Public Naming

公开 API 统一采用短名风格：
- `PointAt`
- `Bounds`
- `Count`
- `Data`
- `Contains`
- `Length`
- `Area`

规则：
- 不再新增新的 `GetXxx` 风格公开 API。
- 当前代码中的 2D 旧 `GetXxx` 风格公开调用已经收口完成；后续若因兼容需要保留，也只应作为过渡层看待。
- `PolylineCount()` / `PolygonCount()` 这类重复语义接口已经收口到 `Count()`，后续不再恢复重复入口。

## 2. Members vs Free Functions

边界统一如下：
- 几何对象的天然属性或直接参数化行为：优先成员方法。
- 跨对象、跨类型或需要额外策略的算法：优先自由函数。

示例：
- 成员方法：`segment.Length()`、`segment.PointAt()`、`segment.Bounds()`
- 自由函数：`Intersect(...)`、`ProjectPointToSegment(...)`、`Relate(...)`、`Offset(...)`

规则：
- 不再继续增加与成员方法完全同构的自由函数入口。
- 文档和测试示例优先展示成员方法用法。

当前收口清单见：
- `docs/api-member-free-function-checklist.md`

当前执行结论：
- 段相关基础行为以成员方法为主入口：
  - `Length()`
  - `Bounds()`
  - `PointAt()`
  - `PointAtLength()`
- 下面这批自由函数应视为兼容层，而不是主推荐入口：
  - `Length(segment)`
  - `Bounds(segment)`
  - `PointAt(segment, ...)`
  - `PointAtLength(segment, ...)`
- 下面这批自由函数继续作为主公开算法入口保留：
  - `Distance(...)`
  - `ProjectPointTo...(...)`
  - `Intersect(...)`
  - `Relate(...)`
  - `Contains(...)`
  - `Offset(...)`
  - `Area(...)`
  - `Centroid(...)`

## 3. Type Exposure

值类型继续保持轻量、清晰、可复制。

规则：
- 简单值类型优先透明表示。
- 容器类型可以保留轻封装。
- 索引/服务类不要再继续放大可变内部暴露。

具体收口：
- `MultiPolyline2d` / `MultiPolygon2d` 可暂时保留可变 `Data()`。
- `GeometryBoxTree2d` / `GeometryKDTree2d` / `GeometrySegmentSearch2d` 长期应收敛到只读 `Data()` 或移除可变 `Data()`。

## 4. Internal Layer Direction

`search / boolean / offset` 不应继续各自维护越来越多的局部相似 helper。

规则：
- 后续 polygon 相关预处理优先往共享内部层收敛。
- 不急着公开类型，但要逐步形成内部模块。

建议内部方向：
- `GeometryPolygonPreprocessInternal.*`
- `GeometryPolygonGraphInternal.*`

优先承载：
- split segment
- duplicate undirected edge cleanup
- face ring extraction
- candidate metadata / scoring support

## 5. Immediate Policy

从现在开始执行的最小规则：
- 不新增 `GetXxx` 风格公开 API。
- 不新增与成员方法完全重复的自由函数。
- polygon 新能力优先考虑是否能放进共享内部层，而不是继续堆在单个 `.cpp` 里。
- 新文档和新测试示例统一按短名风格写。

## Bottom Line

2D SDK 当前不需要推翻重做，但必须停止继续扩散风格和边界债务。

先收口 2D，再扩展 3D，会明显降低后续设计和实现成本。

