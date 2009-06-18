/**********************************************************************
 * --------------------------------------------------------------------
 * Automatic Licence Plate Recognition
 * --------------------------------------------------------------------
 *
 * File:        syntactical_analysis.hpp
 * Author:	Tambet Masik
 * Created:	2009.05.06
 * Description: Licence plate syntactical analysis module. 
 *
 **********************************************************************/
#include <map> 
#include <string>
#include <iostream>

#define MIN_PLATE_SIZE  4
#define MIN_ALNUM_COUNT 3
#define DEFAULT_PLATE_SIZE 6
 
using namespace std;

char cvSymbolToLetter(char symbol);
char cvSymbolToNumber(char symbol);
int  cvValidPlate( string plate );
string cvAnalyzePlate( string plate );

string cvAnalyzePlate( string plate ){
  if( plate.length() == DEFAULT_PLATE_SIZE ){
    plate[0] = cvSymbolToNumber(plate[0]);
    plate[1] = cvSymbolToNumber(plate[1]);
    plate[2] = cvSymbolToNumber(plate[2]);
    
    plate[3] = cvSymbolToLetter(plate[3]);
    plate[4] = cvSymbolToLetter(plate[4]);
    plate[5] = cvSymbolToLetter(plate[5]);
  }
  return plate;
}

char cvSymbolToNumber(char symbol){
  map<char, char> sym_hash;
  sym_hash['A'] = '4';
  sym_hash['L'] = '4';
  sym_hash['I'] = '1';
  sym_hash['O'] = '0';
  sym_hash['U'] = '0';
  sym_hash['S'] = '5';
  return isdigit(symbol) ? symbol : sym_hash[symbol];
}

char cvSymbolToLetter(char symbol){
  map<char, char> sym_hash;
  sym_hash['4'] = 'A';
  sym_hash['1'] = 'I';
  sym_hash['0'] = 'O';
  sym_hash['5'] = 'S';
  return isalpha(symbol) ? symbol : sym_hash[symbol];
} 

int cvValidPlate( string plate ){
  int num_digits = 0, num_letters = 0, num_non_alnum = 0;
  for (int i = 0; i < plate.length(); i++){
    
    if(isalpha(plate.at(i)))
      num_letters++;    /* Increment letter count     */

    if(isdigit(plate.at(i)))
      num_digits++;     /* Increment digit count      */

    if(!isalnum(plate.at(i)))
      num_non_alnum++;  /* Increment non-alnum count  */

  }
  int alnum = num_digits + num_letters;
  return (alnum > plate.length() / 2) & num_non_alnum < MIN_ALNUM_COUNT & plate.length() > MIN_PLATE_SIZE ? 1 : 0; 
}

