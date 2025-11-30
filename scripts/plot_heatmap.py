#!/usr/bin/env python3
"""
绘制热力图脚本
输入 CSV 文件，输出两张热力图（sub_graphs_rate 和 incomplete_graphs_rate）
"""

import argparse
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os
from pathlib import Path


def plot_heatmap(csv_path: str, output_path: str = None):
    """
    读取 CSV 文件并绘制热力图
    
    Args:
        csv_path: 输入的 CSV 文件路径
        output_path: 输出目录路径，如果为 None 则使用 CSV 文件所在目录
    """
    # 读取 CSV 文件
    df = pd.read_csv(csv_path)
    
    # 检查必要的列是否存在
    required_columns = ['node_error_rate', 'edge_error_rate', 
                       'sub_graphs_rate', 'incomplete_graphs_rate']
    for col in required_columns:
        if col not in df.columns:
            raise ValueError(f"CSV 文件缺少必要的列: {col}")
    
    # 确定输出路径
    if output_path is None:
        output_dir = Path(csv_path).parent
    else:
        output_dir = Path(output_path)
        output_dir.mkdir(parents=True, exist_ok=True)
    
    # 获取 CSV 文件名（不含扩展名）用于生成输出文件名
    csv_name = Path(csv_path).stem
    
    # 创建透视表用于热力图
    # 横轴：edge_error_rate，纵轴：node_error_rate
    pivot_sub = df.pivot(index='node_error_rate', 
                        columns='edge_error_rate', 
                        values='sub_graphs_rate')
    
    pivot_incomplete = df.pivot(index='node_error_rate', 
                               columns='edge_error_rate', 
                               values='incomplete_graphs_rate')
    
    # 设置绘图风格
    sns.set_style("whitegrid")
    plt.rcParams['figure.dpi'] = 100
    plt.rcParams['savefig.dpi'] = 300
    
    # 绘制 sub_graphs_rate 热力图
    plt.figure(figsize=(12, 8))
    sns.heatmap(pivot_sub, 
                annot=True,  # 不显示数值，因为数据点较多
                fmt='.1f',
                cmap='YlOrRd',
                cbar_kws={'label': 'Sub Graphs Rate (%)'},
                linewidths=0.5,
                linecolor='gray')
    plt.title('Sub Graphs Rate Heatmap', fontsize=16, fontweight='bold')
    plt.xlabel('Edge Error Rate (%)', fontsize=12)
    plt.ylabel('Node Error Rate (%)', fontsize=12)
    plt.tight_layout()
    
    # 保存 sub_graphs_rate 热力图
    output_file_sub = output_dir / f"{csv_name}_sub_graphs_rate.png"
    plt.savefig(output_file_sub, bbox_inches='tight')
    print(f"已保存: {output_file_sub}")
    plt.close()
    
    # 绘制 incomplete_graphs_rate 热力图
    plt.figure(figsize=(12, 8))
    sns.heatmap(pivot_incomplete, 
                annot=True,  # 不显示数值，因为数据点较多
                fmt='.1f',
                cmap='YlGnBu',
                cbar_kws={'label': 'Incomplete Graphs Rate (%)'},
                linewidths=0.5,
                linecolor='gray')
    plt.title('Incomplete Graphs Rate Heatmap', fontsize=16, fontweight='bold')
    plt.xlabel('Edge Error Rate (%)', fontsize=12)
    plt.ylabel('Node Error Rate (%)', fontsize=12)
    plt.tight_layout()
    
    # 保存 incomplete_graphs_rate 热力图
    output_file_incomplete = output_dir / f"{csv_name}_incomplete_graphs_rate.png"
    plt.savefig(output_file_incomplete, bbox_inches='tight')
    print(f"已保存: {output_file_incomplete}")
    plt.close()
    
    print(f"\n热力图绘制完成！")
    print(f"输出目录: {output_dir}")


def main():
    parser = argparse.ArgumentParser(
        description='从 CSV 文件绘制热力图',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
示例用法:
  python plot_heatmap.py input.csv
  python plot_heatmap.py input.csv -o output_dir/
  python plot_heatmap.py build/linux/x86_64/release/out/heatmap_data_k4.csv -o plots/
        """
    )
    
    parser.add_argument('csv_path', 
                       type=str, 
                       help='输入的 CSV 文件路径')
    
    parser.add_argument('-o', '--output', 
                       type=str, 
                       default=None,
                       dest='output_path',
                       help='输出目录路径（默认为 CSV 文件所在目录）')
    
    args = parser.parse_args()
    
    # 检查输入文件是否存在
    if not os.path.exists(args.csv_path):
        print(f"错误: 文件不存在: {args.csv_path}")
        return 1
    
    try:
        plot_heatmap(args.csv_path, args.output_path)
        return 0
    except Exception as e:
        print(f"错误: {e}")
        return 1


if __name__ == '__main__':
    exit(main())

