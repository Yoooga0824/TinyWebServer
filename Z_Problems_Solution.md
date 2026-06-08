# 🎯 TinyWebServer 项目问题与解决总结

## 1. 环境配置：WSL 安装后闪退及网络代理冲突

**问题现象：** 在 Windows 上通过 `wsl --install` 安装 Ubuntu 后，点击图标闪退，或在 PowerShell 中执行 `wsl` 报错「灾难性故障」和 `Wsl/Service/E_UNEXPECTED`。

**原因分析：**

- 未启用「虚拟机平台」或 BIOS 虚拟化[^1]
- Windows 代理软件（如 Clash、v2rayN）与 WSL 的 NAT 网络模式冲突

**解决方法：**

1. 在「启用或关闭 Windows 功能」中勾选「适用于 Linux 的 Windows 子系统」和「虚拟机平台」，并重启
2. 创建 `%UserProfile%\.wslconfig` 文件，添加镜像网络配置：

```ini
[wsl2]
networkingMode=mirrored
dnsTunneling=true
firewall=true
autoProxy=true
```

3. 重启 WSL：`wsl --shutdown`

**收获：** 理解了 WSL 2 的网络架构及代理冲突的通用解决方法，同时学会了使用 `.wslconfig` 进行定制。

## 2. MySQL 认证插件不匹配

**问题现象：** 运行 `./server` 后只打印 IP 和端口，然后卡住，没有报错，浏览器无法访问。通过添加 `printf` 定位到 `connPool->init` 处无响应。

**原因分析：** Ubuntu 默认安装的 MySQL 8.4 中，`root` 用户使用 `auth_socket` 插件认证（无需密码，但只能通过 `sudo mysql` 登录），而项目代码中使用密码 `root` 或自定义密码连接，导致 `mysql_real_connect` 认证失败。失败后 `sql_connection_pool` 中的循环因超时而长时间阻塞，表现为卡住。

**解决方法：** 创建专用数据库用户 `tiny`，使用兼容的 `caching_sha2_password` 认证，并授权访问项目数据库。

**三种认证插件简述：**

- **auth_socket**（操作系统 socket 认证）：不验证密码，而是验证操作系统用户。以 Linux 的 `root` 用户登录系统时，直接运行 `sudo mysql`，MySQL 看到你是系统的 root，就让你进去了。
- **caching_sha2_password**（MySQL 8.x 默认，推荐）：使用 SHA-256 算法对密码进行哈希存储，并引入缓存机制，第一次验证后把结果缓存在内存里，后续连接速度极快。
- **mysql_native_password**（旧式密码，已被淘汰）：使用旧的 SHA-1 算法。

创建用户并授权：

```sql
CREATE USER 'tiny'@'localhost' IDENTIFIED WITH caching_sha2_password BY 'Cyj050824';
GRANT ALL PRIVILEGES ON yooogadb.* TO 'tiny'@'localhost';
```

测试连接：

```bash
mysql -u tiny -pCyj050824 -e "SELECT * FROM yooogadb.user;"
```

查看用户配置：

```bash
sudo mysql -e "SELECT Host, User, plugin FROM mysql.user;"
```

**收获：** 使用 `bear -- make` 生成 `compile_commands.json`，供 IDE 使用。

---

## 参考文献

[^1]: BIOS 虚拟化（通常指 Intel VT-x 或 AMD-V）是一项允许单个物理 CPU 模拟多个虚拟 CPU 的硬件底层技术。它的核心作用是让虚拟机（VM）能够直接访问真实的物理硬件资源，而不是仅仅通过软件进行「翻译」。
