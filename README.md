# SmartLab Training

智慧计算与优化实验室实习项目

目前Lab1、Lab2的各两种解法均完成

## Lab1 Graph Coloring

* 线性规划（使用Gurobi求解器）

  |    算例     | 理论值  | 最优解  |    耗时    |
  | :-------: | :--: | :--: | :------: |
  | DSJC125.1 |  5   |  5   |  0.666   |
  | DSJC250.1 |  8   |  9   |  16.043  |
  | DSJC250.1 |  8   |  8   | Time Out |


* 启发式算法（禁忌搜索)

  更换为clang++以后，迭代速度约为100万次每秒。

  环境：MacBook Air (13-inch, Early 2015) 1.6 GHz Intel Core i5

  实现要点：

  * 贪心的初始化

  * 随机蓄水池抽样

  * 根据局部最优指定随机禁忌步长

  * 局部最优时加入随机扰动

  * 数据结构等细节优化...

    ​

  |    算例     | 理论值  | 最优解  |    耗时    |    迭代     |
  | :-------: | :--: | :--: | :------: | :-------: |
  | DSJC125.1 |  5   |  5   |  0.006   |   7924    |
  | DSJC250.1 |  8   |  8   |  0.177   |  132435   |
  | DSJC250.5 |  28  |  28  |  27.023  | 10925138  |
  | DSJC250.5 |  28  |  29  |  1.430   |  609658   |
  | DSJC250.9 |  72  |  72  | 2103.552 | 349298771 |
  | DSJC250.9 |  72  |  73  |  28.303  |  5471920  |
  | DSJC500.1 |  12  |  12  | 155.834  | 62133360  |
  | DSJC500.1 |  12  |  13  |  0.469   |  211447   |

## Lab2 P-Center

* 线性规划（使用Gurobi求解器）

  |  算例   | 理论值  | 最优解  |   耗时   |
  | :---: | :--: | :--: | :----: |
  | pmed1 | 127  | 127  | 92.812 |
  | pmed2 |  98  |  98  | 98.125 |
  | pmed3 |  93  |  93  | 61.646 |

* 启发式算法（禁忌搜索）

  实现要点：

  * 贪心初始化

  * 随机蓄水池抽样

  * 局部最优的随机禁忌步长

  * 局部最优的扰动

  * 对于D表和F表“模拟增删”，节约用时

  * 优先队列等数据结构优化...

    ​

  |   算例   | 理论值  | 最优解  |   耗时   |  迭代   |
  | :----: | :--: | :--: | :----: | :---: |
  | pmed1  | 127  | 127  | 0.001  |  12   |
  | pmed2  |  98  |  98  | 0.004  |  71   |
  | pmed3  |  93  |  93  | 0.027  |  304  |
  | pmed4  |  74  |  74  | 0.115  | 2641  |
  | pmed10 |  20  |  21  | 15.167 | 80351 |
  | pmed12 |  51  |  51  | 18.019 | 57393 |