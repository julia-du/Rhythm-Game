/* main.sv
   FPGA SystemVerilog for E155 MicroPs Final Project: Rhythm Game
   Julia Du, jdu@hmc.edu, 12/8/23
*/

/*  main
    Top-level module for the FPGA.
   
    Inputs:  beatCLK     - clock signal that determines the rate at which beats scroll down the display
	         beatL/R     - determines when a beat should start scrolling down on the left/right side
             hitL/R      - determines when the screen should show a hit on the left/right
             missL/R     - determines when the screen should show a miss on the left/right
		     gameover    - determines if the game is over or not
		     round[1:0]  - round number
			               0 = round 1, 1 = round 2, 2 = round 3, 3 = win
	Outputs: R/G/B1 - red/green/blue values for the 1st row (out of the 2 rows being addressed)
	         R/G/B2 - red/green/blue values for the 2nd row (out of the 2 rows being addressed)
			 CLK    - clock signal to send R/G/B data for the 2 rows to the display
			 LAT    - latch signal that marks the end of the RGB data for the 2 rows
			 OE     - active low output enable
			          set to 1 to turn off the display
			 ABCD   - address of the rows being addressed
			          0000 = rows 1 and 16, 1111 = rows 17 and 32     
*/
module main(input  logic beatCLK, beatL, beatR, hitL, hitR, missL, missR, gameover,
            input  logic [1:0] round,
            output logic R1, G1, B1, R2, G2, B2, CLK, LAT, OE,
            output logic [3:0] ABCD);

	logic [31:0] matrix [31:0];
	logic hitL_long, hitR_long, missL_long, missR_long;
	
	// Internal Oscillator
	logic int_osc;
	HSOSC #(.CLKHF_DIV(2'b01)) // 24 MHz
        hf_osc (.CLKHFPU(1'b1), .CLKHFEN(1'b1), .CLKHF(int_osc));
	
	display_matrix    dm(int_osc, beatCLK, !beatL, !beatR, hitL, hitR, missL, missR, gameover, round, matrix, 
						 hitL_long, hitR_long, missL_long, missR_long);
	display_interface di(int_osc, hitL_long, hitR_long, missL_long, missR_long, matrix, R1, G1, B1, R2, G2, B2, CLK, LAT, OE, ABCD);

endmodule

/*  display_matrix
    Keeps track of the game state and outputs the correct display matrix.
	
	Inputs: beatCLK, beatL/R, hitL/R, missL/R, gameover, round[1:0]
	Output: matrix[31:0][31:0] - represents which LEDs on the 32x32 screen should be on
	        hitL_long, hitR_long, missL_long, missR_long - essentially the same as hitL, hitR, missL, and missR, but holds each of them for a set amount of time
*/
module display_matrix(input logic int_osc, beatCLK, beatL, beatR, hitL, hitR, missL, missR, gameover,
                      input logic [1:0] round,
                      output logic [31:0] matrix [31:0],
                      output logic hitL_long, hitR_long, missL_long, missR_long);
	
	logic [31:0] beatL_displayed, beatR_displayed;
	logic [31:0] play_screen [31:0], end_screen [31:0];
	
	// FSMs
	beats b(beatCLK, beatL, beatR, beatL_displayed, beatR_displayed);
	hits  h(int_osc, hitL, hitR, missL, missR, hitL_long, hitR_long, missL_long, missR_long);
	
	// Choose end_screen or play_screen
	display_play  dp(beatL_displayed, beatR_displayed, hitL_long, hitR_long, missL_long, missR_long, round, play_screen);
	display_end   de(gameover, round, end_screen);
	always_comb begin
		if(gameover || round == 3)
			matrix = end_screen;
		else
			matrix = play_screen;
	end
	
endmodule

/*  hits
	FSM to make hits and misses linger on the screen for longer.

	Inputs:  displayCLK - a 24 MHz clock signal
	         hitL, hitR, missL, missR
	Outputs: hitL_long, hitR_long, missL_long, missR_long
*/
module hits (input logic displayCLK, hitL, hitR, missL, missR,
			 output logic hitL_long, hitR_long, missL_long, missR_long);
	
	logic [22:0] counterL, counterR;                          // Adjusted based on desired length of linger
	logic [1:0] L_state, R_state, next_L_state, next_R_state; // For each state, 0 represents showing a miss, 
	                                                          // 1 represents showing a hit, 2 represents showing nothing
	
	// State Register
	always_ff @(posedge displayCLK) begin
		L_state <= next_L_state;
		R_state <= next_R_state;
		if (hitL || missL) counterL <= 0; else counterL <= counterL + 1;
		if (hitR || missR) counterR <= 0; else counterR <= counterR + 1;
	end
	
	// Next State Logic
	always_comb begin
		if (hitL)       next_L_state = 1;
		else if (missL) next_L_state = 0;
		else if(counterL[22] == 1) next_L_state = 2;
		else next_L_state = L_state;
			
		if (hitR)       next_R_state = 1;
		else if (missR) next_R_state = 0;
		else if(counterR[22] == 1) next_R_state = 2;
		else next_R_state = R_state;
	end
	
	// Output Logic
	assign hitL_long = (L_state == 1);
	assign hitR_long = (R_state == 1);
	assign missL_long = (L_state == 0);
	assign missR_long = (R_state == 0);
	
