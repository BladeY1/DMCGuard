# 编译说明
## 导入项目
请确保安装了最新的Android Studio。使用Android Studio打开该文件夹，软件会自动将其识别为项目文件夹。

## 创建签名密钥
由于Android系统的安全措施，不对APK文件进行签名会导致其无法在Android真机上安装。点击工具栏上方的“Builder”选项 ，在下拉菜单里选择“Genarate Singed Bundle/APK”。

生成窗口会要求开发者输入相关的密钥文件和密码。若没有可以创建一个新的密钥文件，点击“Create new...”即可打开新建密钥文件的对话框，在对话框中输入相应的信息即可。

## 生成APK安装包
继续在生成窗口使用新创建的密钥文件对APK进行签名即可获得签名的APK文件。将得到的APK文件发送到Android真机即可安装使用。