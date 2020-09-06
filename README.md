## MiniSQL  总体设计报告

### 1   MiniSQL系统概述

#### 1.1   背景

##### 1.1.1 编写目的

通过自行实现MiniSQL系统，加深对于数据库工作原理的理解。

##### 1.1.2 项目背景

学习了数据库系统课程之后，对于数据库的构成和使用有了一定的了解，具备了实现一个MiniSQL系统的能力，使用C++编程语言进行系统设计和实现。

#### 1.2   功能描述

##### 1.2.1 数据类型

数据库中的信息包含三种基本数据类型:int, float, char(n)，其中char(n)满足1<=n<=255

##### 1.2.2 表的创建和删除

一个表最多可以定义32个属性，各属性可以指定为是否unique;支持单属性的主键定义。

创建表的语法如下:
```mysql
create table 表名 (
  列名 类型 ,
  列名 类型 ,
  ...
  列名 类型 ,
  primary key ( 列名 )
);
```


例:
```mysql
create table student (
	sno char(8),
    sname char(16) unique,
    sage int,
    sgender char (1),
    primary key ( sno )
);
```


删除表的功能是将从数据库中删除表，类似于MySQL中的drop table。语法如下:

```mysql
drop table 表名;
drop table student;
```

##### 1.2.3 索引的建立和删除

对于表的主属性自动建立B+树索引，对于声明为unique的属性可以通过SQL语句由用户指定建立/删除B+树索引，因此，所有的B+树索引都是单属性单值的。

创建索引的语法如下：
```mysql
create index 索引名 on 表名 ( 列名 );
create index stunameidx on student ( sname );  # 例
```
删除索引的语法如下：
```mysql
drop index 索引名;
drop index stunameidx;             # 例
```
  对于建立索引的项目可以通过B+树进行查找，提高效率。
##### 1.2.4 查找记录

可以通过指定用and连接的多个条件进行查询，对于三种类型支持等值查询；对于int和float类型支持区间查询，考虑实际中的使用情况，对于char(n)类型，不支持区间查询。语法如下：

```mysql
select * from 表名;  #无条件查询，获得表中所有记录
select * from 表名 where 条件; #条件查询
其中“条件”具有以下格式：列 op 值 and 列 op 值 … and 列 op 值。

op是算术比较符：= <>   <   >   <=  >=
select * from student; #例
select * from student where sno = ‘88888888’;
select * from student where sage > 20 and sgender = ‘F’ and ……;
```

##### 1.2.5 插入记录
一次插入一条记录，语法如下：
```mysql
insert into 表名 values ( 值1 , 值2 , … , 值n );
insert into student values (‘12345678’,’wy’,22,’M’);    #例
```
##### 1.2.6 删除记录

类似**1.2.4查找记录**，允许无条件和带条件地删除多条记录，语法如下：
```mysql
delete from 表名;
delete from 表名 where 条件;
delete from student;    # 例
delete from student where sno = ‘88888888’ and age > 20 and ……;
```
##### 1.2.7 打开/切换数据库

打开或者切换使用的数据库，会存储当前内存和buffer中的所有信息到文件系统，然后打开需要的数据库，将需要的信息读入buffer和内存。
```mysql
create database db_name;    # 创建数据库并使用该数据库
use db_name;          #使用数据库
```
##### 1.2.8 退出MiniSQL系统

输入该命令可以正常退出MiniSQL，缓存中的信息将写入文件系统。
```mysql
quit;
```
##### 1.2.9 执行MiniSQL脚本文件语句

语法如下:
```c++
execfile 文件名;
```
将顺序执行脚本中的语句，如果遇到quit命令则结束执行并正常退出系统。

#### 1.3   运行环境和配置

系统：windows10

编译器：visual studio2017即以上

### 2   MiniSQL系统结构设计

#### 2.1   总体设计：

