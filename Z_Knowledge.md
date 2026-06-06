## 📖 在 TinyWebServer 中学习到的知识
### 1.运行服务器
```bash 
./server 127.0.0.1 9006
```
### 2.监听端口
```bash
sudo ss -lntp | grep 9006
```
### 3.sigaction比 signal更可靠，
sigaction语义明确、行为一致、可控性强，而signal在不同 Unix / Linux 上表现不一样，充满历史坑。
### 4.POSIX 标准
POSIX（Portable Operating System Interface，可移植操作系统接口）​ 是一套由 IEEE 制定、被 ISO/IEC 标准化的操作系统接口规范。
### 5.双指针和单指针
```cpp
void setNull(MYSQL* p) {
    p = nullptr;   // 只修改了形参，外部实参不变
}

void setNullCorrect(MYSQL** pp) {
    *pp = nullptr; // 修改了外部实参
}

MYSQL* conn = some_value;
setNull(conn);         // conn 不变
setNullCorrect(&conn); // conn 变为 nullptr
```