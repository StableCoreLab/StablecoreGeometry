# 成员方法与自由函数检查清单

本文用于判断某个能力更适合做成员方法还是自由函数。

## 原则

- 面向对象天然属性优先做成员方法
- 需要同时处理多个对象、容差、上下文或结构化结果时，优先做自由函数
- 不要为了实现方便，就把临时 helper 扩散成公共面

## 适合做成员方法的能力

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

## 适合做自由函数的能力

- 距离
- 投影
- 相交
- 包含与关系查询
- boolean
- offset
- section / cut / split
- 拓扑构建
- 重建 / 修复 / 预处理

## 当前后续指引

- 保持 `SearchPoly` 和 `BodyBoolean` 走稳定发布 API 路线，放在源码 `Include/` 里
- 保持 `Section`、`Healing` 和 `BrepConversion` 作为内部实现空间，并维持清晰的 pass 边界
- 新增能力时继续沿用 `Options / Result / Issue` 风格
- 如果某个 helper 成了产品依赖，就先评估它是否应该升级为正式 API

## 检查问题

1. 这个 API 是否只依赖对象自身？
2. 是否需要容差、上下文或多个对象？
3. 是否返回结构化数据，而不是简单属性？
4. 是否只是为了实现方便而暴露 helper？
5. 是否符合当前源码 `Include/` 风格？

如果第 2 到第 5 项里有任意一项为“是”，通常更适合做自由函数。
