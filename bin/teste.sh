#!/bin/bash


run_commands() {
    for command in "$@"; do
        echo "Running command: $command"
        eval "$command"
        if [ $? -ne 0 ]; then
            echo "Error executing command: $command"
        fi
    done
}


commands=(
    "./client execute 50 -u \"echo Hello, World!\""
    "./client execute 80 -u \"hostname\""
    "./client execute 100 -u \"wc -l\""      
    "./client execute 90 -u \"ls -l\""
    "./client execute 120 -u \"ls -l /etc/\""        
    "./client execute 190 -p \"ls -l | wc -l\""
    "./client execute 90 -p \"ls -l | wc -l\""
    "./client execute 90 -p \"ls -l | sleep 10\""
)

run_commands "${commands[@]}"