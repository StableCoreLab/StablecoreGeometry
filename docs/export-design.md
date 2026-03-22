# DLL 导出层设计文档

## 1. 文档目的

本文档用于定义几何库未来生成 `dll + import lib + headers` 时的导出规则。

本文档解决的是“怎么导出”，不是“几何语义怎么定义”。

重点覆盖以下内容：

- `GEOMETRY_API` 宏如何组织
- 哪些类型允许作为导出层的一部分
- 哪些模板类型只允许停留在内部实现层
- 是否需要显式实例化
- 当前内部容器、多态和 STL 结构是否允许直接跨 DLL 边界暴露

本文档是 `sdk-design.md` 的工程落地版约束。

## 1.1 文档关系

- `library-design.md` 负责总设计和模块边界
- `sdk-design.md` 负责发布形态和 ABI 总原则
- 本文档负责把 ABI 总原则具体化为可落地的导出规则

执行要求：

- 若未来开始做 DLL 构建，本文件中的规则必须先于正式导出代码落地
- 若某个类型的对外暴露方式与本文档冲突，应优先修改设计，不得先硬导出再补文档

## 2. 当前执行结论

第一版 DLL 导出层统一遵守以下原则：

- 导出层主路径只支持 `double`
- 导出层不直接承诺任意模板实例
- 导出层不直接暴露当前 `Polyline2<T>` 和 `Polygon2<T>` 的 STL/多态内部组织
- 导出层优先导出稳定值类型和自由函数算法入口
- 复杂对象若需要跨 DLL 边界长期稳定存在，应使用稳定壳类 + pImpl
- 是否使用 pImpl 的判断标准是 ABI 风险和状态复杂度，而不是算法复杂度

## 3. `GEOMETRY_API` 宏规则

未来应统一引入导出宏头文件，例如：

```cpp
// include/export/GeometryExport.h
#if defined(_WIN32)
#  if defined(GEOMETRY_BUILD_DLL)
#    define GEOMETRY_API __declspec(dllexport)
#  elif defined(GEOMETRY_USE_DLL)
#    define GEOMETRY_API __declspec(dllimport)
#  else
#    define GEOMETRY_API
#  endif
#else
#  define GEOMETRY_API
#endif
```

执行要求：

- 不允许在各个头文件里散落重复的导出宏定义
- 所有未来需要正式导出的类型或函数，必须统一依赖同一个导出宏头文件
- `GEOMETRY_BUILD_DLL` 用于库自身编译
- `GEOMETRY_USE_DLL` 用于外部程序使用 DLL

## 4. 当前建议可直接导出的对象

第一版建议优先导出简单、稳定、值语义明确的对象。

推荐对象：

- `Point2d`
- `Vector2d`
- `Box2d`

条件是：

- 它们的布局保持简单
- 不依赖 STL 容器
- 不依赖虚继承层
- 不把复杂所有权语义暴露给外部
- 不依赖复杂生命周期管理
- 成员字段本身就可以作为 ABI 承诺

这类对象更适合作为 SDK 头文件中的基础数据类型。

对于这类对象，推荐直接把模板类的 `double` 实例收敛为正式 SDK 类型，例如：

- `using Point2d = Point2<double>;`
- `using Vector2d = Vector2<double>;`
- `using Box2d = Box2<double>;`

执行约束：

- 不再额外声明一套与模板实例平行存在的导出专用类型
- 不再长期保留类似 `GeoPoint2d`、`GeoVector2d`、`GeoBox2d` 这样的双轨命名
- 对外 SDK 直接以 `Point2d`、`Vector2d`、`Box2d` 作为正式类型名

同类规则也适用于简单结果结构体，例如：

- `SegmentProjection2d`
- 未来可能新增的交点结果、关系判断结果、测量结果

这些结果对象若仅由简单值类型组成，也应直接导出，不必额外包成 pImpl 类。

## 5. 当前不建议直接导出的对象

以下对象当前不建议直接作为稳定 ABI 类型导出：

- `Segment2<T>`
- `LineSegment2<T>`
- `ArcSegment2<T>`
- `Polyline2<T>`
- `Polygon2<T>`

原因各不相同，但都足够重要。

### 5.1 `Segment2<T>` 不建议直接导出

原因：

- 它是抽象基类
- 依赖虚函数分发
- 当前并未围绕 DLL 边界设计生命周期和分配释放策略

