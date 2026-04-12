# NavigationSystem 接口说明

本文档关注项目中的核心类、关键字段、主要方法和接口语义，适合开始改代码前阅读。

## 1. 接口总览

项目中的接口可以按下面几类理解：

- 数据模型接口：`Point2D`、`Node`、`Edge`、`Graph`
- 路径规划接口：`PathFinder`、`PathResult`
- 地图生成接口：`MapGenerator`
- 交通接口：`TrafficModel`、`TrafficSimulator`
- 空间索引接口：`BoundingBox`、`QuadTree`
- 文件接口：`GraphIO`
- GUI 交互接口：`ControlPanel`、`MapScene`、`MapView`

## 2. 数据模型接口

## 2.1 `Point2D`

定义位置：`src/core/graph/Node.h`

职责：

- 表示二维坐标
- 提供基础几何距离计算

关键字段：

- `double x`
- `double y`

关键方法：

- `distanceTo(const Point2D& other) const`

说明：

- 这是所有空间相关能力的最小基础单元
- 节点坐标、四叉树查询点、GUI 定位都依赖它

## 2.2 `Node`

定义位置：`src/core/graph/Node.h`

职责：

- 表示图中的一个节点

关键字段：

- `Id id_`
- `Point2D position_`

关键方法：

- `getId() const`
- `getPosition() const`
- `setPosition(const Point2D&)`

语义说明：

- 节点 ID 使用 `uint32_t`
- `INVALID_ID` 用于表达无效节点

## 2.3 `Edge`

定义位置：`src/core/graph/Edge.h`

职责：

- 表示两个节点之间的道路连接
- 同时承载交通容量和车辆计数

关键字段：

- `Id id_`
- `Node::Id source_`
- `Node::Id target_`
- `double length_`
- `double capacity_`
- `uint32_t carCount_`

关键方法：

- `getSource() const`
- `getTarget() const`
- `getLength() const`
- `getCapacity() const`
- `getCarCount() const`
- `setCapacity(double)`
- `setCarCount(uint32_t)`
- `incrementCarCount()`
- `decrementCarCount()`

语义说明：

- 结构上是边，仿真上也是交通载体
- 动态路径与热力图都依赖 `capacity_` 和 `carCount_`

## 2.4 `Graph`

定义位置：

- `src/core/graph/Graph.h`
- `src/core/graph/Graph.cpp`

职责：

- 维护整个路网图
- 管理节点、边和邻接关系

核心存储：

- `nodes_`
- `edges_`
- `adjacencyList_`

关键方法：

- `addNode(const Point2D&)`
- `addNodeWithId(Node::Id, const Point2D&)`
- `addEdge(Node::Id, Node::Id)`
- `addEdgeWithId(Edge::Id, Node::Id, Node::Id, double, double)`
- `getNode(Node::Id)`
- `getEdge(Edge::Id)`
- `getAdjacentEdges(Node::Id) const`
- `getNeighbors(Node::Id) const`
- `isConnected() const`
- `clear()`
- `reserve(size_t, size_t)`

重要语义：

- 图按无向图处理
- 一条边会同时挂到两个节点的邻接表上
- `addEdge()` 会自动按节点几何距离计算边长
- `addEdge()` 默认容量为 `10.0`
- `addNodeWithId()` 和 `addEdgeWithId()` 主要用于文件恢复

## 3. 路径规划接口

## 3.1 `PathResult`

定义位置：`src/core/algorithms/PathFinder.h`

职责：

- 封装路径查询结果

关键字段：

- `std::vector<Node::Id> pathNodes`
- `std::vector<Edge::Id> pathEdges`
- `double totalCost`
- `bool found`

语义说明：

- `found` 表示是否真正找到路径
- `pathNodes` 和 `pathEdges` 既用于业务，也用于界面高亮
- `totalCost` 的单位由具体算法决定

## 3.2 `PathFinder`

定义位置：`src/core/algorithms/PathFinder.h`

职责：

- 统一所有路径算法的调用方式

核心方法：

```cpp
virtual PathResult findPath(const Graph& graph, Node::Id start, Node::Id end) = 0;
```

设计意义：

- 为 GUI 和控制层屏蔽算法差异
- 为后续新增算法提供稳定扩展点

## 3.3 `DijkstraPathFinder`

定义位置：

- `src/core/algorithms/DijkstraPathFinder.h`
- `src/core/algorithms/DijkstraPathFinder.cpp`

职责：

- 按道路长度计算最短路径

关键点：

- 使用优先队列
- 使用前驱边映射重建路径
- 返回的 `totalCost` 是距离

适用场景：

- 静态路由
- 作为基线算法

## 3.4 `DynamicPathFinder`

定义位置：

- `src/core/algorithms/DynamicPathFinder.h`
- `src/core/algorithms/DynamicPathFinder.cpp`

职责：

- 基于当前交通状态计算最快路径

关键点：

- 算法框架仍是 Dijkstra
- 但边权来自 `TrafficModel::calculateTravelTime()`
- 返回的 `totalCost` 表示时间成本

