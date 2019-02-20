//
//  main.cpp
//  TempPathV2.0
//
//  Created by 游宇杰 on 2019/1/24.
//  Copyright © 2019 游宇杰. All rights reserved.
//

#include <sys/time.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <deque>
#include <queue>
using namespace std;
//存储边
struct Edge{
    int source;
    int dest;
    int startTime;
    int interval;
    Edge(int u,int v,int t,int w)
    {
        this->source=u;
        this->dest=v;
        this->startTime=t;
        this->interval=w;
    }
};
vector<Edge> edges;
vector<vector<int>> nodes;
//设置边是否访问过
//bool* visited;

int V,E;
const int inf=1e9;
//查询
vector<int> sources;
vector<int> dests;
//图描述边的形式 source dest startTime interval,下标从1开始
void readGraph(const char* filePath)
{
    FILE* file=fopen(filePath,"r");
    
    int x;
    x=fscanf(file,"%d %d",&V,&E);
    
    nodes.resize(V);
    //edges.resize(E);
    //visited=new bool[E];
    int u,v,t,w;
    for(int i=0;i<E;i++)
    {
        x=fscanf(file,"%d %d %d %d",&u,&v,&t,&w);
        edges.push_back(Edge(u,v,t,w));
    }
    fclose(file);
}

bool  earliest_comp(const Edge &a,const Edge &b)
{
    
    return a.startTime+a.interval>b.startTime+b.interval;
}

bool fastest_comp(const Edge &a,const Edge &b)
{
    return a.interval<=b.interval;
}
/*
 *   将边按照抵达时间进行逆序排列后
 */
void process_earliest_graph()
{
    sort(edges.begin(),edges.end(),earliest_comp);
    for(int i=0;i<edges.size();i++)
    {
        //将保存边的下标
        nodes[edges[i].dest].push_back(i);
    }
}

/*
 *    按照飞行时间进行升序排列
 */
void process_fastest_graph()
{
    sort(edges.begin(),edges.end(),fastest_comp);
    for(int i=0;i<edges.size();i++)
    {
        //将保存边的下标
        nodes[edges[i].dest].push_back(i);
    }
}

/**
 最早抵达路径的主要思想是:
 1.使用双端队列进行存储点，这里的点是按照三元组(v,t,w)
 v表示目的地,t表示出发时间,w表示飞行时间
 2.每次都是将时间最早抵达的点存储在队列的前面,即t+w最小放在队列首部
 3.每次去队列首部进行深度遍历,这样保证了每一个点到达目的地的时间为最早遍历
 然后其他的边不在需要遍历了
 4.如果深度遍历没有合适的边后,然后进行回溯再次进行深度遍历
 主要还是利用有限制条件的深度搜索,可以理解为一种动态规划
 时间复杂度为O(2E)
 */
void earliest(int source,int dest)
{
    //source,startTime,source to dest time
    deque<pair<int,pair<int,int>>> queue;
    //保存最早各点到dest
    vector<int> earliest(V,inf);
    
    vector<bool> visited(E,false);
    //将目的点添加到双端队列
    queue.push_back(make_pair(dest,make_pair(inf,0)));
    while(!queue.empty())
    {
        pair<int,pair<int,int>> node=queue.front();
        queue.pop_front();
        for(int i=0;i<nodes[node.first].size();i++)
        {
            int edgeSeq=nodes[node.first][i];
            if(!visited[edgeSeq])
            {
                int destTime=edges[edgeSeq].startTime+edges[edgeSeq].interval;
                
                if(destTime<=node.second.first)
                {
                    visited[edgeSeq]=true;
                    int time=node.second.second;
                    if(node.first==dest)
                    {
                        earliest[edges[edgeSeq].source]=destTime;
                        time=destTime;
                    }
                    else if(earliest[edges[edgeSeq].source]>node.second.second)
                        earliest[edges[edgeSeq].source]=node.second.second;
                    queue.push_front(make_pair(edges[edgeSeq].source,make_pair(edges[edgeSeq].startTime,time)));
                }
            }
        }
    }
    
    cout<<source<<"->"<<dest<<"的最早抵达时间是:"<<earliest[source]<<endl;
    
}
/**
 void shortest(int source,int dest)
 {
 //source,startTime,dist
 deque<pair<int,pair<int,int>>> queue;
 //保存最早各点到dest
 vector<int> shortest(V,inf);
 queue.push_back(make_pair(dest,make_pair(inf,0)));
 while(!queue.empty())
 {
 pair<int,pair<int,int>> node=queue.front();
 queue.pop_front();
 for(int i=0;i<nodes[node.first].size()&&!visited[nodes[node.first][i]];i++)
 {
 int edgeSeq=nodes[node.first][i];
 if(!visited[edgeSeq])
 {
 int destTime=edges[edgeSeq].startTime+edges[edgeSeq].interval;
 
 if(destTime<node.second.first)
 {
 visited[edgeSeq]=true;
 int dist=node.second.second+1;
 if(shortest[edges[edgeSeq].source]>dist)
 shortest[edges[edgeSeq].source]=dist;
 queue.push_front(make_pair(edges[edgeSeq].source,make_pair(edges[edgeSeq].startTime,dist)));
 }
 }
 }
 }
 cout<<source<<"->"<<dest<<"的最短路径是:"<<shortest[source]<<endl;
 }*/

