#ifndef CONNECTION_POOL  //原本是下划线开头，不太安全
#define CONNECTION_POOL

#include <stdio.h>
#include <list>
#include <mysql/mysql.h>
#include <error.h>
#include <string.h>
#include <iostream>
#include <string>
#include "../lock/locker.h"

using namespace std;

class connection_pool {
public:
	MYSQL *GetConnection();				 //从池中取出一个空闲连接。如果没有空闲连接，会等待（通过 sem 信号量）。
	bool ReleaseConnection(MYSQL *conn); //将使用完的连接归还到池中，并增加 sem 计数。
	int GetFreeConn();					 //返回当前空闲连接数（仅用于监控）。
	void DestroyPool();					 //销毁所有连接，释放资源。通常在程序退出时调用。

	//单例模式：获取全局唯一的连接池实例。
	static connection_pool *GetInstance();

	void init(string url, string User, string PassWord, string DataBaseName, int Port, unsigned int MaxConn); 
	
	connection_pool();
	~connection_pool();

private:
	unsigned int MaxConn;  //最大连接数
	unsigned int CurConn;  //当前已使用的连接数
	unsigned int FreeConn; //当前空闲的连接数

private:
	locker lock;
	list<MYSQL *> connList; //连接池
	sem reserve;

private:
	string url;			 //主机地址
	string Port;		 //数据库端口号
	string User;		 //登陆数据库用户名
	string PassWord;	 //登陆数据库密码
	string DatabaseName; //使用数据库名
};

class connectionRAII {

public:
	connectionRAII(MYSQL **con, connection_pool *connPool);
	~connectionRAII();
	
private:
	MYSQL *conRAII;
	connection_pool *poolRAII;
};

#endif