适用场景：

- 动态路由
- 交通感知路径查询

## 4. 地图生成接口

## 4.1 `MapGenerator`

定义位置：`src/core/generation/MapGenerator.h`

职责：

- 统一地图生成器的抽象接口

核心方法：

```cpp
virtual void generate(Graph& graph, int numNodes, double width, double height) = 0;
```

设计意义：

- 后续可以很方便地增加不同生成策略

## 4.2 `GridPerturbationGenerator`

定义位置：

- `src/core/generation/GridPerturbationGenerator.h`
- `src/core/generation/GridPerturbationGenerator.cpp`

职责：

- 生成规则网格基础上的随机扰动路网

关键配置接口：

- `setPerturbationFactor(double)`
- `setAddDiagonals(bool)`
- `setDiagonalProbability(double)`

生成策略要点：

- 先生成近似规则网格
- 对节点位置施加扰动
- 候选边经 Kruskal 处理得到 MST
- 保留全部 MST 边和一部分非 MST 边
- 根据边长度计算容量

## 5. 交通接口

## 5.1 `TrafficModel`

定义位置：

- `src/core/traffic/TrafficModel.h`
- `src/core/traffic/TrafficModel.cpp`

职责：

- 提供无状态的交通计算公式

关键常量：

- `C`
- `K`
- `VISUAL_YELLOW_RATIO`
- `VISUAL_RED_RATIO`

关键方法：

- `calculateTravelTime(double length, double capacity, double currentCount)`
- `getCongestionRatio(double capacity, double currentCount)`
- `getCongestionStatus(double capacity, double currentCount)`

语义说明：

- 同时服务于动态路径规划和热力图可视化

## 5.2 `TrafficSimulator`

定义位置：

- `src/core/traffic/TrafficSimulator.h`
- `src/core/traffic/TrafficSimulator.cpp`

职责：

- 管理车辆生命周期并推进仿真

关键内部概念：

- `Car`：车辆，记录当前边、剩余时间、路线和路线位置

关键方法：

- `step()`
- `getChangedEdges() const`
- `getGraph()`
- `getActiveCarCount() const`
- `reset()`
- `setSpawnRate(int)`
- `setMaxCars(int)`
- `setTimeStep(double)`

语义说明：

- 每次 `step()` 会先推进旧车，再生成新车
- `changedEdges_` 用于 GUI 做增量刷新
- 当前构造时传入的是 `PathFinder*`，因此仿真器的路线策略可替换

重要实现事实：

- 当前 `MainWindow` 创建仿真器时传入的是 `DijkstraPathFinder`

## 6. 空间索引接口

## 6.1 `BoundingBox`

定义位置：

- `src/core/spatial/BoundingBox.h`
- `src/core/spatial/BoundingBox.cpp`

职责：

- 表示轴对齐包围盒

关键方法：

- `contains(const Point2D&) const`
- `intersects(const BoundingBox&) const`
- `getNorthWest() const`
- `getNorthEast() const`
- `getSouthWest() const`
- `getSouthEast() const`

## 6.2 `QuadTree`

定义位置：

- `src/core/spatial/QuadTree.h`
- `src/core/spatial/QuadTree.cpp`

职责：

- 存储节点的空间分布
- 支持高效空间查询

关键方法：

- `insert(Node::Id, const Point2D&)`
- `findKNearest(const Point2D&, size_t) const`
- `queryRange(const BoundingBox&) const`
- `clear()`

语义说明：

- 它存的是节点位置，不是边
- 业务层通常先查到节点，再回到 `Graph` 找相关边

## 7. 文件接口

## 7.1 `MapFileData`

定义位置：`src/core/io/GraphIO.h`

职责：

- 封装地图文件加载结果

关键字段：

- `Graph graph`
- `double width`
- `double height`

## 7.2 `GraphIO`

定义位置：

- `src/core/io/GraphIO.h`
- `src/core/io/GraphIO.cpp`

职责：

- 保存和加载地图文件

关键方法：

- `save(const std::string&, const Graph&, double, double)`
- `load(const std::string&, MapFileData&)`
- `getLastError()`

语义说明：

- 保存的是图结构和地图尺寸
- 不保存瞬时交通状态
- 加载时会通过 `addNodeWithId()` / `addEdgeWithId()` 恢复原始 ID

## 8. GUI 交互接口

## 8.1 `ControlPanel`

定义位置：

- `src/gui/widgets/ControlPanel.h`
- `src/gui/widgets/ControlPanel.cpp`

职责：

- 提供用户输入入口
- 发出查询请求信号

关键信号：

- `findNearestRequested(double x, double y, int k)`
- `computePathRequested(uint32_t startId, uint32_t endId, RoutingCriteria criteria)`
- `showTrafficNearRequested(double x, double y, double radius)`
- `clearHighlightsRequested()`

关键方法：

- `setNodeIdRange(int, int)`
- `setCoordinateRange(double, double, double, double)`
- `getRoutingCriteria() const`
- `showSpatialQueryResult(...)`
- `showPathResult(...)`
- `showTrafficResult(...)`

