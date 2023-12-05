# OSN Course Project Evaluation

## Some notes

- Only POSIX libraries are allowed (show this to me in your code)
- Only 3 executables must be run (NS, SS, Client). However the client and SS will ofcourse run in multiple terminals.
- Changing the directory structure will lead to a penalty.

## Initialisation Section

### Initialise naming server

```bash
# Open a new terminal
# start running the Naming Server
```

Only one NM should be running at any given moment.

- Show that the NM starts working.

### Connecting storage servers

Open terminals in the following directories, run your storage server executable and give the given paths as the accessible paths

_SS1_

`root_directory/dir_pbcui`

    /dir_bzaca
    /dir_bzaca/file_ofr.txt
    /dir_bzaca/file_qsw.txt
    /dir_bzaca/file_uag.txt
    /dir_bzaca/dir_ccftl/dir_hbsfj
    /dir_qkdez/dir_htcrv/dir_mrrcz
    /dir_qkdez/dir_htcrv/dir_mrrcz/file_imz.txt

_SS2_

`root_directory/dir_gywnw/dir_fzxpq`

    /dir_rzuni/dir_eqfej
    /dir_rzuni/dir_eqfej/file_wux.txt
    /dir_rzuni/dir_aqmhk

_SS3_

`root_directory/dir_vhfih/dir_lhkzx`

    /dir_ctpcj
    /dir_ctpcj/dir_apsck
    /dir_lhkzx/dir_ctpcj/dir_apsck/file_hwa.txt
    /dir_ctpcj/dir_apsck/file_rkn.txt
    /dir_ctpcj/dir_hfccx
    /dir_ctpcj/dir_hfccx/here.txt


_SS4_ 

`root_directory/dir_psjio/dir_ctyuy`

    /dir_bqays
    /dir_bqays/dir_adecw/file_qdh.txt
    /dir_bqays/dir_hlxuh
    /dir_bqays/dir_hlxuh/file_xmu.txt
    /dir_bqays/dir_hlxuh/file_rwf.txt
    /dir_bqays/dir_hlxuh/file_zsb.txt


_SS5_

`root_directory/dir_psjio/dir_kgabk/dir_jtngu`

    /dir_sptuh
    /dir_sptuh/file_nda.txt
    /dir_sptuh/file_ytw.txt

_SS6_

`root_directory/dir_vymdo`

    /dir_pfaau
    /dir_pfaau/dir_cscou
    /dir_pfaau/dir_cscou/dir_mbbdr
    /dir_pfaau/dir_cscou/dir_mbbdr/file_mpe.txt
    /dir_pfaau/dir_cscou/dir_rjmdf
    /dir_pfaau/dir_cscou/dir_rjmdf/file_kdx.txt
    /dir_pfaau/dir_fpkwl
    /dir_pfaau/dir_fpkwl/dir_cgpqn
    /dir_pfaau/dir_fpkwl/dir_gxueg
    /dir_picqp/dir_odqtp
    /dir_picqp/dir_picaz/dir_cdsdh
    /dir_picqp/dir_picaz/dir_cdsdh/file_dar.txt
    /dir_picqp/dir_picaz/dir_cdsdh/file_yyj.txt
    /dir_picqp/dir_picaz/dir_zdoid
    /dir_picqp/dir_picaz/dir_tnaqz
    /dir_picqp/dir_picaz/dir_tnaqz/file_kox.txt

- Show that SS1 and SS2 connect.
- Then show that SS2-SS6 connect.

### Connecting clients

Open 15 different terminals and run the client executable

Two cases:
- Only one client clients
- Other 14 clients also connect

## Operations

*Run each of the cases and show that it works*

### Reading

1. The client asks to read `/dir_ctpcj/dir_hfccx/here.txt`
2. The NM returns the port of SS3
3. The client communicates with SS3
4. The file's contents are printed (`cat root_directory/dir_vhfih/dir_lhkzx/dir_ctpcj/dir_hfccx/here.txt` if you want to see what's inside)
5. The client stops communicating with SS3

**Error Testing:**

1. repeat the same steps as above for reading `/dir_picqp/dir_picaz/dir_tnaqz/file_lcchehe.txt`

2. repeat the same steps for reading `/dir_picqp/dir_picaz/dir_tnaqz`

