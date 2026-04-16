# 下一步任务

你现在继续在 `stablecore-geometry` 上推进当前发布版能力收口。

## 工作约束

- 先满足 `docs/ai-execution-spec.md`
- 不修改已发布 API contract
- 不引入 breaking change
- 代码、测试、文档必须同步落地
- 只要存在歧义，就要保留 gap test

## 当前状态

- 公开入口已经统一到 `Include/Geometry.h`
- 公开命名空间目标是 `Geometry`
- 工程身份继续保留 `SCGeometry`
- 兼容头不再作为默认公开面

## 当前优先级

### P1

- 继续推进 `Section`
- 重点是更一般的非流形拼接、开放曲线与面积裁决

### P2

- 继续补强 `Healing`
- 重点是更一般的多 shell 裁决和复杂开放 shell 修复

### P3

- 深化 `SearchPoly`
- 重点是 richer fake-edge explanation 与 ambiguous recovery

### P4

- 扩展 `BodyBoolean`
- 重点是更一般 touching / overlap 子集与 shell-policy 边界

## 交付要求

每一轮至少包含：

- 一个 capability test
- 一个 edge-case test
- 一个 gap test
- 对应的文档同步

## 建议落点

- `Source/Core/Section.cpp`
- `Source/Brep/Healing.cpp`
- `Include/Core/SearchPoly.h`
- `Source/Core/SearchPoly.cpp`
- `Include/Brep/BodyBoolean.h`

## 提交前检查

- 文档是否只描述当前状态
- 测试是否与能力/差距划分一致
- 是否仍然存在兼容头口径
- 是否仍然出现历史专题叙述
