import networkx as nx
import os
import sys

# 创建一个空的无向图
G = nx.Graph()

# 获取传入的参数
# 确保至少有一个参数传入（除了脚本名）
if len(sys.argv) < 2:
    print("No Data name provided.")
    sys.exit(1)  # 退出程序

DataName = sys.argv[1]  # 获取传入的算法名称

# 直接获取边信息文件路径
DataPath = "data/" + DataName + "/" + DataName
EdgePath = DataPath + ".txt"    # 边路径
BasicCommunityPath = DataPath + "Basic.txt" # 要写入的基本社区的路径

# 从文件中读取边信息并添加到图中
with open(EdgePath, "r") as f:
    for line in f:
        parts = line.strip().split()
        if len(parts) == 3:
            node1, node2, weight = map(int, parts)
            G.add_edge(node1, node2, weight=weight)
        elif len(parts) == 2:
            node1, node2 = map(int, parts)
            G.add_edge(node1, node2)
# Kernighan, B. W.; Lin, Shen (1970). “An efficient heuristic procedure for partitioning graphs.” Bell Systems Technical Journal 49: 291–307. Oxford University Press 2011.
# Kernighan-Lin 使用kernighan_lin_bisection函数进行社区发现
com = list(nx.algorithms.community.kernighan_lin_bisection(G))

# 将社区结果写入文件
with open(BasicCommunityPath, "w") as f:
    for i, community in enumerate(com):
        community_str = ", ".join(map(str, community))
        f.write(f"Community {i+1}: {community_str}\n")


