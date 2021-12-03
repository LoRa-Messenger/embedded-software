# embedded-software
Repository for the embedded software for the LoRa Messenger project.

# TODO


TODO embedded side: 

- Adjust the Threading timings
- Check data conversion between LoRa and BLE

Lora Acknowledgements
- resend lora message if no acknowledge received in a couple cycles
- Deal with removing messages from queue only when ack is received (might need a find function to find the approriate message in queue)

Others
- power monitoring
- experiment with LoRa parameters (spreading factor, antenna, frequency, data rate, etc.)
