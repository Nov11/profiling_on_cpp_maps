用vs：点prof.sln
用clion(cmake)：进prof目录，双击CMakeLists.txt

***
问题：
```
我的的交易系统中有很多的Order需要非常快速地查询、添加、删除。
查询的时候，有些时候会根据front_session_ref 来查询，每个Order的front_session_ref是唯一的，例如《1,2,3》；
有些时候会查询同一个package_leg_id 对应的所有Order，package_leg_id不是唯一的。目前package_leg_id 是 《guid,int》。

咋存？ 

```
测试在test.cpp里。多线程用来加快动态分配内存，否则准备测试数据简直慢的要死。

####想法
想法：
把一个对象当成一条记录， front_session_ref 是主键， package_leg_id是一个属性。这样问题转化为怎么实现索引。
下面假设，这个是内存索引，不用写磁盘。如果内存放不下，得写磁盘的话，改用传统数据库好了。话说，内存数据库也可以考虑吧。
具体的说：
主键索引是稠密索引，插入删除新的记录要维护索引表。 实现上可以用平衡二叉树或者哈希表。
非主键索引可以用哈希表，每个桶里放键值一样的元素指针。其实平衡二叉树也行。

平衡二叉树 这个，具体实现可用红黑树，或者跳跃表也行。
哈希表，比方说传统哈希表，那种桶数目固定，用拉链法处理冲突的。理想情况是桶的数目随着键值数目增长而增长，可以用上线性散列表或者可扩展散列表。

红黑树么，可以用map。主键的用map，非主键的可以用multimap.
哈希表，就是unordered_map,unordered_multimap这两个。
跳跃表似乎boost和stl里都没现成的。

单从时间复杂度上来说，哈希表是常数的，二叉树是指数的，哈希表更好。

底层用unordered_系列容器，得给键值类型准备散列函数和==操作符

####双hash，双map组合
#####实验环境：
CPU：Intel i7-4700MQ
内存： 8G 1600HZ
使用VS Community 2015 自带的编译器。说明一下：这个题目和平台无关，编译器不会对结果造成实质影响，实验结果能说明问题。看邮件的时候用windows上网，顺手用VS写了。
debug/release均为默认配置。debug无优化，release O2优化。

#####验证方法：
假设插入/两种查找/删除操作的比重是相当的。
生成一定数量的order对象，依次执行插入，按主键查找，按package_leg_id查找，删除，统计整个过程的时间。
由于首次运行的结果与之后的相差较大，这里采用多次执行，任意选取第3次执行之后得出的时间作为实验数据。


```
//-------------------debug-------------------------------------
test result(debug): init bucket size:10000
data size        hash container takes(us)	avl container takes(us)
100000		    70326861					63934735
10000			7284512						6215236
1000			681339						566439
100			    69843						50600
debug模式下，hash container上的操作更慢。profiler显示算hash占用了不少时间。
```
```
//-------------------release-------------------------------------
test result(release): init bucket size:10000
data size		hash container takes(us)	avl container takes(us)
1000000		    5971483						6511570
100000		    588445						615483
10000			56498						58232
1000			5220						5406
100			    631							607
test result(release): init bucket size:100000
data size		hash container takes(us)	avl container takes(us)
1000000		    5975458						6497002
100000		    594449						623836
10000			55196						59085
1000			6387						6804
100			    624							657
test result(release): init bucket size:1000000
data size		hash container takes(us)	avl container takes(us)
1000000		    5740553						6559322					<--order objs(216bytes/obj) take at least 216MB
100000		    617890						633241
10000			62041						59514
1000			5358						5505
100			    518							580
```

相对于debug模式，release模式下计算hash的时间大幅下降，hash container在时间上更有优势。
以上是32位模式测的。扩大数据集，使用64位模式测试：
```
//-------------------release（64bit)--------------------------------
test result(release): init bucket size:1000000
data size		hash container takes(us)	avl container takes(us)
10000000		61723154					65170566				<--order objs(272bytes/obj) take at least 2720MB
1000000		    5454603						5862729
100000		    551399						545956
我的机器内存8G，数据集再增长的话，会导致频繁的换页。测试程序本身能跑满一个CPU核心，频繁换页跑满IO也不难，这种情况下机器基本没法用了，实际也测不了。
如果数据集大小超出内存容量，可以考虑用DBMS,比如redis，MySQL。
```


