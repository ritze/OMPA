/*
 * main.cpp
 *
 *  Created on: 16.11.2013
 *      Author: Moritz Lüdecke
 */

#include <getopt.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

#include <src/Analyse.h>

#include "ArrayTest.h"
#include "Debug.h"
#include "DeAndIncrementTest.h"
#include "DependenceTest.h"
#include "MathTest.h"

#define NAME "ompa-test"
#define NAME_FULL "OpenCL Memory Pattern Analyser"
#define AUTHOR "Moritz Lüdecke"
#define VERSION "0.3"

#define GETOPT_VERSION_CHAR 0

//static bool test(const std::string pretext, bool successfull) {
//	std::cout << pretext << ":\n";
//
//	return successfull;
//}

/* Print the usage and help message */
int usage(int status) {
	if (status != EXIT_SUCCESS) {
		std::cout << "Try '" << NAME << " --help' for more information.\n";

	} else {
		std::cout << "Usage: " << NAME << " [OPTION]... [FILE]...\n";
		std::cout << "\n";
		std::cout << "  -f, --force    ignore nonexistent files and arguments,"
		          <<          "never prompt\n";
		std::cout << "  -h, --help     display this help and exit\n";
		std::cout << "      --version  output version information and exit\n";
	}
	return status;
}

int main(int argc, char **argv) {
	int c;
	bool force = false;
	std::list<std::string> files;

	static struct option long_options[] = { { "force", no_argument, NULL, 'f' },
	                                        { "help", no_argument, NULL, 'h' },
	                                        { "version", no_argument, NULL,
	                                          GETOPT_VERSION_CHAR },
	                                        { NULL, 0, NULL, 0 } };

	/* Parse command line options. */
	while ((c = getopt_long(argc, argv, "fh", long_options, NULL)) != -1) {
		switch (c) {

			case GETOPT_VERSION_CHAR:
				std::cout << NAME << " (" << NAME_FULL << ") " << VERSION
				          << "\n";
				std::cout << "Written by " << AUTHOR << ".\n";
				return EXIT_SUCCESS;
				break;

			case 'f':
				force = true;
				break;

			case 'h':
				return usage(EXIT_SUCCESS);

			default:
				return usage(EXIT_FAILURE);
		}
	}

	/* Get filename */
	while (optind < argc) {
		files.push_back(argv[optind++]);
	}

	/* Sort list and delete double entries */
	files.sort();
	files.unique();

	/* Check for regular and accessible files */
	for (std::list<std::string>::iterator it = files.begin(); it != files.end();
	        ++it) {

		bool remove = false;
		struct stat status;
		stat(it->c_str(), &status);

		/* Check for regular file  */
		if (!S_ISREG(status.st_mode)) {
			std::cout << NAME << ": " << it->c_str() << ": No such file\n";
			remove = true;

			/* Check for accessible */
		} else if (access(it->c_str(), R_OK) == -1) {
			std::cout << NAME << ": " << it->c_str() << ": Permission denied\n";
			remove = true;
		}

		if (remove) {
			if (force) {
				if (it == files.end()) {
					files.erase(it);
					continue;
				} else {
					std::list<std::string>::iterator temp = it--;
					files.erase(temp);
				}
			} else {
				return EXIT_FAILURE;
			}
		}
	}

	if (!files.empty()) {
		for (std::list<std::string>::iterator file = files.begin();
		        file != files.end(); ++file) {

			const std::string kernel(
			        std::istreambuf_iterator<char>(
			                std::ifstream((*file).c_str()).rdbuf()),
			        std::istreambuf_iterator<char>());

			Analyse analyse(kernel, false);

			Debug* debug = new Debug(analyse.getSourceManager());
			debug->print(analyse.getKernelInfo());
		}

	} else {
		std::cout << NAME << ": No input file(s). Run default test cases.\n";

		bool successful = true;
		ArrayTest arrayTest;
		DeAndIncrementTest deAndIncrementTest;
		DependenceTest dependenceTest;
		MathTest mathTest;

		std::cout << "ArrayTest:\n";
		successful &= arrayTest.run();

		std::cout << "DeAndIncrementTest:\n";
		successful &= deAndIncrementTest.run();

		std::cout << "DependenceTest:\n";
		successful &= dependenceTest.run();

		std::cout << "MathTest:\n";
		successful &= mathTest.run();

		if (!successful) {
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
