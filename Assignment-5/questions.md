1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

_Shell waits for each child process by calling waitpid() in a loop after forking. This guarantees that all child processes (each handling a segment of the piped command) complete before the shell continues accepting user input. If you omitted waitpid() calls, the parent process wouldn’t collect the exit status of its children, leading to zombie processes. Additionally, the shell might proceed to prompt for new input while some children are still running, potentially causing unsynchronized behavior and resource leaks._ 

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

_After duplicating file descriptors with dup2(), it’s critical to close the original pipe ends that are no longer needed. This prevents the process from unintentionally keeping file descriptors open. For example, if the write end of a pipe isn’t closed in a process that’s meant to only read, the reader may never receive an end-of-file (EOF) signal, causing it to hang waiting for input. Moreover, leaving extra file descriptors open can lead to resource exhaustion and make it harder to manage proper communication between processes._

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

_The cd command changes the shell’s current working directory. Implementing it as an external command would spawn a child process that changes its own directory—this change would not propagate back to the parent shell. Therefore, by making cd a built-in, the shell itself updates its working directory, affecting subsequent commands. If cd were external, the shell’s environment would remain unchanged, causing commands to run in the wrong directory._

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

_To allow an arbitrary number of piped commands, you could allocate your command array dynamically rather than using a fixed-size array (CMD_MAX). For example, you might start with an initial buffer and use realloc() to expand it as more commands are parsed. The trade-offs include:
Memory Management Complexity: Dynamic allocation requires careful error checking and proper deallocation to avoid memory leaks.
Performance Overhead: Frequent resizing (especially with many pipes) might introduce performance overhead.
Resource Limits: While flexibility increases, you must also consider system limits and potential abuse (e.g., extremely long pipelines) that could affect performance or stability._
