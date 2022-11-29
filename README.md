<h1 align="center">Distributed Password Cracker Using MPI and OpenMP</h1>

### Description
Password Cracker program coded in `C++ Language` by applying `Brute Force Algorithm` and `Parallelizing` it using MPI and OpenMP. You can look at detailed description [Here](https://github.com/SameetAsadullah/Distributed-Password-Cracker-Using-MPI-and-OpenMP/blob/main/Project%20Statement.pdf).

### Manual
1) Copy the content of `/etc/shadow` into a file `shadow.txt` and place it into your current working directory:
    ```
    sudo cat /etc/shadow > shadow.txt
    ```
    
2) Use following command to `Compile the Code`:
    ```
    mpic++ -fopenmp main.cpp -lcrypt -o main
    ```

3) Create a file named as `machinefile` in your current working directory. 
    ```
    touch machinefile
    ```

4) The `machinefile` should have the hostname of your pc and the number of processes you want to run on it in parallel i.e, sameet is the hostname of my pc and I want to run 5 processes in parallel. So, its format should be like:
    ```
    sameet:5
    ```

5) Use following commands to `Execute the Code`. Here 5 represents total number of processes to create:
    ```
    mpiexec -n 5 -f machinefile ./main
    ```
    
### Working Screenshots
<div align="center">
  <img src = "https://github.com/SameetAsadullah/Distributed-Password-Cracker-Using-MPI-and-OpenMP/blob/main/extras/working-ss-1.PNG" alt = "" width="700px"/>
</div>
<br/>
<div align="center">
  <img src = "https://github.com/SameetAsadullah/Distributed-Password-Cracker-Using-MPI-and-OpenMP/blob/main/extras/working-ss-2.PNG" alt = "" width="700px"/>
</div>
<br/>
<div align="center">
  <img src = "https://github.com/SameetAsadullah/Distributed-Password-Cracker-Using-MPI-and-OpenMP/blob/main/extras/working-ss-3.PNG" alt = "" width="700px"/>
</div>
