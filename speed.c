/*
 * (C) Copyright 2001-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, d.mueller@elsoft.ch
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* This code should work for both the S3C2400 and the S3C2410
 * as they seem to have the same PLL and clock machinery inside.
 * The different address mapping is handled by the s3c24xx.h files below.
 */

#include "type.h"
#include "common.h"

#define MPLL 0
#define UPLL 1

/* ------------------------------------------------------------------------- */
/* NOTE: This describes the proper use of this file.
 *
 * CONFIG_SYS_CLK_FREQ should be defined as the input frequency of the PLL.
 *
 * get_FCLK(), get_HCLK(), get_PCLK() and get_UCLK() return the clock of
 * the specified bus in HZ.
 */
/* ------------------------------------------------------------------------- */

static ulong get_PLLCLK(int pllreg)
{
	struct s3c24x0_clock_power *clk_power = s3c24x0_get_base_clock_power();
	ulong r, m, p, s;

	if (pllreg == MPLL)
		r = readl(&clk_power->MPLLCON);
	else if (pllreg == UPLL)
		r = readl(&clk_power->UPLLCON);
	m = ((r & 0xFF000) >> 12) + 8;
	p = ((r & 0x003F0) >> 4) + 2;
	s = r & 0x3;

	return (CONFIG_SYS_CLK_FREQ * m * 2) / (p << s);
}

/* return FCLK frequency */
ulong get_FCLK(void)
{
	return get_PLLCLK(MPLL);
}

/* return HCLK frequency */
ulong get_HCLK(void)
{
	ulong clkdivn, camdivn, hclk, fclk;
	struct s3c24x0_clock_power *clk_power = s3c24x0_get_base_clock_power();

	clkdivn = readl(&clk_power->CLKDIVN);
	camdivn = readl(&clk_power->CAMDIVN);
	fclk = get_FCLK();
	switch((clkdivn & 0x6)>>1){
		case 0:
			hclk = fclk;
			break;
		case 1:
			hclk = fclk/2;
			break;
		case 2:
			if(camdivn & (1<<9))
				hclk = fclk/8;
			else
				hclk = fclk/4;
			break;
		case 3:
			if(camdivn & (1<<9))
				hclk = fclk/6;
			else
				hclk = fclk/3;
			break;
	}

	return hclk;
	//return (readl(&clk_power->CLKDIVN) & 2) ? get_FCLK() / 2 : get_FCLK();
}

/* return PCLK frequency */
ulong get_PCLK(void)
{
	struct s3c24x0_clock_power *clk_power = s3c24x0_get_base_clock_power();

	return (readl(&clk_power->CLKDIVN) & 1) ? get_HCLK() / 2 : get_HCLK();
}

/* return UCLK frequency */
ulong get_UCLK(void)
{
	return get_PLLCLK(UPLL);
}
