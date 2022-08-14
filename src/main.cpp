#include <iostream>
#include "mpi.h"
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <unistd.h>
#include <stdio.h>
#include <crypt.h>
#include <fstream>
using namespace std;

// function to find all combinations of passwords starting with character passed as 'pswd' 
// return empty string if password not found or any other process has found the password
string bruteForce(string pswd, string line, string str_salt, char *&abort_message)
{
	char *encrypted;
	int ind = pswd.length() - 1;

	do
	{
        if (strcmp(abort_message, "ABORT") == 0) {  // if password is found by any other process then abort
            return "";
        }

		encrypted = crypt(pswd.c_str(), str_salt.c_str()); 
		
		if (!strcmp(encrypted, line.c_str()))       // if password is found then return the password
		{
			cout << "\nPassword found! It is -> " << pswd << endl;
			return pswd;
		}
		else
		{
			if (ind == 0)   // staring with initial character and appending one more character
			{
				pswd += 'a';
				ind = pswd.length() - 1;
				continue;
			}

			pswd[ind] += 1;
			
			if (pswd[ind] > 122)    // if character has reached 'z'
			{
				int i = ind;
				while (i > 0)   // back traverse and increment the character
				{
                    if (strcmp(abort_message, "ABORT") == 0) {  // if password is found by any other process then abort
                        return "";
                    }

					pswd[i] += 1;
					
					if (i > 1 && pswd[i] > 122 && pswd[i-1] != 'z') // brute force - checking all combinations
					{
						pswd[i - 1] += 1;
						while (i <= ind)
						{
							pswd[i] = 'a';
							i++;
						}
						break;
					}
					
					else if (i == 1 && pswd[i] > 122)   // increasing length by appening 'a' at the end
					{
						while (i <= ind)
						{
							pswd[i] = 'a';
							i++;
						}
						
						pswd += 'a';
						ind = pswd.length() - 1;
						break;
					}
					
					i--;
				}
			}	
		}
			
	} while (strcmp(encrypted, line.c_str()) && pswd.length() < 9);

	return "";
}

