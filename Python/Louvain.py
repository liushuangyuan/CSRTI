import community
import networkx as nx
import sys
import os

# 创建一个空的无向图
G = nx.Graph()

# 获取传入的参数
if len(sys.argv) < 2:
    print("No data name provided.")
    sys.exit(1)  # 如果没有提供数据名，则退出

# 使用传入的数据名构建文件路径
DataName = sys.argv[1]  # 从命令行参数获取数据集名称
DataPath = "data/" + DataName + "/" + DataName    # 构建数据路径
EdgePath = DataPath + ".txt"    # 边路径
BasicCommunityPath = DataPath + "Basic.txt" # 要写入的基本社区的路径

# 读取边信息并添加到图中
with open(EdgePath, "r") as edge_file:
    for line in edge_file:
        parts = line.strip().split()
        if len(parts) == 3:
            node1, node2, weight = map(int, parts)
            G.add_edge(node1, node2, weight=weight)  # 如果边有权重
        elif len(parts) == 2:
            node1, node2 = map(int, parts)
            G.add_edge(node1, node2)  # 如果边没有权重


# Blondel, V.D. et al. Fast unfolding of communities in large networks. J. Stat. Mech 10008, 1-12(2008)
# 使用Louvain算法进行社区发现
partition = community.best_partition(G)

# 输出社区信息到文件
with open(BasicCommunityPath, 'w') as f:
    for com in set(partition.values()):
        nodes_in_community = [node for node in partition.keys() if partition[node] == com]
        f.write("Community {}: ".format(com + 1) + ", ".join(str(node) for node in nodes_in_community) + "\n")
