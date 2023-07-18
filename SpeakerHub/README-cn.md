# 智能音箱配置与使用
## 运行
启动hub.py后即进入监听状态（等待dmcguard的speaker通道连接）。
运行speakerStart.py后，程序等待用户语音输入。

## 模型训练
1.登录声音模型的训练网站https://snowboy.hahack.com
2.点击enable mocrophone后点击record,在等待两秒后开始录制唤醒词，
录制时间一次为三秒，为保证模型的完整性与可识别度，请每个模型至少录制三次
3.录制完毕后点击submit，而后点击download，下载自己训练的模型文件，文件以pmdl格式保存
4.使用唤醒词时在命令行里给出唤醒词模型文件存在的完整路径即可

## snowboy语音识别代码
源代码网址：https://github.com/seasalt-ai/snowboy

## 测试环境配置
使用不同服务器测试时，修改localconfig.py中相关变量即可