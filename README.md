# 注意力增强游戏平台
## 描述
"GameClient"项目为"注意力增强游戏平台"客户端，用于用户登录
## 版本信息
- 编译器版本: 请使用msvc2015(64bit)及以上版本。
- QT Creator版本: 请使用4.11.1及以上版本。
- 当前项目版本: 0.96(beta)

## #0.96(beta)
### 0.96版本更新内容
- 修复部分中文乱码问题，统一使用utf-8标准处理中文，使用中文字符串需使用QString::fromLocal8Bit()
- 修复因login.dat不存在，导致软件崩溃的问题
- 修复"大游戏"启动时，因工作目录设置错误，无法正确启动的问题
- 修复"大游戏"启动时，因工作目录设置错误，无法找到login.dat的问题
- 新增login.dat验证方式：通过命令行向"大游戏"传入login.dat所在目录（不包含文件名），"大游戏"将首选此方法启动
- 新增login.dat验证方式：在USERPROFILE目录中备份一份login.dat文件，若命令行传入文件失败，"大游戏"将使用此方案作为备选方案
- 新增主页按钮（主页未实现）
- 修复"管理"图标过大
### 0.96版本已知Bug
- 主页仅有空按钮，界面和功能尚未实现
- 登录验证存在安全问题（严重），详情见"说明1"
- 无法正确显示用户信息的中文字符（0.95已存在）
- "登出"、"设置"、"WIFI"图标缺失鼠标按下效果(pressed)和鼠标浮动效果(hover)（0.95已存在）
- "脑电程序"图片墙在切换图片时稍有卡顿（0.95已存在）
- 用户仅可使用默认头像，无法自定义头像（0.95已存在）
- 游戏新闻模块当前仅为静态图片资源，无法响应打开链接操作，无法自动更新新闻（0.95已存在）
- 在退出程序时有时会出现警告："线程未正确终止"（0.95已存在）

## #0.95(beta)
### 0.95版本更新日志
- 更换软件LOGO
- 更换登录窗口背景图和图标资源，取消背景透明效果
- 更改主窗口背景图和图标资源
- 主窗口添加图片墙展示"CS:GO"，图片墙具有线性动画效果，可循环播放图片序列组
- 出窗口添加图片墙展示"脑电程序"，图片墙具有线性动画效果，可循环播放图片序列组
- 整合个人信息窗口至主窗口右侧
- 个人信息窗口添加是否展示敏感信息选项
- 主窗口添加游戏新闻展示模块
- 主窗口添加版本描述信息
- 登录信息可自动保存在本地，使用RC4算法加密用户密码
- 添加开源项目easylogging++作为日志管理系统
- 修复 "用户名或密码错误，请重试"错误显示为"数据异常，请重试"
- 修复在错误输入账号密码的情况下仍可能进行登录
- 删除无用按钮"记住密码"、"自动登录"
- 其他轻微Bug修复

### 0.95版本已知Bug
- 无法正确显示用户信息的中文字符
- "管理"图标明显过大
- "登出"、"设置"、"WIFI"图标缺失鼠标按下效果(pressed)和鼠标浮动效果(hover)
- "脑电程序"图片墙在切换图片时稍有卡顿
- 用户仅可使用默认头像，无法自定义头像
- 游戏新闻模块当前仅为静态图片资源，无法响应打开链接操作，无法自动更新新闻
- 在退出程序时有时会出现警告："线程未正确终止"

## 说明
### 说明1
应当使用非对称加密算法对用户登录数据进行加密保护，对称加密算法或流密码算法需要在客户端中保存私钥，攻击者一旦逆向程序获取私钥，则会导致秘钥泄漏，若攻击者同时获取用户登录文件login.dat，则会导致密码泄漏。

**安全的处理方案如下：**

服务端程序使用两套数据库，一套存放AES加密后的密码数据库，AES私钥需安全保存在服务器，该数据库密码可解密，用于用户密码恢复；一套存放哈希"加盐"处理后的密码数据库，该数据库不可解密，用于登录时密码验证。

客户端使用非对称加密（可选择RSA或SM2）处理数据，在用户登录时，需首先使用公钥对用户密码进行加密，公钥可以对外公开，应保存在客户端；在提交加密密码后，服务端使用私钥解密并获取明文密码，私钥不可对外公开，应安全存放在服务器；服务端程序在解密密码后使用哈希"加盐"算法对密码进行单向编码，并与哈希密码数据库比对；如果遇到需要恢复用户密码的情况，服务端管理员可以通过AES私钥对加密密码进行解密，以实现密码恢复。

**注意：** AES私钥可长期使用；RSA公私钥需定期更换，通常可随软件版本更新更换秘钥，若软件版本长期不更新，应当定时（如30天）更换一次。

## 运行截图
<div align=center><img width="800" src ="https://res.cloudinary.com/macc1989/image/upload/v1606638206/samples/%E6%B3%A8%E6%84%8F%E5%8A%9B%E5%A2%9E%E5%BC%BA%E6%B8%B8%E6%88%8F%E5%B9%B3%E5%8F%B0/LoginDialog.png" alt="登录窗口"></div><br>
<div align=center><img width="800" src ="https://res.cloudinary.com/macc1989/image/upload/v1606638206/samples/%E6%B3%A8%E6%84%8F%E5%8A%9B%E5%A2%9E%E5%BC%BA%E6%B8%B8%E6%88%8F%E5%B9%B3%E5%8F%B0/MainWindow.png" alt="主窗口1"></div><br>
<div align=center><img width="800" src ="https://res.cloudinary.com/macc1989/image/upload/v1606638206/samples/%E6%B3%A8%E6%84%8F%E5%8A%9B%E5%A2%9E%E5%BC%BA%E6%B8%B8%E6%88%8F%E5%B9%B3%E5%8F%B0/MainWindow2.png" alt="主窗口2"></div><br>
 <div align=center><img width="800" src ="https://res.cloudinary.com/macc1989/image/upload/v1606638207/samples/%E6%B3%A8%E6%84%8F%E5%8A%9B%E5%A2%9E%E5%BC%BA%E6%B8%B8%E6%88%8F%E5%B9%B3%E5%8F%B0/MainWindow3.png" alt="主窗口3"></div><br>
  <div align=center><img width="800" src ="https://res.cloudinary.com/macc1989/image/upload/v1606638206/samples/%E6%B3%A8%E6%84%8F%E5%8A%9B%E5%A2%9E%E5%BC%BA%E6%B8%B8%E6%88%8F%E5%B9%B3%E5%8F%B0/ManageDialog.png" alt="管理"></div><br>
