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


entity testbench_generate is
port
(
my_in : in std_logic -- input needed to keep modelsim from complainning???
);
end testbench_generate;

architecture rtl of testbench_generate is

----------------------------------------------
--       Component declarations             --
----------------------------------------------

component FSM_generate
port ( CLK,RESET : in std_logic;
PPM_Output: out std_logic;
idle_count, CH1_count, CH2_count, CH3_count, CH4_count, CH5_count, CH6_count: in std_logic_vector(31 downto 0));
end component FSM_generate;

----------------------------------------------
--          Signal declarations             --
----------------------------------------------

signal clk : std_logic;
signal reset : std_logic;

signal s_PPM_Output : std_logic;

signal s_idle_count, s_CH1_count, s_CH2_count, s_CH3_count, s_CH4_count, s_CH5_count, s_CH6_count: std_logic_vector(31 downto 0);

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
    reset <= '0';
    
    s_idle_count <= x"0000000C";
    s_CH1_count <= x"00000005";
    s_CH2_count <= x"00000006";
    s_CH3_count <= x"00000007";
    s_CH4_count <= x"00000008";
    s_CH5_count <= x"00000009";
    s_CH6_count <= x"0000000A";
    
    wait for 40 ns;
    
    s_CH2_count <= x"0000000B";
    wait;
       
  
end process DUT_stimulus;


generation: FSM_generate

port map( CLK      => clk,
            RESET => reset,
            PPM_Output => s_PPM_Output,
            idle_count => s_idle_count,
            CH1_count => s_CH1_count, 
            CH2_count => s_CH2_count,
            CH3_count => s_CH3_count,
            CH4_count => s_CH4_count,
            CH5_count => s_CH5_count,
            CH6_count => s_CH6_count
);


end rtl;