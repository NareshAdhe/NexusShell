#include <iostream>
#include <unistd.h>
#include <string>
#include <sstream>
#include <vector>
#include <sys/wait.h>

using namespace std;

void convertToCharPointerArray(vector<char *> &args, vector<string> &tokens)
{
    for (string &token : tokens)
    {
        args.push_back((char *)token.c_str());
    }
    args.push_back(NULL);
}

void getTokens(vector<string> &tokens, string &command)
{
    stringstream ss(command);

    string arg;

    while (ss >> arg)
    {
        tokens.push_back(arg);
    }
}

void getCommand(int start, int end, vector<string> &command, vector<string> &tokens)
{
    for (int i = start; i < end; i++)
    {
        command.push_back(tokens[i]);
    }
}

int main()
{
    string command;

    while (true)
    {
        cout << endl
             << "NexusShell> ";

        getline(cin, command);

        if (command == "exit")
        {
            break;
        }

        vector<string> tokens;

        getTokens(tokens, command);

        int size = tokens.size();

        bool backgroundProcess = false;

        if (tokens.back() == "&")
        {
            backgroundProcess = true;

            tokens.pop_back();

            size = tokens.size();
        }

        vector<int> pipeIndices;
        for (int i = 0; i < size; i++)
        {
            if (tokens[i] == "|")
            {
                pipeIndices.push_back(i);
            }
        }

        int pipeSize = pipeIndices.size();

        for (int i = 1; i < pipeSize; i++)
        {
            if (pipeIndices[i] - pipeIndices[i - 1] == 1)
            {
                cout << "Invalid pipe command" << endl;
                continue;
            }
        }

        if (pipeIndices.front() == 0 || pipeIndices.back() == tokens.size() - 1)
        {
            cout << "Invalid pipe command" << endl;
            continue;
        }

        if (pipeSize > 0)
        {
            vector<vector<string>> commands(pipeSize + 1);

            for (int i = 0; i <= pipeSize; i++)
            {
                if (i == 0)
                {
                    getCommand(0, pipeIndices[i], commands[i], tokens);
                }
                else
                {
                    getCommand(pipeIndices[i - 1] + 1, pipeIndices[i], commands[i], tokens);
                }
            }

            getCommand(pipeIndices[pipeSize - 1] + 1, size, commands[pipeSize], tokens);

            int prevPipeRead = -1;

            for (int i = 0; i <= pipeSize; i++)
            {

                vector<char *> args;

                convertToCharPointerArray(args, commands[i]);

                int fd[2];
                if (i != pipeSize)
                {
                    pipe(fd);
                }

                pid_t pid = fork();

                if (pid == 0)
                {
                    if (prevPipeRead != -1)
                    {
                        dup2(prevPipeRead, STDIN_FILENO);
                        close(prevPipeRead);
                    }

                    if (i != pipeSize)
                    {
                        dup2(fd[1], STDOUT_FILENO);
                        close(fd[1]);
                    }

                    if (i != pipeSize)
                    {
                        close(fd[0]);
                        close(fd[1]);
                    }

                    execvp(args[0], args.data());

                    perror("execvp failed");
                    exit(1);
                }
                if(i != pipeSize){
                    close(fd[1]);
                    if(prevPipeRead != -1) close(prevPipeRead);
                    prevPipeRead = fd[0];
                }
            }

            for (int i = 0; i <= pipeSize; i++)
            {
                wait(NULL);
            }

            continue;
        }

        vector<char *> args;

        convertToCharPointerArray(args, tokens);

        if (tokens[0] == "cd")
        {
            if (tokens.size() > 1)
            {
                chdir(tokens[1].c_str());
            }

            continue;
        }

        pid_t pid = fork();

        if (pid == 0)
        {
            execvp(args[0], args.data());

            perror("execvp failed");

            exit(1);
        }

        if (backgroundProcess == false)
        {
            wait(NULL);
        }
    }
}