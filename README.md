# leaderElection
The source implement the leader election in distributed environment by zookeeper.
It works as a zookeeper client, connecting the server on the beginning of startting up.
The detailed steps are as below:
1. connect to the specified zookeeper servers;
2. create "/election" znode with default type, which is permanent;
3. create the child znode of "/election" with name specified in the first shell command parameter;
4. get the children of "/election" and set the watcher function so that it'll be informed when the list of children changes;
5. analyse the children list, and find the name of znode who has the smallest number, and that is the leader.

