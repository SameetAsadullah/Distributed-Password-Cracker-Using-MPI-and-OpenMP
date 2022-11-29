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

3) Create a file named as `machinefile` in your current working directory:
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

### Side Notes
- The `/etc/shadow` file stores all the users of the pc and their respective passwords in encryped form.
- The encryption of their passwords are done using `SHA-512 Algorithm` in Ubuntu Linux.
- We can encrypt any string using the `crypt()` function from `crypt.h Library`. It returns us Salt and Encyrpted Password both in a single appended string. It requires two arguments:
    - The string to encrypt.
    - The salt used to encrypt the string.
- We can extract the salt used to encrypt the password from `shadow` file and then use it to encrypt all the strings obtained from brute force and compare with the password.
- In `shadow` file, the username, salt, and password are stored in the following format:
    - `Username` before first `:`
    - Then `Salt` until three `$` signs
    - And `Encrypted Password` after that

### Contributors
- [Sameet Asadullah](https://github.com/SameetAsadullah) 
- [Tayyab Ali](https://github.com/DarkDragz)
- [Tayyab Abbas](https://github.com/tayyababbas2000)
- [Aysha Noor](https://github.com/ayshanoorr)