endmodule

/*  beats
	FSM to keep track of where beats should be displayed on the screen.

	Inputs:  beatCLK, beatL, beatR
	Outputs: beatL/R_displayed[31:0] - determines where beats should be displayed on the left/right side
	                                   32 available spots on each side
*/
module beats(input  logic beatCLK, beatL, beatR,
             output logic [31:0] beatL_displayed, beatR_displayed);
	
	logic [31:0] L_state, R_state, next_L_state, next_R_state;
	
	// State Register
	always_ff @(negedge beatCLK) begin
		L_state <= next_L_state;
		R_state <= next_R_state;
	end
	
	// Next State Logic
	assign next_L_state[31:1] = L_state[30:0];
	assign next_L_state[0] = beatL;
	assign next_R_state[31:1] = R_state[30:0];
	assign next_R_state[0] = beatR;
	
	// Output Logic
	assign beatL_displayed = L_state;
	assign beatR_displayed = R_state;
	
endmodule

/*  display_play
	Outputs the display matrix when playing a round.
    
	Inputs: beatL/R_displayed[31:0], hitL/R, missL/R, round[1:0]
	Output: matrix[31:0][31:0]
*/
module display_play(input  logic [31:0] beatL_displayed, beatR_displayed, 
                    input  logic hitL, hitR, missL, missR,
                    input  logic [1:0]  round,
                    output logic [31:0] matrix [31:0]);
	
	logic [31:0] r1 [4:0], r2 [4:0], r3 [4:0], hit [4:0], miss [4:0];

	// Read patterns from memory
	initial $readmemb("play_r1.txt", r1);
	initial $readmemb("play_r2.txt", r2);
	initial $readmemb("play_r3.txt", r3); 
	initial $readmemb("play_hit.txt", hit);
	initial $readmemb("play_miss.txt", miss);
	
	// Choose patterns based on current game states
	logic [31:0] round_num [4:0], left [4:0], right [4:0];
	always_comb begin
		case(round)
			0: round_num = r1;
			1: round_num = r2;
			2: round_num = r3;
			default: round_num = r1;
		endcase
		
		if (hitL)       left = hit;
		else if (missL) left = miss;
		else begin
			left[0] = 0;
			left[1] = 0;
			left[2] = 0;
			left[3] = 0;
			left[4] = 0;
		end
		
		if (hitR)       right = hit;
		else if (missR) right = miss;
		else begin
			right[0] = 0;
			right[1] = 0;
			right[2] = 0;
			right[3] = 0;
			right[4] = 0;
		end
	end

	
	// Assign pixels on the screen
	assign matrix[0] = 0;
	assign matrix[1] = round_num[0] | left[0];
	assign matrix[2] = round_num[1] | left[1];
	assign matrix[3] = round_num[2] | left[2];
	assign matrix[4] = round_num[3] | left[3];
	assign matrix[5] = round_num[4] | left[4];
	assign matrix[6] = 0;
	assign matrix[7] = 32'b11111111111111111111111111111111;
	assign matrix[8] = 0;
	assign matrix[9] = 0;
	assign matrix[10] = beatL_displayed;
	assign matrix[11] = beatL_displayed;
	assign matrix[12] = beatL_displayed;
	assign matrix[13] = beatL_displayed;
	assign matrix[14] = beatL_displayed;
	assign matrix[15] = 32'b00100000000000000000000000000000;
	assign matrix[16] = 32'b00100000000000000000000000000000;
	assign matrix[17] = beatR_displayed;
	assign matrix[18] = beatR_displayed;
	assign matrix[19] = beatR_displayed;
	assign matrix[20] = beatR_displayed;
	assign matrix[21] = beatR_displayed;
	assign matrix[22] = 32'b00100000000000000000000000000000;
	assign matrix[23] = 32'b00100000000000000000000000000000;
	assign matrix[24] = 32'b11111111111111111111111111111111;
	assign matrix[25] = 0;
	assign matrix[26] = right[0];
	assign matrix[27] = right[1];
	assign matrix[28] = right[2];
	assign matrix[29] = right[3];
	assign matrix[30] = right[4];
	assign matrix[31] = 0;

endmodule

/*  display_end
	Outputs the display matrix when the game is over.

	Inputs: gamemover, round[1:0]
	Output: matrix[31:0][31:0]
*/
module display_end(input  logic gameover,
                   input  logic [1:0] round,
                   output logic [31:0] matrix [31:0]);
	
	logic [31:0] win [31:0], r1 [31:0], r2 [31:0], r3 [31:0];

	// Read patterns from memory
	initial $readmemb("win.txt", win);
	initial $readmemb("lose_r1.txt", r1); 
	initial $readmemb("lose_r2.txt", r2);
	initial $readmemb("lose_r3.txt", r3);
	
	// Assign pixels
	always_comb begin
		if(gameover) begin
			case(round)
				default: matrix = r1;
				1: matrix = r2;
				2: matrix = r3;
			endcase
		end
		else matrix = win;
	end

