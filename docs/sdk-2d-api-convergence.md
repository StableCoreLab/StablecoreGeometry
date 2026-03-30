# 2D API 收敛说明

## 目的

这份文档只做一件事：为当前 2D SDK 设定一组收口规则，避免后续继续扩散命名、职责边界和内部层分叉问题。

## 当前固定规则

### 1. 公开命名保持短名风格

优先：

- `Count()`
- `Bounds()`
- `PointAt()`
- `Length()`
- `Area()`

避免：

- `GetCount()`
- `GetBounds()`
- `GetPointAt()`
- `GetLength()`
- `GetArea()`

### 2. 不再回退到 `GetXxx`

历史 `GetXxx` 风格已经清理，不应重新引入新的公开 API。

### 3. 类型层只承担对象语义

类型层负责：

- 保存数据
- 维护不变量
- 提供天然属性读取

不负责：

- 多对象关系算法
- 复杂重建算法
- 大型恢复流程

### 4. 算法层承担复杂行为

以下能力继续放在自由函数 / 服务层：

- predicate / relation
- distance / projection
- intersection
- boolean
- offset
- topology build
- polygon rebuild

### 5. 统一容差入口

所有几何判定必须走统一容差接口，避免局部散落魔法数。

## 当前已落地的命名结论

当前统一形式包括：

- `MultiPolyline2d::Count()`
- `MultiPolygon2d::Count()`
- `PolygonTopology2d::Count()`

这些形式应视为后续统一基线。

## 新增 API 时的检查清单

新增 API 前，应检查：

1. 是否符合短名风格？
2. 是否会重新引入 `GetXxx`？
3. 是否把复杂算法错误塞回类型层？
4. 是否会让 2D 与未来 3D 的风格分叉？
5. 是否需要同步设计文档？

## 与其他文档的关系

本文是 2D API 收敛规则的总入口，具体边界还应参考：

- `docs/api-member-free-function-checklist.md`
- `docs/sdk-type-design-review.md`
- `docs/session-handoff.md`
