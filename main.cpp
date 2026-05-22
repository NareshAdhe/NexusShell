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

        int pipeIndex = -1;
        for (int i = 0; i < size; i++)
        {
            if (tokens[i] == "|")
            {
                pipeIndex = i;
                break;
            }
        }

        if (pipeIndex == 0 || pipeIndex == tokens.size() - 1)
        {
            cout << "Invalid pipe command" << endl;
            continue;
        }

        if (pipeIndex != -1)
        {
            vector<string> leftTokens, rightTokens;

            getCommand(0, pipeIndex, leftTokens, tokens);
            getCommand(pipeIndex + 1, size, rightTokens, tokens);

            vector<char *> leftCommand, rightCommand;
            
            convertToCharPointerArray(leftCommand, leftTokens);
            convertToCharPointerArray(rightCommand, rightTokens);

            int fd[2];
            pipe(fd);

            pid_t pid1 = fork();

            if (pid1 == 0)
            {
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                execvp(leftCommand[0], leftCommand.data());
            }

            pid_t pid2 = fork();
            if (pid2 == 0)
            {
                close(fd[1]);
                dup2(fd[0], STDIN_FILENO);
                close(fd[0]);
                execvp(rightCommand[0], rightCommand.data());
            }

            close(fd[0]);
            close(fd[1]);

            wait(NULL);
            wait(NULL);

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
        }

        wait(NULL);
    }
}