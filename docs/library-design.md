# 库设计说明

## 1. 定位

StableCore Geometry 是一套面向工程计算的 C++ 几何库，目标是提供稳定、可测试、可发布的几何能力，而不是演示型样例库。

## 2. 当前结构

仓库当前采用以下主结构：

```text
Include/
Source/
UnitTests/
docs/
```

公开头按语义分层：

- `Core`
- `Geometry2d`
- `Geometry3d`
- `Brep`
- `Support`
- `Types`
- `Serialize`
- `Export`

## 3. 公开入口

统一入口为：

```cpp
#include "Geometry.h"
```

如果只需要某一小块能力，可以直接包含对应模块头，而不是总是依赖 umbrella。

## 4. 分层原则

### 4.1 类型层

类型层负责表达几何数据和少量天然属性，例如：

- `Point2d`
- `Vector2d`
- `Box2d`
- `Point3d`
- `Vector3d`
- `Box3d`
- `Line3d`
- `Plane`
- `Triangle3d`

### 4.2 服务层

服务层负责复杂算法、重建、修复和诊断，例如：

- `Section`
- `Healing`
- `BodyBoolean`
- `SearchPoly`
- `BrepConversion`

### 4.3 容差层

容差和数值辅助集中放在：

- `Support/Epsilon.h`
- `Support/Geometry2d/*`
- `Core/GeometryTypes.h` 中的相关导出类型

## 5. 模块职责

### 5.1 Core

负责 umbrella 类型、跨模块操作和最常用的公开入口。

### 5.2 Geometry2d

负责二维基本图元、路径工具和二维几何服务。

### 5.3 Geometry3d

负责曲线、曲面、三维求值和相关几何服务。

### 5.4 Brep

负责 BRep、polyhedron、mesh、healing、拓扑和布尔服务。

### 5.5 Support / Types

负责基础辅助类型、值类型和实现细节类型。

## 6. 发布约束

- 公开 API 只保留当前发布面需要的名字
- 默认安装面只保留当前发布面
- 不再把实现细节暴露到默认发布头集合中
- 需要跨 DLL 的公开数据结构继续通过 `GEOMETRY_API` 导出

## 7. 当前结论

- `Geometry.h` 是唯一推荐 umbrella 入口
- `Core` 是最常用的发布入口
- `Geometry2d` 与 `Geometry3d` 负责几何能力本身
- `Brep` 负责更高阶实体服务
- `Types` 与 `Support` 负责底层基础设施
