# final-project-61

## Cavale
- Backup ss
- LRUs
- Implement operation in the backup as well
- Don't forget tries in integrate copy
- Locks for every file in the trie
- While copying files, send ACK

## Hardik
- Reader Writer Locking
- Error codes and ACK of operation completion to the client

## Mi$hra
- ss.c handles multiple requests at the same time concurrently. Currently we have the handle_connections function. We need to convert it to a thread.
- colours
- fix DELETE
- Change functions in Trie thing

# CHATGPT Links
- <a href="https://chat.openai.com/share/2d643ad0-587f-4fa7-93f5-2c7f51409ab2" target="_blank">ChatGPT Link</a>

# ASSUMPTIONS
- Client exits using the EXIT command onnly
- Copying files works only for .txt files
- If a file has not been created yet, then you can not write into it.
- 