endmodule

/*  display_interface
    Sends the correct inputs to the 32x32 matrix to display the matrix.

	Inputs:  hitL/R, missL/R matrix[31:0][31:0]
	         displayCLK - a 24 MHz clock signal
	Output:  R/G/B1, R/G/B2, CLK, LAT, OE, ABCD[3:0]
*/
module display_interface(input logic displayCLK, hitL, hitR, missL, missR,
                         input logic [31:0] matrix [31:0],
                         output logic R1, G1, B1, R2, G2, B2, 
                                      CLK, LAT, OE,
                         output logic [3:0] ABCD);
	
	logic [3:0] ABCD_state, next_ABCD_state;
	logic R1_state, R2_state, G1_state, G2_state, B1_state, B2_state, next_R1_state, next_R2_state, next_G1_state, next_G2_state, next_B1_state, next_B2_state;
	logic [13:0] counter;                 // Adjust to avoid flickering and ghosting
	parameter OFFSET = 5;
	
	// State Register
	always_ff @(posedge displayCLK) begin // Increment the counter on the positive clock edge
		counter <= counter + 1;
		ABCD_state <= next_ABCD_state;
	end
	always_ff @(negedge displayCLK) begin // Update the RGB values on the negetive clock edge
		R1_state <= next_R1_state;
		R2_state <= next_R2_state;
		G1_state <= next_G1_state;
		G2_state <= next_G2_state;
		B1_state <= next_B1_state;
		B2_state <= next_B2_state;
	end
	
	// Next State Logic
	assign next_ABCD_state = (counter == 36+OFFSET) ? ABCD_state + 1 : ABCD_state; // Change the rows being addressed when counter = 36
	
	always_comb begin 
		if ((counter <= 31+OFFSET) && (counter >= OFFSET-1)) begin                 // Clock in RGB values for counter = 0 to 31
			// Sets left-side hits and misses to green and red, respectively
			if((ABCD_state >= 1) && (ABCD_state <= 5) && (counter >= OFFSET+25) && (counter <= OFFSET+29)) begin
				next_R1_state = hitL  ? 0 : matrix[ABCD_state][31-(counter-OFFSET+1)];
				next_G1_state = missL ? 0 : matrix[ABCD_state][31-(counter-OFFSET+1)];
				next_B1_state = hitL || missL ? 0 : matrix[ABCD_state][31-(counter-OFFSET+1)];
				next_R2_state = matrix[ABCD_state+16][31-(counter-OFFSET+1)];
				next_G2_state = matrix[ABCD_state+16][31-(counter-OFFSET+1)];
				next_B2_state = matrix[ABCD_state+16][31-(counter-OFFSET+1)];
			end
			// Sets left-side hits and misses to green and red, respectively
			else if((ABCD_state >= 10) && (ABCD_state <= 14) && (counter >= OFFSET+25) && (counter <= OFFSET+29)) begin
				next_R1_state = matrix[ABCD_state][31-(counter-OFFSET+1)];
				next_G1_state = matrix[ABCD_state][31-(counter-OFFSET+1)];
				next_B1_state = matrix[ABCD_state][31-(counter-OFFSET+1)];
				next_R2_state = hitR  ? 0 : matrix[ABCD_state+16][31-(counter-OFFSET+1)];
				next_G2_state = missR ? 0 : matrix[ABCD_state+16][31-(counter-OFFSET+1)];
				next_B2_state = hitR || missR ? 0 : matrix[ABCD_state+16][31-(counter-OFFSET+1)];
			end
			// All remaining pixels are white
			else begin
				next_R1_state = matrix[ABCD_state][31-(counter-OFFSET+1)];
				next_G1_state = matrix[ABCD_state][31-(counter-OFFSET+1)];
				next_B1_state = matrix[ABCD_state][31-(counter-OFFSET+1)];
				next_R2_state = matrix[ABCD_state+16][31-(counter-OFFSET+1)];
				next_G2_state = matrix[ABCD_state+16][31-(counter-OFFSET+1)];
				next_B2_state = matrix[ABCD_state+16][31-(counter-OFFSET+1)];
			end
		end
		else begin
			next_R1_state = 0;
			next_G1_state = 0;
			next_B1_state = 0;
			next_R2_state = 0;
			next_G2_state = 0;
			next_B2_state = 0;
		end
	end
	
	// Output Logic
	assign ABCD = ABCD_state - 1;
	assign R1 = R1_state;
	assign R2 = R2_state;
	assign G1 = G1_state;
	assign G2 = G2_state;
	assign B1 = B1_state;
	assign B2 = B2_state;
	
	assign CLK = ((counter <= 31+OFFSET) && (counter >= OFFSET)) ? displayCLK : 0; // Clock in RGB values for counter = 0 to 31
	assign LAT = (counter == 33+OFFSET);                                           // Latch data when counter = 33
	assign OE  = (counter == 34+OFFSET);                                           // Turn off the display when counter = 34
	
endmodule