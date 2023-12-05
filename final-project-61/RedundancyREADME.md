# Doubts.md 
1. Should we acoount for ss disconnection during file tranfer?
2. Are we closing all the socket_fds
3. If Naming servef get disconnected in the middle do we need to handle stuff.
4. Client port not being sent properly in the storage to naming server thing.
5. num_ss pe lock lagana hai na?


# Changes I'm Making :
1. DEFINED THE BUFFERSIZE
2. ADDED THE CODE TO SEND INFO FROM SS TO NS FOR BACKING UP AND PUTTING IN TRIE
3. RECIEVING SHIT IN NS.