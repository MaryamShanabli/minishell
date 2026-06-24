/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   banner.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mshanabl <mshanabl@student.42amman.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/24 22:22:53 by mshanabl          #+#    #+#             */
/*   Updated: 2026/06/24 22:44:58 by mshanabl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

#define C1  "\033[48;2;247;193;187m  "
#define C2  "\033[48;2;243;154;147m  "
#define C3  "\033[48;2;240;118;110m  "
#define C4  "\033[48;2;250;154;75m  "
#define Y   "\033[48;2;194;215;198m  "
#define G   "\033[48;2;97;142;100m  "
#define E   "\033[0m  "
#define TEXT_G1 "\033[38;2;247;193;187m"
#define TEXT_G2 "\033[38;2;243;154;147m"
#define TEXT_G3 "\033[38;2;240;118;110m"
#define TEXT_G4 "\033[38;2;250;154;75m"
#define RESET   "\033[0m"

static void	print_banner_blocks_top(void)
{
	printf("\033[H\033[J");
	printf(E E E C1 C1 C1 E);
	printf(E E E E E C2 E C2 E);
	printf(E E E E E E C3 E E);
	printf(E E E E C4 E C4 E C4 E E E E "\n");
	printf(E E C1 C1 C1 C1 C1);
	printf(E E E E C2 C2 C2 C2 C2);
	printf(E E E E E C3 C3 C3 E);
	printf(E E E E E C4 C4 C4 E E E E "\n");
	printf(E E C1 C1 Y C1 C1);
	printf(E E E E C2 C2 Y C2 C2);
	printf(E E E E C3 C3 Y C3 C3);
	printf(E E E E C4 C4 Y C4 C4 E E E E "\n");
	printf(E E C1 C1 C1 C1 C1);
	printf(E E E E E C2 C2 C2 E);
	printf(E E E E E C3 C3 C3 E);
	printf(E E E E E C4 C4 C4 E E E E "\n");
	printf(E E E C1 C1 C1 E);
	printf(E E E E E E C2 E E);
	printf(E E E E E E C3 E E);
	printf(E E E E C4 E C4 E C4 E E E E "\n");
}

static void	print_banner_blocks_bottom(void)
{
	printf(E E   E E G E E        E E E E   E E G E E        E E E E);
	printf(E E G E E        E E E E   E E G E E        E E E E "\n");
	printf(E E   E G G E E        E E E E   E G G E E        E E E E);
	printf(E G G E E        E E E E   E G G E E        E E E E "\n");
	printf(E E   E E G G E        E E E E   E E G G E        E E E E);
	printf(E E G G E        E E E E   E E G G E        E E E E "\n");
	printf(E E   E E G E E        E E E E   E E G E E        E E E E);
	printf(E E G E E        E E E E   E E G E E        E E E E "\n");
	printf(E E   E E G E E        E E E E   E E G E E        E E E E);
	printf(E E G E E        E E E E   E E G E E        E E E E "\n\n");
}

static void	print_banner_title(void)
{
	usleep(20000);
	printf(TEXT_G1 "  ███▄ ▄███  ██  ███▄    ██  ██  ███████  ");
	printf("██   ██  ████████  ██       ██  \n");
	printf(TEXT_G2 "  ██ ▀█▀ ██  ██  ██ ▀█   ██  ██  ██       ");
	printf("██   ██  ██        ██       ██  \n");
	printf(TEXT_G3 "  ██     ██  ██  ██  ▀█  ██  ██  ███████  ");
	printf("███████  ██████    ██       ██  \n");
	printf(TEXT_G4 "  ██     ██  ██  ██   ▀█ ██  ██       ██  ");
	printf("██   ██  ██        ██       ██  \n");
	printf(TEXT_G4 "  ██     ██  ██  ██    ▀███  ██  ███████  ");
	printf(TEXT_G4 "██   ██  ████████  ████████ ████████  \n\n" RESET);
}

static void	print_banner_signature(void)
{
	usleep(20000);
	printf("                           " TEXT_G1 "m");
	printf(TEXT_G1 "shanabl" TEXT_G2 "   ✦   " TEXT_G4 "oalfoqha\n\n" RESET);
}

void	print_minishell_masterpiece(void)
{
	print_banner_blocks_top();
	print_banner_blocks_bottom();
	print_banner_title();
	print_banner_signature();
}
