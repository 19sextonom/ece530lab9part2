`timescale 1ns / 1ps

//
module BaudRateGeneratorTB;

	reg Reset, Clock;

	wire uartClk;
//module BaudRateGenerator (uartClock, Reset, clock, BaudRate, ClockFrequency);
//clock frequency is 1600Hz and baud rate is 20Hz.
//Each baud frequency is 20x16=320 Hz
//1600/320=5. One baud pulse is generated for every five clock cycles.
	BaudRateGenerator uut (uartClk, Reset, Clock, 20'd20, 30'd1600);

	initial begin 		Reset = 1; Clock = 0; end
	
	always #5 Clock=~Clock;
	
	initial fork
	#0 Reset = 1;  #23 Reset=0;
	#400 $stop;
	join
	
endmodule

