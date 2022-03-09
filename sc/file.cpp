#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>

std::string v_cipher(const std::string& senha, const std::string& msg)
{
	const int n = msg.size(), ns = senha.size();
	// inicializa string com n de tamanho, todas posicoes contem \0.
	std::string res(n,'\0'); 

	for(int i = 0, j = 0; i < n; i++)
	{
		if(msg[i] == ' ' || msg[i] == '\n' || msg[i] == '.' || msg[i] == ',')
			res[i] = msg[i];
		else
		{	
			//calculo do shift do alfabeto + ascii A
			res[i] = (msg[i]+senha[j]-'A'*2)%26 + (msg[i] < 'a')*'A' + (msg[i] >= 'a');
			//lida com diferenca de tamanho entre a senha e a mensagem
			j = (j+1)%ns;
		}
	}
	return res;
}

std::string v_decipher(const std::string& senha, const std::string& cgm)
{
	const int n = cgm.size(), ns = senha.size();
	std::string res(n, 0);
	for(int i = 0,j=0; i < n; i++)
	{
		if(cgm[i] == ' ' || cgm[i] == '\n' || cgm[i] == '.' || cgm[i] == ',')
			res[i] = cgm[i];
		else
		{
			//calculo inverso do shift do alfabeto usando a senha e criptograma + ascii A
			res[i] = cgm[i] - senha[j] + (senha[j]>cgm[i])*26 + (cgm[i] < 'a')*'A' + (cgm[i] >= 'a');
			j = (j+1)%ns;
		}
	}
	return res;
}

void portugues(double* table)
{
	double frequencies[] = {
		14.63,
		1.04,
		3.88,
		4.99,
		12.57,
		1.02,
		1.30,
		1.28,
		6.18,
		0.40,
		0.02,
		2.78,
		4.74,
		5.05,
		10.73,
		2.52,
		1.20,
		6.53,
		7.81,
		4.34,
		4.63,
		1.67,
		0.10,
		0.21,
		0.01,
		0.47
	};

	for(int i = 0; i < 26; i++)
		table[i]=frequencies[i];
}

void ingles(double* table)
{
	double frequencies[] = {
		8.167,
		1.492,
		2.782,
		4.253,
		12.702,
		2.228,
		2.015,
		6.094,
		6.966,
		0.153,
		0.722,
		4.025,
		2.406,
		6.749,
		7.507,
		1.929,
		0.095,
		5.987,
		6.327,
		9.056,
		2.758,
		0.978,
		2.360,
		0.150,
		1.974,
		0.074
	};

	for(int i = 0; i < 26; i++)
		table[i]=frequencies[i];
}

int find_sub(std::string &cipher, int l, int r, char* substr)
{

}

std::vector<int> get_frq(std::vector<std::pair<int,int>>& key_prob)
{
	
}

void run(std::string &cipher, int *table, std::string fileout)
{
	const int n = cipher.size();
	std::vector<bool> dp(n, 0);
	std::vector<std::pair<int,int>> key_prob(20);
	get_frq(cipher,table);
}

int main(int argc, char** argv)
{
	if(argc < 3)
	{
		printf("Faltam argumentos!\n");
		return 0;
	}
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
	}
	else
		cipher = argv[2];
	run(cipher, table, argv[3]);
}