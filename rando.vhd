library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.std_logic_signed.all;
use IEEE.numeric_std.all;

entity rando is 
port(  
    clk : in std_logic;
    rst : in std_logic;
    number : out std_logic_vector(7 downto 0)
    );
end rando;

architecture operation of rando is

	signal temp : std_logic_vector(7 downto 0) := "00000001";

begin
process(clk)

  variable pseudo : std_logic := '0';
  
begin
if rising_edge(clk) then

	pseudo := temp(4) XOR temp(3) XOR temp(2) XOR temp(0);
	temp <= pseudo & temp(7 downto 1);
	
	end if; 
end process;

	number <= temp;
	
end operation;