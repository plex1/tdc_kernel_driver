echo "start" > /dev/mod_tdc
sleep 0
echo "stop" > /dev/mod_tdc
cat /dev/mod_tdc | head -n 1
