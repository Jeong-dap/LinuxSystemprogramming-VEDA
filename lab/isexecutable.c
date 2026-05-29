#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "minishell.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define OP_NONE 0
#define OP_PIPE 1
#define OP_OUT 2
#define OP_IN 3
#define OP_AND 4
#define OP_BACKGROUND 5

int get_op_type(char **args, char ***end)
{
    char **arg = args;
    for (; *arg != NULL; arg++)
    {
        if (strcmp(*arg, "|") == 0)
        {
            *arg = NULL;
            *end = arg;
            return OP_PIPE;
        }
        if (strcmp(*arg, ">") == 0)
        {
            *arg = NULL;
            *end = arg;
            return OP_OUT;
        }
        if (strcmp(*arg, "<") == 0)
        {
            *arg = NULL;
            *end = arg;
            return OP_IN;
        }
        if (strcmp(*arg, "&&") == 0)
        {
            *arg = NULL;
            *end = arg;
            return OP_AND;
        }
        if (strcmp(*arg, "&") == 0)
        {
            *arg = NULL;
            *end = arg;
            return OP_BACKGROUND;
        }
    }
    *end = arg;
    if (*args != NULL)
    {
        if ((*(arg - 1))[strlen(*(arg - 1)) - 1] == '&')
        {
            (*(arg - 1))[strlen(*(arg - 1)) - 1] = '\0';
            return OP_BACKGROUND;
        }
    }
    return OP_NONE;
}
// 주어진 입출력 파일 디스크립터로 명령어를 실행하는 함수
pid_t execute(char **arglist, int fd_in, int fd_out)
{
    pid_t pid = -1;
    if (arglist == NULL || arglist[0] == NULL)
        return -1;

    // 1. 임시로 표준 입출력 백업
    fflush(stdout);
    int saved_in = dup(STDIN_FILENO);
    int saved_out = dup(STDOUT_FILENO);
    if (saved_in == -1 || saved_out == -1)
    {
        perror("dup");
        goto exit_execute;
    }

    // 2. 리디렉션 적용
    if ((fd_in != -1 && dup2(fd_in, STDIN_FILENO) == -1) || (fd_out != -1 && dup2(fd_out, STDOUT_FILENO) == -1))
    {
        perror("dup2");
        goto exit_execute;
    }

    // 3. 빌트인 명령어 확인 및 실행
    if (check_builtin(arglist))
    {
        fflush(stdout); // 빌트인 명령어의 출력을 버퍼에서 리디렉션된 fd로 비움
        goto exit_execute;
    }

    switch (pid = fork())
    {
    case -1:
        perror("fork");
        goto exit_execute;
    case 0:
        execvp(arglist[0], arglist);
        perror("execvp");
        exit(EXIT_FAILURE);
    default:
        goto exit_execute;
    }
exit_execute: 
    if (saved_in != -1) {
        dup2(saved_in, STDIN_FILENO);
        close(saved_in);
    }
    if (saved_out != -1) {
        dup2(saved_out, STDOUT_FILENO);
        close(saved_out);
    }
    if(fd_in != -1) close(fd_in);
    if(fd_out != -1) close(fd_out);
    return pid;
}

