# 리눅스 시스템 프로그래밍 (LSNP)

리눅스 시스템 프로그래밍 강의 실습 코드 저장소입니다.  
각 챕터별 예제(ex*.c)와 미니쉘 LAB 프로젝트로 구성되어 있습니다.

---

## 디렉토리 구조

```
lsnp/
├── ch01/       # 시간 함수 (time, ctime)
├── ch02/       # 라이브러리 작성 기법
├── ch03/       # 파일 시스템 (stat, 파일 종류, 권한, inode)
├── ch04/       # 디바이스 파일 & 터미널 I/O (open/read/write, termios)
├── ch05/       # 프로세스 생성 (system, fork, exec)
├── ch06/       # 시그널 (sigset, sigprocmask, sigaction)
├── ch06-2/     # 프로세스 그룹 & 세션 (setpgid, kill to group)
├── ch07/       # IPC — 파이프 (popen, pipe, dup+exec)
└── lab/        # 미니쉘 프로젝트
```

> **gitignore 적용 항목**  
> `lecture_notes/` (강의 자료 PDF), 컴파일된 바이너리(`ex01`~`ex09`, `myshell` 등),  
> 오브젝트 파일(`*.o`), ch03 런타임 데이터(`data`, `data1`, `data2`, `myhosts`)

---

## 챕터별 학습 내용

### ch01 — 시간 함수
| 파일 | 내용 |
|------|------|
| ex01.c | `time()`으로 유닉스 타임스탬프 획득 |
| ex02.c | `ctime()`으로 사람이 읽기 쉬운 날짜 문자열 변환 |

### ch03 — 파일 시스템
| 파일 | 내용 |
|------|------|
| ex01.c | `stat()` + `S_IS*` 매크로로 파일 종류 및 권한 판별 |
| ex02–ex05.c | 하드 링크, 심볼릭 링크, 디렉토리 순회 |

### ch04 — 디바이스 파일 & 터미널
| 파일 | 내용 |
|------|------|
| ex01.c | `/dev/pts/N`에 `open`/`write`로 터미널 출력 |
| ex02.c | `termios`로 raw 모드 전환 — 비밀번호 마스킹 구현 |

### ch05 — 프로세스 생성
| 파일 | 내용 |
|------|------|
| ex01.c | `system()`으로 쉘 명령 실행 (내부: fork → exec → wait) |
| ex02.c | `fork()`로 부모/자식 분기 |
| ex03–ex06.c | `exec` 계열 함수, `wait`/`waitpid` |

### ch06 — 시그널
| 파일 | 내용 |
|------|------|
| ex01.c | `sigfillset` + `sigdelset` + `sigprocmask`로 시그널 마스킹 |
| ex02–ex05.c | `signal()`, `sigaction()`, `pause()`, `alarm()` |

### ch06-2 — 프로세스 그룹 & 세션
| 파일 | 내용 |
|------|------|
| ex01.c | `setpgid()`로 새 프로세스 그룹 생성, `kill(-pgid)`로 그룹 전송 |
| ex02–ex06.c | 세션 리더, 제어 터미널, SIGHUP |

### ch07 — IPC (파이프)
| 파일 | 내용 |
|------|------|
| ex01.c | `popen("grep 'Hello'", "w")`으로 파이프 출력 |
| ex02.c | `pipe()` + `fork()`로 자식→부모 단방향 통신 |
| ex03.c | `dup()` + `execlp("ls")`로 stdout을 파이프로 리다이렉트 |

---

## LAB — 미니쉘 (`lab/`)

`fork` + `exec` + 시그널 처리를 결합한 미니 POSIX 쉘입니다.

### 파일 구성

| 파일 | 역할 |
|------|------|
| main.c | 프롬프트 루프, SIGINT 무시, SIGCHLD 등록 |
| parsing.c | 입력 라인 → `char**` 토큰 배열 변환 |
| isbuiltin.c | 내장 명령어 (`cd`, `pwd`, `hostname`) 처리 |
| isexecutable.c | 외부 실행 파일 `fork`/`exec`, 파이프(`\|`), 백그라운드(`&`) |
| child_handler.c | SIGCHLD 핸들러 — `waitpid`로 좀비 프로세스 회수 |
| minishell.h | 함수 선언 및 `BT_TYPE` 구조체 |

### 빌드 & 실행

```bash
cd lab
make          # myshell 생성
./myshell
```

```
minishell> ls -l
minishell> pwd
minishell> cd /tmp
minishell> ls | grep my
minishell> sleep 5 &
minishell> exit
```

### 지원 기능
- 내장 명령어: `cd`, `pwd`, `hostname`, `exit`
- 외부 명령어 실행 (fork + exec)
- 파이프 (`|`)
- 백그라운드 실행 (`&`)
- Ctrl+C: 쉘 자신은 무시, 실행 중인 자식 프로세스에만 전달

---

## 빌드 환경

- OS: Linux (Ubuntu/Debian 계열)
- 컴파일러: GCC
- 표준: POSIX.1-2008

각 챕터 예제는 단일 파일로 구성되어 있어 아래 명령으로 바로 컴파일할 수 있습니다.

```bash
gcc -o ex01 ex01.c
./ex01
```
