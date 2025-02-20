1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**: We use fork() before calling execvp() because execvp() replaces the current process with the new program. If we were to call execvp() directly within the shell process, the shell itself would be replaced, making it impossible to continue executing additional commands. By using fork(), we create a separate child process that executes the desired command, while the parent process (the shell) remains running and can continue accepting user input. This separation allows the shell to manage multiple commands, handle foreground and background processes, and implement features like job control. Without fork(), every command execution would terminate the shell, making it unusable.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If fork() fails, it returns -1, indicating that the system could not create a new child process. This failure usually occurs due to resource exhaustion, such as running out of available process slots or memory. In my implementation, if fork() fails, an error message is displayed using perror("fork failed"), and a non-zero exit code is returned to indicate failure. This ensures that the shell does not crash and can continue running, allowing the user to attempt other commands even if process creation fails.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  The execvp() function searches for the specified command in directories listed in the PATH environment variable. If the command is provided as an absolute path (e.g., /bin/ls), execvp() directly executes it. However, if only the command name is given (e.g., ls), execvp() checks each directory in PATH sequentially until it finds the corresponding executable file. If no matching executable is found, execvp() fails and returns an error. The PATH variable can be viewed and modified by the user, allowing for custom command locations to be included in the search path.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The wait() function ensures that the parent process (the shell) waits for the child process (the executed command) to complete before proceeding. This prevents the shell from immediately returning to the command prompt while the child process is still running. Without calling wait(), child processes would become "zombie processes" because the operating system retains their exit status until the parent retrieves it. Accumulation of zombie processes can lead to resource exhaustion. Additionally, without wait(), commands might execute in an unpredictable order, as the shell would continue accepting input without ensuring the previous command has finished.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  The WEXITSTATUS() macro extracts the exit status of a child process from the value returned by waitpid(). This is important because it allows the shell to determine whether a command executed successfully or failed. When a process terminates normally, it returns an exit code, typically 0 for success and non-zero values for errors. By using WEXITSTATUS(status), the shell can capture and display meaningful exit codes, aiding in debugging and automation, where scripts depend on command success or failure.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation of build_cmd_buff() correctly processes quoted arguments by treating them as a single argument, preserving spaces inside quotes. It does this by detecting opening and closing quotes and ensuring that spaces within them are not treated as argument separators. This is necessary because commands like echo "Hello World" should pass "Hello World" as a single argument instead of splitting it into separate words. Without this handling, quoted arguments would be incorrectly parsed, leading to unexpected behavior when running commands.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  Compared to the previous assignment, I modified the parsing logic to eliminate pipe handling, focusing only on single-command execution. I also improved argument processing by handling quoted strings correctly and reducing unnecessary whitespace. One unexpected challenge was ensuring that leading and trailing spaces were removed while preserving spaces within quotes. Additionally, refactoring required careful restructuring to align with the new cmd_buff_t structure, which simplified the parsing process but required adjusting existing logic.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals in a Linux system are used for asynchronous communication between processes and the operating system. They notify processes of events such as termination requests, interruptions, or errors. Unlike other IPC mechanisms like message queues or shared memory, signals are lightweight and do not require explicit synchronization. Instead, they can be sent instantly to a process, triggering predefined handlers or default behaviors. This makes signals useful for handling urgent events, such as a user pressing Ctrl+C to terminate a process.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  
SIGKILL (9): This signal immediately terminates a process and cannot be caught, ignored, or handled. It is often used to forcefully stop unresponsive processes when SIGTERM fails.
SIGTERM (15): This is the default termination signal, allowing a process to clean up before exiting. Many programs handle SIGTERM to close files, release resources, or perform other shutdown procedures.
SIGINT (2): This signal is typically sent when the user presses Ctrl+C in the terminal. It allows processes to handle an interruption gracefully, but if unhandled, it terminates the process.


- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives SIGSTOP, it is immediately paused and moved to the stopped state. Unlike SIGINT, SIGSTOP cannot be caught or ignored because it is handled directly by the operating system. This ensures that processes can be forcibly suspended when necessary. The process can later be resumed using SIGCONT, allowing job control features like suspending and resuming background processes in a shell.
