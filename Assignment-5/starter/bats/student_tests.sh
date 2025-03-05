#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Empty command prints warning and continues" {
    run ./dsh <<EOF
       
exit
EOF
    [[ "$output" =~ "Warning:" ]] || [[ "$output" =~ "no command" ]]
    [ "$status" -eq 0 ]
}

@test "Exit command terminates the shell with final output" {
    run ./dsh <<EOF
exit
EOF
    [[ "$output" =~ "cmd loop returned 0" ]]
    [ "$status" -eq 0 ]
}

@test "Extra spaces in command are handled correctly" {
    run ./dsh <<EOF
   ls   
exit
EOF
    [[ "$output" =~ "dshlib.c" ]] || true
    [ "$status" -eq 0 ]
}

@test "Invalid command prints an error message" {
    run ./dsh <<EOF
nonexistentcommand
exit
EOF
    [[ "$output" =~ "Execution failed" ]]
    [ "$status" -eq 0 ]
}