/**
 最短路径的主要思想是:
 1.利用广度优先搜有进行搜索,选取目的点作为起点
 2.搜索目的点的孩子节点，然后使用孩子节点进行BFS
 3.如果dist即最短距离小于已知的距离,更新最短距离,标记这条边已读即可
 时间复杂度为O(2E)
 */
void shortest(int source,int dest)
{
    //source,startTime,dist
    queue<pair<int,pair<int,int>>> queue;
    //保存最早各点到dest
    vector<int> shortest(V,inf);
    vector<bool> visited(E,false);
    queue.push(make_pair(dest,make_pair(inf,0)));
    while(!queue.empty())
    {
        pair<int,pair<int,int>> node=queue.front();
        queue.pop();
        for(int i=0;i<nodes[node.first].size()&&!visited[nodes[node.first][i]];i++)
        {
            int edgeSeq=nodes[node.first][i];
            if(!visited[edgeSeq])
            {
                int destTime=edges[edgeSeq].startTime+edges[edgeSeq].interval;
                
                if(destTime<node.second.first)
                {
                    visited[edgeSeq]=true;
                    int dist=node.second.second+1;
                    if(shortest[edges[edgeSeq].source]>dist)
                        shortest[edges[edgeSeq].source]=dist;
                    queue.push(make_pair(edges[edgeSeq].source,make_pair(edges[edgeSeq].startTime,dist)));
                }
            }
        }
    }
    cout<<source<<"->"<<dest<<"的最短路径是:"<<shortest[source]<<endl;
}


/**
 最晚离开路径主要思想是:
 1.利用广度优先搜有进行搜索,选取目的点作为起点
 2.搜索目的点的孩子节点，然后使用孩子节点进行BFS
 3.如果latest即最晚出发时间小于遍历的出发时间,更新最晚出发时间,标记这条边已读即可
 时间复杂度为O(2E)
 */
void latest(int source,int dest)
{
    //source,startTime,source to dest time
    queue<pair<int,pair<int,int>>> q;
    vector<int> latest(V,0);
    vector<bool> visited(E,false);
    q.push(make_pair(dest,make_pair(inf,0)));
    while(!q.empty())
    {
        pair<int,pair<int,int>> p=q.front();
        q.pop();
        for(int i=0;i<nodes[p.first].size();i++)
        {
            
            int edgeSeq=nodes[p.first][i];
            if(!visited[edgeSeq])
            {
                
                int destTime=edges[edgeSeq].startTime+edges[edgeSeq].interval;
                if(destTime<=p.second.first)
                {
                    visited[edgeSeq]=true;
                    int time=p.second.second;
                    if(p.first==dest)
                    {
                        latest[edges[edgeSeq].source]=edges[edgeSeq].startTime;
                        time=edges[edgeSeq].startTime;
                    }
                    else if(latest[edges[edgeSeq].source]<edges[edgeSeq].startTime)
                        latest[edges[edgeSeq].source]=edges[edgeSeq].startTime;
                    q.push(make_pair(edges[edgeSeq].source,make_pair(edges[edgeSeq].startTime,time)));
                }
            }
            
        }
    }
    cout<<source<<"->"<<dest<<"的最晚离开时间是:"<<latest[source]<<endl;
}