3. repeat the same steps for reading `/dir_picqp/dir_picaz/dir_tnaqz/file_lcc.txt`

### Writing

1. The client asks to read `/dir_pfaau/dir_cscou/dir_mbbdr/file_mpe.txt`
2. The NM returns the port of SS6
3. The client communicates with SS6
4. Make a few changes (you might need to pass the required changes in step 1 if they have implemented it like that)
5. The file's contents are changed (`cat root_directory/dir_vymdo/dir_pfaau/dir_cscou/dir_mbbdr/file_mpe.txt` if you want to see what's inside)
6. The client stops communicating with SS6

**Error Testing:**
1. repeat the same steps for reading `/dir_picqp/dir_picaz/dir_tnaqz/file_lcchehe.txt`
2. repeat the same steps for reading `/dir_picqp/dir_picaz/dir_tnaqz`

### Getting info about files

1. The client asks to read `/dir_pfaau/dir_cscou/dir_mbbdr/file_mpe.txt`
2. The NM returns the port of SS6
3. The client communicates with SS6
4. The file's perms are printed (`ls root_directory/dir_vymdo/dir_pfaau/dir_cscou/dir_mbbdr/file_mpe.txt` if you want to see the perms)
5. The client stops communicating with SS6
6. Manually change the perms of the file using `chmod` and then repeat steps 1-5 and show that the changes are reflected.

**for directories:**
- Now, repeat the same steps for reading `/dir_picqp/dir_picaz/dir_tnaqz`

**Error Testing:**
1. repeat the same steps for reading `/dir_picqp/dir_picaz/dir_tnaqz/file_lcchehe.txt`

### Creating

1. Client asks to create a file `whats.txt` in `/dir_bqays/dir_hlxuh`
2. NM figures out that the path is in SS4
3. NM creates the file
4. NM informs the client that the file has been created.

**Parallel Commands:**

One client should send the first request and another client the second.

1. Client asks to create a directory `tempqqh` in `/dir_bqays/dir_hlxuh`
2. NM figures out that the path is in SS4
3. NM creates the directory
4. NM informs the client that the file has been created.

1. Client asks to create a file `hi.txt` in `/dir_bqays/dir_hlxuh`
2. NM figures out that the path is in SS4
3. NM creates the file
4. NM informs the client that the file has been created.

**Error Testing:**

1. Client asks to create a directory `tempqqh2` in `/dir_bqays/dir_hlxuh/tempqqh`
2. NM figures out that the path is in SS4
3. NM creates the directory
4. NM informs the client that the file has been created.

### Deleting

**Case 1:**
1. Client asks to delete the file `/dir_bqays/dir_hlxuh/whats.txt`
2. NM figures out that the path is in SS4
3. NM deletes the file
4. NM informs the client that the file has been deleted.

**Case 2:**
1. Client asks to delete the folder `/dir_rzuni/dir_aqmhk`
2. NM figures out that the path is in SS2
3. NM deletes the directory
4. NM informs the client that the file has been deleted.

**Case 3:**
1. Client asks to delete directory `/dir_bqays/dir_hlxuh/tempqqh`
2. NM figures out that the path is in SS4
3. NM delets the directory
4. NM informs the client that the file has been created.

**Important:**
Show that the copying and deletion are not being done using absolute paths in the naming server. The NS should tell the SS's to do the needful.

### Copying

Before everything, connect SS7 and SS8:

_SS7_

`root_directory/a_song_ice_fire`

    /plot_dir
    /plot_dir/plot.txt

_SS8_

`root_directory/gravity_falls`

    /gravity_subdir
    /gravity_subdir/so-good.webp
    /gravity_subdir2

*Both of them should connect*

If the two new SS don't connect, run the following with some other file and non-empty directories.

**Case 1:**
1. Client asks to copy `/plot_dir/plot.txt` to `/gravity_subdir`
2. NM notices that `/plot_dir/plot.txt` is in SS7
3. NM copies plot.txt to SS8's `/gravity_subdir` directory
4. NM informs client
5. `/gravity_subdir/plot.txt` should have been created

**Case 2:**
1. Client asks to copy the directory `/dir_bqays/dir_hlxuh` to `/gravity_subdir`
2. NM notices that `/dir_bqays/dir_hlxuh` is in SS8
3. NM copies the directory and all the _ACCESSIBLE_ paths within it to SS8's `/gravity_subdir` directory
4. NM informs client
5. Show whether copying has been done correctly

