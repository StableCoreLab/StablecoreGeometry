# 文档同步跟踪

本文档只保留当前同步状态。

## 已对齐内容

- 公开入口：`Include/Geometry.h`
- 公开命名空间目标：`Geometry`
- 工程身份：`SCGeometry`
- 目录分层：`Core`、`Geometry2d`、`Geometry3d`、`Brep`、`Support`、`Types`、`Serialize`、`Export`
- 命名规则：见 `docs/final-naming-plan.md`
- 测试分层：见 `docs/test-capability-coverage.md`

## 当前同步要求

- 文档必须只描述当前发布 API
- 不再保留历史专题叙述
- 不再保留历史过程记录
- 能力差距可以保留，但必须是当前有效差距

## 当前待同步项

- `Section` 的当前 gap 说明
- `Healing` 的当前 gap 说明
- `SearchPoly` 的当前 gap 说明
- `BodyBoolean` 的当前 gap 说明

## 结论

- 代码、测试和文档现在都应围绕当前发布版结构书写
- 如果新能力已经稳定，就应尽快从 gap 侧移入 capability 侧
