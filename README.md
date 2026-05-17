*This project has been created as part of the 42 curriculum by mshanabl, oalfoqha.*

# Minishell — As beautiful as a shell

## Description

Minishell is a simplified Unix shell written in C. It recreates the core behavior of Bash: executing commands, handling pipes and redirections, expanding environment variables, and managing signals.

The project focuses on two key areas of systems programming: process management (fork, execve, waitpid) and file descriptor manipulation (dup2, pipes, redirections). Building it gives a solid understanding of how a real shell works internally.

Features:
- Interactive prompt with command history
- External command execution via `PATH` or absolute/relative paths
- Built-ins: `echo -n`, `cd`, `pwd`, `export`, `unset`, `env`, `exit`
- Redirections: `<`, `>`, `>>`, `<<` (heredoc)
- Pipelines via `|`
- Environment variable expansion (`$VAR`, `$?`)
- Single and double quote handling
- Signals: `ctrl-C` (new prompt), `ctrl-D` (exit), `ctrl-\` (ignored)

---

## Instructions

```bash
make
./minishell
```

### Tests to try

```bash
# echo and quotes
echo hello world
echo -n no newline
echo "$HOME"
echo '$HOME'

# variables
export NAME=42
echo $NAME
unset NAME
echo $NAME

# exit codes
true
echo $?
false
echo $?

# redirections
echo hello > /tmp/out.txt
cat < /tmp/out.txt
echo appended >> /tmp/out.txt
cat /tmp/out.txt

# heredoc
cat << EOF
line one
line two
EOF

# pipes
echo hello | cat
ls | grep ".c" | wc -l
echo hello | cat | cat | cat | cat | cat

# builtins
pwd
cd /tmp
pwd
cd
pwd
env | grep PATH
export | grep HOME

# exit edge cases
exit 42
exit abc
exit 9999999999999999999

# syntax error — shell must print error and stay alive
|
echo still alive

# memory and fd leak check
valgrind --leak-check=full --show-leak-kinds=all --track-fds=yes \
  --suppressions=readline.supp ./minishell
```

---

## Resources

- `man 2 fork`, `man 2 execve`, `man 2 pipe`, `man 2 dup2`, `man 2 waitpid`, `man 7 signal` — primary references for every system call used.
- [Understanding Unix Processes in C](https://medium.com/@chelbiahmed501/understanding-unix-processes-in-c-a-practical-introduction-to-fork-and-wait-4c437d80daff)
- [Writing Your Own Shell](https://www.cs.purdue.edu/homes/grr/SystemsProgrammingBook/Book/Chapter5-WritingYourOwnShell.pdf)

### How AI was used

- **Learning**: AI was used to understand concepts like process trees, file descriptor inheritance across fork/exec, and signal propagation — helping us ask better questions and go deeper with peers.
- **Test generation**: An automated suite comparing minishell against interactive Bash via PTY ran 93 tests across builtins, pipes, redirections, heredocs, and expansions.
- **Valgrind analysis**: AI helped design and interpret valgrind runs confirming zero definite leaks and no file descriptor leaks.

All AI-generated content was reviewed, tested, and validated by the project authors before being used. No AI was used to write the core shell implementation.