**Case 3:**
1. Client asks to copy `/gravity_subdir/intro.txt` to `/gravity_subdir2`
2. NM notices that `/gravity_subdir/intro.txt` is in SS8
3. NM copies intro.txt to SS8's `/gravity_subdir2` directory
4. NM informs client
5. Show if the necessary has been done

## Other specs

### Multiple Clients

**Case 1:**
1. Ask 10 clients to read 10 different files at once (like write the commands in all of their terminals and hit enter super fast on all of them)
2. Atleast 7 should be able to read the files. The other 3 can show a timeout message.

**Case 2:**
1. Ask 10 clients to read the same file at once (like write the commands in all of their terminals and hit enter super fast on all of them)
2. Atleast 7 should be able to read the file. The other 3 can show a timeout message.

**Case 3:**
1. Ask 5 clients to write the same file at once (like write the commands in all of their terminals and hit enter super fast on all of them)
2. Only one of them should be able to.

**Case 4:**
1. Ask 3 clients to write the same file and 2 clients to read from it at once (like write the commands in all of their terminals and hit enter super fast on all of them)
2. Only one of them should be able to write but both the readers should either be able to read or shown a message that they can't.

### Error codes

These can be numeric, alphanumeric, alphabetical or even entire phrases.

- Show the definitions
- Show the part of the code where they are comminicated back to the client.

### Bookkeeping

1. Logging and Message Display: Implement a logging mechanism where the NM records every request or acknowledgment received from clients or Storage Servers. Additionally, the NM should display or print relevant messages indicating the status and outcome of each operation. This bookkeeping ensures traceability and aids in debugging and system monitoring.

- show that you are logging neccessary info in the NM

IP Address and Port Recording: The log should include relevant information such as IP addresses and ports used in each communication, enhancing the ability to trace and diagnose issues.

1. Whenever an SS connects, its port and IP address (NOTE: it's fine if it's only the port) are stored in the log/printed by the NM

- show that you are logging the above.

### Search in Naming Server

1. Search optimisation
2. LRU Cache

- show code for both functionalities.

### Redundancy

/dir_ctpcj/dir_apsck/file_rkn.txt - SS3

SSx and SSy - backup servers for SS3.

- Close SS3
    - Client should be able to read `/dir_ctpcj/dir_apsck/file_rkn.txt`

- Close SSx
    - client should be able to read `/dir_ctpcj/dir_apsck/file_rkn.txt`

- Close SSy
    - client should NOT be able to read `/dir_ctpcj/dir_apsck/file_rkn.txt`

1. Bring SS3 back online. (Run it in the original directory and give it the same set of accessible path).
    - I should be able to write to `/dir_ctpcj/dir_apsck/file_rkn.txt`
2. Call them SSx and SSy - servers storing the redundant information now.

1. Write some changes to `/dir_ctpcj/dir_apsck/file_rkn.txt`
2. Close SS3
3. Wait for about 30 seconds

- I should be able to write to `/dir_ctpcj/dir_apsck/file_rkn.txt`
- I should be able to access  `/dir_ctpcj/dir_apsck/file_rkn.txt` after closing SS3.

**Important:**
- Show in your code that you are not using absolute paths.
- Show that the server goes offline when asked to. 

### Copying cont.

Finally, 

1. Close all the SS
2. Close all the clients
3. Close the NM.
4. Start the NM.
5. Connect SS1.
6. Connect SS9 (given below)

_SS9_ : small stress test SS for copying

`root_directory/dir_gywnw/dir_fzxpq/dir_wewny`

    /dir_juqnm/file_pzb.txt
    /dir_juqnm/file_veg.txt
    /dir_juqnm/file_vpx.txt
    /dir_juqnm/big_dir_test
    /dir_juqnm/big_dir_test/big_file_70MB.txt
    /dir_juqnm/big_dir_test/med_file_10MB.txt
    /dir_juqnm/big_dir_test/med_file_15MB.txt

- I should be able to copy /dir_juqnm/big_dir_test/med_file_10MB.txt to anywhere is SS1
- I should be able to copy /dir_juqnm/big_dir_test/med_file_15MB.txt to anywhere is SS1
- I should be able to copy /dir_juqnm/big_dir_test/big_file_70MB.txt to anywhere is SS1