#pragma once
#include <fstream>

void scanner(std::ifstream &fin);
void print_tokens_string(std::ofstream &out);
void print_key_tab(std::ofstream &out);
void print_idn_tab(std::ofstream &out);