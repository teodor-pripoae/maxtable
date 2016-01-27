Maxtable provides high performance combined with availability, scalability and reliability. It is intended to be used as the backend storage infrastructure for data intensive apps such as MetaData server,MicroBlog,Data analysis,KeyBase etc.

Maxtable stores data in a table, sorted by a primary key(the first column defaultly). There are two types for data in the table, varchar and int. Scaling is achieved by splitting tables into contiguous ranges and assigning them up to different physical machines. There are two types of servers in a Maxtable cluster, Ranger Servers which hold some ranges of the data and Meta Servers which handle meta management works and oversee the Ranger Servers. A single Range Server may hold many discontinuous ranges, the Meta Server is responsible for farming them out in an intelligent way. If a single range fills up, the range is split in half(middle-split). The top half of the range remain in the current range and allocate a new range to save the lower half of the range, two ranges still locate at the current Ranger Server till the Ranger Server become overload, the Rebalancer will trigger Meta Server to reassign some ranges of the data locating at the overload Ranger Servers to other Range Servers that have enough space. The default maximum range size is 1000MB. The list of ranges and where they live is stored in a table called rangerserverlist that actually lives within Meta Server.

MaxTable has been deployed in production settings on large clusters to manage multiple terabytes of storage.

MaxTable is implemented in C language.

MaxTable source code is released under the terms of the GNU General Public License Version 3.0.

To become a contributor to this project, please send e-mail to yingfei.xue@gmail.com