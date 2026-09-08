# 并行计算代码实现

本仓库记录并行计算课程中的代码实现，涵盖 MPI、OpenMP 等并行编程技术。

## 环境要求

- **编译器**：GCC/G++ 或 MSVC（Windows）
- **MPI 实现**：MPICH 或 OpenMPI
- **OpenMP**：编译器内置支持（GCC 4.2+，MSVC 2015+）
- **操作系统**：Windows / Linux / macOS

### Windows 环境配置
```bash
# 安装 MS-MPI（Windows）
# 下载地址：https://www.microsoft.com/en-us/download/details.aspx?id=100593

# 安装 MinGW-w64（包含 GCC 和 OpenMP 支持）
# 下载地址：https://www.mingw-w64.org/
```

---

## 目录结构与运行说明

### lab0/ - MPI 与 OpenMP 基础测试

本目录包含并行编程环境的测试程序，用于验证 MPI 和 OpenMP 是否正确安装。

| 程序 | 说明 | 运行命令 |
|------|------|----------|
| `mpi_test1` | MPI 基本通信测试 | `mpiexec -n 4 ./mpi_test1` |
| `openmp_test1` | OpenMP 并行区域测试 | `./openmp_test1` |
| `openmp_test2` | OpenMP 循环并行化测试 | `./openmp_test2` |
| `openmp_test3` | OpenMP 任务并行测试 | `./openmp_test3` |
| `openmp_test_0` | OpenMP 基础语法测试 | `./openmp_test_0` |

**编译方式**：
```bash
# MPI 程序编译
mpicc -o mpi_test1 mpi_test1.c

# OpenMP 程序编译（需开启 -fopenmp）
gcc -fopenmp -o openmp_test1 openmp_test1.c
```

---

### lab1/ - 并行正弦计算

使用并行方法计算正弦函数值，对比串行与并行性能。

| 程序 | 说明 | 运行命令 |
|------|------|----------|
| `calsin` | 正弦函数并行计算 | `./calsin` |

**编译方式**：
```bash
gcc -fopenmp -o calsin calsin.c -lm
```

---

### lab2/ - 稀疏矩阵运算

稀疏矩阵的存储与运算并行化实现。

| 程序 | 说明 | 运行命令 |
|------|------|----------|
| `sparse` | 稀疏矩阵乘法/转置 | `./sparse` |

**编译方式**：
```bash
gcc -fopenmp -o sparse sparse.c -lm
```

---

### lab3/ - MPI 稀疏矩阵并行

使用 MPI 实现分布式稀疏矩阵运算。

| 程序 | 说明 | 运行命令 |
|------|------|----------|
| `sparse_mpi` | MPI 并行稀疏矩阵运算 | `mpiexec -n 4 ./sparse_mpi` |

**编译方式**：
```bash
mpicc -o sparse_mpi sparse_mpi.c -lm
```

---

### lab4/ - 综合并行程序

包含多个并行编程练习。

| 程序 | 说明 | 运行命令 |
|------|------|----------|
| `hello` | MPI Hello World | `mpiexec -n 4 ./hello` |
| `test1` | 并行归约操作 | `mpiexec -n 4 ./test1` |
| `test2` | 并行点对点通信 | `mpiexec -n 4 ./test2` |
| `test3` | 并行集合通信 | `mpiexec -n 4 ./test3` |
| `check` | 环境检查程序 | `./check` |

**编译方式**：
```bash
mpicc -o hello hello.c
mpicc -o test1 test1.c
mpicc -o test2 test2.c
mpicc -o test3 test3.c
mpicc -o check check.c
```

---

## 通用运行指南

### Windows 系统
1. 打开命令提示符（CMD）或 PowerShell
2. 进入对应目录：`cd lab0`
3. 执行表格中的运行命令

### Linux / macOS 系统
```bash
# 给可执行文件添加执行权限
chmod +x ./程序名

# 运行 MPI 程序（需指定进程数）
mpiexec -n 4 ./程序名

# 运行 OpenMP 程序（可设置线程数）
export OMP_NUM_THREADS=4
./程序名
```

### 常见问题
- **找不到 `mpiexec` 命令**：确认 MPI 已安装并添加到系统 PATH
- **OpenMP 程序无法运行**：检查编译器是否支持 `-fopenmp` 标志
- **运行时缺少 DLL**：Windows 用户将 MS-MPI 的 `bin` 目录添加到 PATH

---

## 作者

- **作者**：李浪云
- **仓库地址**：https://gitee.com/lilangyun/parallel-computing

---

## 许可证

本项目仅供学习交流使用。