结论：

- `Segment2<T>` 当前只作为内部抽象层存在
- 不把它直接作为 DLL 稳定 ABI 主体

### 5.2 `LineSegment2<T>` 与 `ArcSegment2<T>` 暂不直接承诺为 ABI 类型

虽然它们比 `Segment2<T>` 更稳定，但当前仍不建议直接把模板形态拿去做 DLL 承诺。

执行结论：

- 若未来需要直接暴露线段和圆弧对象，应只考虑 `LineSegment2d` 与 `ArcSegment2d`
- 即便如此，也应先完成导出宏、显式实例化和头文件组织收敛
- 在正式 SDK 阶段前，不把它们现在的模板头文件直接视为最终发布接口

### 5.3 `Polyline2<T>` 与 `Polygon2<T>` 当前禁止直接作为 ABI 类型导出

这是当前最需要明确写死的规则。

原因：

- `Polyline2<T>` 当前内部使用 `std::vector<std::shared_ptr<Segment2<T>>>`
- `Polygon2<T>` 直接依赖 `Polyline2<T>`
- 这意味着它们的当前实现同时暴露了：
  - STL 容器
  - 智能指针
  - 抽象基类层次
  - 动态分发和所有权细节

这些都不适合作为第一版 DLL 稳定 ABI 承诺。

执行结论：

- 当前 `Polyline2d` 不能直接以现有内部结构对外导出
- 当前 `Polygon2d` 不能直接以现有内部结构对外导出
- 若未来外部程序需要创建或消费连续线/多边形，应导出稳定壳类，并把内部表示收进 pImpl

## 5.4 pImpl 适用标准

以下对象应优先考虑使用 pImpl，而不是直接导出内部实现：

- 内部包含 STL 容器、智能指针、多态对象图或缓存结构
- 未来实现细节很可能调整，但不希望破坏 ABI
- 生命周期管理复杂，构造、拷贝、移动、析构策略需要统一收口
- 对象不变量较重，不能简单依赖公开字段维持正确性

当前第一批候选对象：

- `Polyline2d`
- `Polygon2d`

这类对象的导出推荐形式是：

- 对外头文件暴露稳定类声明
- 私有成员只保留 `Impl` 指针
- 容器、多态层次、缓存和辅助结构全部留在实现文件或库体内部

## 6. 模板导出规则

当前模板导出必须遵循以下硬规则：

- 模板可以存在于内部语义层
- 模板不能默认等于“自动属于 DLL 对外 API”
- 只有被明确选中的具体实例，才允许进入导出候选范围

第一版执行结论：

- 只考虑 `double` 实例进入 DLL 收敛层
- 不考虑把 `int` 版本做成正式导出承诺
- 不考虑让调用方自行依赖内部模板实现来完成 DLL SDK 接入
- 对于简单值类型，`double` 实例本身就是 SDK 正式类型落点，而不是再包一层独立导出类型

## 7. 显式实例化规则

若未来决定让某些 `double` 版本类型真正进入编译产物导出层，则必须使用显式实例化策略收敛实现边界。

建议形式：

```cpp
extern template class Point2<double>;
extern template class Vector2<double>;
extern template class Box2<double>;
```

并在对应 `.cpp` 中做：

```cpp
template class Point2<double>;
template class Vector2<double>;
template class Box2<double>;
```

执行要求：

- 只有准备正式导出的具体实例，才做显式实例化规划
- 若某个模板类型仍需要完全头文件化，则不得同时把它描述成“已收进 DLL 的稳定 ABI 类型”
- 对 `Point2<double>`、`Vector2<double>`、`Box2<double>` 这类简单实例，若被选为正式 SDK 类型，应避免再维护第二套平行实现类

## 8. 头文件组织规则

未来对外 SDK 头文件应与内部实现头文件分层。

建议结构：

```text
include/
  export/
    GeometryExport.h
  sdk/
    Geometry.h
    GeometryApi.h
    GeometryTypes.h
    GeometryAlgorithms.h
    Polyline2d.h
    Polygon2d.h
  types/
  algorithm/
```

执行结论：

- `types/` 下当前头文件优先视为内部设计与实现头文件
- 正式 SDK 阶段应新增 `sdk/` 入口头文件，而不是让外部调用方直接无边界包含整个内部目录
- `sdk/` 头文件只能暴露被正式承诺的 API 面

