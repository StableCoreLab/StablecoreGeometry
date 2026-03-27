# Member vs Free Function API Checklist

## Purpose

这份清单用于把 2D SDK 中“成员方法 vs 自由函数”的收口建议落成可执行列表。

目标：
- 明确哪些自由函数应长期保留
- 明确哪些自由函数只应视为兼容层
- 避免后续继续增加与成员方法完全同构的重复入口

## Boundary Rule

统一规则：
- 几何对象自身的天然属性或直接参数化行为：优先成员方法
- 跨对象、跨类型、带额外策略或返回算法结果 struct 的行为：优先自由函数

按这个规则：
- `Length()` / `Bounds()` / `PointAt()` / `PointAtLength()` 属于成员方法主入口
- `Intersect(...)` / `ProjectPointTo...(...)` / `Relate(...)` / `Offset(...)` / `Distance(...)` / `Contains(...)` / `Area(...)` / `Centroid(...)` 属于自由函数主入口

## Keep As Primary Free Functions

这些自由函数应长期保留，并作为公开算法入口继续存在。

### GeometryMetrics.h

- `DistanceSquared(const Point2d&, const Point2d&)`
- `Distance(const Point2d&, const Point2d&)`
- `DistanceSquared(const Point2d&, const LineSegment2d&)`
- `DistanceSquared(const Point2d&, const ArcSegment2d&)`
- `DistanceSquared(const Point2d&, const Segment2d&)`
- `Distance(const Point2d&, const LineSegment2d&)`
- `Distance(const Point2d&, const ArcSegment2d&)`
- `Distance(const Point2d&, const Segment2d&)`
- `Contains(const Box2d&, const Point2d&, eps)`
- `Intersects(const Box2d&, const Box2d&, eps)`

说明：
- 这些都不是对象自身“天然属性”
- 它们是跨对象判断、度量或过滤行为，保留为自由函数是合理的

### GeometryProjection.h

- `ProjectPointToLineSegment(...)`
- `ProjectPointToArcSegment(...)`
- `ProjectPointToSegment(...)`
- `ProjectPointToLine(...)`
- `ProjectPointToPlane(...)`
- `ParameterAtLength(...)`
- `TangentAt(...)`
- `NormalAt(...)`

说明：
- 这些要么是算法行为，要么返回投影/参数/导矢等派生结果
- 继续保留为自由函数合理

### GeometryIntersection.h

- 所有 `Intersect(...)`

### GeometryTopology.h

- `Contains(const Polygon2d&, const Polygon2d&, eps)`
- `Relate(const Polygon2d&, const Polygon2d&, eps)`
- `BuildPolygonTopology(...)`

### GeometryOffset.h

- 所有 `Offset(...)`

### GeometryShapeOps.h

- `Length(const Polyline2d&)`
- `Bounds(const Polyline2d&)`
- `Bounds(const Polygon2d&)`
- `Area(const Polygon2d&)`
- `Centroid(const Polygon2d&)`

说明：
- `Polyline2d` / `Polygon2d` 当前仍维持“值对象 + shape ops 自由函数”的使用模式
- 这批入口不建议现在强行成员化

## Keep As Compatibility Layer

这些自由函数与现有成员方法完全同构，建议视为兼容层。

### GeometryMetrics.h

- `Length(const LineSegment2d&)`
- `Length(const ArcSegment2d&)`
- `Length(const Segment2d&)`
- `Bounds(const LineSegment2d&)`
- `Bounds(const ArcSegment2d&)`
- `Bounds(const Segment2d&)`

对应成员：
- `segment.Length()`
- `segment.Bounds()`

### GeometryProjection.h

- `PointAt(const LineSegment2d&, double)`
- `PointAt(const ArcSegment2d&, double)`
- `PointAt(const Segment2d&, double)`
- `PointAtLength(const LineSegment2d&, double, bool)`
- `PointAtLength(const ArcSegment2d&, double, bool)`
- `PointAtLength(const Segment2d&, double, bool)`

对应成员：
- `segment.PointAt(parameter)`
- `segment.PointAtLength(length, clampToSegment)`

建议处理方式：
- 继续保留实现，避免立刻做破坏性 API 变更
- 在文档与测试中不再作为主推荐入口
- 若未来需要标记弃用，可优先从这批函数开始

## Policy For New APIs

从现在开始：
- 不新增与成员方法完全同构的自由函数
- 新增段相关基础行为时，先判断是否应直接落到 `Segment2d` 派生类成员
- 新增跨类型算法时，再考虑放进 `GeometryMetrics` / `GeometryProjection` / `GeometryIntersection` / `GeometryTopology` / `GeometryOffset`

## Recommended Usage In Docs And Tests

推荐写法：
- `segment.Length()`
- `segment.Bounds()`
- `segment.PointAt(t)`
- `segment.PointAtLength(length)`
- `ProjectPointToSegment(point, segment)`
- `Intersect(first, second)`
- `Offset(polygon, distance)`

不再推荐作为示例主写法：
- `Length(segment)`
- `Bounds(segment)`
- `PointAt(segment, t)`
- `PointAtLength(segment, length)`
