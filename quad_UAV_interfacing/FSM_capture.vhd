----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 02/11/2020 05:34:37 PM
-- Design Name: 
-- Module Name: FSM_capture - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


-- library declaration
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.STD_LOGIC_unsigned.all;
-- entity
entity FSM_capture is
port ( PPM_Input,CLK,RESET : in std_logic;
frame_counter_out    : out std_logic_vector(31 downto 0);
--Y           : out std_logic_vector(3 downto 0);
CH1_count, CH2_count, CH3_count, CH4_count, CH5_count, CH6_count, Idle_count: out std_logic_vector(31 downto 0));


end FSM_capture;
-- architecture
architecture fsm of FSM_capture is
	type state_type is (Gap0,Idle,Gap1,CH1,Gap2,CH2,Gap3,CH3,Gap4,CH4,Gap5,CH5,Gap6,CH6);
	signal PS,NS : state_type;
	signal count   : std_logic_vector(31 downto 0);
	signal reset_counter : std_logic;
	signal sample1, sample2, sample3, sample4, sample5 : std_logic;
	signal frame_counter : std_logic_vector(31 downto 0) := x"00000000";
begin
	sync_proc: process(CLK,NS,RESET, PPM_Input, sample1, sample2, sample3, sample4, sample5)
	begin
	if (RESET = '1') then
		PS <= Gap0;
	    sample1 <= '0';
		sample2 <= '0';
		sample3 <= '0';
		sample4<= '0';
		sample5<='0';
	elsif (rising_edge(CLK)) then
		PS <= NS;
		sample1 <= PPM_Input;
		sample2 <= sample1;
		sample3 <= sample2;
		sample4<= sample3;
		sample5<=sample4;
	end if;
	end process sync_proc;

	comb_proc: process(PS,PPM_Input, sample1, sample2, sample3,sample4,sample5,count)
	begin
	--count <= x"00000000"; -- pre-assign FSM outputs
	case PS is
	    when Gap0 =>
	        reset_counter <= '0';
	        if ((PPM_Input and sample1 and sample2 and sample3 and sample4 and sample5) = '1') then NS <= Idle; 
			else NS <= Gap0;
			end if;
		when Idle => -- items regarding state ST0
		    reset_counter <= '0';
			if ((PPM_Input or sample1 or sample2 or sample3 or sample4 or sample5) = '0') then 
			     reset_counter <= '1';
			     NS <= Gap1; 
			else NS <= Idle;
			end if;
	    when Gap1 =>
	        reset_counter <= '0';
	        if ((PPM_Input and sample1 and sample2 and sample3 and sample4 and sample5) = '1') then NS <= CH1; 
			else NS <= Gap1;
			end if;
		when CH1 => -- items regarding state ST1
		    reset_counter <= '0';
			if ((PPM_Input or sample1 or sample2 or sample3 or sample4 or sample5) = '0') then 
			     reset_counter <= '1';
			     if(count > x"00061A80")then
			     --61A80
			         NS <= Gap1;
			     else
			         NS <= Gap2;
			     end if;  
			else NS <= CH1;
			end if;
	    when Gap2 =>
	        reset_counter <= '0';
	        if ((PPM_Input and sample1 and sample2 and sample3 and sample4 and sample5) = '1') then NS <= CH2;
			else NS <= Gap2;
			end if;
		when CH2 => -- items regarding state ST1
		     reset_counter <= '0';
			 if ((PPM_Input or sample1 or sample2 or sample3 or sample4 or sample5) = '0') then 
			     reset_counter <= '1';
			     if(count > x"00061A80")then
			         NS <= Gap1;
			     else
			         NS <= Gap3;
			     end if;
			else NS <= CH2;
			end if;
	    when Gap3 =>
	        reset_counter <= '0';
	        if ((PPM_Input and sample1 and sample2 and sample3 and sample4 and sample5) = '1') then NS <= CH3;
			else NS <= Gap3;
			end if;
		when CH3 => -- items regarding state ST1
			reset_counter <= '0';
			if ((PPM_Input or sample1 or sample2 or sample3 or sample4 or sample5) = '0') then 
			 reset_counter <= '1';
                 if(count > x"00061A80")then
                         NS <= Gap1;
                 else
                         NS <= Gap4;
                 end if;
			else NS <= CH3;
			end if;
	    when Gap4 =>
	        reset_counter <= '0';
	        if ((PPM_Input and sample1 and sample2 and sample3 and sample4 and sample5) = '1') then NS <= CH4;
			else NS <= Gap4;
			end if;
		when CH4 => -- items regarding state ST1
			reset_counter <= '0';
			if ((PPM_Input or sample1 or sample2 or sample3 or sample4 or sample5) = '0') then
			 reset_counter <= '1';
			     if(count > x"00061A80")then
                         NS <= Gap1;
                 else
                         NS <= Gap5;
                 end if;
			else NS <= CH4;
			end if;
	    when Gap5 =>
	        reset_counter <= '0';
	        if ((PPM_Input and sample1 and sample2 and sample3 and sample4 and sample5) = '1') then NS <= CH5;
			else NS <= Gap5;
			end if;
		when CH5 => -- items regarding state ST1
			reset_counter <= '0';
			if ((PPM_Input or sample1 or sample2 or sample3 or sample4 or sample5) = '0') then 
			 reset_counter <= '1';
			     if(count > x"00061A80")then
                         NS <= Gap1;
                 else
                         NS <= Gap6;
                end if;
			else NS <= CH5;
			end if;
	    when Gap6 =>
	        reset_counter <= '0';
	        if ((PPM_Input and sample1 and sample2 and sample3 and sample4 and sample5) = '1') then NS <= CH6;
			else NS <= Gap6;
			end if;
		when CH6 => -- items regarding state ST1
			reset_counter <= '0';
			if ((PPM_Input or sample1 or sample2 or sample3 or sample4 or sample5) = '0') then 
			 reset_counter <= '1';
			     if(count > x"00061A80")then
                         NS <= Gap1;
                 else
                         NS <= Gap0;
                 end if;
			else NS <= CH6;
			end if;
		when others => -- the catch all condition
			NS <= Gap0; -- worry about that later
			reset_counter <= '0';
	end case;
