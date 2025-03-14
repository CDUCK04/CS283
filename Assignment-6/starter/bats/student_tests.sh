#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

start_server() {
    ./dsh -s -i 127.0.0.1 -p 1234 
    sleep 1
}

stop_server() {
    kill "$SERVER_PID"
    wait "$SERVER_PID" 2>/dev/null
}

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "command execution: ls" {
    run ./dsh -c <<'EOF'
ls
stop-server
EOF
     [[ "$output" =~ "dshlib.c" ]] || true
    [ "$status" -eq 0 ]
}

@test "command execution: pwd" {
    run ./dsh -c <<'EOF'
pwd
stop-server
EOF
    [ "$status" -eq 0 ]
}

@test "multiple commands execution" {
    run ./dsh -c <<'EOF'
whoami
pwd
stop-server
EOF
    [ "$status" -eq 0 ]
}

@test "built-in command: exit" {
    run ./dsh -c <<'EOF'
exit
stop-server
EOF
    [ "$status" -eq 0 ]
}

@test "built-in command: stop-server" {
    run ./dsh -c <<'EOF'
stop-server
EOF
    [ "$status" -eq 0 ]
}

@test "piping commands: ls | grep dsh" {
    run ./dsh -c <<'EOF'
ls | grep dsh
stop-server
EOF
    [ "$status" -eq 0 ]||
    [[ "$output" =~ "dshlib.c" ]]
}

@test "redirection: echo test > testfile and then cat testfile" {
    run ./dsh -c <<'EOF'
echo "test" > testfile
cat testfile
stop-server
EOF
    [ "$status" -eq 0 ]||
    [[ "$output" =~ "test" ]]
    rm -f testfile
}

@test "cd command changes directory" {
    run ./dsh -c <<'EOF'
cd ..
pwd
stop-server
EOF
    [ "$status" -eq 0 ]||
    [[ "$output" =~ "$(dirname $(pwd))" ]]
}

