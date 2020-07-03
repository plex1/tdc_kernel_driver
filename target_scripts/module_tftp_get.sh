tftp -g -r module_load.sh 192.168.1.100
tftp -g -r module_remove.sh 192.168.1.100
tftp -g -r module_measure.sh 192.168.1.100
tftp -g -r config_ethernet.sh 192.168.1.100
chmod +x module_load.sh
chmod +x module_remove.sh
chmod +x module_measure.sh
chmod +x config_ethernet.sh
