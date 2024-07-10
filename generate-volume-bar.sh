#!/bin/bash
# Position of the bar character
n=$1

# Initialize the variable
output="|"
# Generate the string
for (( i=1; i<=100; i++ )); do
    if [[ $i -eq $n ]]; then
        output+="▉"
    else
        output+=" "
    fi
done

output+="|"

# Now you can use the variable $output
# echo $output
echo "<span foreground='black' size='x-large'>$output</span>"
