/* 
 * File:   main.cpp
 * Author: sunsmile
 *
 * Created on 2016年5月3日, 下午8:44
 * 
 * Usage:
 * 
 * 
 */

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <zookeeper/zookeeper.h>
using namespace std;

void leader_watcher(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx)
{
}

void zkleader_string_completion(int rc, const char *name, const void *data)
{
}

void stat_completion(int rc, const struct Stat *stat, const void *data)
{
}


void printIfLeader(zhandle_t* zkhandle, watcher_fn fn);
void get_watcher(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx);
int wgetChildren(zhandle_t* zkhandle, string path, vector<string>& strings, watcher_fn fn);
string getSmallestNoNode(vector<string> nodelist);
bool splitString(vector<string> nodelist, map<string, int>& nodemap);
string nodename = "";

int main(int argc, char** argv)
{
	if (argc !=3) {
		cout << "Error parameter, useage: ./" << argv[0] << " serverip:port nodename" << endl;
		return -1;
	}
	//初始化需要的参数
	nodename = argv[2];
	string servs =  argv[1];//"172.17.0.2:2181"; //",172.17.0.3:2181,172.17.0.4:2181";
	int timeout = 300;

	//初始化zookeeper句柄
	zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
	zhandle_t* zkhandle = zookeeper_init(servs.c_str(), leader_watcher, timeout, 0, NULL, 0);
	if (zkhandle == NULL) {
		cerr << "Error when connecting to zookeeper servers..." << endl;
		return -1;
	}

	//创建永久节点/election
	string path = "/election";
	int ret = zoo_acreate(zkhandle, path.c_str(), "1", 1,
		&ZOO_OPEN_ACL_UNSAFE, 0, zkleader_string_completion, "acreate");
	if (ret) {
		cerr << "Error " << ret << " for acreate" << endl;
		return -1;
	}

	//创建/election节点的子节点，通过命令行参数argv[1]指定
	path = "/election/" + nodename;
	ret = zoo_acreate(zkhandle, path.c_str(), "1", 1,
		&ZOO_OPEN_ACL_UNSAFE, ZOO_SEQUENCE | ZOO_EPHEMERAL,
		zkleader_string_completion, "acreate");
	if (ret) {
		fprintf(stderr, "Error %d for %s\n", ret, "acreate");
		exit(EXIT_FAILURE);
	}

	//输出是否为leader以及设置节点变化的监听函数
	printIfLeader(zkhandle, get_watcher);

	getchar();
	//释放zookeeper句柄
	zookeeper_close(zkhandle);
	return 0;
}

/*
 *  * 打印自己是否为leader，或者是哪个节点的follower
 */
void printIfLeader(zhandle_t* zkhandle, watcher_fn fn)
{
	vector<string> children;
	wgetChildren(zkhandle, "/election", children, fn);

	if (children.empty()) {
		return;
	}

	for (auto child : children) {
		cout << child << endl;
	}

	string smallestNode = getSmallestNoNode(children);
	cout << "znodeName: " << nodename << ", smallest node: " << smallestNode << endl;
	if ((!nodename.empty()) && smallestNode == nodename) {
		cout << "I'm the leader now" << endl;
	} else {
		cout << "I'm follower of " << smallestNode << endl;
	}
}

/*
 * 监听节点列表变化的回调函数
 */
void get_watcher(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx)
{
	cout << "node list changing, type:" << type << ", state: " << state << ", path: " << path;
	printIfLeader(zh, get_watcher);
}

/*
 * 获取path下的子节点列表
 */
int wgetChildren(zhandle_t* zkhandle, string path, vector<string>& children, watcher_fn fn)
{
	struct String_vector strChildren;
	int ret = zoo_wget_children(zkhandle, path.c_str(), fn, NULL, &strChildren);
	if (ret != ZOK) {
		cerr << "Error " << ret << " for wgetChilren." << endl;
	} else {
		for (int i = 0; i < strChildren.count; i++) {
			children.push_back(strChildren.data[i]);
		}
	}
	return ret;
}

/*
 * 从nodelist中给出的节点列表中找到最后10位组成的数字最小的那个元素
 */
string getSmallestNoNode(vector<string> nodelist)
{
	if (nodelist.empty()) {
		return "";
	}

	map<string, int> nodemap; //<nodeName, nodeNumber>
	splitString(nodelist, nodemap);

	if (nodemap.size() == 1) {
		return nodemap.begin()->first;
	}

	int minNum = 999999999;
	string minName = "";
	for (auto node : nodemap) {
		if (minNum > node.second) {
			minNum = node.second;
			minName = node.first;
		}
	}

	return minName;
}

/*
 * nodelist中每个元素都是一个字符串，其组成格式为：nodename0000000001，最后十位为数字
 * 本函数将nodelist中的每个字符串分解为<nodename, 1>这样的元素对，通过nodemap返回
 */
bool splitString(vector<string> nodelist, map<string, int>& nodemap)
{
	string prefix, postfix;
	int sn;
	for (auto node : nodelist) {
		prefix = node.substr(0, node.length() - 10);
		postfix = node.substr(node.length() - 10, node.length());
		sn = atoi(postfix.c_str());
		cout << prefix << ", " << postfix << ", " << sn << endl;
		nodemap[prefix] = sn;
	}
	return true;
}
