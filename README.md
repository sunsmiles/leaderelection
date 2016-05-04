# leaderElection
The source implement the leader election in distributed environment by zookeeper.
It works as a zookeeper client, connecting the server on the beginning of startting up.
##The detailed steps are as below:
1. connect to the specified zookeeper servers;
2. create "/election" znode with default type, which is permanent;
3. create the child znode of "/election" with name specified in the first shell command parameter;
4. get the children of "/election" and set the watcher function so that it'll be informed when the list of children changes;
5. analyse the children list, and find the name of znode who has the smallest number, and that is the leader.

##Build
Before compile the leaderElection, the zookeeper environment should be installed.
Download the source codes of zookeeper from [here](http://mirrors.cnnic.cn/apache/zookeeper/zookeeper-3.4.8/zookeeper-3.4.8.tar.gz),
or clone source code from github: git clone git@github.com:apache/zookeeper.git.
Decompress the package, then enter into the root directory zookeeper-3.4.8, run the following shell commands to install the zookeeper c language environment:
~~~shell
cd src/c
./configure
make
sudo make install 
~~~
The default installation location for zookeeper libraries are /usr/local/lib, and /usr/local/include/zookeeper for header files.

Add the following line into the end of ~/.bashrc, then run the command "source ~/.bashrc"
~~~shell
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
~~~

Now the leaderElection can be compiled correctly, by enter into its root directory and run make.

##Getting Started
The executable file is in the directory: dist/Debug/GNU-Linux, after enter into this directory, you can run the follow commands:
~~~shell
./leaderelection 172.17.0.1:2181 aa
./leaderelection 172.17.0.1:2181 bb
./leaderelection 172.17.0.1:2181 cc
~~~

##Examples

~~~shell
root@32552712113b:~/le/dist/Debug/GNU-Linux# ./leaderelection 172.17.0.1:2181 bb
aa0000000000
bb0000000001
aa, 0000000000, 0
bb, 0000000001, 1
znodeName: bb, smallest node: aa
I'm follower of aa


root@32552712113b:~/le/dist/Debug/GNU-Linux# ./leaderelection 172.17.0.1:2181 aa
aa0000000000
aa, 0000000000, 0
znodeName: aa, smallest node: aa
I'm the leader now
node list changing, type:4, state: 3, path: /electionaa0000000000
bb0000000001
aa, 0000000000, 0
bb, 0000000001, 1
znodeName: aa, smallest node: aa
I'm the leader now
~~~