// main function
int main(int argc, char **argv)
{
    int rank, root = 0, nprocs, namelen;
    char processorName[10];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Get_processor_name(processorName, &namelen);

    if (nprocs == 1) {
        cout << "Processes must be greater than 1 !!!" << endl;
        return 0;
    }

    int size = 26;
    int alphabets_for_slaves = size / (nprocs - 1);
    int alphabets_for_master = size % (nprocs - 1); 

    if (rank == root) {     // master process
        bool check = 0;
        string user_name, line, temp, str_salt;

        // taking user_name input
        cout << "Enter user name for which to crack password: ";
        cin >> user_name;
        
        // file reading for /etc/shadow
        fstream file("shadow.txt", ios::in);
        while (file)
        {
            file >> line;
            if (line.length() > user_name.length())
            {
                temp = line.substr(0,user_name.length());
                if (temp == user_name)  // if user-name exists in the file
                {
                    check = 1;
                    int dollar_count = 0;
                    line = line.substr(user_name.length() + 1, line.length());
    
                    int i = 0;
                    // extracting salt from the line
                    while (i < line.length())
                    {
                        if (line[i] == '$')
                        {
                            dollar_count++;
                        }
                            
                        if (line[i] == ':')
                        {
                            line = line.substr(0, i);
                            break;
                        }
                        
                        if (dollar_count == 3)
                        {
                            str_salt = line.substr(0, i+1);
                            dollar_count++;
                        }   
                        i++;
                    }
                    break;
                }
            }
        }
        file.close();

        // converting string into char* to send to the slaves
        char *temp_line = new char[line.size() + 1];
		line.copy(temp_line, line.size() + 1);
		temp_line[line.size()] = '\0';

        // converting string into char* to send to the slaves
        char *temp_salt = new char[str_salt.size() + 1];
		str_salt.copy(temp_salt, str_salt.size() + 1);
		temp_salt[str_salt.size()] = '\0';

        // sending sizes and necessary information to slaves
        int starting_index = alphabets_for_master + 1;
        int line_size = line.size() + 1, salt_size = str_salt.size() + 1;
        for (int i = 1, j = 0; i < nprocs; ++i) {
            MPI_Send(&line_size, 4, MPI_INT, i, 1230, MPI_COMM_WORLD);
            MPI_Send(&salt_size, 4, MPI_INT, i, 1231, MPI_COMM_WORLD);
            MPI_Send(temp_line, line_size, MPI_CHAR, i, 1232, MPI_COMM_WORLD);
            MPI_Send(temp_salt, salt_size, MPI_CHAR, i, 1233, MPI_COMM_WORLD);
            MPI_Send(&starting_index, 4, MPI_INT, i, 1234, MPI_COMM_WORLD);
            starting_index += alphabets_for_slaves;
        }

        // making 2 threads - one will call brute force to find password on master if needed - second will receive from slaves if anyone has found the password
        // and then sends abort message to all other processes as well
        char *abort_message = new char[6];
        #pragma omp parallel num_threads(2)
        {
            if (omp_get_thread_num() == 0) {    // to do brute force for master
                for (int i = 0; i < alphabets_for_master; ++i) {
                    string alphabet = "";
                    alphabet += 97 + i;
                    string recv = bruteForce(alphabet, line, str_salt, abort_message);
                    if (recv != "") {   // if password has found
                        cout << "Master process on " << processorName << " machine has cracked the password :-)\n";
                        strcpy(abort_message, "ABORT");
                        break;
                    }
                }
            }

            else {  // to receive from slaves if anyone has found the password and then send abort message to all other processes
                MPI_Request recvRequest;
                MPI_Status recvStatus;
                int flag = 0;
                char data[28];

                // receiving from processes
                MPI_Irecv(data, 28, MPI_CHAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &recvRequest);
                while(!flag) {
                    MPI_Test(&recvRequest, &flag, &recvStatus);
                    if (strcmp(abort_message, "ABORT") == 0) {
                        strcpy(data, "I have found the number :-)");
                        break;
                    }
                }

                // password found so abort other processes
                if (flag) {
                    strcpy(abort_message, "ABORT");
                }

                // sending message to all other processes to abort
                if (strcmp(data, "I have found the number :-)") == 0) {
                    if (nprocs != 2) {
                        cout << "Aborted all processes on other machines !!!" << endl;
                    }
                    for (int i = 1; i < nprocs; ++i) {  // telling slaves to abort search
                        MPI_Send(abort_message, 6, MPI_CHAR, i, 1236, MPI_COMM_WORLD);
                    }
                }
            }
        }
    } 

    else {    // slave processes

        // receiving sizes from the master
        int starting_index, line_size, salt_size;
        MPI_Recv(&line_size, 4, MPI_INT, 0, 1230, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&salt_size, 4, MPI_INT, 0, 1231, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // initialising arrays and then receiving information i.e. salt, hash from the master
        char *line = new char[line_size], *str_salt = new char[salt_size];
        MPI_Recv(line, line_size, MPI_CHAR, 0, 1232, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(str_salt, salt_size, MPI_CHAR, 0, 1233, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&starting_index, 4, MPI_INT, 0, 1234, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        char *abort_message = new char[6];
        #pragma omp parallel num_threads(2) // making two threads, one for cracking passcode and one for receiving abort message from master
        {
            if (omp_get_thread_num() == 0) {    // thread to crack passcode
                for (int i = 0; i < alphabets_for_slaves; ++i) {    // brute force on all characters
                    string alphabet = "";
                    alphabet += (96 + i + starting_index);
                    string recv = bruteForce(alphabet, line, str_salt, abort_message);
                    if (recv != "") {   // if password found then send message to master
                        char message[] = "I have found the number :-)";
                        MPI_Send(message, 28, MPI_CHAR, 0, 1235, MPI_COMM_WORLD);
                        cout << "Slave " << rank << " process on " << processorName << " machine has cracked the password :-)\n";
                        break;
                    }

                    if (strcmp(abort_message, "ABORT") == 0) {  // if password has found by any other process then abort
                        break;
                    }
                }

            } else {    // thread to receive abort message
                MPI_Recv(abort_message, 6, MPI_CHAR, 0, 1236, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
    }
    MPI_Finalize();
}