#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include "tables.h"

#define KEY_SIZE 21
#define CHECK_INVAL 25

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

int find_sub(std::string &cipher, int l, int r, int d)
{
	int i,j;
	r-=1;
	for(i = l; i < r; i++)
	{
		for(j = 0; j < 2 && cipher[i+j] == cipher[d+j]; j++);
		if(j == 2)
			return i;
	}
	return -1;
}

std::string strip(std::string& cipher)
{
	const int n = cipher.size();
	std::string res(n, 0);
	for(int i = 0, j = 0; i < n; i++)
		if(!(cipher[i] == ' ' || cipher[i] == '\n' || cipher[i] == '.' || cipher[i] == ','))
		{
			res[j] = cipher[i] + (cipher[i] < 'a')*('a'-'A');
			j++;
		}
	return res;
}

void get_frq(std::string& cipher, std::vector<std::pair<int,int>>& key_prob)
{
	const int n = cipher.size();
	int now, div, ls;

	for(int i = 0; i < n; i++)
	{
		ls = i;
		while(true)
		{
			now = find_sub(cipher,ls+2,n,ls);
			if(now == -1)
				break;
			div = now-i;
			for(int z = 2; z <= KEY_SIZE; z++)
				if(div%z==0)
					key_prob[z].second++;
			ls = now;
		}
	}
}

void get_ltt_frq(std::string& cipher, std::vector<std::vector<double>>& letter_freq)
{
	const int n = cipher.size();
	for(int i = 0,j = 0; i < n; i++)
		if(cipher[i]>= 'a' && cipher[i] <= 'z'){
			letter_freq[j%KEY_SIZE][cipher[i] - 'a']++;
			j++;
		}
}

void do_key(
	std::string& cipher, std::string& stripped, 
	int key_s, double* table, 
	std::vector<std::vector<double>>& letter_freq,
	std::vector<std::vector<double>>& letter_lcl,
	std::vector<std::vector<std::pair<int,double>>>& shift_dens,
	std::string& fileout
)
{
	std::vector<int> it(key_s,0);
	std::vector<double> letter_total(key_s, 0.0);
	double best = 1e9;
	int l = -1, tmp;
	std::fstream file;
	std::string key(key_s,0);

	for(int i = 0; i < key_s; i++)
		for(int j = 0; j < 26; j++)
		{
			letter_lcl[i][j]=0.0;
			shift_dens[i][j]=std::pair<int,double>(j, 0.0);
		}

	for(int i = 0; i < KEY_SIZE; i++)
	{
		for(int j = 0; j < 26; j++)
		{
			letter_lcl[i%key_s][j] += letter_freq[i][j];
			letter_total[i%key_s] += letter_freq[i][j];
		}
	}

	for(int i = 0; i < key_s; i++)
		for(int j = 0; j < 26; j++)
			letter_lcl[i][j] = (letter_lcl[i][j]*100.0)/letter_total[i];
	for(int i = 0; i < key_s; i++)
	{
		for(int j = 0; j < 26; j++)
			std::cout << letter_lcl[i][j] << ' ';
		
		std::cout << '|' << letter_total[i] << '\n';
	}
	
	for(int i = 0; i < key_s; i++) // pos na senha
	{
		for(int j = 0; j < 26; j++) // qtd de shift
			for(int z = 0; z < 26; z++)
			// letras do alfabeto
				shift_dens[i][j].second += abs(letter_lcl[i][(j+z)%26] - table[z]);	
		
		std::sort(shift_dens[i].begin(), shift_dens[i].end(), 
			[&](std::pair<int,double> a, std::pair<int,double> b)
			{return a.second < b.second;}
		);
	}

	for(int i = 0; i < key_s; i++)
	{
		for(int j = 0; j < 26; j++)
			std::cout << (char)(shift_dens[i][j].first + 'A') << ' ' << shift_dens[i][j].second << ' ';
		std::cout << '\n';
	}
	for(int i = 0; i < key_s; i++)
		key[i] = 'a' + shift_dens[i][0].first;
	
	file.open(fileout, std::ofstream::out | std::ofstream::trunc);
	while(true)
	{
		file << key << "|||" << v_decipher(key, cipher) << "\n\n\n";
		best = 1e9;
		l = -1;
		for(int i = 0; i < key_s; i++)
			if(it[i] < 25)
				if(l==-1 || best - shift_dens[l][it[l]].second > shift_dens[i][it[i] + 1].second - shift_dens[i][it[i]].second)
				{
					best = shift_dens[i][it[i] + 1].second;
					l = i;
				}
		

		if(l == -1) break;
		it[l]++;
		key[l] = 'a' + shift_dens[l][it[l]].first;
		
	}
	file.flush();
	std::cout << "pull\n";
	std::cin >> tmp;
	file.close();
}

void run(std::string &cipher, double *table, std::string fileout)
{
	const int n = cipher.size();
	std::vector<bool> dp(n, 0);
	std::vector<std::pair<int,int>> key_prob(KEY_SIZE);
	std::string facl = strip(cipher);
	std::vector<std::vector<double>> letter_freq(KEY_SIZE, std::vector<double>(26,0.0));
	std::vector<std::vector<double>> letter_lcl(KEY_SIZE,  std::vector<double>(26,0.0));
	std::vector<std::vector<std::pair<int, double>>> shift_dens(KEY_SIZE, 
											   std::vector<std::pair<int, double>>(26));

	for(int i = 0 ; i < KEY_SIZE; i++)
		key_prob[i] = std::pair<int,int>(i,0);

	get_frq(facl, key_prob);
	
	get_ltt_frq(facl, letter_freq);

	std::sort(key_prob.begin(), key_prob.end(), 
		[](std::pair<int,int> a, std::pair<int,int> b)
		{ return a.second > b.second;}
	);			
	for(int i = 0; i < KEY_SIZE; i++)
		std::cout << key_prob[i].first << ' ' << key_prob[i].second << ' ';
	std::cout << '\n';

	for(int i = 0; i < KEY_SIZE; i++)
		if(key_prob[i].second != 0)
			do_key(
				cipher,
				facl,
				key_prob[i].first,
				table,
				letter_freq, 
				letter_lcl, 
				shift_dens,
				fileout
			);
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
		std::string a = argv[1], cipher = argv[2];
		if(strstr(argv[2],".txt"))
		{
			std::ifstream file(argv[2]);
			std::stringstream strbf;
			strbf << file.rdbuf();
			cipher = strbf.str();
			file.close();
			std::cout << v_decipher(a,cipher);
		}
		else
			std::cout << v_comparer(a,cipher);
		return 0;	
	}
	std::string cipher;
	double table[26];
	if(strcmp(argv[1], "pt")==0)
		portugues(table);
	else
		ingles(table);
	
	if(strstr(argv[2],".txt"))
	{
		std::ifstream file(argv[2]);
		std::stringstream strbf;
		strbf << file.rdbuf();
		cipher = strbf.str();
		file.close();
	}
	else
		cipher = argv[2];
	run(cipher, table, argv[3]);
}