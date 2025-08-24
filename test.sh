

JSON="{\"expression\": \"75\", \"type\": \"wob\", \"settings\": {\"window_timeout\": 5000, \"anchor\": \"top-right\"}}"

echo "$JSON" > ./examples/overlay.fifo

sleep 3
# Define the JSON content
JSON="{\"expression\": \"75\", \"type\": \"wob\", \"settings\": {\"window_timeout\": 5000, \"anchor\": \"top-left\"}}"

echo "$JSON" > ./examples/overlay.fifo
