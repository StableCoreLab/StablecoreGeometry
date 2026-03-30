# 成员方法与自由函数 API 清单

## 目的

这份清单用于把当前 2D SDK 中“成员方法 vs 自由函数”的收口建议固定成可执行规则。

## 总规则

- 对象天然属性优先做成员方法
- 涉及两个及以上对象的算法优先做自由函数
- 需要容差、上下文或结果结构体的算法优先做自由函数
- 不要为追求面向对象形式，把复杂算法硬塞进类型层

## 适合放成员方法的能力

以下能力通常只依赖对象自身状态，适合保留为成员方法：

- `Count()`
- `Bounds()`
- `Length()`
- `Area()`
- `IsClosed()`
- `IsEmpty()`
- `VertexCount()`
- `VertexAt(...)`
- `StartPoint()`
- `EndPoint()`
- `OuterRing()`
- `HoleCount()`
- `HoleAt(...)`

判断标准：

- 不需要外部对象
- 不需要容差上下文
- 返回的是对象天然属性或直接视图

## 适合放自由函数的能力

以下能力原则上应放自由函数：

- 距离
- 投影
- 相交
- 包含 / 关系判定
- boolean
- offset
- cut / split
- topology build
- rebuild / repair / preprocess

原因：

- 这类能力通常涉及多个对象
- 常需要容差或上下文
- 往往需要结构化结果，不适合伪装成简单成员访问

## 当前 2D SDK 约束

当前应继续坚持：

- 不重新扩散 `GetXxx` 风格 API
- `Count()` 统一替代历史 `GetCount()`
- 不把 `DistanceTo`、`ProjectTo` 这类复杂行为重新塞回类型成员方法
- 不在 polygon / polyline / segment 类型里继续增加大算法成员函数

## 评审检查项

新增 API 时，逐项检查：

1. 这个能力是否只依赖对象自身状态？
2. 是否涉及容差、上下文或多个对象？
3. 是否可能需要结构化返回值？
4. 是否会让类型层承担算法职责？
5. 是否会让命名风格重新分叉？

如果第 2 到第 5 项里任一答案为“是”，优先做自由函数。

## 当前文档联动

这份清单应与以下文档保持一致：

- `docs/sdk-2d-api-convergence.md`
- `docs/sdk-type-design-review.md`
- `docs/segment-design.md`
- `docs/polyline-design.md`
- `docs/polygon-design.md`
