# SmartLab Training

智慧计算与优化实验室实习项目

目前Lab1、Lab2的各两种解法均完成

## Lab1 Graph Coloring

* 线性规划（使用Gurobi求解器）

  可以看出，使用Gurobi的求解速度并不理想

  |    算例     | 理论值  | 实际解  |   耗时    |
  | :-------: | :--: | :--: | :-----: |
  | DSJC125.1 |  5   |  5   |  0.666  |
  | DSJC125.5 |  17  |  19  | 226.025 |
  | DSJC250.1 |  8   |  9   | 16.043  |

* 启发式算法（禁忌搜索)

  更换为clang++编译以后，迭代速度约为40-80万次每秒，最高130万次每秒

  环境：MacBook Air (13-inch, Early 2015) 1.6 GHz Intel Core i5

  实现要点：

  * 贪心的初始化

  * 随机蓄水池抽样

  * 根据局部最优指定随机禁忌步长

  * 局部最优时加入随机扰动

  * 数据结构等细节优化...

    ​

  |    算例     | 理论值  | 实际解  |   耗时    |    迭代    |
  | :-------: | :--: | :--: | :-----: | :------: |
  | DSJC125.1 |  5   |  5   | 0.0016  |   2015   |
  | DSJC125.5 |  17  |  17  |  0.258  |  253959  |
  | DSJC125.9 |  44  |  44  |  0.032  |  14416   |
  | DSJC250.1 |  8   |  8   |  0.048  |  47322   |
  | DSJC250.5 |  28  |  28  | 10.295  | 3798622  |
  | DSJC250.9 |  72  |  72  | 0.9491  |  199075  |
  | DSJC500.1 |  12  |  12  | 155.834 | 62133360 |

<img src="http://oklhb00qa.bkt.clouddn.com/simple-min.jpg" style="zoom:50%;" />

## Lab2 P-Center

* 线性规划（使用Gurobi求解器）

  |  算例   | 理论值  | 实际解  |   耗时   |
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

  * 优先队列等数据结构优化

  * shell脚本自动运行算例并生成Markdown表格

    ​

  |   算例   | 理论值  | 实际解  |   耗时    |   迭代   |
  | :----: | :--: | :--: | :-----: | :----: |
  | pmed1  | 127  | 127  | 0.0001  |   2    |
  | pmed2  |  98  |  98  |  0.004  |   71   |
  | pmed3  |  93  |  93  |  0.004  |   75   |
  | pmed4  |  74  |  74  |  0.013  |  643   |
  | pmed5  |  48  |  48  |  0.008  |  146   |
  | pmed6  |  84  |  84  |  0.025  |  193   |
  | pmed7  |  64  |  64  |  0.010  |  107   |
  | pmed8  |  55  |  55  | 0.0297  |  261   |
  | pmed9  |  37  |  37  |  0.352  |  3084  |
  | pmed10 |  20  |  21  |  1.19   | 11192  |
  | pmed11 |  59  |  59  | 0.0029  |   8    |
  | pmed12 |  51  |  51  |  1.411  |  4019  |
  | pmed13 |  36  |  38  | 11.0796 | 51748  |
  | pmed14 |  26  |  29  |  76.71  | 349102 |
  | pmed15 |  18  |  21  |  7.822  | 30798  |
  | pmed16 |  47  |  47  |  0.008  |   23   |
  | pmed17 |  39  |  39  | 5.8134  | 17539  |
  | pmed18 |  28  |  32  | 0.7654  |  1951  |
  | pmed19 |  18  |  23  | 28.875  | 65482  |
  | pmed20 |  13  |  20  |  3.593  |  9519  |
  | pmed21 |  40  |  40  |  0.065  |   53   |
  | pmed22 |  38  |  39  |   0.9   |  1494  |
  | pmed23 |  22  |  26  | 18.287  | 21669  |
  | pmed24 |  15  |  21  |  0.727  |  849   |
  | pmed25 |  11  |  17  | 15.418  | 25485  |
  | pmed26 |  38  |  38  | 0.0227  |   13   |
  | pmed27 |  32  |  32  | 26.722  | 31988  |
  | pmed28 |  18  |  22  |  1.52   |  1496  |
  | pmed29 |  13  |  18  | 7.3237  |  6699  |
  | pmed30 |  9   |  15  |  4.202  |  3893  |
  | pmed31 |  30  |  30  | 0.0155  |   8    |
  | pmed32 |  29  |  30  | 0.6373  |  431   |
  | pmed33 |  15  |  20  | 5.4863  |  3440  |
  | pmed34 |  11  |  17  | 7.7033  |  5233  |
  | pmed35 |  30  |  30  |  1.366  |  531   |
  | pmed36 |  27  |  28  | 0.8656  |  521   |
  | pmed37 |  15  |  20  |  2.629  |  1115  |
  | pmed38 |  29  |  29  | 0.0465  |   11   |
  | pmed39 |  23  |  24  |  8.12   |  3719  |
  | pmed40 |  13  |  18  | 5.4575  |  1734  |

