// (c) Copyright 1995-2023 Xilinx, Inc. All rights reserved.
// 
// This file contains confidential and proprietary information
// of Xilinx, Inc. and is protected under U.S. and
// international copyright and other intellectual property
// laws.
// 
// DISCLAIMER
// This disclaimer is not a license and does not grant any
// rights to the materials distributed herewith. Except as
// otherwise provided in a valid license issued to you by
// Xilinx, and to the maximum extent permitted by applicable
// law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND
// WITH ALL FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES
// AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, INCLUDING
// BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-
// INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE; and
// (2) Xilinx shall not be liable (whether in contract or tort,
// including negligence, or under any other theory of
// liability) for any loss or damage of any kind or nature
// related to, arising under or in connection with these
// materials, including for any direct, or any indirect,
// special, incidental, or consequential loss or damage
// (including loss of data, profits, goodwill, or any type of
// loss or damage suffered as a result of any action brought
// by a third party) even if such damage or loss was
// reasonably foreseeable or Xilinx had been advised of the
// possibility of the same.
// 
// CRITICAL APPLICATIONS
// Xilinx products are not designed or intended to be fail-
// safe, or for use in any application requiring fail-safe
// performance, such as life-support or safety devices or
// systems, Class III medical devices, nuclear facilities,
// applications related to the deployment of airbags, or any
// other applications that could lead to death, personal
// injury, or severe property or environmental damage
// (individually and collectively, "Critical
// Applications"). Customer assumes the sole risk and
// liability of any use of Xilinx products in Critical
// Applications, subject only to applicable laws and
// regulations governing limitations on product liability.
// 
// THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS
// PART OF THIS FILE AT ALL TIMES.
// 
// DO NOT MODIFY THIS FILE.


// IP VLNV: xilinx.com:user:UARTmodule2023:1.0
// IP Revision: 3

(* X_CORE_INFO = "UARTmodule2023,Vivado 2016.2" *)
(* CHECK_LICENSE_TYPE = "LEDcounter_scutimer_interrupt_UARTmodule2023_0_1,UARTmodule2023,{}" *)
(* CORE_GENERATION_INFO = "LEDcounter_scutimer_interrupt_UARTmodule2023_0_1,UARTmodule2023,{x_ipProduct=Vivado 2016.2,x_ipVendor=xilinx.com,x_ipLibrary=user,x_ipName=UARTmodule2023,x_ipVersion=1.0,x_ipCoreRevision=3,x_ipLanguage=VERILOG,x_ipSimLanguage=MIXED,TRANSMITTED_BITS=4,BAUDRATE=0x04B00,FREQUENCY=0x05F5E100}" *)
(* DowngradeIPIdentifiedWarnings = "yes" *)
module LEDcounter_scutimer_interrupt_UARTmodule2023_0_1 (
  rx,
  tx,
  tx_full,
  rx_data_present,
  read_from_uart,
  write_to_uart,
  rx_data,
  tx_data,
  transmitted_bits,
  clock,
  reset
);

input wire rx;
output wire tx;
output wire tx_full;
output wire rx_data_present;
input wire read_from_uart;
input wire write_to_uart;
output wire [7 : 0] rx_data;
input wire [7 : 0] tx_data;
output wire [3 : 0] transmitted_bits;
(* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 clock CLK" *)
input wire clock;
(* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 reset RST" *)
input wire reset;

  UARTmodule2023 #(
    .TRANSMITTED_BITS(4),
    .BAUDRATE('H04B00),
    .FREQUENCY('H05F5E100)
  ) inst (
    .rx(rx),
    .tx(tx),
    .tx_full(tx_full),
    .rx_data_present(rx_data_present),
    .read_from_uart(read_from_uart),
    .write_to_uart(write_to_uart),
    .rx_data(rx_data),
    .tx_data(tx_data),
    .transmitted_bits(transmitted_bits),
    .clock(clock),
    .reset(reset)
  );
endmodule