end process comb_proc;

	Counter: process(CLK, PS, NS)
	begin
		if(CLK'event and CLK ='1') then
            if(reset_counter ='1') then
                count <=x"00000000";
            else
                count <= count +1;
            end if;
            if (PS = CH1 and NS = Gap2) then
                CH1_count <= count + 1;
            elsif (PS = CH2 and NS = Gap3) then
                CH2_count <= count + 1;
            elsif (PS = CH3 and NS = Gap4) then
                CH3_count <= count + 1;
            elsif (PS = CH4 and NS = Gap5) then
                CH4_count <= count + 1;
            elsif (PS = CH5 and NS = Gap6) then
                CH5_count <= count + 1;
            elsif (PS = CH6 and NS = Gap0) then
                CH6_count <= count + 1;
            elsif (PS = Idle and NS = Gap1) then
                Idle_count <= count + 1;
                frame_counter <= frame_counter + 1;                
            end if;
            frame_counter_out <= frame_counter + 1;
         end if;
		
	 end process;
	 

-- faking some state variable outputs
--with PS select
--	Y <= "0000" when Gap0,
--		"0001" when Idle,
--		"0010" when Gap1,
--		"0011" when CH1,
--		"0100" when Gap2,
--		"0101" when CH2,
--		"0110" when Gap3,
--		"0111" when CH3,
--		"1000" when Gap4,
--		"1001" when CH4,
--		"1010" when Gap5,
--	    "1011" when CH5,
--		"1100" when Gap6,
--		"1101" when CH6,
--		"0000" when others;
end fsm;
