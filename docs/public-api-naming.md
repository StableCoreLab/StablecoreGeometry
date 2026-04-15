# 公共 API 命名规范

本文定义 Geometry 对外稳定接口的命名规则。

当前符号审计和收敛状态请见 [`docs/public-api-naming-audit.md`](public-api-naming-audit.md)。

## 目标

- 保持 `include/sdk` 作为唯一面向产品的公开表面。
- 优先使用简短、表达意图明确的名字，避免臃肿的 helper 风格命名。
- 在可行时保留旧名字作为别名，以维持兼容性。

## 规范入口

- `sdk/Geometry.h` 是给使用者的 umbrella 入口。
- `sdk/GeometryApi.h` 是更宽的 SDK 聚合头文件。
- `sdk/GeometryTypes.h` 放置共享的 SDK 结果类型和辅助类型。

## 命名规则

- 类型和结果优先使用名词命名。
- 操作优先使用动词或动词短语命名。
- 布尔行为优先使用 `Is*`、`Has*`、`Can*`、`Contains`、`Intersects`、`Project`、`Convert`、`Heal`、`Section`、`Validate` 这类形式。
- 内在查询优先使用 `PointAt`、`Bounds`、`Length`、`Area`、`VertexCount`、`HoleCount`、`FaceCount`。
- 不要把临时 helper 名字暴露到公开头文件里。
- 除非新名字已经稳定，且旧名字可以继续作为兼容别名，否则不要直接改公共符号名。

## 成员方法与自由函数

- 对于属于单一对象的属性，优先使用成员方法。
- 对于需要组合多个对象，或者需要共享容差 / 上下文参数的算法，优先使用自由函数。
- 如果为了兼容保留自由函数，实现应放在 `src` 中，避免在多个翻译单元里重复实现。

## 目录布局

- `include/common` 放共享 epsilon / 底层常量。
- `include/types` 放基础几何类型。
- `include/sdk` 放稳定的公开 SDK 头文件。
- `src/common` 放共享实现辅助代码。
- `src/sdk` 放公开 SDK 行为的实现。

## 兼容规则

- 如果必须改名，先保留旧名字作为薄包装或别名，给下游迁移时间。
- 不要在引入新规范名的同一轮里直接删除旧公共名字，除非旧名字从未发布过。
