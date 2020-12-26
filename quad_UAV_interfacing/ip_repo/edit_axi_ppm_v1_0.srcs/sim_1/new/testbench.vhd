-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
--                                                                           --
-- File Name: TB_Network_Detection.vhd                                                  --
-- Author: Phillip Jones (phjones@iastate.edu  )                             --
-- Date: 2/1/2018                                                           --
--                                                                           --
-- Description: Base testbench for generating stimulus input for             -- 
-- DUT  (Device Under Test)                                                  --
--                                                                           --
-------------------------------------------------------------------------------
-------------------------------------------------------------------------------
library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.STD_LOGIC_ARITH.ALL;
use IEEE.STD_LOGIC_UNSIGNED.ALL;


entity testbench is
port
(
my_in : in std_logic -- input needed to keep modelsim from complainning???
);
end testbench;

architecture rtl of testbench is

----------------------------------------------
--       Component declarations             --
----------------------------------------------

-- Device under test

component FSM_capture
port ( PPM_Input,CLK,RESET : in std_logic;
Y           : out std_logic_vector(3 downto 0);
frame_counter_out    : out std_logic_vector(31 downto 0);
CH1_count, CH2_count, CH3_count, CH4_count, CH5_count, CH6_count, Idle_count: out std_logic_vector(31 downto 0));
end component FSM_Capture;

----------------------------------------------
--          Signal declarations             --
----------------------------------------------

  -- Make sure to add addtional states here if you want a longer sequence of inputs sent to DUT!!
type my_input_states is (S0, S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11, 
                         S12, S13, S14, S15, STOP_TEST);


signal  input_state    : my_input_states;  -- Direct which input vector to use

signal clk : std_logic;
signal reset : std_logic;

signal s_PPM_Input : std_logic;

signal s_Idle_count, s_CH1_count, s_CH2_count, s_CH3_count, s_CH4_count, s_CH5_count, s_CH6_count: std_logic_vector(31 downto 0);

signal s_Y  : std_logic_vector(3 downto 0);


signal s_frame_counter_out    : std_logic_vector(31 downto 0);

begin


-- Processes

-------------------------------------------
-------------------------------------------
-- Process Name: system_clk_gen          --
--                                       --
-- Description: Generat clock to run the --
-- simulation.                           --
--                                       --
--                                       --
-------------------------------------------
-------------------------------------------  
system_clk_gen : process   -- 500 MHz clock
begin
  clk <= '0';
  wait for 10 ns;
    loop
      wait for 1 ns;
      clk <= '1';
      wait for 1 ns;
      clk <= '0';
    end loop;
end process system_clk_gen;


-------------------------------------------
-------------------------------------------
-- Process Name: toggle_reset            --
--                                       --
-- Description: Toggle system reset.     --
-- used if DUT requires a reset signal   --
--                                       --
--                                       --
-------------------------------------------
-------------------------------------------  



------------------------------------------------------------
------------------------------------------------------------
--                                                        --
-- Process Name: DUT stimulus                             --
--                                                        --
-- Send inputs to dut. Holds inputs for HOLD_INPUT_reg    --
--  clk cycles                                            --
--                                                        --
------------------------------------------------------------
------------------------------------------------------------
DUT_stimulus : process
begin
    --wait for 1 ns;
    reset <= '0';
     
     

     
     s_PPM_Input <= '0';
    -- wait for 400 ns;
    wait for 40 ns;

     s_PPM_Input <= '1';   -- idle 
   --  wait for 10680 ns;   
     wait for 500 ns;   
     
     s_PPM_Input <= '0';
     --wait for 400 ns;
     wait for 40 ns;
     
     s_PPM_Input <= '1'; -- channel 1
     --wait for 1523 ns;
     wait for 100 ns;
     
     s_PPM_Input <= '0';
     --wait for 400 ns; 
     wait for 40 ns;
   
     s_PPM_Input <= '1'; -- channel 2
     --wait for 1469 ns;  
      wait for 140 ns;
     
     s_PPM_Input <= '0';
     --wait for 400 ns;
     wait for 40 ns;
     
     s_PPM_Input <= '1'; -- channel 3
     --wait for 1468 ns;
     wait for 180 ns;
     
     
     s_PPM_Input <= '0';
     --wait for 400 ns;
     wait for 40 ns;
     
     s_PPM_Input <= '1'; -- channel 4
     --wait for 1510 ns;
     wait for 220 ns;
     
     s_PPM_Input <= '0';
     --wait for 400 ns;
     wait for 40 ns;
     
     s_PPM_Input <= '1'; -- channel 5
     --wait for 1200 ns;
     wait for 260 ns;
     
     s_PPM_Input <= '0';
     --wait for 400 ns;
     wait for 40 ns;
     
     s_PPM_Input <= '1'; -- channel 6 
    -- wait for 1796 ns;
     wait for 280 ns;
       
     --wait for 1 ns;
    

end process DUT_stimulus;




capturing: FSM_capture

port map(   PPM_Input => s_PPM_Input,
            CLK      => clk,
            RESET => reset,
            Idle_count => s_Idle_count,
            CH1_count => s_CH1_count, 
            CH2_count => s_CH2_count,
            CH3_count => s_CH3_count,
            CH4_count => s_CH4_count,
            CH5_count => s_CH5_count,
            CH6_count => s_CH6_count,
            Y => s_Y,
            frame_counter_out => s_frame_counter_out
);


end rtl;