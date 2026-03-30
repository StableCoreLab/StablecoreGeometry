# 3D 几何类型设计

## 1. 目的

本文定义 StableCore 三维库第一阶段的基础类型层。

## 2. 设计目标

第一阶段类型层的目标是：

- 提供稳定、透明、可复制的值类型
- 为服务层与参数对象层提供统一输入输出
- 保持与 2D 类型风格一致
- 为后续 polyhedron / BRep / mesh 提供共同基础

## 3. 第一阶段基础类型

建议第一阶段稳定以下类型：

- `Point3d`
- `Vector3d`
- `Direction3d`
- `Intervald`
- `Box3d`
- `Matrix3d`
- `Transform3d`
- `Plane`
- `Line3d`
- `Ray3d`
- `LineSegment3d`
- `Triangle3d`

## 4. 类型层原则

### 4.1 优先值类型

这些类型应尽量保持：

- 小而透明
- 直接可复制
- 不依赖堆分配
- 不使用 PImpl

### 4.2 只承载天然属性

类型层适合提供：

- 数据访问
- 简单构造
- 不变量检查
- 明确天然的属性读取

不适合承载：

- 多对象求交
- 多对象关系判定
- 大型恢复逻辑

### 4.3 命名风格与 2D 一致

保持短名风格，例如：

- `PointAt`
- `Bounds`
- `Length`
- `Area`

不回到 `GetXxx`。

## 5. 关键类型说明

### 5.1 `Point3d` 与 `Vector3d`

- `Point3d` 表示位置
- `Vector3d` 表示位移 / 方向量
- 两者语义不能混用

### 5.2 `Direction3d`

- 表示归一化方向
- 与一般 `Vector3d` 区分开，减少上层重复归一化负担

### 5.3 `Plane`

- 表示几何平面，而不是参数曲面对象
- 必须与 `PlaneSurface` 分离

### 5.4 `Transform3d`

- 负责统一表达三维变换
- 后续所有对象变换都应尽量汇聚到该类型

## 6. 与参数对象层的边界

类型层不等于参数对象层。

例如：

- `Line3d` 是基础值类型
- `LineCurve3d` 是参数曲线对象
- `Plane` 是基础值类型
- `PlaneSurface` 是参数曲面对象

这条边界必须保持清楚。

## 7. 当前固定结论

当前 3D 类型层应固定以下结论：

- 先稳定值类型，再扩展参数对象
- `Plane` / `PlaneSurface` 分离
- `Line3d` / `LineCurve3d` 分离
- 类型层继续保持轻量与透明
