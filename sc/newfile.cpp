#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include "tables.cpp"

#define CHECK_INVAL 25

int KEY_SIZE, TIMES;
std::ofstream FILEOUT;

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

void get_ltt_frq(std::string& cipher, std::vector<std::vector<double>>& letter_freq)
{
	std::vector<double> letter_total(KEY_SIZE, 0.0);
	const int n = cipher.size();
	for(int i = 0,j = 0; i < n; i++)
		if(cipher[i]>= 'a' && cipher[i] <= 'z'){
			letter_freq[j%KEY_SIZE][cipher[i] - 'a']+=1.0;
			letter_total[j%KEY_SIZE]+=1.0;
			j++;
		}
	for(int i = 0; i < KEY_SIZE; i++)
		for(int j = 0; j < 26; j++)
			letter_freq[i][j] = (letter_freq[i][j]*100)/letter_total[i];
}

void run(std::string &cipher, double *table)
{
	const int n = cipher.size();
	std::string facl = strip(cipher), key(KEY_SIZE,0);
	std::vector<std::vector<double>> letter_freq(KEY_SIZE, std::vector<double>(26,0.0));
	std::vector<std::vector<std::pair<int, double>>> shift_dens(KEY_SIZE, 
											   std::vector<std::pair<int, double>>(26,{0,0.0}));
	std::vector<int> positions(KEY_SIZE,0);
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
		std::sort(shift_dens[i].begin(), shift_dens[i].end(), 
			[&](std::pair<int,double> a, std::pair<int,double> b)
			{return a.second < b.second;}
		);
	}
	for(int i = 0; i < KEY_SIZE; i++)
		key[i] = 'a' + shift_dens[i][0].first;

	int l;
	for(int i =0 ; i < TIMES; i++)
	{	
		FILEOUT << key << " ----> " << v_decipher(key, cipher) << "\n\n\n\n";
		l = -1;
		for(int j = 0; j < KEY_SIZE; j++)
		{
			if(positions[j] < 25)
				if(l == -1 || (shift_dens[l][positions[l]+1].second - shift_dens[l][positions[l]].second > 
								shift_dens[j][positions[j]+1].second-shift_dens[j][positions[j]].second))
				{	
					l = j;
				}
			
			if(l==-1)
				return;

			positions[l]++;
			key[l] = 'a' + shift_dens[l][positions[l]].first;
		}
	}
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
	
	{
		std::fstream file;
		file.open(argv[3], std::ofstream::out | std::ofstream::trunc);
		file.close();
	}
	FILEOUT.open(argv[3]);
	std::string cipher;
	double table[26];
	if(strcmp(argv[1], "pt"))
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

	TIMES = atoi(argv[6]);

	const int last = atoi(argv[5]);
	for(int i = atoi(argv[4]); i <= last; i++)
	{
		KEY_SIZE = i;
		run(cipher, table);
	}
}