语义说明：

- 它本身不负责执行查询
- 只是输入和结果展示容器

## 8.2 `RoutingCriteria`

定义位置：`src/gui/widgets/ControlPanel.h`

职责：

- 描述路径查询策略

当前枚举值：

- `ShortestDistance`
- `FastestTime`

## 8.3 `MapScene`

定义位置：

- `src/gui/view/MapScene.h`
- `src/gui/view/MapScene.cpp`

职责：

- 把 `Graph` 转换成场景图元
- 管理路径、高亮、查询点和交通颜色
- 处理节点点击选路

关键方法：

- `loadMap(const Graph&)`
- `clearMap()`
- `setGraph(Graph*)`
- `setPathFinder(PathFinder*)`
- `updateEdgeCongestion(Edge::Id, int)`
- `clearPathSelection()`
- `highlightNodes(...)`
- `highlightEdges(...)`
- `clearSpatialHighlights()`
- `findPathById(Node::Id, Node::Id)`
- `showTrafficEdges(...)`
- `clearTrafficHighlights()`
- `updateTrafficHighlights(...)`

关键信号：

- `pathFound(const PathResult&)`
- `statusMessage(const QString&)`
- `errorOccurred(const QString&, const QString&)`

语义说明：

- 是图数据与可视化之间最关键的适配层
- 路径查询后的绘制也在这里完成

## 8.4 `MapView`

定义位置：

- `src/gui/view/MapView.h`
- `src/gui/view/MapView.cpp`

职责：

- 控制视图缩放、平移和聚焦

关键方法：

- `zoomToFit()`
- `updateLOD()`
- `focusOnPoint(double, double, double)`
- `focusOnBounds(const QRectF&, double)`

语义说明：

- `LOD_THRESHOLD` 以下会隐藏节点
- 这样做是为了大图缩小时保持可读性和渲染效率

## 8.5 `NodeItem`

定义位置：

- `src/gui/items/NodeItem.h`
- `src/gui/items/NodeItem.cpp`

职责：

- 表示可点击节点图元

关键点：

- 视觉半径和点击命中半径不同
- 高亮时显示更大
- `boundingRect()` 和 `shape()` 被专门重写以提高点击体验

## 8.6 `EdgeItem`

定义位置：

- `src/gui/items/EdgeItem.h`
- `src/gui/items/EdgeItem.cpp`

职责：

- 表示道路图元
- 根据拥堵状态更新颜色和线宽

关键方法：

- `updateStyle(int congestionStatus)`

## 8.7 `GenerateMapDialog`

定义位置：

- `src/gui/widgets/GenerateMapDialog.h`
- `src/gui/widgets/GenerateMapDialog.cpp`

职责：

- 收集生成地图的参数

关键方法：

- `getNodeCount() const`
- `getMapWidth() const`
- `getMapHeight() const`

## 8.8 `MainWindow`

定义位置：

- `src/gui/MainWindow.h`
- `src/gui/MainWindow.cpp`

职责：

- 作为项目的总编排类

管理对象：

- `Graph`
- `GridPerturbationGenerator`
- `TrafficSimulator`
- `DijkstraPathFinder`
- `DynamicPathFinder`
- `QuadTree`
- `MapScene`
- `MapView`
- `ControlPanel`

关键方法：

- `loadGraph(const Graph&)`
- `generateNewMap(int, double, double)`
- `updatePathfinder()`
- `rebuildQuadTree()`
- `startSimulation()`

关键槽函数：

- `onGenerateMap()`
- `onSaveMap()`
- `onLoadMap()`
- `onZoomToFit()`
- `onToggleSimulation()`
- `onSimulationStep()`
- `onFindNearestRequested(...)`
- `onComputePathRequested(...)`
- `onShowTrafficNearRequested(...)`
- `onClearHighlightsRequested()`

语义说明：

- 几乎所有跨模块协作都在这里发生
- 新功能接入时，通常都要从这里经过一次

## 9. 常见扩展落点

### 增加新的路径算法

- 实现新的 `PathFinder`
- 在 `MainWindow` 中创建并接入
- 在 `ControlPanel` 中增加策略入口

### 增加新的地图生成器

- 实现新的 `MapGenerator`
- 在 `MainWindow` 中切换生成器
- 如有参数差异，扩展 `GenerateMapDialog`

### 增加新的查询功能

- 在 `ControlPanel` 增加输入和信号
- 在 `MainWindow` 新增槽函数和业务逻辑
- 在 `MapScene` 增加新的可视化方法

### 修改地图文件格式

- 修改 `GraphIO::save()`
- 修改 `GraphIO::load()`
- 考虑版本兼容与错误提示

## 10. 一句话记忆法

如果需要快速记住项目的接口结构，可以记成下面这句话：

“`Graph` 管数据，`PathFinder` 管算法，`TrafficSimulator` 管动态状态，`GraphIO` 管持久化，`MainWindow` 管编排，`MapScene` 管展示。”
