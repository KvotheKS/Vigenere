#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <string>
#include <set>
#include <cstring>
#include <fstream>
#include <sstream>
#include "tables.h"

double L_QUALITY = 1e9;

//funcao de comparacao que serve para filtrar as melhores chaves
bool set_cmp(const std::pair<std::string, double>& a, const std::pair<std::string, double>& b)
{
	return a.second < b.second;
}

int KEY_SIZE;
std::set<std::pair<std::string, double>, decltype(&set_cmp)> best_keys(&set_cmp);

//faz uma senha que transforma msg em cgm
std::string v_comparer(std::string& msg, std::string cgm)
{
	const int n = msg.size();
	std::string res(n,'\0');
	for(int i =0; i < n; i++)
	{
		if(!((msg[i] >= 'A' && msg[i] <= 'Z') || (msg[i] >= 'a' && msg[i] <= 'z')))
			res[i] = msg[i];
		else
			res[i] = (cgm[i] - msg[i] + (cgm[i]<msg[i])*26)%26 + 'a';
			
	}
	return res;
}

//Cifra de Vigenere
std::string v_cipher(std::string& senha, const std::string msg)
{
	const int n = msg.size(), ns = senha.size();
	// inicializa string com n de tamanho, todas posicoes contem \0.
	std::string res(n,'\0'); 

	for(int i = 0, j = 0; i < n; i++)
	{
		if(!((msg[i] >= 'A' && msg[i] <= 'Z') || (msg[i] >= 'a' && msg[i] <= 'z')))
			res[i] = msg[i];
		else
		{	
			if(msg[i] < 'a' && senha[j] >= 'a') senha[j] += 'A' - 'a';
			else if(msg[i] >= 'a' && senha[j] < 'a') senha[j] += 'a' - 'A';
			//calculo do shift do alfabeto + ascii A
			const int tmp = (msg[i]>='a')*'a' + (msg[i]<'a')*'A';
			res[i] = (msg[i]+senha[j]-tmp*2)%26 + tmp;
			//lida com diferenca de tamanho entre a senha e a mensagem
			j = (j+1)%ns;
		}
	}
	return res;
}

//decifra um criptograma Vigenere
std::string v_decipher(std::string& senha, const std::string cgm)
{
	const int n = cgm.size(), ns = senha.size();
	std::string res(n, 0);
	for(int i = 0,j=0; i < n; i++)
	{
		if(!((cgm[i] >= 'A' && cgm[i] <= 'Z') || (cgm[i] >= 'a' && cgm[i] <= 'z')))
			res[i] = cgm[i];
		else
		{
			if(cgm[i] < 'a' && senha[j] >= 'a') senha[j] += 'A' - 'a';
			else if(cgm[i] >= 'a' && senha[j] < 'a') senha[j] += 'a' - 'A';
			//calculo inverso do shift do alfabeto usando a senha e criptograma + ascii A/a
			res[i] = cgm[i] - senha[j] + (senha[j]>cgm[i])*26 + 
								(cgm[i]>='a')*'a' + (cgm[i]<'a')*'A';
			j = (j+1)%ns;
		}
	}
	return res;
}

//transforma a string inicial em uma string apenas com letras ascii
std::string strip(std::string& cipher)
{
	const int n = cipher.size();
	std::string res(n, 0);
	for(int i = 0, j = 0; i < n; i++)
		if((cipher[i] >= 'A' && cipher[i] <= 'Z') || (cipher[i] >= 'a' && cipher[i] <= 'z'))
		{
			res[j] = cipher[i] + (cipher[i] < 'a')*('a'-'A');
			j++;
		}
	return res;
}

//pega as frequencias relativas das letras para todas as letras da chave
void get_ltt_frq(std::string& cipher, std::vector<std::vector<double>>& letter_freq)
{
	std::vector<double> letter_total(KEY_SIZE, 0.0);
	const int n = cipher.size();
	//calculo de frequencias absolutas
	for(int i = 0,j = 0; i < n; i++)
		if(cipher[i]>= 'a' && cipher[i] <= 'z'){
			letter_freq[j%KEY_SIZE][cipher[i] - 'a']+=1.0;
			letter_total[j%KEY_SIZE]+=1.0;
			j++;
		}
	//calculo de frequencias relativas
	for(int i = 0; i < KEY_SIZE; i++)
		for(int j = 0; j < 26; j++)
			letter_freq[i][j] = (letter_freq[i][j]*100)/letter_total[i];
}

