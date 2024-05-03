In HW7, we combine the code of HW2 and the source code and implement an FIR low pass filter, collect the acceleration data of STM 32 board and then filter these data. The procedure are the following:  
1. Go to the source code link (https://github.com/ARM-software/CMSIS_4/tree/master/CMSIS), then choose DSP_Lib -> Exampls -> arm_fir_example/ARM -> arm_fir_example_f32.c. The sample data are in the file called arm_fir_data.c.
2. Combine arm_fir_example_f32.c with HW2's code. We add the data and the filter part into HW2's code and change the format of the trasmitted data. The code is used to collect the data of STM32 board, and send them to the server side.
3. In CMSIS/DSP_Lib/Source/TransformFunctions/arm_bitreversal2.S, add "#define __CC_ARM" before line 43.
4. Remeber to change the remote port in the above code, and add the libraries of BSP and CMSIS-DSP.  
5. We provide a MATLAB code such that it can generate random data and the filter data of it. Use the code to generate one set of data, and test to ensure that the main.cpp runs correctly. Besides, we also provide another testing data.  
6. In json file, remember to change the IP value to current IP, and the value of "use-tls-socket" should be set to false. Also remember to change the WIFI-related items to your own. Finally, change the float to "std".  
7. For socket server, we receive the data sent from the main.cpp, and print the data out in the format of plot. Ensure to change the IP address to your current IP.  
8. Now, start to work. First, run main.cpp and socket_server.py and check that the STM 32 board and the server are connected to each other. Then press the button to collect acceleration data for 3.2 seconds (to collect 320 data). Then the data will be transmitted to the server, and the server can plot the data.  
9. Note that if we collect to much data in one time, we need to run the program again.  
10. The following plot shows that the FIR low pass filter works perfectly (compare filtered data generated from main.cpp and MATLAB to ensure that our main.cpp is correct).  ![Figure_1](https://github.com/Seth7007/embedded_system_HW7/assets/140083766/c32c77a0-60e9-43ca-a0d9-13487837da42)  
11. The following plot shows that the collected data and the outcome of filtering. ![438231912_7632413383490173_7725555864280158541_n](https://github.com/Seth7007/embedded_system_HW7/assets/140083766/4deecc69-c0b8-4eef-91a9-c0e1d4231239)  
12. From the data and its filtering counterpart, you can see that there are some delays in the filtered data.  