/**
 最快抵达路径的主要思想:
 1.利用广度优先搜索进行遍历,首先选取目的边作为起始点
 2.搜索起始点的孩子节点,然后放入队列中,选取它的兄弟节点遍历
 3.将时间契合的边标记为已读,这里需要注意的是可以遍历为访问的边和保存的点中
 的最快抵达路径小于已经存在的最快抵达路径
 */
void fastest(int source,int dest)
{
    //source,startTime,arrival time
    queue<pair<int,pair<int,int>>> q;
    vector<int> fastest(V,inf);
    q.push(make_pair(dest,make_pair(inf,inf)));
    while(!q.empty())
    {
        pair<int,pair<int,int>> p=q.front();
        q.pop();
        for(int i=0;i<nodes[p.first].size();i++)
        {
            int edgeSeq=nodes[p.first][i];
            int destTime=edges[edgeSeq].startTime+edges[edgeSeq].interval;
            if(destTime<=p.second.first)
            {
                if(p.first==dest)
                {
                    fastest[edges[edgeSeq].source]=edges[edgeSeq].interval;
                    q.push(make_pair(edges[edgeSeq].source, make_pair(edges[edgeSeq].startTime,destTime)));
                }else{
                    if(p.second.second-edges[edgeSeq].startTime<fastest[edges[edgeSeq].source])
                        fastest[edges[edgeSeq].source]=p.second.second-edges[edgeSeq].startTime;
                    q.push(make_pair(edges[edgeSeq].source, make_pair(edges[edgeSeq].startTime,p.second.second)));
                }
            }
        }
    }
    cout<<source<<"->"<<dest<<"的最快路径时间是:"<<fastest[source]<<endl;
}

void read_queries(const char* filePath)
{
    FILE* file=fopen(filePath,"r");
    
    int x;
    
    int num;
    x=fscanf(file,"%d",&num);
    
    int u,v;
    for(int i=0;i<num;i++)
    {
        x=fscanf(file,"%d %d",&u,&v);
        sources.push_back(u);
        dests.push_back(v);
    }
    
    fclose(file);
}

//最早抵达
void run_earliest(){
    timeval start,end;
    gettimeofday(&start, 0);
    for(int i=0;i<sources.size();i++)
    {
        earliest(sources[i],dests[i]);
    }
    gettimeofday(&end, 0);
    printf("最早抵达路径的运行时间是:%.3fs\n",end.tv_sec-start.tv_sec+(double)(end.tv_usec-start.tv_usec)/1e6);
}

//最短路径
void run_shortest(){
    timeval start,end;
    gettimeofday(&start, 0);
    for(int i=0;i<sources.size();i++)
    {
        shortest(sources[i],dests[i]);
    }
    gettimeofday(&end, 0);
    printf("最短抵达路径的运行时间是:%.3fs\n",end.tv_sec-start.tv_sec+(double)(end.tv_usec-start.tv_usec)/1e6);
}
//最晚离开路径
void run_latest()
{
    timeval start,end;
    gettimeofday(&start, 0);
    for(int i=0;i<sources.size();i++)
    {
        latest(sources[i],dests[i]);
    }
    gettimeofday(&end, 0);
    printf("最晚离开路径的运行时间是:%.3fs\n",end.tv_sec-start.tv_sec+(double)(end.tv_usec-start.tv_usec)/1e6);
}

//最快路径
void run_fastest()
{
    timeval start,end;
    gettimeofday(&start, 0);
    for(int i=0;i<sources.size();i++)
    {
        fastest(sources[i],dests[i]);
    }
    gettimeofday(&end, 0);
    printf("最晚离开路径的运行时间是:%.3fs\n",end.tv_sec-start.tv_sec+(double)(end.tv_usec-start.tv_usec)/1e6);
}

int main(int argc, const char * argv[]) {
    
//    readGraph("/Users/youyujie/Downloads/graph3.txt");
//
//    process_earliest_graph();
//
//    read_queries("/Users/youyujie/Downloads/query3.txt");
//
//
//    run_earliest();
//
//    run_shortest();
//
//    run_latest();
    
    //run_fastest();
    
    const char* option=argv[1];

    readGraph(argv[2]);

    read_queries(argv[3]);

    process_earliest_graph();
    
    if(!strcmp(option,"earliest"))
    {
        run_earliest();
    }else if(!strcmp(option,"shortest"))
    {
        run_shortest();
    }else if(!strcmp(option,"latest"))
    {
        run_latest();
    }else if(!strcmp(option,"fastest"))
    {
        run_fastest();
    }

    return 0;
}