void is_executable(char **arglist)
{
    char **cmd_arglist = arglist; // 현재 실행 대상이 될 명령어의 시작점
    char **curr_ptr = arglist;    // 다음 연산자를 탐색할 위치
    char **end = NULL;            // 실행 대상 인자의 끝을 가리키는 포인터
    int fd_in = -1;               // 현재 명령어에 적용될 입력 파일 디스크립터 (파이프 입력을 이월받음)
    int fd_out = -1;              // 현재 명령어에 적용될 출력 파일 디스크립터
    pid_t pids[1024];             // 파이프라인내의 모든 프로세스 id를 저장할 배열
    int pid_count = 0;
    int background = 0;
    pid_t pid;

chain_loop:
    end = NULL;
    int nxt_op = get_op_type(curr_ptr, &end);
    switch (nxt_op)
    {
    case OP_NONE:
        pid = execute(cmd_arglist, fd_in, fd_out);
        fd_in = -1;
        fd_out = -1;
        if (pid > 0)
            pids[pid_count++] = pid;
        break;

    case OP_PIPE:
        {
            int pipefd[2];
            if (pipe(pipefd) == -1)
            {
                perror("pipe");
                goto error_exit;
            }
            
            if (fd_out != -1)
                close(pipefd[1]);
            else
                fd_out = pipefd[1];

            // 현재 파이프 단락 실행
            pid_t p_pid = execute(cmd_arglist, fd_in, fd_out);
            fd_in = -1;
            fd_out = -1;
            if (p_pid > 0)
                pids[pid_count++] = p_pid;
            else {
                close(pipefd[0]);
                goto error_exit;
            }
            // 다음 루프를 돌기 위한 상태 초기화
            cmd_arglist = end + 1;
            curr_ptr = end + 1;
            fd_in = pipefd[0]; // 다음 명령의 입력으로 이월
            fd_out = -1;
            goto chain_loop;
        }

    case OP_OUT:
        {
            if (end == NULL || end[1] == NULL)
            {
                fprintf(stderr, "minishell: syntax error near '>'\n");
                goto error_exit;
            }
            if (fd_out != -1)
                close(fd_out); // 이미 설정된 리디렉션이 있다면 닫고 새로 열기

            fd_out = open(end[1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd_out == -1)
            {
                perror(end[1]);
                goto error_exit;
            }

            // 명령어 실행은 건너뛰고 파일명 다음 인자부터 스캔을 계속 진행
            // cmd_arglist(명령어의 첫 인자 정보)는 그대로 유지됩니다.
            curr_ptr = end + 2;
            goto chain_loop;
        }

    case OP_IN:
        {
            if (end == NULL || end[1] == NULL)
            {
                fprintf(stderr, "minishell: syntax error near '<'\n");
                goto error_exit;
            }
            if (fd_in != -1)
                close(fd_in); // 이전 파이프의 읽기 fd가 들어있었다면 닫고 교체

            fd_in = open(end[1], O_RDONLY);
            if (fd_in == -1)
            {
                perror(end[1]);
                goto error_exit;
            }

            curr_ptr = end + 2;
            goto chain_loop;
        }

    case OP_AND:
        {
            // 현재까지 구성된 명령어 실행 및 대기
            pid_t and_pid = execute(cmd_arglist, fd_in, fd_out);
            fd_in = -1;
            fd_out = -1;
            if (and_pid > 0)
                pids[pid_count++] = and_pid;

            // AND 연산자 이므로 이전 파이프라인의 모든 프로세스가 정상 종료될 때까지 대기
            int status;
            int last_status = 0;
            for (int i = 0; i < pid_count; i++)
            {
                waitpid(pids[i], &status, 0);
                if (pids[i] == and_pid)
                    last_status = status;
            }
            pid_count = 0; // pid 리스트 비우기

            // 앞 명령어가 실패(exit code != 0)했다면 && 뒤 명령어는 실행하지 않고 리턴
            if (WIFEXITED(last_status) && WEXITSTATUS(last_status) != 0)
                goto error_exit;

            // 성공했다면 계속 진행
            cmd_arglist = end + 1;
            curr_ptr = end + 1;
            fd_in = -1;
            fd_out = -1;
            goto chain_loop;
        }

    case OP_BACKGROUND:
        background = 1;
        pid_t bg_pid = execute(cmd_arglist, fd_in, fd_out);
        fd_in = -1;
        fd_out = -1;
        if (bg_pid > 0)
        {
            pids[pid_count++] = bg_pid;
            printf("[background] %d\n", bg_pid);
        }
        break;
    }

error_exit:
    // 백그라운드 실행이 아니면 파이프라인에 소속된 모든 자식 프로세스 종료 대기
    if (!background)
    {
        for (int i = 0; i < pid_count; i++)
            waitpid(pids[i], NULL, 0);
    }
    if (fd_in != -1) close(fd_in);
    if (fd_out != -1) close(fd_out);
}