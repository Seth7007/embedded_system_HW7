In HW7, we combine the code of HW2 and the source code and implement an FIR low pass filter, collect data (acceleration) of STM 32 board and then filter these data. The procedure are the following:  
1. Go to the source code link (https://github.com/ARM-software/CMSIS_4/tree/master/CMSIS), then choose DSP_lib -> Exampls ->arm_fir_example/ARM -> arm_fir_example_f32.c. The sample data are in the file called arm_fir_data.c.
2. Combine arm_fir_example_f32.c with HW2's code. We add the data and the filter part into HW2's code and change the format of the trasmitted data. The code is used to collect the data of STM32 board, and send them to the server side.
3. Remeber to change the remote port in the above code, add the libraries of BSP and CMSIS-DSP.  
4. We provide a MATLAB code such that it can generate random data and the filter data of it. Use the code to generate one set of data, and test to ensure that the main.cpp runs correctly.
5. In json file, remember to change the IP value to current IP, and the value of "use-tls-socket" should be set to false. Also remember to change the WIFI-related items to your own. Finally, change the float to "std".  
6. For socket server, we receive the data sent from the main.cpp, and print the data out in the format of plot. Ensure to change the IP address to your current IP.  
7. 
