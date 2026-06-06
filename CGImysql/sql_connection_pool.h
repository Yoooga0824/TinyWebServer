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
	//单例模式：获取全局唯一的连接池实例。
	static connection_pool *GetInstance();

	MYSQL *GetConnection();				 //从池中取出一个空闲连接。如果没有空闲连接，会等待（通过 sem 信号量）。
	bool ReleaseConnection(MYSQL *conn); //将使用完的连接归还到池中，并增加 sem 计数。
	int GetFreeConn();					 //返回当前空闲连接数（仅用于监控）。
	void DestroyPool();					 //销毁所有连接，释放资源。通常在程序退出时调用。

	void init(string url, string User, string PassWord, string DataBaseName, int Port, unsigned int MaxConn);
	
	connection_pool();
	~connection_pool();

private:
	unsigned int MaxConn;  //最大连接数
	unsigned int CurConn;  //当前已使用的连接数
	unsigned int FreeConn; //当前空闲的连接数

private:
	locker lock;  //互斥锁，用于保护连接池的共享资源（如 connList 和连接计数器）。
	list<MYSQL *> connList; //连接池
	sem *reserve;   //信号量，用于控制连接池中可用连接的数量。当一个线程获取连接时，sem 的计数会减少；当线程释放连接时，计数会增加。

private:
	string url;			 //主机地址
	string Port;		 //数据库端口号
	string User;		 //登陆数据库用户名
	string PassWord;	 //登陆数据库密码
	string DatabaseName; //使用数据库名
};

class connectionRAII {

public:
	connectionRAII(MYSQL **con, connection_pool *connPool);  //构造函数：获取一个数据库连接，并将其保存在 con 指针中。同时，保存连接池的指针以便后续释放连接。
	~connectionRAII();  //析构函数：当 connectionRAII 对象被销毁时，自动将连接归还给连接池，确保资源的正确释放。
	
private:
	MYSQL *conRAII;  //保存获取的数据库连接指针。当 connectionRAII 对象被销毁时，这个指针将被用来释放连接。
	connection_pool *poolRAII;  //保存连接池的指针，以便在析构函数中调用 ReleaseConnection 方法将连接归还给连接池。
};

#endif
