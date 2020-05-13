#include <functional>
#include <fstream>
#include <vector>
#include <cstring>
#include <unordered_map>
#include <iomanip>
#include <iostream>
#include <bits/stdc++.h>

#include "Rating.h"

using namespace std;

struct HashPair{
  template<class T1, class T2>
  size_t operator()(const pair<T1, T2>& p)const{
    auto hash1 = hash<T1>{}(p.first);
    auto hash2 = hash<T2>{}(p.second);
    return hash1 ^ hash2;
  }
};

typedef pair<uint64_t,uint64_t> IDS;
typedef pair<IDS,size_t> index_entry;


class RandomFile{
  unordered_map<IDS, size_t,HashPair> index;
  size_t size;
  string random_file;
  string index_file;
  string info_file;
  size_t get_size();
  void set_size(size_t new_size);
public:
  RandomFile()= default;
  RandomFile(string name){
    // inicializar los nombres
    this->random_file = name + ".bin";
    this->index_file = "index_" + name + ".bin";
    this->info_file = "info_" + name + ".bin";
    // made the files
    ofstream file;
    file.open(this->random_file, ios::binary);
    file.close();
    file.open(this->index_file, ios::binary);
    file.close();
    //poner el size = 0
    file.open(this->info_file, ios::binary);
    size_t aux = 0;
    this->size = aux;
    file.write((char*) &aux, sizeof(aux));
    file.close();
  }
  void open(string name);
  void update_csv(string file_name);
  void print_file();
  void insert(Rating to_insert);
  void update_index();
  Rating search(uint64_t user = 0, uint64_t movie = 0);
};
size_t RandomFile::get_size(){
  ifstream inFile;
  size_t aux;
  inFile.open(info_file, ios::binary);
  inFile.seekg(0);
  inFile.read((char*)&(aux), sizeof(size_t));
  inFile.close();
  return aux;
}
void RandomFile::set_size(size_t new_size){
  ofstream outFile;
  outFile.open(info_file, ios::binary);
  outFile.seekp(0);
  outFile.write((char*)&(new_size), sizeof(size_t));
  outFile.close();
}

void RandomFile::open(string name){
  //Crea un RandomFile 
  fstream fs;
  this->random_file = name + ".bin";
  this->index_file = "index_" + name + ".bin";
  this->info_file = "info_" + name + ".bin";
  this->size = get_size();
}


void RandomFile::update_csv(string file_name){
    ifstream inFile;
    ofstream outFile, outIndex, outI;
    //open the csv file
    inFile.open(file_name);
    outFile.open(this->random_file,ios::in | ios::binary | ios::app);
    outIndex.open(this->index_file,ios::in | ios::binary | ios::app);
    string aux;
    size_t num_ratings = 0;
    cout << get_size() <<endl;
    if(inFile.is_open()){
      getline(inFile,aux );
      Rating tmp;
      string str;
      while(getline(inFile,aux )){
        stringstream ss(aux);
        getline(ss,str,',');
        tmp.userId = stof(str);
        getline(ss,str,',');
        tmp.movieId = stof(str);
        getline(ss,str,',');
        tmp.rating = stof(str);
        getline(ss,str,',');
        tmp.timestamp = stof(str);
        //insert in the binary file
        outFile.write((char*) &tmp, sizeof(tmp));
        // aqui se debe insertar en el index
        ++num_ratings;
      }
      set_size(num_ratings + get_size());
      inFile.close();
      outFile.close();
      outIndex.close();
      
    }else cout << "Can't open the file "<<file_name <<endl;}

void RandomFile::print_file(){
  ifstream inFile;
  cout << get_size() <<endl;
  inFile.open(this->random_file, ios::binary);
  if(inFile.is_open()){
    Rating i;
    while(inFile.read((char*)&i, sizeof(i))){
      cout<<setw(6) << i.userId << setw(6) << i.movieId <<setw(6)<< i.rating << setw(15) << i.timestamp << endl;
    }
    inFile.close();
  }else cout << "No se pudo abrir el archivo" <<endl;
}

void RandomFile::insert(Rating to_insert){
  //Write on data file
  ofstream outFile;
  outFile.open(random_file, ios::in | ios::binary | ios::app);
  if(outFile.is_open()){
    outFile.write((char*)& to_insert, sizeof(to_insert));
    outFile.close();
  }else{
    cout<<"Couldn't open the data file"<<endl;
  }

  //Write on index file
  index_entry entry;
  entry.first.first = to_insert.userId;
  entry.first.second = to_insert.movieId;
  entry.second = get_size();
  ofstream indexFile;
  indexFile.open(index_file, ios::in| ios::binary | ios::app);
  if(indexFile.is_open()){
    indexFile.write((char*)& entry, sizeof(entry));
    indexFile.close();
    set_size(entry.second + 1);
  }else{
    cout<<"Couldn't open the index file"<<endl;
    return;
  }
}

void RandomFile::update_index(){
  ifstream indexFile;
  indexFile.open(index_file, ios::binary);
  if(indexFile.is_open()){
    index_entry aux;

    while(indexFile.read((char*)&aux, sizeof(aux))){
      index.insert({aux.first,aux.second});
    }
    indexFile.close();
  }else{
    std::cout<<"Couldn't open the index file"<<std::endl;
    return;
  }
}

Rating RandomFile::search(uint64_t userid_s, uint64_t movieid_s){
  Rating ans;
  IDS pair_s(userid_s,movieid_s);
  size_t address;
  RandomFile::update_index();
  ifstream file;
  if(index.find(pair_s) == index.end()){
    std::cout<<"Not found"<<std::endl;
    return ans;
  }else{
    address = index.find(pair_s)->second;
    file.open(random_file, ios::binary);
    file.seekg(address*sizeof(ans));
    file.read((char*)&ans,sizeof(ans));
    file.close();
    std::cout<<"Found"<<std::endl;
    return ans;
  }
}


int main(){
  Rating a{2,3363,4.0,1192913596};
  RandomFile rf("RandomFile");
  rf.update_csv("rating.csv");
  rf.insert(a);
  rf.search(2,3363);
  rf.print_file();


  return 0;
}
