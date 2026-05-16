# 交接说明

本文档只保留当前状态和当前缺口，不保留历史流水。

## 当前公开面

- 源码统一入口为 `Include/Geometry.h`
- 安装后的公开头文件根目录为 `Include/SCGeometry`
- 公共命名空间目标为 `Geometry`
- 工程身份保留 `SCGeometry`
- 公共头按 `Core`、`Geometry2d`、`Geometry3d`、`Brep`、`Support`、`Types`、`Serialize`、`Export` 分层

## 当前能力概况

- 2D 基础类型、路径、偏移、关系、布尔、搜索能力已稳定
- 3D 的 section、healing、body boolean、conversion 已形成当前发布面
- 公共 API 以语义化命名为主，默认安装面只保留当前发布面

## 当前主要缺口

- `Section` 的更一般非流形拼接与开放曲线面域裁剪
- `Healing` 的更一般多 shell 裁切
- `SearchPoly` 的 richer fake-edge explanation 与 ambiguous recovery
- `BodyBoolean` 的更一般 touching / overlap 子集

## 下一步建议

1. 优先推进 `Section`
2. 继续补强 `Healing`
3. 深化 `SearchPoly`
4. 扩展 `BodyBoolean`

## 备注

- 当前文档、代码和测试应以最新发布 API 为准
- 不再维护历史专题叙述和过程记录
