"""
最短路径导航提示生成代码

使用OSMnx和NetworkX库，结合OpenStreetMap数据，计算清华大学内两个地点之间的最短路径，并生成导航提示。

作者: Owen
"""
import osmnx as ox
import networkx as nx
import matplotlib.pyplot as plt
import math


# 计算两个节点之间的方向角度
def get_bearing(node1, node2):
    if node1 is None or node2 is None:
        return None

    x1, y1 = G.nodes[node1]["x"], G.nodes[node1]["y"]
    x2, y2 = G.nodes[node2]["x"], G.nodes[node2]["y"]
    angle = math.degrees(math.atan2(y2 - y1, x2 - x1))
    return (angle + 360) % 360 if angle >= 0 else angle


# 根据方向角度计算转向提示
def get_turn_direction(previous_node, current_node, next_node):
    prev_bearing = get_bearing(previous_node, current_node)
    next_bearing = get_bearing(current_node, next_node)

    if prev_bearing is None or next_bearing is None:
        return "Heading to the first Node"

    angle_diff = (next_bearing - prev_bearing + 360) % 360

    if angle_diff < 45:
        return "Continue"
    elif 45 <= angle_diff < 135:
        return "Left"
    elif 135 <= angle_diff < 225:
        return "Turn around"
    elif 225 <= angle_diff < 315:
        return "Right"
    else:
        return "Continue"

'''
Optional
若不从本地导入清华大学图论模型，则使用以下代码通过OpenStreetMap在线获取清华大学附近的道路网络数据
location_point = (39.9992892, 116.3179999)
G1 = ox.graph_from_point(location_point, dist=1200, simplify=False)
G = ox.simplification.simplify_graph(G1)
'''

#从本地导入清华大学道路网络数据，为.graphml格式文件
G = ox.load_graphml('graph.graphml')

'''定义导航函数
其中origin_point与destination_point分别为起始点与目标点坐标，格式为(纬度,经度)
例如
origin_point = (40.0096846, 116.3214106)
destination_point = (40.0023142, 116.3244232)
'''
def navigation(origin_point, destination_point):

    # 获取起始点和目标点最邻近的道路节点
    origin_node = ox.nearest_nodes(G, origin_point[1], origin_point[0])
    destination_node = ox.nearest_nodes(G, destination_point[1], destination_point[0])

    # 使用Dijkstra算法计算最短路径
    route = nx.shortest_path(G, origin_node, destination_node, weight="length")

    # 打印导航提示
    previous_node = None
    for i in range(len(route) - 1):
        current_node = route[i]
        next_node = route[i + 1]
        turn_direction = get_turn_direction(previous_node, current_node, next_node)
        print(f"At node {current_node}: {turn_direction}")
        previous_node = current_node

    # 计算最短路径长度并绘制道路网络图
    distance = nx.shortest_path_length(G, origin_node, destination_node, weight="length")
    fig, ax = ox.plot_graph_route(G, route)
    print(f"Total distance: {distance}")
    plt.show()
    #返回最短路线长度
    return distance


#使用范例
origin_point_test = (40.0096846, 116.3214106)
destination_point_test = (40.0023142, 116.3244232)

navigation(origin_point_test, destination_point_test)