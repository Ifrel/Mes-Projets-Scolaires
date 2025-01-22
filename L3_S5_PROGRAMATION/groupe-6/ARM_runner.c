/*
ELF Loader - chargeur/implanteur d'exécutables au format ELF à but pédagogique
Copyright (C) 2012 Guillaume Huard
Ce programme est libre, vous pouvez le redistribuer et/ou le modifier selon les
termes de la Licence Publique Générale GNU publiée par la Free Software
Foundation (version 2 ou bien toute autre version ultérieure choisie par vous).

Ce programme est distribué car potentiellement utile, mais SANS AUCUNE
GARANTIE, ni explicite ni implicite, y compris les garanties de
commercialisation ou d'adaptation dans un but spécifique. Reportez-vous à la
Licence Publique Générale GNU pour plus de détails.

Vous devez avoir reçu une copie de la Licence Publique Générale GNU en même
temps que ce programme ; si ce n'est pas le cas, écrivez à la Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
États-Unis.

Contact: Guillaume.Huard@imag.fr
		 ENSIMAG - Laboratoire LIG
		 51 avenue Jean Kuntzmann
		 38330 Montbonnot Saint-Martin
*/
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "csapp.h"
#include "debug.h"
#include "arm_simulator_interface.h"

#include "util.h"
#include "structure.h"

void usage(char *name)
{
	fprintf(stderr, "Usage:\n"
					"%s [ --help ] [ --host hostname ] [ --service servicename ] [ --debug file ] file\n\n"
					"Loads a sample ARM code to a remote simulator. The --debug flag enables the output produced by "
					"calls to the debug function in the named source file.\n",
			name);
}


void sample_run(char *hostname, char *servicename, char *filename)
{

	FILE *fd = Fopen(filename, "rb");

	ELF32_FILE f;

	read_file(fd, &f);

	Elf32_ShdrFull *section_text = find_section(&f, ".text");

	Elf32_Word sec_size = section_text->header.sh_size;

	uint32_t *code = (uint32_t *)section_text->content;

	Elf32_ShdrFull *section_data = find_section(&f, ".data");
	uint32_t *data = (uint32_t *)section_text->content;

	for (int i = 0; i < sec_size; i++)
	{
		debug(" inst : %08x\n", reverse_4(code[i]));
	}
	debug("sec_size = %d\n", sec_size);

	arm_simulator_data_t simulator;

	debug("Connecting to simulator using host %s and service %s\n", hostname, servicename);
	simulator = arm_connect(hostname, servicename);
	debug("Fetching code to simulator and setting PC\n");
	arm_write_memory(simulator, BASE_TEXT_ADDRESS, code, sec_size);

	arm_write_memory(simulator, BASE_DATA_ADDRESS, data, section_data->header.sh_size);

	arm_write_register(simulator, 15, BASE_TEXT_ADDRESS);
	debug("Running simulator\n");
	arm_run(simulator);
	debug("End of simulation\n");

	fclose(fd);
}

int main(int argc, char *argv[])
{
	int opt;
	char *hostname, *servicename;

	struct option longopts[] = {
		{"debug", no_argument, NULL, 'd'},
		{"host", required_argument, NULL, 'H'},
		{"service", required_argument, NULL, 'S'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}};

	hostname = NULL;
	servicename = NULL;
	while ((opt = getopt_long(argc, argv, "S:H:dh", longopts, NULL)) != -1)
	{
		switch (opt)
		{
		case 'H':
			hostname = optarg;
			break;
		case 'S':
			servicename = optarg;
			break;
		case 'h':
			usage(argv[0]);
			exit(0);
		case 'd':
			add_debug_to("ARM_runner.c");
			break;
		default:
			fprintf(stderr, "Unrecognized option %c\n", opt);
			usage(argv[0]);
			exit(1);
		}
	}

	sample_run(hostname, servicename, argv[optind]);
	return 0;
}
