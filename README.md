### WHAT DOES IT DO
	This program is a Vigenère cipher/decipher with an additional function, which is the auto-key. The latter is an automatic way to break Vigenère ciphered texts, where the output is composed of the best keys the program can find. The analysis made to determine the best keys is called Frequency Analysis.

### HOW TO USE

The program receives variable arguments:

* original_word cipher_word -> key
* key cipher_file.txt -> deciphered_text
* language cipher_file.txt output_file.txt -> output_file.txt 
	In this case, we are guessing the key used in encrypting the currently ciphered file. The output file contains guesses with acompanning
	deciphered texts.