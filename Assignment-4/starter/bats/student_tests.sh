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

@test "Check if 'pwd' runs without errors" {
    run ./dsh <<EOF
pwd
EOF
    [ "$status" -eq 0 ]
}

@test "Check if 'whoami' runs without errors" {
    run ./dsh <<EOF
whoami
EOF
    [ "$status" -eq 0 ]
}

@test "Check 'cd' without arguments does nothing" {
    run ./dsh <<EOF
cd
EOF
    [ "$status" -eq 0 ]
}

@test "Check 'cd' changes directory and returns correctly" {
    initial_dir=$(pwd)

    run ./dsh <<EOF
cd bats
pwd
EOF

    stripped_output=$(echo "$output" | sed 's/dsh2>//g' | awk 'NR==1' | tr -d '\t\n\r\f\v')
    expected_output="$initial_dir/bats"

    echo "Captured stdout: $stripped_output"
    echo "Expected: $expected_output"

    [ "$stripped_output" = "$expected_output" ]
    [ "$status" -eq 0 ]

    cd "$initial_dir"
}


@test "Check 'exit' terminates the shell" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
}

@test "Echo handles multiple spaces inside quotes" {
    run ./dsh <<EOF
echo " hello     world "
EOF
    stripped_output=$(echo "$output" | sed 's/dsh2>//g' | awk 'NR==1' | tr -d '\t\n\r\f\v')
    expected_output=" hello     world "

    echo "Captured stdout: $stripped_output"
    echo "Expected: $expected_output"

    [ "$stripped_output" = "$expected_output" ]
}

@test "Echo handles empty quoted string" {
    run ./dsh <<EOF
echo ""
EOF
    stripped_output=$(echo "$output" | sed 's/dsh2>//g' | awk 'NR==1' | tr -d '\t\n\r\f\v')

    echo "Captured stdout: $stripped_output"
    echo "Expected: (empty)"

    [ -z "$stripped_output" ] 
}

@test "Echo handles single-word quoted string" {
    run ./dsh <<EOF
echo "hello"
EOF
    stripped_output=$(echo "$output" | sed 's/dsh2>//g' | awk 'NR==1' | tr -d '\t\n\r\f\v')
    expected_output="hello"

    echo "Captured stdout: $stripped_output"
    echo "Expected: $expected_output"

    [ "$stripped_output" = "$expected_output" ]
}


@test "Empty input does nothing" {
    run ./dsh <<EOF

EOF
    [ "$status" -eq 0 ]
}

@test "Running a command with multiple spaces" {
    run ./dsh <<EOF
   ls     -l
EOF
    [ "$status" -eq 0 ]
}

@test "Running an external command with arguments" {
    run ./dsh <<EOF
ls -l /tmp
EOF
    [ "$status" -eq 0 ]
}
