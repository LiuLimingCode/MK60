# MK60底层

标签： Cortex-M C

---

#### 更新日志
+ 2019年12月
    + 重写 Menu 使得：
		+ 1.菜单由原来用32位内存保存变量，改为15位，在相同的存储空间上能存储更多的变量。
		+ 2.将程序中需要使用外部资源（存储器、显示器、GPIO）的函数独立封装，方便程序的移植和修改。
		+ 3.重写程序，并增加很多注释，使程序更逻辑、更能读懂。
		+ 4.菜单操作界面和操作方法不变。


