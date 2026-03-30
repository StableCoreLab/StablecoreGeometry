# 3D Geometry 服务层设计

## 1. 目的

本文定义 StableCore 三维库中的公共服务层，重点覆盖 predicate、projection、intersection、measure、search 等能力。

## 2. 服务层职责

服务层负责：

- 面向多个对象的几何算法
- 使用统一容差与上下文
- 返回结构化结果
- 为上层 polyhedron / BRep / mesh 算法提供底座

服务层不负责：

- 保存复杂拓扑状态
- 承担对象生命周期管理
- 混入恢复逻辑与编辑逻辑

## 3. 推荐能力分组

### 3.1 Relation / Predicate

负责：

- 点对平面位置
- 向量平行 / 垂直
- 基础包含与侧别判断
- 简单对象关系判定

### 3.2 Projection

负责：

- 点投影到直线
- 点投影到平面
- 后续扩展到 curve / surface 投影

### 3.3 Intersection

负责：

- line-plane
- plane-plane
- 后续 curve-surface、surface-surface

### 3.4 Measure / Distance

负责：

- 长度
- 面积
- 体积
- 点到线 / 面距离
- 物体 bounds

### 3.5 Search

负责：

- AABB / box tree 风格查询
- 最近点 / 最近对象查询
- 后续 section / validation 的空间加速

## 4. 结果类型规则

复杂服务不能只返回 `bool`。

应优先返回结构体，例如：

- `LineProjection3d`
- `PlaneProjection3d`
- `LinePlaneIntersection3d`
- `PlanePlaneIntersection3d`

结构体中应尽量明确表达：

- 是否成功
- 是否平行 / 重合 / 退化
- 结果对象
- 参数位置
- 误差或状态标签

## 5. 容差与上下文

3D 服务层必须统一依赖：

- `GeometryTolerance3d`
- `GeometryContext3d`

规则：

- 不在局部算法中散落裸 `1e-6`、`1e-9`
- angle、distance、parameter 语义分开管理
- 上下文通过参数传入，不靠全局状态

## 6. 与类型层的边界

以下能力不建议重新做成成员函数：

- `ProjectPointToPlane(...)`
- `Intersect(line, plane, tolerance)`
- `Distance(point, plane)`
- `IsParallel(v0, v1, tolerance)`

这些都应保留在自由函数 / 服务层中。

## 7. 与上层模块的关系

服务层是以下模块的共同底座：

- `Curve3d` / `Surface`
- `PolyhedronBody`
- `TriangleMesh`
- `BrepBody`
- validation / healing
- section / tessellation

## 8. 当前固定结论

当前 3D 服务层设计应固定：

- 统一结果结构
- 统一容差入口
- 统一自由函数风格
- 不让上层对象各自复制一套基础几何服务
