`default_nettype none

module incrementer (
    input wire         clk_i,
    input wire         rst_i,

    input  wire        valid_i,
    output wire        ready_o,
    input  wire [31:0] data_i,
    input  wire        last_i,
    
    output wire        valid_o,
    input  wire        ready_i,
    output wire [31:0] data_o,
    output wire        last_o
);

    assign ready_o = ready_i;
    assign valid_o = valid_i;
    assign data_o  = data_i + 1;
    assign last_o  = last_i;
    
endmodule
