# Curve3d 与 Surface 设计

## 1. 目的

本文定义 StableCore 三维库中参数化曲线与参数化曲面层的公共设计。

## 2. 设计目标

这一层的目标是：

- 提供稳定的参数对象协议
- 为 projection、intersection、section、tessellation 提供统一基础
- 保持与值类型层、拓扑层边界清晰

## 3. 基本对象

建议至少规划：

- `Curve3d`
- `LineCurve3d`
- `Surface`
- `PlaneSurface`

后续再扩展到 `NurbsCurve3d`、`NurbsSurface` 等高阶对象。

## 4. 基础协议

### 4.1 `Curve3d`

建议最小协议包括：

- 参数范围访问
- `PointAt(t)`
- 导数求值
- `Bounds()`
- 基础投影与长度辅助接口

### 4.2 `Surface`

建议最小协议包括：

- `u / v` 参数范围访问
- `PointAt(u, v)`
- 偏导与法向求值
- `Bounds()`
- 基础投影辅助接口

## 5. 与值类型层的边界

必须保持：

- `Line3d` 是值类型
- `LineCurve3d` 是参数曲线对象
- `Plane` 是值类型
- `PlaneSurface` 是参数曲面对象

不能把这两层混成同一对象。

## 6. 当前固定结论

- `Curve3d` / `Surface` 是 3D 参数对象层的核心协议
- `PlaneSurface` 与 `Plane` 分离
- 参数对象层服务于上层算法，不承担拓扑职责
