# Point2 与 Vector2 开发设计文档

## 1. 文档目的

本文档用于定义几何库中二维点与二维向量的开发方案。后续开发人员必须按本文档实现，不再以“建议性设计”理解。

本文档覆盖以下内容：

- 类型结构
- 职责边界
- 成员方法边界
- 运算符边界
- 容差规则
- 零向量与归一化规则

## 2. 总体设计

二维点与二维向量采用“共享存储 + 分离语义类型”的结构。

必须按以下结构实现：

```cpp
template <typename T>
struct Vec2Storage
{
    T x{};
    T y{};
};

template <typename T>
struct Point2 : Vec2Storage<T>
{
};

template <typename T>
struct Vector2 : Vec2Storage<T>
{
};
```

必须提供以下别名：

```cpp
using Point2d = Point2<double>;
using Point2i = Point2<int>;

using Vector2d = Vector2<double>;
using Vector2i = Vector2<int>;
```

## 3. 禁止采用的方案

禁止将点和向量定义为同一个底层类型的别名，例如：

```cpp
using Point2d = Vector2<double>;
using Vector2d = Vector2<double>;
```

禁止原因如下：

- 点和向量在几何语义上不同
- 如果它们是同一个类型，编译器无法约束不合理运算
- 会直接破坏接口边界

本库必须满足：

- 底层坐标存储可以共享
- 对外语义类型必须分离

## 4. 类型职责

### 4.1 `Vec2Storage<T>`

`Vec2Storage<T>` 只负责共享坐标存储。

必须承担的职责：

- 保存 `x`
- 保存 `y`
- 提供中性的基础数据能力

禁止承担的职责：

- 容差判断
- 几何关系判断
- 点与向量的语义运算
- 距离、投影、相交等算法

### 4.2 `Point2<T>`

`Point2<T>` 只表示二维位置。

必须遵循以下原则：

- 保持轻量
- 只暴露符合点语义的能力
- 不得被设计成通用二维数值向量

### 4.3 `Vector2<T>`

`Vector2<T>` 只表示二维位移或方向。

必须遵循以下原则：

- 承载纯向量基础能力
- 不承载容差判断
- 不承载复杂几何算法

## 5. `Vec2Storage<T>` 开发要求

`Vec2Storage<T>` 必须只实现共享存储层能力。

允许实现的内容：

- 默认构造
- `(x, y)` 构造
- 严格相等比较
- 简单设置函数

禁止实现的内容：

- `Length()`
- `LengthSquared()`
- `Normalized()`
- `IsZero()`
- `Dot()`
- `Cross()`
- 投影相关方法
- 距离相关方法
- 求交相关方法

原因：

- `Vec2Storage<T>` 是共享实现层，不允许携带几何语义

## 6. `Point2<T>` 开发要求

### 6.1 类型语义

`Point2<T>` 表示二维位置。

后续实现中必须始终保持这一语义，不得将其当作普通二维向量泛化使用。

### 6.2 必须支持的能力

`Point2<T>` 必须支持：

- 默认构造
- `(x, y)` 构造
- 严格相等比较
- `Point + Vector -> Point`
- `Point - Vector -> Point`
- `Point - Point -> Vector`

### 6.3 明确禁止的能力

`Point2<T>` 禁止支持：

- `Point + Point`
- `Point * scalar`
- `Point / scalar`
- 一元负号
- `Length()`
- `LengthSquared()`
- `Normalized()`

### 6.4 实现原则

后续实现中，凡是会把“位置”误当成“向量”的能力，一律不得加到 `Point2<T>` 上。

## 7. `Vector2<T>` 开发要求

### 7.1 类型语义

`Vector2<T>` 表示二维位移或方向。

### 7.2 必须支持的能力

`Vector2<T>` 必须支持：

- 默认构造
- `(x, y)` 构造
- 严格相等比较
- `Vector + Vector -> Vector`
- `Vector - Vector -> Vector`
- `Vector * scalar -> Vector`
- `Vector / scalar -> Vector`
- `scalar * Vector -> Vector`
- 一元负号
- `LengthSquared()`
- `Length()`

### 7.3 不得作为成员函数直接提供的能力

`Vector2<T>` 不得直接提供以下成员函数：

- `IsZero()`
- `Dot()`
- `Cross()`
- `Normalized()`

原因：

- `IsZero()` 属于带容差规则的判断
- `Dot()` 和 `Cross()` 属于双对象关系函数
- `Normalized()` 会引入零向量处理策略，必须统一管理

## 8. 容差规则

本库必须采用统一容差策略。

