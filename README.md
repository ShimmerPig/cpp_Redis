# cpp_Redis

## 基本流程介绍
首先是对服务端的初始化，包括数据库的初始化以及命令集合的初始化。
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190220113202156.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQwOTU1Mjg3,size_16,color_FFFFFF,t_70)
<br>

在客户端连接之后，开始创建客户端对其进行初始化，并且将其与服务端对应的数据库进行连接。
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190220113217410.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQwOTU1Mjg3,size_16,color_FFFFFF,t_70)
<br>

在客户端发送命令之后，服务端接受命令，对命令的合法性进行判断，然后在命令集合中查找相关命令并执行，最后返回执行结果给客户端。

![在这里插入图片描述](https://img-blog.csdnimg.cn/20190220113230102.jpg?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQwOTU1Mjg3,size_16,color_FFFFFF,t_70)

<br><br>
