# stablecore-geometry

一套面向工程计算的 C++ 几何库。

## 目标

- 提供可复用的二维、三维几何能力
- 保持公开 API 稳定、可测试、可发布
- 以 section、healing、boolean、conversion、validation、search 为核心工作流

## 当前范围

- 二维值类型与二维几何服务
- 三维值类型、曲线、曲面、BRep、polyhedron 支持
- section、healing、boolean、projection、relation、measurement、validation、mesh conversion 服务
- 文本序列化辅助与搜索索引辅助

## 公开入口

外部使用优先包含统一入口：

```cpp
#include "Geometry.h"
```

`Geometry.h` 是稳定的 umbrella 入口，会聚合当前主要公开头文件。

需要更细的包含粒度时，可直接使用下列模块：

- `Core`：umbrella 类型与跨模块操作
- `Geometry2d`：二维基本图元与路径工具
- `Geometry3d`：曲线、曲面与三维求值辅助
- `Brep`：BRep、polyhedron、mesh、healing、拓扑服务

更底层的公开面分得更细：

- `Support/Epsilon.h`：共享容差常量
- `Support/Geometry2d`：二维标量、谓词、归一化辅助
- `Types/Geometry2d`：二维值类型
- `Types/Geometry3d`：三维值类型与线性代数辅助
- `Types/Detail`：实现细节辅助类型
- `Serialize/GeometryText.h`：文本序列化辅助
- `Export/GeometryExport.h`：导入导出标注

需要跨 DLL 的公开值类型和结果结构，统一从这些公共头导出，并使用 `GEOMETRY_API`：

- `Core/GeometryTypes.h`
- 其他公开 API 头中直接导出的值类型

umbrella 中可直接获得的常用别名包括：

- `Point2d`, `Vector2d`, `Box2d`
- `Point3d`, `Vector3d`, `Direction3d`, `Box3d`, `Intervald`
- `Line3d`, `Ray3d`, `LineSegment3d`, `Triangle3d`
- `Transform3d`, `Matrix3d`, `Plane`

如果只需要某一小块能力，建议直接包含对应头文件，而不是总是走 umbrella。

发布安装会刻意收口：

- 兼容头和细节头不进入默认安装面
- 默认安装只包含当前支持的发布 API

## 设计原则

- 面向工程，而不是面向演示
- 数据结构清晰
- 容差语义显式
- 核心算法测试先行

更多说明见：

- [`docs/library-design.md`](docs/library-design.md)
- [`docs/final-naming-plan.md`](docs/final-naming-plan.md)
- [`docs/test-capability-coverage.md`](docs/test-capability-coverage.md)

## 构建

默认本地构建环境为 Visual Studio 2022 x64。

常用方式：

1. 通过脚本生成解决方案：

```bat
GenerateGeometryVS2022.bat
```

该脚本会在仓库根目录外生成到 `../Build/GeometryVs2022`，并打开 `SCGeometry.sln`。

2. 或直接使用 CMake preset：

```bat
cmake --preset vs2022-x64
cmake --build --preset vs2022-x64-build --config Release
cmake --build --preset vs2022-x64-build --config RelWithDebInfo
```

仓库还包含 `.vscode/settings.json`，可供 VS Code CMake Tools 直接复用同一套 preset。

## 安装

安装布局按配置与平台分开：

### 发布布局

- `bin/Release/x64`：DLL 和可执行文件
- `bin/RelWithDebInfo/x64`：带调试信息的 DLL 和可执行文件
- `lib/Release/x64`：导入库和静态库
- `lib/RelWithDebInfo/x64`：带调试信息的导入库和静态库
- `Include`：公开头文件
- `lib/cmake/SCGeometry`：CMake 包文件

安装后，下游项目可这样使用：

```cmake
find_package(SCGeometry CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE SCGeometry::Geometry)
```

本地安装前缀示例：

```bat
cmake --install build --config Release --prefix D:\\code\\stablecore\\Geometry\\install
cmake --install build --config RelWithDebInfo --prefix D:\\code\\stablecore\\Geometry\\install
```