若后续补齐序列化与测试支持，建议结构进一步收敛为：

```text
include/
  export/
    GeometryExport.h
  sdk/
    Geometry.h
    GeometryApi.h
    GeometryTypes.h
    GeometryAlgorithms.h
  serialize/
  types/
  algorithm/
tests/
  support/
```

执行约束：

- `serialize/` 负责稳定语义数据的序列化和反序列化
- `tests/support/` 负责测试断言、样例构造和回放辅助
- 不把持久化格式、测试辅助和调试拼装代码散落到各个核心类型实现中

## 9. 算法导出优先级

相较于直接导出复杂类型，第一版更适合优先导出受控算法入口。

例如未来可优先考虑：

- 点到线段距离
- 点到边段投影
- 线段求交
- 包围盒关系判断

这些 API 的优势是：

- 输入输出更容易收敛到简单值类型
- 比直接暴露复杂对象图更稳定
- 更符合 DLL 场景
- 算法复杂度可以留在实现内部，不需要因此把数据对象改成 pImpl

执行建议：

- 第一版 SDK 优先考虑“值类型 + 算法函数”
- `Polyline2d` 和 `Polygon2d` 这类复杂对象延后进入 ABI 层
- 不因为算法复杂就引入 pImpl 算法类
- 只有当算法必须绑定复杂内部状态、缓存或上下文时，才考虑对象化并使用 pImpl

## 10. 对当前代码的落地影响

从现在开始，代码和文档应按以下方式理解：

- `Point2<T>`、`Vector2<T>`、`Box2<T>` 仍可继续模板化实现
- `Segment2<T>` 体系继续作为内部抽象和算法支撑层
- `Polyline2<T>`、`Polygon2<T>` 当前只服务内部设计与实现，不等于未来 DLL 稳定对象
- 算法主路径继续优先围绕 `double` 收敛

这意味着当前开发方向不用推翻，但以后不能再默认认为：

- “头文件里有什么，SDK 就直接给什么”

进一步执行要求：

- 不要求所有几何类都内建成员形式的序列化/反序列化方法
- 简单值类型允许采用“外置但近旁”的轻量序列化组织
- 复杂对象的序列化必须外置到独立模块
- 所有稳定对外类型都应支持 `IsValid()`
- 浮点几何对象和结果类型应逐步补齐近似比较能力
- 稳定类型和算法结果应具备稳定调试输出能力，服务日志和测试

## 11. 当前实现约束

当前仓库的 SDK 入口层先采用更窄的发布面，优先导出值类型和自由函数，而不是直接把复杂类型图作为第一版 ABI。

这意味着：

- `Point2d`、`Vector2d`、`Box2d` 可以作为当前 SDK 的稳定起点
- `SegmentProjection2d` 这类结果结构体适合作为算法返回值
- `Polyline2d`、`Polygon2d` 未来若进入 SDK，应走 pImpl 形式
- 后续如果要把线段、多段线、多边形纳入导出层，仍然要先补齐更正式的 ABI 收敛方案

进一步收敛要求：

- `Point2d`、`Vector2d`、`Box2d` 应直接成为 SDK 正式类型名
- 不再长期维持“模板类型 + Geo 导出类型”双轨并存
- 序列化应统一走独立序列化入口，不在多个位置维护重复编码逻辑
- 每个可序列化类型都应具备 round-trip 测试
- 测试支持层应统一提供近似断言和样例构造能力

## 12. 当前执行清单

后续若进入 DLL 导出实现阶段，必须至少完成以下事项：

1. 新增统一导出宏头文件
2. 明确 `sdk/` 对外头文件入口
3. 选择第一批正式导出的 `double` 类型
4. 明确哪些模板做显式实例化
5. 明确哪些复杂对象继续留在内部，不直接跨 DLL 边界暴露

## 13. 最终执行结论

后续开发必须按以下原则执行：

- `GEOMETRY_API` 统一通过单一宏头文件定义
- 第一版 DLL 导出层只围绕 `double` 收敛
- 当前模板类型体系不直接等于 DLL 稳定接口
- `Polyline2d` 与 `Polygon2d` 当前禁止以现有内部结构直接对外导出
- 复杂几何对象的 ABI 包装层必须后续单独设计，不得直接把当前 STL/多态实现暴露出去

本文档即为后续 DLL 导出层的工程设计依据。