//eh o programa em questao. A partir da variavel global KEY_SIZE, geramos as 15
//melhores chaves de tamanho KEY_SIZE e filtramos todas pelo std::set, de forma que 
//no fim do programa apenas as melhores chaves estao disponiveis.
void run(std::string &cipher, double *table)
{
	const int n = cipher.size();
	std::string facl = strip(cipher), key(KEY_SIZE,0);
	std::vector<std::vector<double>> letter_freq(KEY_SIZE, std::vector<double>(26,0.0));
	std::vector<std::vector<std::pair<int, double>>> shift_dens(KEY_SIZE, 
											   std::vector<std::pair<int, double>>(26,{0,0.0}));
	double quality = 0.0;

	get_ltt_frq(facl, letter_freq);

	for(int i = 0; i < KEY_SIZE; i++) // pos na senha
	{
		for(int j = 0; j < 26; j++)
		{ // qtd de shift
			shift_dens[i][j].first = j;
			for(int z = 0; z < 26; z++)
			// letras do alfabeto
				shift_dens[i][j].second += abs(letter_freq[i][(j+z)%26] - table[z]);	
		}
		// Ordena as letras para a posicao i da chave.
		std::sort(shift_dens[i].begin(), shift_dens[i].end(), 
			[&](std::pair<int,double> a, std::pair<int,double> b)
			{return a.second < b.second;}
		);
	}
	// carrega a chave com as melhores letras de cada posicao
	// inicializa a qualidade da chave
	for(int i = 0; i < KEY_SIZE; i++)
	{
		key[i] = 'a' + shift_dens[i][0].first;
		quality += shift_dens[i][0].second;
	}

	const auto it = prev(best_keys.end());
	// Filtro das melhores chaves geradas baseado na qualidade/tamanho_chave
	const double qks = quality/((double)KEY_SIZE);
	if(best_keys.size() == 0 || qks < L_QUALITY)
		best_keys.insert({key, qks});	
	L_QUALITY = qks;
}

void breaking_hub(std::string& cipher, double* table, std::string fileout)
{
	const int last = std::min(45, (int)cipher.size());

	for(int i = 2; i <= last; i++)
	{
		KEY_SIZE = i; //tamanho da chave que estamos tentando
		run(cipher, table); //processo de criacao e ordenacao das chaves geradas
	}
	
	std::ofstream file;
	//serve para limpar os dados anteriores da file
	file.open(fileout, std::ofstream::out | std::ofstream::trunc); 
	file.close();
	file.open(fileout);
	//serve para pegar as chaves em ordem de qualidade
	for(auto it = best_keys.begin(); it != best_keys.end(); it++)
	{
		std::string key = it->first;
		file << key << " ----> " << v_decipher(key, cipher) << "\n\n\n\n";
	}
}
//serve para quando apenas 2 argumentos sejam entregados para o programa
//se a segunda entrada for uma file, utilisamos o primeiro argumento para decifrar
//o arquivo. Caso contrario, criamos uma senha de mesmo tamanho das strings fornecidas
//que transforme a string1 em string2.
void utilities(std::string lf, std::string rg)
{
	if(lf.find(".txt") != std::string::npos)
	{
		std::ifstream file(lf);
		std::stringstream strbf;
		strbf << file.rdbuf();
		rg = strbf.str();
		file.close();
		std::cout << v_decipher(lf,rg);
	}
	else
		std::cout << v_comparer(lf,rg);
		
}

void load_table(double* table, std::string language)
{
	if(language == "pt")
		portugues(table);
	else
		ingles(table);
}

std::string load_cipher(std::string f_input)
{
	if(f_input.find(".txt") != std::string::npos)
	{
		std::ifstream file(f_input);
		std::stringstream strbf;
		strbf << file.rdbuf();
		file.close();
		return strbf.str();
	}
	else
		return f_input;
}

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		printf("Faltam argumentos!\n");
		return 0;
	}
	if(argc == 3)
	{
		utilities(argv[1], argv[2]);
		return 0;	
	}
	
	std::string cipher = load_cipher(argv[2]);
	
	double table[26];
	load_table(table, argv[1]);

	breaking_hub(cipher,table,argv[3]);
}