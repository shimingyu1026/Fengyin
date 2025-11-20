#include <iostream>
#include <print>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

// 1. 定义图的类型
// boost::adjacency_list<存储方式, 顶点列表, 边类型, 顶点属性, 边属性>
using Graph = boost::adjacency_list<
    boost::vecS,      // EdgeList：使用 std::vector 存储边
    boost::vecS,      // VertexList：使用 std::vector 存储顶点
    boost::undirectedS // Directed：无向图 (undirected)
>;

// 用于给顶点赋予名称的属性
enum { A, B, C, D, E, N }; // 定义顶点索引
const char* name[] = { "A", "B", "C", "D", "E" };

int main() {
    // 2. 创建图的实例
    Graph g(N); // N = 5 个顶点

    // 3. 添加边
    // add_edge(源顶点索引, 目标顶点索引, 图实例)
    boost::add_edge(A, B, g);
    boost::add_edge(A, D, g);
    boost::add_edge(B, C, g);
    boost::add_edge(D, E, g);
    boost::add_edge(C, E, g);
    
    std::println("--- 图结构 ---");
    // 打印图的结构 (使用 BGL 提供的工具函数)
    boost::print_graph(g, name);
    
    std::println("\n--- 遍历邻接点 ---");
    
    // 4. 遍历顶点和邻接点
    // 遍历顶点 A 的邻接点
    
    // out_edge_iterator 是一个迭代器对，指向顶点的所有出边
    Graph::out_edge_iterator ei, ei_end;
    
    // out_edges 函数获取顶点 A 的所有出边迭代器
    std::tie(ei, ei_end) = out_edges(A, g);

    std::cout << "顶点 A 的邻接点: ";
    for (; ei != ei_end; ++ei) {
        // target() 函数获取边的目标顶点
        std::cout << name[target(*ei, g)] << " ";
    }
    std::cout << std::endl;

    return 0;
}