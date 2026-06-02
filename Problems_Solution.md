## 🎯 TinyWebServer 项目问题与解决总结
### 1. 环境配置：WSL 安装后闪退及网络代理冲突
+ 问题现象：在 Windows 上通过 ```wsl --install``` 安装 Ubuntu 后，点击图标闪退，或在 PowerShell 中执行 wsl 报错 灾难性故障 和 Wsl/Service/E_UNEXPECTED。

+ 原因分析：未启用“虚拟机平台”或 BIOS 虚拟化。Windows 代理软件（如 Clash、v2rayN）与 WSL 的 NAT 网络模式冲突。

+ 解决方法：在“启用或关闭 Windows 功能”中勾选“适用于 Linux 的 Windows 子系统”和“虚拟机平台”，并重启;创建 %UserProfile%\.wslconfig 文件，添加镜像网络配置：
```
[wsl2]
networkingMode=mirrored
dnsTunneling=true
firewall=true
autoProxy=true
```
重启 WSL：```wsl --shutdown```
+ 收获：理解了 WSL 2 的网络架构及代理冲突的通用解决方法，同时学会了使用 .wslconfig 进行定制。





```sudo mysql -e "SELECT Host, User, plugin FROM mysql.user;"```