#####总结1：
从实验结果看，在release模式下，数据集在100000以上时，使用hash容器性能上更具优势。
数据集在100000以下时，两种容器差别不大。
hash容器初始bucket对执行时间的影响不明显，可按照预估数据集大小选取。
结果说明之前的想法是合理的。
若实际的数据集大小在100000左右，优先选hash容器。

***

####不同平台的实验结果不一样
但，linux上时间消耗跟预期的不同。因此考察了每个操作步骤的时间消耗。
例如VS Release模式，每个操作的执行时间是这样的：

```
data set of 1000000 is ready.
start test of hash
test of insert :		541629us
test of get :			310246us
test of range get :		3508650us
test of remove :		929010us
test of hash takes :	5293711us
start test of avl
test of insert :		785978us
test of get :			386025us
test of range get :		3634647us
test of remove :		1084213us
test of avl takes :		5894195us
```
hash版本在每一步上的用时都比较少。

同样的程序，linux上编译执行出来的结果是这样的：
（机器不同，linux机器性能差一些，CPU是Intel i5-2540M，使用ubuntu，g++版本5.4.0，O3优化）

```
data set of 1000000 is ready.
start test of hash
test of insert takes:		334961us
test of get takes:			225020us
test of range get takes:	3876166us
test of remove takes:		1699786us
test of hash takes:			6136015us
start test of avl
test of insert takes:		734608us
test of get takes:			304014us
test of range get takes:    3192544us
test of remove takes:		833587us
test of avl takes:			5064828us
```
hash版本在插入和按照主键查询的时候更有效率，而平衡二叉树的范围查找和删除操作更快。



数据集大小10000000的情况如下，可以发现，删除和范围查询的时间与插入和主键查询相差一个数量级。
```
data set of 10000000 is ready.
start test of hash
test of insert takes:		4554487us
test of get takes:			2489368us
test of range get takes:	39070648us
test of remove takes:		16515289us
test of hash takes:	63052976us
start test of avl
test of insert takes:		8864648us
test of get takes:			4097757us
test of range get takes:	32800296us
test of remove takes:		10158488us
test of avl takes:	56176627us
```

在数据量小于1000000时，hash container在linux上的总时间更长，只有插入操作比二叉树快。
数据量是10000000时，hash container 在插入和主键查找上更快，范围查找和删除速度比二叉树慢。


#####总结2：
综上，选择数据结构需要考虑编译器和操作数量。
VS上hash container性能全线优于二叉树，g++下hash的插入和主键查询优于二叉树，而范围查询和删除则是二叉树更快。Clang应用范围不广，不测了。
同一份使用stl的代码，运行时间差异不同，可推断这些差异与stl库的实现有关。

只讨论stl,假定最终运行在linux平台上，且数据量在1000000以上:
* 如果四种操作中主键查询的操作比重更高，则适用unorder_map。
* 如果经常删除，则适用二叉树。因为删除的时间消耗比插入和主键选择多2倍左右。

***

####混合hash和二叉树
既然linux上unordered_map插入和主键选择性能好，map系列的删除和范围查询更快，把这两种hash和二叉树组和起来，看看效果。
试着用hash建主键索引，用二叉树建属性索引(Hybrid.h)，或者对换一下(H2.h)。

只选取一段数据集10000000的，数据集小两个数量级的见txt文档
```
data set of 10000000 is ready.
start test of hash
test of insert takes:   4168696us
test of get takes:      2409123us
test of range get takes:        39530000us
test of remove takes:   16800919us
test of hash takes:     62908828us
start test of avl
test of insert takes:   8952765us
test of get takes:      4036392us
test of range get takes:        32949870us
test of remove takes:   10167489us
test of avl takes:      56111378us
start test of hybrid
test of insert takes:   6868514us
test of get takes:      2765535us
test of range get takes:        33195956us
test of remove takes:   7027907us
test of hybrid takes:   49857982us
start test of h2
test of insert takes:   6314419us
test of get takes:      4223341us
test of range get takes:        39723745us
test of remove takes:   20210080us
test of h2 takes:       70471657us
```
#####总结3：
总时间上，hybrid版本更好。
在1000000， 100000也是hybrid版本更好。
hybrid版本在删除上更快，范围查找速度与纯二叉树版本类似。

***

####结论
* 如果目标平台是linux，可以选择用unordered_map对主键做索引，用multimap对属性做索引。
* 用VS的话，混合的版本没有明显优势，首选unordered_map/unordered_multimap组合。
