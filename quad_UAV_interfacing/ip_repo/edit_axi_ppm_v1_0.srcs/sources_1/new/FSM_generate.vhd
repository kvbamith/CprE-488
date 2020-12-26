----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 02/13/2020 07:48:33 PM
-- Design Name: 
-- Module Name: FSM_generate - Behavioral
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


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_unsigned.all;
-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity FSM_generate is
port ( CLK,RESET : in std_logic;
PPM_Output: out std_logic;
idle_count, CH1_count, CH2_count, CH3_count, CH4_count, CH5_count, CH6_count: in std_logic_vector(31 downto 0));
end FSM_generate;

architecture Behavioral of FSM_generate is
    type state_type is (Idle,CH1,CH2,CH3,CH4,CH5,CH6);
	signal PS,NS : state_type;
	signal count_value : std_logic_vector(31 downto 0);
	signal reset_counter : std_logic := '1';
	signal switch_state : std_logic := '0';
	signal temp : std_logic_vector(31 downto 0);
	signal cycle_count: std_logic_vector(31 downto 0);
	signal idle_buffer, CH1_buffer, CH2_buffer, CH3_buffer, CH4_buffer, CH5_buffer, CH6_buffer: std_logic_vector(31 downto 0);
begin
	sync_proc: process(CLK,NS,RESET)
	begin
	if (RESET = '1') then
		PS <= Idle;
	elsif (CLK'event and CLK ='1') then
		PS <= NS;
	end if;
	end process sync_proc;

	comb_proc: process(PS, switch_state)
	begin
	case PS is
		when Idle => -- items regarding state ST0	
			if (switch_state = '1') then NS <= CH1; 
			else NS <= Idle; 
			end if;
		when CH1 => -- items regarding state ST1
			if (switch_state = '1') then NS <= CH2; 
			else NS <= CH1; 
			end if;
		when CH2 => -- items regarding state ST1
			if (switch_state = '1') then NS <= CH3; 
			else NS <= CH2;
			end if;
		when CH3 => -- items regarding state ST1
			if (switch_state = '1') then NS <= CH4; 
			else NS <= CH3;
			end if;
		when CH4 => -- items regarding state ST1
			if (switch_state = '1') then NS <= CH5; 
			else NS <= CH4;
			end if;
		when CH5 => -- items regarding state ST1
			if (switch_state = '1') then NS <= CH6; 
			else NS <= CH5;
			end if;
		when CH6 => -- items regarding state ST1
			if (switch_state = '1') then NS <= Idle; 
			else NS <= CH6; 
			end if;
		when others => -- the catch all condition
			NS <= Idle; -- worry about that later
	end case;
end process comb_proc;

	Counter: process(PS, CLK, temp, reset_counter, count_value,idle_count,CH1_count,CH2_count,CH3_count, CH4_count,CH5_count,CH6_count, idle_buffer, CH1_buffer, CH2_buffer, CH3_buffer, CH4_buffer, CH5_buffer, CH6_buffer)
	begin 
	if(CLK'event and CLK ='1') then
	    if(reset_counter ='1') then -- beginning of a channel
			temp <= x"00000000";
			reset_counter <= '0';
			switch_state <='0';
			PPM_Output <= '0';
		else  -- within the channel
			temp <= temp + 1;
			if (temp + 2 < x"00009C40") then
			-- 00009c40
				PPM_Output <= '0';
				switch_state <='0';
			elsif (temp + 2 >= x"00009C40" and temp + 2 < count_value) then
				PPM_Output <= '1';
				switch_state <='0';
			elsif (temp + 2 >= count_value) then
				switch_state <= '1';
				reset_counter <= '1';
				PPM_Output <= '0';
		     end if;
		end if;
		
		if(PS = Idle) then
		    idle_buffer <= idle_count;
		    CH1_buffer <= CH1_count;
		    CH2_buffer <= CH2_count;
            CH3_buffer <= CH3_count;
            CH4_buffer <= CH4_count;
            CH5_buffer <= CH5_count;
            CH6_buffer <= CH6_count;
            count_value <= idle_count;
        elsif(PS= CH1) then
            count_value <= CH1_buffer;
        elsif(PS= CH2) then
            count_value <= CH2_buffer; 
        elsif(PS= CH3) then
            count_value <= CH3_buffer;
        elsif(PS= CH4) then
            count_value <= CH4_buffer;    
        elsif(PS= CH5) then
            count_value <= CH5_buffer; 
        elsif(PS= CH6) then
            count_value <= CH6_buffer;
        end if;
        
	end if;
		
	 end process;

end Behavioral;
