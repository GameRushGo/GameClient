# 注意力增强游戏平台
## 描述
"GameClient"项目为"注意力增强游戏平台"客户端。
## 版本信息
- 编译器版本
请使用msvc2015(64bit)及以上版本。

- QT Creator版本
请使用4.11.1及以上版本。

- 当前项目版本
0.95(beta)

## 当前版本更新内容
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

## 当前版本已知Bug
- 无法正确显示用户信息的中文字符
- "管理"图标明显过大
- "登出"、"设置"、"WIFI"图标缺失鼠标按下效果(pressed)和鼠标浮动效果(hover)
- "脑电程序"图片墙在切换图片时稍有卡顿
- 用户仅可使用默认头像，无法自定义头像
- 游戏新闻模块当前仅为静态图片资源，无法响应打开链接操作，无法自动更新新闻
- 在退出程序时有时会出现警告："线程未正确终止"

## 运行截图
<div align=center><img width="500" src ="https://res.cloudinary.com/macc1989/image/upload/v1606638206/samples/%E6%B3%A8%E6%84%8F%E5%8A%9B%E5%A2%9E%E5%BC%BA%E6%B8%B8%E6%88%8F%E5%B9%B3%E5%8F%B0/LoginDialog.png" alt="登录窗口"></div><br>
<div align=center><img width="800" src ="https://res.cloudinary.com/macc1989/image/upload/v1606638206/samples/%E6%B3%A8%E6%84%8F%E5%8A%9B%E5%A2%9E%E5%BC%BA%E6%B8%B8%E6%88%8F%E5%B9%B3%E5%8F%B0/MainWindow.png" alt="主窗口1"></div><br>
<div align=center><img width="800" src ="https://res.cloudinary.com/macc1989/image/upload/v1606638206/samples/%E6%B3%A8%E6%84%8F%E5%8A%9B%E5%A2%9E%E5%BC%BA%E6%B8%B8%E6%88%8F%E5%B9%B3%E5%8F%B0/MainWindow2.png" alt="主窗口2"></div><br>
 <div align=center><img width="800" src ="https://res.cloudinary.com/macc1989/image/upload/v1606638207/samples/%E6%B3%A8%E6%84%8F%E5%8A%9B%E5%A2%9E%E5%BC%BA%E6%B8%B8%E6%88%8F%E5%B9%B3%E5%8F%B0/MainWindow3.png" alt="主窗口3"></div><br>
  <div align=center><img width="500" src ="https://res.cloudinary.com/macc1989/image/upload/v1606638206/samples/%E6%B3%A8%E6%84%8F%E5%8A%9B%E5%A2%9E%E5%BC%BA%E6%B8%B8%E6%88%8F%E5%B9%B3%E5%8F%B0/ManageDialog.png" alt="管理"></div><br>
