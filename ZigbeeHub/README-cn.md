# Zigbee Hub  配置与使用

## 1 软件准备

### 1.1 作为Hub的树莓派

将hub.py拷贝到工作目录下，并安装需要的依赖库：

```bash
pip install -r ./hub/requirements.txt
```

请注意如果使用了其他USB串口设备，请在`hub.py`中进行修改。

推荐使用虚拟环境或包管理器进行环境隔离，以免影响其他软件的运行。

在`./hub/localconfig.py`中，需要填写Zigbee服务器地址，服务器连接密钥和HTTPS根证书。以下为一种示例：

```python
zigbee_server_url = "https://zigbee.server"
server_key = "test"
ca = "cert/rootCA.pem"
```

### 1.2 Zigbee 开发板

使用配置好Ti Z-Stack环境的IAR Embedded Workbench打开`./Z-Stack-trans/Projects/zstack/ZMain/TI2530DB/ZMain.c`，连接Zigbee仿真器后进行烧录。

请注意两块开发板需要分别烧录Coordinator和EndDevice。

![烧录连接](https://picx.zhimg.com/80/v2-b4a1ffdfa175b9023729cc62d2e55877_r.jpg)

### 2 硬件准备

如下图所示，连接作为Hub的树莓派和Zigbee开发板。运行DMCGuard的树莓派也需要进行同样的连接。

请注意，开发板的供电方式应选择USB，否则将无法使用串口功能。

![与树莓派连接](https://picx.zhimg.com/80/v2-8612c2eb74077adeadcaa50639a77627_r.jpg)

### 3 运行

`hub.py`使用了少量版本特性，需要使用python 3.5及以上版本运行。

由于对串口进行了读写，需使用管理员权限运行`hub.py`。

```bash
sudo python3 ./hub/hub.py
```
