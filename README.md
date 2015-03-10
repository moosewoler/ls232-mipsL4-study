ls232-mipsL4-study
==================

这是一个用来自学mipsL4微内核技术的项目，希望最终能够运行于龙芯1B开发板上。

1 关于mipsL4微内核
------------------

[mipsL4](http://www.cse.unsw.edu.au/~disy/L4/MIPS/)是现有L4微内核中唯一一个可运行于mips架构的L4微内核实现。不过该项目已经停止开发。

2 关于龙芯1B
------------------

[龙芯1B](http://www.loongson.cn/product_info.php?id=42)是一款MIPS32指令集的嵌入式处理器。

3 记录
------------------

2014-11-08

删除Makefile.conf中的-noshare选项，这个选项是做什么的？老版本中的选项吗？

将Makefile.conf中的-fullwarn改成-Wall。

将kernel\Makefile中的-T改成-Ttext

用以下awk程序段可以实现build号自动加1
BEGIN {OFS="=";FS="="}
/hword_t l4_build/ {$2 = $2 + 1}
                   {print $0}

