1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**: fgets() is a good choice because it reads a full line of user input, including spaces, while ensuring buffer overflow protection by limiting the number of characters read. Additionally, fgets() stops reading when it encounters a newline (\n) or reaches the specified buffer size, making it well-suited for handling command-line inputs in a shell.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**:  Using malloc() allows for dynamic memory allocation, making the shell more flexible in handling user input of varying sizes. If we used a fixed-size array, it might waste memory when handling short commands or be too small for long commands, causing truncation. Dynamic allocation ensures that we only use as much memory as needed and allows us to reallocate memory if necessary.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**: Trimming spaces ensures that commands are correctly parsed and executed without errors. If we don’t trim spaces, the shell might interpret a command as an empty string or pass incorrect arguments, leading to unexpected behavior. For example, " ls " (with spaces) might not be recognized as "ls", causing execution failures.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**: 
Output Redirection (> and >>): ls > output.txt writes the output to a file instead of displaying it. Challenge is that the shell must open the file for writing and redirect standard output.
Input Redirection (<): sort < file.txt takes input from a file. Challenge is that shell must replace standard input with the file's contents.
Error Redirection (2>): gcc mycode.c 2> errors.txt captures error messages in a separate file. Challenge is managing different file descriptors correctly.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection modifies where input and output go while piping connects the output of one command as input to another. Piping allows chaining multiple commands dynamically whereas redirection primarily deals with files.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  Keeping STDOUT and STDERR separate allows users to differentiate between expected results and errors. This separation is useful in automation and debugging, as users can redirect errors to log files while still seeing valid output.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  Our shell should display error messages separately from normal output. It should allow merging outputs, where STDERR is redirected to STDOUT, enabling combined processing. This ensures better error tracking while maintaining usability.
