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
echo "<span font_desc='Sans 24' foreground='black'>Hello, world!</span>\n<span font_desc='Sans 24' foreground='black'>Hello, world!</span>"
# echo "<span foreground='white' background='black' size='500%'></span>\n<span foreground='white' background='black' size='200%'>$output</span>"