因此：

- `IsZero()` 不得放入 `Vec2Storage<T>`
- `IsZero()` 不得作为 `Vector2<T>` 成员函数

容差相关接口必须放在外部谓词或算法模块中统一管理。

统一形式如下：

```cpp
IsZero(value, eps);
IsZero(vector, eps);
```

执行要求：

- 不允许将容差逻辑散落到类型内部
- 不允许在点和向量类型中偷偷嵌入默认容差判断

## 9. `Dot()` 与 `Cross()` 开发要求

`Dot()` 与 `Cross()` 必须实现为自由函数，不得实现为成员函数。

统一形式如下：

```cpp
Dot(lhs, rhs);
Cross(lhs, rhs);
```

执行要求：

- 不得写成 `vector.Dot(other)`
- 不得写成 `vector.Cross(other)`

原因：

- 这两个函数描述的是两个对象之间的关系
- 它们必须与后续 `Distance(a, b)`、`Intersect(a, b)` 保持统一风格

## 10. 零向量规则

### 10.1 定义规则

对于浮点向量：

- 零向量必须按容差判断

对于整数向量：

- 零向量按严格零判断

### 10.2 执行原则

不得静默吞掉零向量问题。

特别是在归一化场景中，禁止采用以下行为：

- 零向量归一化后直接返回零向量

因为这会掩盖退化输入，并把错误传播到后续算法中。

## 11. 归一化开发要求

### 11.1 第一版必须提供的接口

第一版应提供：

```cpp
TryNormalize(vector, normalized, eps)
```

接口语义必须是：

- 当向量接近零向量时，返回失败
- 当归一化成功时，输出单位向量

### 11.2 第一版暂不提供的接口

第一版不要求提供成员版：

```cpp
Normalized()
```

原因：

- 该接口会强制引入零向量处理策略
- 当前阶段统一采用可失败接口更稳妥

## 12. 运算符开发要求

### 12.1 `Point2<T>` 必须支持的运算

- `Point + Vector -> Point`
- `Point - Vector -> Point`
- `Point - Point -> Vector`
- `==`
- `!=`

### 12.2 `Point2<T>` 禁止支持的运算

- `Point + Point`
- `Point * scalar`
- `Point / scalar`
- 一元负号

### 12.3 `Vector2<T>` 必须支持的运算

- `Vector + Vector -> Vector`
- `Vector - Vector -> Vector`
- `Vector * scalar -> Vector`
- `Vector / scalar -> Vector`
- `scalar * Vector -> Vector`
- 一元负号
- `==`
- `!=`

### 12.4 暂不支持的运算

以下运算当前阶段不做：

- `Vector + Point`

原因：

- 与 `Point + Vector` 语义重复
- 当前阶段保留单一方向接口，降低接口膨胀

## 13. 比较规则

`operator==` 与 `operator!=` 只表示严格分量比较。

不得将容差比较实现为运算符重载。

近似比较必须走独立接口，例如：

```cpp
IsEqual(lhs, rhs, eps);
```

执行要求：

- `==` 不得包含容差
- `!=` 不得包含容差
- 容差比较必须显式调用

## 14. 第一版实现清单

后续开发必须至少完成以下内容。

`Vec2Storage<T>`：

- 默认构造
- `(x, y)` 构造
- 严格相等比较

`Point2<T>`：

- `Point + Vector`
- `Point - Vector`
- `Point - Point`

`Vector2<T>`：

- 向量加减
- 标量乘除
- 一元负号
- `LengthSquared()`
- `Length()`

外部自由函数或算法层：

- `IsZero(value, eps)`
- `IsZero(vector, eps)`
- `Dot(lhs, rhs)`
- `Cross(lhs, rhs)`
- `TryNormalize(vector, normalized, eps)`

## 15. 开发验收标准

后续实现完成后，必须满足以下验收标准：

- `Point2<T>` 与 `Vector2<T>` 是不同类型
- 点类型无法进行点加点等不合理运算
- 向量类型具备基本线性运算能力
- 所有容差判断都在外部统一接口中完成
- `Dot()` 与 `Cross()` 为自由函数
- 归一化采用可失败接口
- 严格比较与容差比较完全分离

## 16. 最终执行结论

后续开发必须按以下原则执行：

- 共享底层坐标存储
- 分离点与向量语义类型
- 点保持轻量
- 向量承载基础线性能力
- 容差判断外置
- 双对象关系函数外置
- 归一化优先采用可失败接口

本文档即为 `Point2<T>` 与 `Vector2<T>` 的实现依据。
