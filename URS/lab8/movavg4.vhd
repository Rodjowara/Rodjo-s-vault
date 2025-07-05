library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;

entity movavg4 is
  port (
	clk : in std_logic;
	rst : in std_logic;

	-- Adresna sabirnica
	port_id : in std_logic_vector(7 downto 0);

	-- Podatkovna sabirnica za pisanje u registar, spaja se na out_port od KCPSM6
	in_port : in std_logic_vector(7 downto 0);
	write_strobe : in std_logic;

	-- Podatkovna sabirnica za čitnaje iz registra, spaja se na in_port od KCPSM6
	out_port : out std_logic_vector(7 downto 0);
	read_strobe : in std_logic

  ) ;
end entity ; -- movavg4

architecture arch of movavg4 is -- Nadopunite opis arhitekture sklopa
	signal r3: std_logic_vector(7 downto 0) := (others => '0');
	signal r2: std_logic_vector(7 downto 0) := (others => '0');
	signal r1: std_logic_vector(7 downto 0) := (others => '0');
	signal r0: std_logic_vector(7 downto 0) := (others => '0');
	signal sum: std_logic_vector(7 downto 0) := (others => '0');
	signal count: integer range 0 to 4 := 0;
begin
	process(clk, rst)
	variable temp_sum: unsigned(9 downto 0);
	variable v_r3, v_r2, v_r1, v_r0 : unsigned(7 downto 0);
	begin
		if rst = '1' then
			r3 <= (others => '0');
			r2 <= (others => '0');
			r1 <= (others => '0');
			r0 <= (others => '0');

			count <= 0;
			sum <= (others => '0');

		elsif rising_edge(clk) then
			if write_strobe = '1' and port_id = x"00" then

				v_r3 := unsigned(r2);
      			v_r2 := unsigned(r1);
      			v_r1 := unsigned(r0);
      			v_r0 := unsigned(in_port); 

				r3 <= std_logic_vector(v_r3);
     			r2 <= std_logic_vector(v_r2);
      			r1 <= std_logic_vector(v_r1);
      			r0 <= std_logic_vector(v_r0);

				if count < 3 then
					count <= count + 1;
				else
					count <= 4;
					temp_sum := resize(v_r3, 10) + resize(v_r2, 10) + resize(v_r1, 10) + resize(v_r0, 10);
					temp_sum := temp_sum srl 2;
					sum <= std_logic_vector(temp_sum(7 downto 0));
				end if;

			elsif write_strobe = '1' and port_id = x"02" then
				r3 <= (others => '0');
				r2 <= (others => '0');
				r1 <= (others => '0');
				r0 <= (others => '0');

				count <= 0;
				sum <= (others => '0');

			end if;

			if read_strobe = '1' and port_id = x"01" then
				out_port <= sum;
			else
				out_port <= (others => '0');
			end if;

		end if;
	end process;
end architecture arch;