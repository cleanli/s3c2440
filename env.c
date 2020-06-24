
#define CONFIG_ENV_OFFSET 0x8000
#define CONFIG_ENV_OFFSET_REDUND 0xc000
#define CONFIG_ENV_SIZE 0x4000
uint8 env_ram[CONFIG_ENV_SIZE];
uint8 env_ram_redund[CONFIG_ENV_SIZE];

void env_init()
{
    nand_reset();
    nand_read_ll(env_ram, CONFIG_ENV_OFFSET, CONFIG_ENV_SIZE);
    nand_read_ll(env_ram_redund,
            CONFIG_ENV_OFFSET_REDUND, CONFIG_ENV_SIZE);
}

/************************************************************************
 * Command interface: print one or all environment variables
 */

/*
 * state 0: finish printing this string and return (matched!)
 * state 1: no matching to be done; print everything
 * state 2: continue searching for matched name
 */
int printenv(char *name, int state)
{
	int i, j;
	char c, buf[17];

	i = 0;
	buf[16] = '\0';

	while (state && env_get_char(i) != '\0') {
		if (state == 2 && envmatch((uchar *)name, i) >= 0)
			state = 0;

		j = 0;
		do {
			buf[j++] = c = env_get_char(i++);
			if (j == sizeof(buf) - 1) {
				if (state <= 1)
					lprintf(buf);
				j = 0;
			}
		} while (c != '\0');

		if (state <= 1) {
			if (j)
				lprintf(buf);
			putch('\n');
		}

		if (ctrlc())
			return -1;
	}

	if (state == 0)
		i = 0;
	return i;
}
