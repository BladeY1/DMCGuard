# Manufacture Server
执行 `python app.py`来运行服务器。

## requirement
输入以下指令安装依赖。
```
pip install -r requirements.txt
 ```
<!--
## Database
[参考](http://www.ityouknow.com/python/2019/10/30/python_web_database-046.html)
-->

## AWS cert
从AWS控制平台获得物品的证书，将其放入/cert/aws文件夹下。

## SSL config 
### 使用mkcert
可以使用mkcert配置生成SSL证书。将获得的证书文件放入/cert/server文件夹下可。
并更改config.py里的SSL_KEYS_1和SSL_KEYS_2
[参考](https://www.jianshu.com/p/7cb5c2cffaaa)
### 使用openssl工具
查阅相关资料使用openssl工具手动构建CA机构并签发证书。


# !请注意，你必须将你的服务器IP、ZigBee Server IP、Speaker Server IP正确填写在每个config.py的MAN_ADDR、ZIGBEE_ADDR、SPEAKER_ADDR中。
如果你想在一个远程云服务器上同时部署三个服务器，你可以使用不同端口比如5001、5002、5003，只需要更改app.py的501行"port = "
但是你需要在config.py中增加两个IP: ZIGBEE_IP和SPEAKER_IP,并更改app.py的448行的ZIGBEE_ADDR、SPEAKER_ADDR为ZIGBEE_IP、SPEAKER_IP