![img](https://github.com/Liang-ZX/miniSQL/blob/master/clip_image002.jpg)

主程序main初始化Buffer Manager对象, Catalog Manager对象,Record Manager对象实例；读入用户输入，调用Interpreter模块进行解释。

Interpreter模块解释输入，判断语法和语义准确性，调用API模块提供接口。

API模块调用Index Manager, Record Manager, Catalog Manager，并输出正确执行的结果和执行错误的故障信息。

四个Manager之间的相互关系如图所示。

#### 2.2   Interpreter 模块：

##### 2.2.1 模块概述

Interpreter接收并解释用户输入的命令，生成命令的内部数据结构表示，同时检查命令的语法正确性和语义正确性，对正确的命令调用API层提供的函数执行。

##### 2.2.2 总体实现思路

Interpreter模块的核心是执行语义解析和语法检查，同时完成包括退出数据库，执行命令文件等功能。

在语义解析方面，参考https://github.com/callMeName/MiniSql 实现了getWord函数，实现了对用户的输入进行字符串拆分，逐关键字返回的功能。 

```c++
string Interpreter::getWord(string &s, int &pos)
```

具体实现上，getWord只会返回逐个单词，以及“,”、“(”和“)”三种运算符，自动过滤其余运算符。所有返回的内容都会被解析为string，需要interprete执行类型转换。

在语法检查方面，遵从MySQL语法规范，逐单词进行检查，且要求创建表时必须给出主键。此外，对于输入的数据统一调用CatalogManager获得表的各属性的类型信息执行类型检查。若命令合法则调用API执行命令，否则输出报错信息。

在执行命令文件方面，Interpreter类并没有实现，而是交给main函数完成，main中的程序主循环将命令文件和输入指令统一为以语句为单位的字符串，调用interpreter解析。

值得一提的是，interpreter的API使用多态指针，以赋予其更大的灵活性。

#### 2.3   API模块：

##### 2.3.1 模块概述

API模块是整个系统的核心，其主要功能为提供执行MiniSQL语句的接口，供Interpreter层调用。该接口以Interpreter层解释生成的命令内部表示为输入，根据Catalog Manager提供的信息确定执行规则，并调用Record Manager、Index Manager和Catalog Manager提供的相应接口进行执行，并输出正确执行结果或者错误信息。

#### 2.4   Catalog Manager模块

##### 2.4.1 模块概述

Catalog Manager负责管理数据库的所有模式信息，包括：

1. 数据库中所有表的定义信息，包括表的名称、表中字段（列）数、主键、定义在该表上的索引。

2. 表中每个字段的定义信息，包括字段类型、是否唯一等。

3. 数据库中所有索引的定义，包括所属表、索引建立在那个字段上等。

Catalog Manager还必需提供访问及操作上述信息的接口，供Interpreter和API模块使用。

#### 2.5   Record Manager模块：

##### 2.5.1 模块概述

Record Manager负责管理记录表中数据的数据文件。主要功能为实现数据文件的创建与删除、记录的插入、删除与查找操作，并对外提供相应的接口。其中记录的查找操作要求能够支持不带条件的查找和带一个条件的查找（包括等值查找、不等值查找和区间查找）。

#### 2.6   Index manager模块：

##### 2.6.1 模块概述

IndexManager模块的主要作用是在表格的unique属性上建立索引，以提高数据检索的效率，索引文件采用B+树的数据结构。IndexManager模块定义了三个类，分别为：Node类、BPT类和Index_Manager类。其中Node类定义了B+树中的每个节点，并且提供了相应的函数供BPT类进行调用。BPT类负责管理整棵B+树，通过调用Node中的成员函数实现数据的插入、删除和搜索等功能。Index_Manager类管理一张表格上的所有索引文件，通过三个map容器存储三个不同类型的B+树索引。Index_Manager对外界提供创建索引文件、删除索引文件、以及数据的插入、删除、查询等接口。

##### 2.6.2 B+树类BPT

BPT类管理整棵B+树索引，通过一个指向B+树根节点的指针保存整棵B+树，BPT类对Index_Manager类提供了四个接口，分别为Insert_Key、Delete_Key、Delete_All和Search_Key。四者的功能分别为：插入key、删除key、删除所有key、检索key所对应的地址信息。

##### 2.6.3 接口与交互

接口信息

| 接口         | 功能                     |
| ------------ | ------------------------ |
| Create_Index | 创建索引文件             |
| Drop_Index   | 删除索引文件             |
| Drop_All     | 删除表上所有索引文件     |
| Clear_Index  | 清空表上所有索引文件数据 |
| Search       | 查询数据                 |
| Insert       | 插入                     |
| Delete       | 删除                     |

每个Index_Manager对象管理一张表格上的所有索引，创建对象时构造函数接受一个string类型的参数代表表格的名字，随后构造函数会将该表上已经存在的所有索引文件从硬盘读入内存构造B+树索引，析构函数将内存中的B+树删除并将所有索引写入硬盘。

在交互方面，RecordManager通过Insert和Delete两个接口在表格的数据发生变化的时候更新索引文件中的数据，API通过调用Create_Index、Drop_Index、Drop_All和Clear_Index四个接口分别实现创建索引、删除索引、删除表上所有索引、清空索引文件中所有数据的功能。另外，IndexManager通过CatalogManager提供的接口获得表上的模式信息，并通过BufferManager实现与硬盘之间的数据交换。

#### 2.7   BufferManager模块：

##### 2.7.1 模块概述

Buffer Manager负责缓冲区的管理，主要功能有：

1、根据需要，读取指定的数据到系统缓冲区或将缓冲区中的数据写出到文件（**Page on Demand**，请求式分页）

2、实现**LRU**算法，当缓冲区满时选择合适的页进行替换

3、记录缓冲区中各页的状态，如是否被修改过等（**dirty bit**）

4、提供缓冲区页的**pin**功能，及锁定缓冲区的页，不允许替换出去

为提高磁盘I/O操作的效率，缓冲区与文件系统交互的单位是块，块的大小设置为4KB。

##### 2.7.2 总体实现思路

Buffer Manager模块首先定义了文件和块的元数据结构，即sqlFile和sqlBlcok结构。在sqlBlock结构中保存dirty bit和pin的相关信息，同时保存一个指向对应文件元结构的指针。同一个文件的不同sqlBlock采用链表方式连接。sqlFile保存文件名、在缓存中块数量等信息，同时保存指针指向该文件对应缓存中的块首，同一个buffer manager的不同sqlFile也采用链表方式连接。

Buffer Manager同时维护一个sqlBlock* Pool[bnum]用来储存所有分配的数据块元信息。采用数组的方式，地址空间连续，在后续LRU等算法实现时，有更高的效率。

对外接口以block num为单位，实现包括读文件、写文件、删除文件、加锁、解锁、将所有块写回磁盘等函数。

##### 2.7.5 关键算法实现说明

###### 2.7.5.1 LRU

不论是ReadFile还是WriteFile都会打开相应文件，然后尝试找到一个块，把文件内容写入缓存中。寻找块会调用私有方法
```c++
sqlBlock* getUsableBlock(const string db_name, sqlFile* fileInfo); //LRU here
```
该函数会先在已有的缓存中查找，若已有该块数据则返回对应的sqlBlock*,否则检查已分配的块数量是否已经超过了允许的上限，若否，则重新malloc一个块来存放数据；若是，则使用LRU算法替换least recently used的块，将其内容写回磁盘（脏写），然后读入要处理的数据。同时对其它已在缓存区中的数据块执行时间戳更新，以保证一致性。

此外，如果块已经上锁，则不允许换出，除了在关闭数据库时，会强制释放，否则必须手动解锁后，才能换出。

###### 2.7.5.2 Page On Demand请求式分页

只有执行LRU算法或者关闭数据库时，才会强制将块的内容写回磁盘中，否则用户的所有操作，包括readFile, writeFile等均只在缓冲区完成，而暂不写回磁盘中（逻辑读写）。这里的请求式分页较为简单，以单个块为单位，不执行其它需要硬件支持的功能。

###### 2.7.5.3 Dirty Bit脏写

对于读入缓冲区中的块，在执行write操作时，会将块的脏位(dirty bit)置为1，与磁盘交换块时，只有脏的块才会写入，否则块直接被新的数据覆盖。这一部分的实现在函数writeBlocktoDisk()中。
